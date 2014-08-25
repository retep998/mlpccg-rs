// Copyright © 2014, Peter Atashian

use std::cell::RefCell;
use std::collections::HashMap;
use std::comm::{Receiver, Sender};
use std::io::{Acceptor, Listener, TcpListener, TcpStream};
use std::io::{BufferedReader, BufWriter, MemReader, MemWriter};
use std::io::stdin;
use std::rand::random;
use std::task::TaskBuilder;

type Id = u32;
type Packet = Vec<u8>;

fn main() {
    println!("NoLifePony Server");
    let mut server = Server::new();
    server.run();
    println!("Shutting down");
}

enum ToServer {
    RemovePlayer(Id),
    HandlePacket(Id, Packet),
    AddPlayer(TcpStream),
    ListPlayers,
}

enum ToPlayer {
    Packet(Packet),
    Disconnect,
}

struct Room {
    id: Id,
}

struct Player {
    id: Id,
    name: String,
    player_tx: Sender<ToPlayer>,
    server_tx: Sender<ToServer>,
}

impl Player {
    /// Sends packets to clients
    fn sender(rx: Receiver<ToPlayer>, mut tcp: TcpStream) {
        for msg in rx.iter() {
            match msg {
                Packet(mut msg) => {
                    assert!(msg.len() >= 4);
                    let len = msg.len() as u32 - 4;
                    // Write the length in the space we reserved earlier
                    BufWriter::new(msg.mut_slice_to(4)).write_be_u32(len).unwrap();
                    match tcp.write(msg.as_slice()) {
                        Ok(_) => (),
                        Err(_) => break,
                    }
                },
                Disconnect => break,
            }
        }
        // Done here so close the tcp stream
        // We don't really care about the errors here
        let _ = tcp.close_read();
        let _ = tcp.close_write();
    }

    /// Receives packets from clients
    fn receiver(tx: Sender<ToServer>, tcp: TcpStream, id: Id) {
        struct Packets {
            buf: BufferedReader<TcpStream>,
        }
        impl Iterator<Packet> for Packets {
            fn next(&mut self) -> Option<Packet> {
                self.buf.read_be_u32().and_then(|len| self.buf.read_exact(len as uint)).ok()
            }
        }
        let mut packets = Packets {
            buf: BufferedReader::new(tcp),
        };
        for packet in packets {
            match tx.send_opt(HandlePacket(id, packet)) {
                Ok(_) => (),
                Err(_) => break,
            }
        }
        // We're done so tell the server to remove the player
        let _ = tx.send_opt(RemovePlayer(id));
    }

    fn new_packet() -> MemWriter {
        let mut buf = MemWriter::new();
        // Reserve space for the length
        buf.write_be_u32(0).unwrap();
        buf
    }

    fn send(&self, packet: MemWriter) {
        let packet = packet.unwrap();
        match self.player_tx.send_opt(Packet(packet)) {
            Ok(_) => return,
            Err(_) => {
                self.server_tx.send(RemovePlayer(self.id));
                return
            }
        }
    }

    fn default_name(id: Id) -> String {
        format!("Pony{:08X}", id)
    }

    fn new(id: Id, mut tcp: TcpStream, recv_tx: Sender<ToServer>) -> Player {
        let name = Player::default_name(id);
        match tcp.peer_name() {
            Ok(ip) => println!("{} connected from {}", name, ip),
            Err(_) => println!("{} connected from unknown", name),
        }
        let server_tx = recv_tx.clone();
        let (send_tx, send_rx) = channel();
        let tcp_rx = tcp.clone();
        let tcp_tx = tcp;
        // Custom stack size because 2 MB per task is way too much
        // Plus these tasks don't really do any recursion, so we're okay
        TaskBuilder::new().stack_size(0x8000).spawn(proc() Player::sender(send_rx, tcp_tx));
        TaskBuilder::new().stack_size(0x8000).spawn(proc() Player::receiver(recv_tx, tcp_rx, id));
        Player {
            id: id,
            name: name,
            player_tx: send_tx,
            server_tx: server_tx,
        }
    }

    fn send_id(&self) {
        let mut p = Player::new_packet();
        p.write_be_u16(0x4).unwrap();
        p.write_be_u32(self.id).unwrap();
        self.send(p);
    }

    fn send_players_joined(&self, server: &Server) {
        let mut p = Player::new_packet();
        p.write_be_u16(0xB).unwrap();
        p.write_be_u32(server.players.len() as u32).unwrap();
        for (_, player) in server.players.iter() {
            let player = player.borrow();
            p.write_be_u32(player.id).unwrap();
            p.write_be_u32(player.name.len() as u32).unwrap();
            p.write_str(player.name.as_slice()).unwrap();
        }
        self.send(p);
    }

