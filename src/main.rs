
#![feature(macro_rules)]

use std::collections::HashMap;
use std::comm::{Receiver, Sender};
use std::io::{Acceptor, BufferedReader, BufWriter, Listener, MemWriter, TcpListener, TcpStream};
use std::io::stdin;
use std::rand::random;

type Id = u32;
type NullResult<T> = Result<T, ()>;
type Packet = Vec<u8>;

enum Message {
    RemovePlayer(Id),
    SendPacket(Id, Packet),
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

macro_rules! try(
    ($e:expr) => (match $e { Ok(e) => e, Err(_) => return Err(()) })
)

fn main() {
    println!("NoLifePony Server");
    let mut server = Server::new();
    server.run();
    println!("Shutting down");
}

fn new_packet() -> NullResult<MemWriter> {
    let mut buf = MemWriter::new();
    try!(buf.write([0, 0, 0, 0]));
    Ok(buf)
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
        println!("Got packet!");
        send.send(SendPacket(id, packet));
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

fn commands(_: Sender<Message>) -> NullResult<()> {
    let mut cin = stdin();
    for line in cin.lines() {
        let line = try!(line);
        let line: String = line.as_slice().chars().map(|c| c.to_lowercase())
            .filter(|c| *c != '\n' && *c != '\r').collect();
        match line.as_slice() {
            _ => println!("Unknown command"),
        }
    }
    Ok(())
}

impl Player {
    fn new(id: Id, tcp: TcpStream, send: Sender<Message>) -> Player {
        let tcp2 = tcp.clone();
        spawn(proc() packet_receiver(send, tcp2, id));
        let (send, recv) = channel::<Packet>();
        let tcp2 = tcp.clone();
        spawn(proc() packet_sender(recv, tcp2));
        Player {
            id: id,
            tcp: tcp,
            name: String::new(),
            send: send,
        }
    }
}

impl Server {
    fn new() -> Server {
        let (send, recv) = channel::<Message>();
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
    fn add_player(&mut self, mut tcp: TcpStream) {
        match tcp.peer_name() {
            Ok(name) => println!("Player connected from {}", name),
            Err(_) => println!("Failed to get player peer name"),
        }
        let player = Player::new(self.gen_id(), tcp, self.send.clone());
        self.players.insert(player.id, player);
    }
    fn run(&mut self) {
        let send = self.send.clone();
        spawn(proc() listener(send));
        let send = self.send.clone();
        spawn(proc() commands(send).unwrap());
        loop {
            let message = self.recv.recv();
            match message {
                AddPlayer(tcp) => self.add_player(tcp),
                _ => println!("Not handled yet"),
            }
        }
    }
}
