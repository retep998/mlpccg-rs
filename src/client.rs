
use std::io::timer::sleep;
use std::io::{BufferedWriter, TcpStream};

fn client() {
    let tcp = TcpStream::connect("127.0.0.1", 273).unwrap();
    let mut buf = BufferedWriter::new(tcp);
    loop {
        buf.write_be_u32(6).unwrap();
        buf.write_be_u16(0x1).unwrap();
        buf.write_be_u32(0).unwrap();
        sleep(15000);
    }
}

fn main() {
    for i in range(0, 100u) {
        spawn(proc() client());
        println!("{}", i);
    }
}
