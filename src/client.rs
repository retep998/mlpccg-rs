
#![feature(phase)]
#[phase(plugin)]
extern crate green;

use std::io::timer::sleep;
use std::io::{BufferedWriter, TcpStream};
use std::task::TaskBuilder;

fn client() {
    let tcp = TcpStream::connect("127.0.0.1", 273).unwrap();
    let mut buf = BufferedWriter::new(tcp);
    loop {
        buf.write_be_u32(6).unwrap();
        buf.write_be_u16(0x1).unwrap();
        buf.write_be_u32(0).unwrap();
        buf.flush();
        sleep(1000);
    }
}

green_start!(main)
fn main() {
    for i in range(0, 1000u) {
        println!("{}", i);
        TaskBuilder::new().stack_size(32768).spawn(proc() client());
        sleep(10);
    }
}
