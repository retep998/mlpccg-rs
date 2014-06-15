#![feature(phase)]
#[phase(plugin)] extern crate green;

green_start!(main)

fn main() {
    use std::io::{Acceptor, Listener, TcpListener, TcpStream};
    use std::sync::{Arc, RWLock};
    print!("{}", include_str!("motd.txt"));
    let listener = TcpListener::bind("0.0.0.0", 273);
    let mut acceptor = listener.listen();
    fn handle_client(mut stream: TcpStream) {
        match stream.peer_name() {
            Ok(name) => println!("Connection from {}", name),
            Err(e) => fail!("Failed to get name: {}", e),
        }
    }
    println!("Starting server");
    for stream in acceptor.incoming() {
        match stream {
            Ok(stream) => spawn(proc() handle_client(stream)),
            Err(e) => fail!("Failed to accept connection: {}", e),
        }
    }
}
