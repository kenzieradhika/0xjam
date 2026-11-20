#!/usr/bin/env python3
"""0xJam host CLI — kontrol ESP32 jammer & S3 multitool via serial.

Usage:
  python3 0xjam.py --port /dev/ttyUSB0 --fw jammer   # classic: s/a/c/h/j/x
  python3 0xjam.py --port /dev/ttyACM0 --fw s3       # S3: b/u/l/i/n/s/w/g
  python3 0xjam.py --port /dev/ttyUSB0 --fw jammer --cmd a   # sekali jalan

Dependency: pip install pyserial
"""
import argparse
import time
import sys

import serial  # pyserial

JAMMER_KEYS = {
    "s": "scan AP + client",
    "a": "deauth semua AP",
    "c": "deauth client AP terpilih",
    "h": "channel hop on/off",
    "j": "BT jam on/off",
    "x": "stop semua",
}
S3_KEYS = {
    "b": "BadUSB payload",
    "u": "U2F stub",
    "l": "BLE spam",
    "i": "IR send",
    "n": "NFC read UID",
    "s": "Sub-GHz CC1101",
    "w": "iButton read",
    "g": "GPIO read",
}


def open_port(port: str, baud: int = 115200) -> serial.Serial:
    try:
        return serial.Serial(port, baud, timeout=1)
    except serial.SerialException as e:
        sys.exit(f"[!] gagal buka {port}: {e}")


def read_serial(ser: serial.Serial, seconds: float = 2.0) -> str:
    ser.timeout = 0.2
    end = time.time() + seconds
    out = []
    while time.time() < end:
        line = ser.readline()
        if line:
            out.append(line.decode(errors="replace").rstrip())
            end = time.time() + 0.5  # perpanjang selama masih ada data
    return "\n".join(out)


def send(ser: serial.Serial, key: str) -> None:
    ser.write(key.encode())
    print(read_serial(ser, 3.0))


def main() -> None:
    p = argparse.ArgumentParser(description="0xJam host controller")
    p.add_argument("--port", required=True)
    p.add_argument("--fw", choices=["jammer", "s3"], required=True)
    p.add_argument("--cmd", help="kirim satu perintah lalu keluar")
    p.add_argument("--baud", type=int, default=115200)
    args = p.parse_args()

    keys = JAMMER_KEYS if args.fw == "jammer" else S3_KEYS
    ser = open_port(args.port, args.baud)
    time.sleep(2)  # tunggu board reset saat port dibuka
    read_serial(ser, 1)  # buang banner boot

    if args.cmd:
        if args.cmd not in keys:
            sys.exit(f"[!] perintah '{args.cmd}' tidak dikenal. Pilihan: {list(keys)}")
        send(ser, args.cmd)
        return

    print(f"0xJam ({args.fw}) @ {args.port}. Perintah: " +
          ", ".join(f"{k}={v}" for k, v in keys.items()) + " | q=keluar")
    while True:
        c = input("> ").strip().lower()
        if c == "q":
            break
        if c in keys:
            send(ser, c)
        else:
            print(f"[!] tidak dikenal. Pilihan: {list(keys)}")


if __name__ == "__main__":
    main()
