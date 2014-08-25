// Copyright © 2014, Peter Atashian

use std::io::timer::sleep;
use std::io::{TcpStream};
use std::sync::Arc;
use std::sync::atomics::{AtomicUint, SeqCst};
use std::task::TaskBuilder;
use std::time::duration::Duration;

fn main() {
    let count = Arc::new(AtomicUint::new(0));
    loop {
        let clone = count.clone();
        TaskBuilder::new().stack_size(32768).spawn(proc() {
            let mut tcp = match TcpStream::connect("127.0.0.1", 273) {
                Ok(tcp) => tcp,
                Err(_) => return,
            };
            println!("+{}", clone.fetch_add(1, SeqCst));
            loop {
                if tcp.read_u8().is_err() {
                    println!("-{}", clone.fetch_add(-1, SeqCst));
                    return;
                }
            }
        });
        sleep(Duration::seconds(1));
    }
}
