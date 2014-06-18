
#![feature(phase, macro_rules)]

#[phase(plugin)]
extern crate green;

use std::collections::{HashMap, SmallIntMap};
use std::comm::{Receiver, Sender};
use std::io::{Acceptor, BufferedReader, BufferedWriter, BufReader, IoResult, Listener, MemWriter, TcpListener, TcpStream, stdin};
use std::sync::{Arc, RWLock};

type Opcode = u16;
type Id = u32;

macro_rules! try(
    ($e:expr, $f:expr) => (match $e { Ok(e) => e, Err(_) => return $f })
)

green_start!(main)
fn main() {
}


fn packet_sender(recv: Receiver<Vec<u8>>, tcp: TcpStream) {
    let mut buf = BufferedWriter::new(tcp);
    for msg in recv.iter() {
        println!("Got packet to send!");
        try!(buf.write_be_u32(msg.len() as u32), ());
        try!(buf.write(msg.as_slice()), ());
        try!(buf.flush(), ());
    }
}
enum Message {
    RemovePlayer(Id),
    SendPacket(Id, Vec<u8>),
    AddPlayer(TcpStream),
    Shutdown,
}
fn packet_receiver(send: Sender<Message>, tcp: TcpStream, id: Id) {
    let mut buf = BufferedReader::new(tcp);
    (|| {
        let len = try!(buf.read_be_u32(), ());
        let data = try!(buf.read_exact(len as uint), ());
        try!(send.send_opt(SendPacket(id, data)), ());
    })();
    try!(send.send_opt(RemovePlayer(0)), ());
}
fn broker(recv: Receiver<Message>) {
    
}
fn listener() {

}
fn commands(send: Sender<Message>) {
    let cin = stdin();
    
}

/*
type Opcode = u16;
type Id = u32;
type Handler = fn(&RWLock<Server>, &RWLock<Player>, BufReader);

fn main() {
    print!("{}", include_str!("motd.txt"));
    let listener = TcpListener::bind("0.0.0.0", 273);
    let mut acceptor = listener.listen();
    println!("Starting server");
    for stream in acceptor.incoming() {
        match stream {
            Ok(stream) => spawn(proc() {
                match handle_client(stream) {
                    Ok(_) => (),
                    Err(e) => println!("Connection aborted: {}", e),
                }
            }),
            Err(e) => fail!("Failed to accept connection: {}", e),
        }
    }
}

fn new_packet(opcode: Opcode) -> IoResult<MemWriter> {
    let mut buf = MemWriter::new();
    try!(buf.write_be_u16(opcode));
    Ok(buf)
}

#[deriving(Share)]
struct Player {
    name: String,
    id: Id,
}

impl Player {
    fn handle_ping(server: &RWLock<Server>, player: &RWLock<Player>, buf: BufReader) {
        
    }
    fn send(player: &RWLock<Player>, buf: &MemWriter) -> IoResult<()> {
        let data = buf.get_ref();
        let mut lock = player.write();
        let stream = &mut lock.deref_mut().stream;
        try!(stream.write_be_u32(data.len() as u32));
        try!(stream.write(data));
        Ok(())
    }
}

struct Server {
    handlers: SmallIntMap<Handler>,
    players: HashMap<Id, Arc<RWLock<Player>>>,
}

impl Server {
    fn new() -> Server {
        Server{handlers: Server::gen_handlers(), players: HashMap::new()}
    }
    fn gen_handlers() -> SmallIntMap<Handler> {
        let mut map = SmallIntMap::new();
        unimplemented!();
        map
    }
}

fn handle_client(mut stream: TcpStream) -> IoResult<()> {
    println!("Connection from {}", try!(stream.peer_name()));
    loop {
        let length = try!(stream.read_be_u32());
        let data = try!(stream.read_exact(length as uint));
        let mut buf = BufReader::new(data.as_slice());
        let opcode = try!(buf.read_be_u16());
        match opcode {
            1 => {
                let code = try!(buf.read_be_u32());
                println!("Ping: {}", code);
                try!(stream.write_be_u32(6));
                try!(stream.write_be_u16(2));
                try!(stream.write_be_u32(code));
            }
            _ => println!("Unknown opcode: {}", opcode),
        }
    }
}
*/
