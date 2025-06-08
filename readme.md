# ESP32C3 IoT Servo & LED Web Control

A simple ESP32C3 project to control a servo and an LED via a responsive web UI.  
The ESP32C3 runs as a WiFi Access Point and serves a web page for real-time control and configuration.

## Features

- **WiFi AP Mode:** ESP32C3 creates its own WiFi network.
- **Responsive Web UI:** Mobile-friendly interface (HTML/CSS/JS in SPIFFS).
- **Manual Control:** Turn LED ON/OFF, move servo to ON/OFF positions.
- **Servo Position Settings:** Adjust and save ON/OFF angles.
- **Timer Function:** Enable/disable periodic toggling with adjustable interval.
- **Persistent Config:** Servo positions and timer interval saved in SPIFFS.
- **Dynamic Pin Display:** Web UI shows actual GPIO pins in use.

## Getting Started

### Hardware

- ESP32C3 development board
- Servo motor (connected to `servoPin`, default GPIO 4)
- LED with resistor (connected to `ledPin`, default GPIO 2)

### Software

- [PlatfomIO](https://platformio.org/) with ESP32 board support
- [ESP32FS Tool](https://github.com/me-no-dev/arduino-esp32fs-plugin) for uploading SPIFFS files

### Folder Structure

```
ESP32C3-IoT-tutorial/
├── data/
│   ├── index.html
│   ├── style.css
│   └── script.js
└── src/
    └── main.cpp
```

### Setup

1. **Clone this repository.**
2. **Open in IDE.**
3. **Upload SPIFFS files:**
   - Use the ESP32 Sketch Data Upload tool to upload the `data/` folder to SPIFFS.
4. **Upload the firmware:**
   - Compile and upload `src/main.cpp` to your ESP32C3 board.

### Usage

1. Power on the ESP32C3.  
2. Connect your device to the WiFi network:  
   - **SSID:** `ESP32C3-LED`  
   - **Password:** `12345678`
3. Open a browser and go to [http://192.168.4.1/](http://192.168.4.1/)
4. Use the web UI to control the LED and servo, adjust settings, and save configurations.

## Customization

- Change `ledPin` and `servoPin` in `main.cpp` to match your wiring.
- Edit `index.html`, `style.css`, or `script.js` in the `data/` folder to customize the UI.

## License

MIT License
