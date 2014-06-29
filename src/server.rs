
#![feature(phase)]
#[phase(plugin)]
extern crate green;

use std::cell::RefCell;
use std::collections::HashMap;
use std::comm::{Receiver, Sender};
use std::io::{Acceptor, BufferedReader, BufWriter, Listener, MemReader, MemWriter, TcpListener, TcpStream};
use std::io::stdin;
use std::rand::random;
use std::task::TaskBuilder;

type Id = u32;
type NullResult<T> = Result<T, ()>;
type Packet = Vec<u8>;

enum Message {
    RemovePlayer(Id),
    HandlePacket(Id, Packet),
    AddPlayer(TcpStream),
    ListPlayers,
}

struct Player {
    id: Id,
    tcp: TcpStream,
    name: String,
    send: Sender<Packet>,
}

struct Server {
    players: HashMap<Id, RefCell<Player>>,
    send: Sender<Message>,
    recv: Receiver<Message>,
}

green_start!(main)
fn main() {
    println!("NoLifePony Server");
    let mut server = Server::new();
    server.run();
    println!("Shutting down");
}


fn new_packet() -> MemWriter {
    let mut buf = MemWriter::new();
    buf.write([0, 0, 0, 0]).unwrap();
    buf
}

fn packet_sender(recv: Receiver<Packet>, mut tcp: TcpStream) {
    for mut msg in recv.iter() {
        let len = msg.len() as u32 - 4;
        {
            let mut buf = BufWriter::new(msg.mut_slice_to(4));
            buf.write_be_u32(len).unwrap();
        }
        let msg = msg;
        match tcp.write(msg.as_slice()) {
            Ok(_) => (),
            Err(_) => return,
        }
    }
}


fn packet_receiver(send: Sender<Message>, tcp: TcpStream, id: Id) {
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
        send.send(HandlePacket(id, packet));
    }
    send.send(RemovePlayer(id));
}

fn listener(send: Sender<Message>) {
    let listen = TcpListener::bind("0.0.0.0", 273);
    let mut accept = listen.listen();
    for stream in accept.incoming() {
        match stream {
            Ok(stream) => send.send(AddPlayer(stream)),
            Err(_) => return,
        }
    }
}

fn commands(send: Sender<Message>) {
    let mut cin = stdin();
    for line in cin.lines() {
        let line = line.unwrap();
        let line: String = line.as_slice().chars().map(|c| c.to_lowercase())
            .filter(|c| *c != '\n' && *c != '\r').collect();
        match line.as_slice() {
            "list" => send.send(ListPlayers),
            _ => println!("Unknown command"),
        }
    }
}

impl Player {
    fn send(&self, packet: MemWriter) {
        let packet = packet.unwrap();
        match self.send.send_opt(packet) {
            Ok(_) => return,
            Err(_) => return, //Remove player if this happens
        }
    }
    fn default_name(id: Id) -> String {
        format!("Pony{:08X}", id)
    }
    fn new(id: Id, mut tcp: TcpStream, sender: Sender<Message>) -> Player {
        let (send, recv) = channel::<Packet>();
        let tcp2 = tcp.clone();
        TaskBuilder::new().stack_size(32768).spawn(proc() packet_sender(recv, tcp2));
        let tcp2 = tcp.clone();
        TaskBuilder::new().stack_size(32768).spawn(proc() packet_receiver(sender, tcp2, id));
        let name = Player::default_name(id);
        match tcp.peer_name() {
            Ok(ip) => println!("{} connected from {}", name, ip),
            Err(_) => println!("{} connected from unknown", name),
        }
        Player {
            id: id,
            tcp: tcp,
            name: name,
            send: send,
        }
    }
    fn send_id(&self) {
        let mut p = new_packet();
        p.write_be_u16(0x4).unwrap();
        p.write_be_u32(self.id).unwrap();
        self.send(p);
    }
    fn send_players_joined(&self, server: &Server) {
        let mut p = new_packet();
        p.write_be_u16(0xB).unwrap();
        p.write_be_u32(server.players.len() as u32).unwrap();
        server.players(|player| {
            p.write_be_u32(player.id).unwrap();
            p.write_be_u32(player.name.len() as u32).unwrap();
            p.write_str(player.name.as_slice()).unwrap();
        });
        self.send(p);
    }
    fn send_player_joined(&self, player: &Player) {
        let mut p = new_packet();
        p.write_be_u16(0xB).unwrap();
        p.write_be_u32(1).unwrap();
        p.write_be_u32(player.id).unwrap();
        p.write_be_u32(player.name.len() as u32).unwrap();
        p.write_str(player.name.as_slice()).unwrap();
        self.send(p);
    }
    fn send_player_left(&self, id: Id) {
        let mut p = new_packet();
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
        let mut p = new_packet();
        p.write_be_u16(0x2).unwrap();
        p.write_be_u32(ping).unwrap();
        self.send(p);
    }
}

impl Server {
    fn new() -> Server {
        let (send, recv) = channel::<Message>();
        let send2 = send.clone();
        spawn(proc() listener(send2));
        let send2 = send.clone();
        spawn(proc() commands(send2));
        Server {
            players: HashMap::new(),
            send: send,
            recv: recv,
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
        let player = Player::new(id, tcp, self.send.clone());
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
            player.tcp.close_read().unwrap();
            player.tcp.close_write().unwrap();
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
            let message = self.recv.recv();
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
}
