# 0xJam

WiFi + Bluetooth jammer & multitool berbasis ESP32 (inspirasi Flipper Zero).

## Isi

| Folder | Board | Fitur |
|---|---|---|
| `esp32_jammer` | ESP32 classic (WROOM-32D) | WiFi deauth + BT classic inquiry spam |
| `esp32s3_multitool` | ESP32-S3 | BadUSB, U2F (stub), BLE spam, IR, NFC (PN532), Sub-GHz (CC1101), iButton, GPIO |

## Wiring (S3)

- PN532 (NFC): I2C SDA=GPIO21, SCL=GPIO22
- CC1101 (Sub-GHz): SPI SCK=GPIO13, MISO=GPIO12, MOSI=GPIO11, CS=GPIO10
- IR LED: GPIO4
- iButton (OneWire DS1990A): GPIO15

## Libs

- `IRremoteESP8266`
- `OneWire`
- `Adafruit PN532`
- `SmartRC-CC1101-Driver-Lib`

Board S3: aktifkan USB CDC on boot.

## Kontrol Serial (S3, 115200)

Kirim huruf: `b`=BadUSB, `u`=U2F, `l`=BLE spam, `i`=IR, `n`=NFC, `s`=Sub-GHz, `w`=iButton, `g`=GPIO.

## Legal

Jamming RF butuh izin. Gunakan hanya pada perangkat sendiri/yang diberi izin.
