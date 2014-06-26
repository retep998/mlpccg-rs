
#![feature(macro_rules)]

use std::collections::HashMap;
use std::comm::{Receiver, Sender};
use std::io::{Acceptor, BufferedReader, BufWriter, Listener, MemReader, MemWriter, TcpListener, TcpStream};
use std::io::stdin;
use std::rand::random;

type Id = u32;
type NullResult<T> = Result<T, ()>;
type Packet = Vec<u8>;

enum Message {
    RemovePlayer(Id),
    HandlePacket(Id, Packet),
    AddPlayer(TcpStream),
}

struct Player {
    id: Id,
    tcp: TcpStream,
    name: String,
    send: Sender<Packet>,
}

struct Server {
    players: HashMap<Id, Player>,
    send: Sender<Message>,
    recv: Receiver<Message>,
}

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
        println!("Got packet to send!");
        let len = msg.len() as u32 - 4;
        {
            let mut buf = BufWriter::new(msg.mut_slice_to(4));
            buf.write_be_u32(len).unwrap();
        }
        let msg = msg;
        tcp.write(msg.as_slice()).unwrap();
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

fn commands(_: Sender<Message>) {
    let mut cin = stdin();
    for line in cin.lines() {
        let line = line.unwrap();
        let line: String = line.as_slice().chars().map(|c| c.to_lowercase())
            .filter(|c| *c != '\n' && *c != '\r').collect();
        match line.as_slice() {
            _ => println!("Unknown command"),
        }
    }
}

impl Player {
    fn default_name(id: Id) -> String {
        format!("Pony{:08X}", id)
    }
    fn new(id: Id, mut tcp: TcpStream, send: Sender<Message>) -> Player {
        let tcp2 = tcp.clone();
        spawn(proc() packet_receiver(send, tcp2, id));
        let (send, recv) = channel::<Packet>();
        let tcp2 = tcp.clone();
        spawn(proc() packet_sender(recv, tcp2));
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
        self.send.send(p.unwrap());
    }
    fn send_players(&self, server: &Server) {
        let mut p = new_packet();
        p.write_be_u16(0xB).unwrap();
        p.write_be_u32(server.players.len() as u32).unwrap();
        for (_, player) in server.players.iter() {
            p.write_be_u32(player.id).unwrap();
            p.write_be_u32(player.name.len() as u32).unwrap();
            p.write_str(player.name.as_slice()).unwrap();
        }
        self.send.send(p.unwrap());
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
    fn gen_id(&self) -> Id {
        loop {
            let id = random();
            if self.players.find(&id).is_none() { return id }
        }
    }
    fn add_player(&mut self, tcp: TcpStream) {
        let id = self.gen_id();
        let player = Player::new(id, tcp, self.send.clone());
        self.players.insert(player.id, player);
        let player = self.players.get(&id);
        player.send_id();
        player.send_players(self);
    }
    fn remove_player(&mut self, id: Id) {
        let mut player = match self.players.pop(&id) {
            Some(player) => player,
            None => return,
        };
        println!("{} disconnected", player.name);
        player.tcp.close_read().unwrap();
        player.tcp.close_write().unwrap();
    }
    fn handle_packet(&mut self, id: Id, packet: Packet) {
        let player = match self.players.find_mut(&id) {
            Some(player) => player,
            None => { println!("Packet from non-existant player: {}", id); return },
        };
        let mut packet = MemReader::new(packet);
        let opcode = match packet.read_be_u16() {
            Ok(opcode) => opcode,
            Err(_) => { println!("Failed to read opcode from player: {}", id); return },
        };
        match opcode {
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
            }
        }
    }
}