    fn send_player_joined(&self, player: &Player) {
        let mut p = Player::new_packet();
        p.write_be_u16(0xB).unwrap();
        p.write_be_u32(1).unwrap();
        p.write_be_u32(player.id).unwrap();
        p.write_be_u32(player.name.len() as u32).unwrap();
        p.write_str(player.name.as_slice()).unwrap();
        self.send(p);
    }

    fn send_player_left(&self, id: Id) {
        let mut p = Player::new_packet();
        p.write_be_u16(0xF).unwrap();
        p.write_be_u32(1).unwrap();
        p.write_be_u32(id).unwrap();
        self.send(p);
    }

    fn send_pong(&self, mut packet: MemReader) {
        let ping = match packet.read_be_u32() {
            Ok(ping) => ping,
            Err(_) => { println!("Failed to read ping from {}", self.name); return },
        };
        let mut p = Player::new_packet();
        p.write_be_u16(0x2).unwrap();
        p.write_be_u32(ping).unwrap();
        self.send(p);
    }
}

struct Server {
    players: HashMap<Id, RefCell<Player>>,
    tx: Sender<ToServer>,
    rx: Receiver<ToServer>,
}

impl Server {
    fn new() -> Server {
        let (tx, rx) = channel();
        let listen_tx = tx.clone();
        spawn(proc() Server::listener(listen_tx));
        let command_tx = tx.clone();
        spawn(proc() Server::commands(command_tx));
        Server {
            players: HashMap::new(),
            tx: tx,
            rx: rx,
        }
    }

    fn player(&self, id: Id, func: |&Player|) {
        match self.players.find(&id) {
            Some(player) => func(&*player.borrow()),
            None => (),
        }
    }

    fn mut_player(&self, id: Id, func: |&mut Player|) {
        match self.players.find(&id) {
            Some(player) => func(&mut*player.borrow_mut()),
            None => (),
        }
    }

    fn players(&self, func: |&Player|) {
        for (_, player) in self.players.iter() { func(&*player.borrow()) }
    }

    fn gen_id(&self) -> Id {
        loop {
            let id = random();
            if self.players.find(&id).is_none() { return id }
        }
    }

    fn add_player(&mut self, tcp: TcpStream) {
        let id = self.gen_id();
        let player = Player::new(id, tcp, self.tx.clone());
        self.players(|p| p.send_player_joined(&player));
        self.players.insert(player.id, RefCell::new(player));
        self.player(id, |player| {
            player.send_id();
            player.send_players_joined(self);
        });
    }

    fn remove_player(&mut self, id: Id) {
        self.mut_player(id, |player| {
            println!("{} disconnected", player.name);
            let _ = player.player_tx.send_opt(Disconnect);
        });
        self.players.remove(&id);
        self.players(|player| player.send_player_left(id));
    }

    fn handle_packet(&self, id: Id, packet: Packet) {
        let player = match self.players.find(&id) {
            Some(player) => player,
            None => { println!("Packet from non-existant player: {}", id); return },
        };
        let player = &*player.borrow();
        let mut packet = MemReader::new(packet);
        let opcode = match packet.read_be_u16() {
            Ok(opcode) => opcode,
            Err(_) => { println!("Failed to read opcode from player: {}", id); return },
        };
        match opcode {
            1 => player.send_pong(packet),
            _ => println!("Unknown opcode {} from {}", opcode, player.name),
        }
    }

    fn run(&mut self) {
        loop {
            let message = self.rx.recv();
            match message {
                AddPlayer(tcp) => self.add_player(tcp),
                RemovePlayer(id) => self.remove_player(id),
                HandlePacket(id, packet) => self.handle_packet(id, packet),
                ListPlayers => {
                    print!("Connected: ");
                    self.players(|player| print!("{}, ", player.name));
                    println!("");
                },
            }
        }
    }

    fn listener(tx: Sender<ToServer>) {
        let listen = TcpListener::bind("0.0.0.0", 273);
        let mut accept = listen.listen();
        for tcp in accept.incoming() {
            match tcp {
                Ok(tcp) => tx.send(AddPlayer(tcp)),
                Err(_) => return,
            }
        }
    }

    fn commands(tx: Sender<ToServer>) {
        let mut cin = stdin();
        for line in cin.lines() {
            let line = line.unwrap();
            let line: String = line.as_slice().chars().map(|c| c.to_lowercase())
                .filter(|c| *c != '\n' && *c != '\r').collect();
            match line.as_slice() {
                "list" => tx.send(ListPlayers),
                _ => println!("Unknown command"),
            }
        }
    }
}
