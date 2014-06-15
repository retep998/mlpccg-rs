#![feature(phase)]
#[phase(plugin)] extern crate green;

use std::collections::{SmallIntMap};
use std::io::{Acceptor, BufReader, IoError, Listener, TcpListener, TcpStream};
use std::sync::{Arc, RWLock};

green_start!(main)

type Opcode = u16;

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

struct Server {
    handlers: SmallIntMap<fn()>,
}

impl Server {
    fn new() -> Server {
        unimplemented!();
    }
    fn gen_handlers() -> SmallIntMap<fn()> {
        unimplemented!();
    }
}

fn handle_client(mut stream: TcpStream) -> Result<(), IoError> {
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
