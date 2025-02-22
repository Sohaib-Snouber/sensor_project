# **LoRa Communication with ESP32**

This project demonstrates **wireless sensor data transmission** using **LoRa SX126x** modules and **ESP32** microcontrollers. It consists of two nodes:

- **`Sensors_TX.ino`** → **Sender Node**: Reads data from sensors and transmits it over LoRa.
- **`Sensors_RX.ino`** → **Receiver Node**: Receives LoRa packets and displays the received sensor data.

## 📡 **Project Overview**

The **Sender Node** collects data from:
- **BH1750 Light Sensor** (Lux measurement)
- **ME2-O2-D20 Oxygen Sensor** (O₂ concentration %)
- Transmits the data over **LoRa (868 MHz)**.

The **Receiver Node**:
- Listens for LoRa packets.
- Extracts and prints sensor data.
- Displays RSSI & SNR for **signal quality analysis**.

## 🚀 **Hardware Components**

1. **ESP32 Board** (x2)
2. **SX126x LoRa Module** (x2)
3. **BH1750 Light Sensor** (connected to sender)
4. **ME2-O2-D20 Oxygen Sensor** (connected to sender)
5. **Jumper Wires & Breadboard**

---

## ⚙️ **Circuit Connections**

### **Sender Node (ESP32 & Sensors)**
| Component       | ESP32 Pin  |
|---------------|-----------|
| **LoRa MISO**    | GPIO 11   |
| **LoRa MOSI**    | GPIO 10   |
| **LoRa SCLK**    | GPIO 9    |
| **LoRa NSS**     | GPIO 8    |
| **LoRa RESET**   | GPIO 12   |
| **LoRa BUSY**    | GPIO 13   |
| **BH1750 (SDA)** | GPIO 36   |
| **BH1750 (SCL)** | GPIO 0    |
| **O₂ Sensor**    | GPIO 7 (ADC1_CH0) |

### **Receiver Node (ESP32 & LoRa)**
| Component       | ESP32 Pin  |
|---------------|-----------|
| **LoRa MISO**    | GPIO 11   |
| **LoRa MOSI**    | GPIO 10   |
| **LoRa SCLK**    | GPIO 9    |
| **LoRa NSS**     | GPIO 8    |
| **LoRa RESET**   | GPIO 12   |
| **LoRa BUSY**    | GPIO 13   |

---

## 🛠 **Installation & Setup**

### 1️⃣ **Clone the Repository**
```sh
https://github.com/Sohaib-Snouber/sensor_project.git
```

### 2️⃣ **Install Dependencies**

Ensure you have installed the required **Arduino libraries**:

- **LoRaRF** (for SX126x communication)
- **BH1750 Light Sensor**

To install:
1. Open **Arduino IDE**.
2. Go to **Sketch → Include Library → Manage Libraries**.
3. Search and install:
   - `"LoRaRF"`
   - `"BH1750"`

### 3️⃣ **Upload the Code**
- Open **`Sensors_TX.ino`** → Upload to **Sender ESP32**.
- Open **`Sensors_RX.ino`** → Upload to **Receiver ESP32**.

---

## 📡 **Expected Serial Output**

### **Sender Output**
```
Begin LoRa radio
Oxygen Sensor Preheating... Wait 20-30 minutes for stable readings.
Light: 300.50 lux
Oxygen Concentration: 20.95 %
Data Sent via LoRa!
```

### **Receiver Output**
```
Light: 300.50 lux, O2: 20.95%
Packet status: RSSI = -40 dBm | SNR = 12.50 dB
```

---

## 📝 **License**
This project is open-source under the **MIT License**.

