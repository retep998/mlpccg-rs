
#![feature(phase)]
#[phase(plugin)]
extern crate green;

use std::io::timer::sleep;
use std::io::{TcpStream};
use std::task::TaskBuilder;
use std::sync::Arc;
use std::sync::atomics::{AtomicUint, SeqCst};

green_start!(main)
fn main() {
    let count = Arc::new(AtomicUint::new(0));
    loop {
        let clone = count.clone();
        TaskBuilder::new().stack_size(32768).spawn(proc() {
            let mut tcp = match TcpStream::connect("24.28.66.222", 8484) {
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
        sleep(1);
    }
}
