#include <SX126x.h>
#include <SPI.h>
#include <Wire.h>
#include <BH1750.h>

// LoRa SPI Pins
#define VSPI_MISO 11
#define VSPI_MOSI 10
#define VSPI_SCLK 9
#define VSPI_SS   2

#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
#define VSPI FSPI
#endif

// LoRa and SPI
SX126x LoRa;
SPIClass vspi = SPIClass(VSPI);

// Light and Gas Sensors
BH1750 lightMeter1(0x23); // Light sensor at I2C address 0x23
const int o2SensorPin = 7; // Gas sensor on GPIO7 (ADC1_CH0)


// Constants for Oxygen Sensor (ME2-O2-D20)
const float VRef = 3.3;  // ESP32 ADC Reference Voltage (0 - 3.3V)
const int ADCResolution = 1023;  // 12-bit ADC
uint8_t counter = 0;

void setup() {
  // Start Serial Communication
  Serial.begin(115200);

  // Initialize LoRa SPI Communication
  vspi.begin(VSPI_SCLK, VSPI_MISO, VSPI_MOSI);
  LoRa.setSPI(vspi, 16000000);

  Serial.println("Begin LoRa radio");
  int8_t nssPin = 8, resetPin = 12, busyPin = 13, irqPin = -1, txenPin = -1, rxenPin = -1;
  if (!LoRa.begin(nssPin, resetPin, busyPin, irqPin, txenPin, rxenPin)) {
      Serial.println("Something wrong, can't begin LoRa radio");
      while (1);
  }

  // Optionally configure TCXO or XTAL used in RF module
  // Different RF module can have different clock, so make sure clock source is configured correctly
  // uncomment code below to use TCXO
  Serial.println("Set RF module to use TCXO as clock reference");
  uint8_t dio3Voltage = SX126X_DIO3_OUTPUT_1_8;
  uint32_t tcxoDelay = SX126X_TCXO_DELAY_10;
  LoRa.setDio3TcxoCtrl(dio3Voltage, tcxoDelay);
  // uncomment code below to use XTAL
  //uint8_t xtalA = 0x12;
  //uint8_t xtalB = 0x12;
  //Serial.println("Set RF module to use XTAL as clock reference");
  //LoRa.setXtalCap(xtalA, xtalB);

  // Optionally configure DIO2 as RF switch control
  // This is usually used for a LoRa module without TXEN and RXEN pins
  LoRa.setDio2RfSwitch(true);
  // Set frequency to 915 Mhz
  Serial.println("Set frequency to 868 Mhz");
  LoRa.setFrequency(868000000);

  // Set TX power, default power for SX1262 and SX1268 are +22 dBm and for SX1261 is +14 dBm
  // This function will set PA config with optimal setting for requested TX power
  Serial.println("Set TX power to +17 dBm");
  LoRa.setTxPower(17, SX126X_TX_POWER_SX1262); 

  // Configure modulation parameter including spreading factor (SF), bandwidth (BW), and coding rate (CR)
  // Receiver must have same SF and BW setting with transmitter to be able to receive LoRa packet
  Serial.println("Set modulation parameters:\n\tSpreading factor = 7\n\tBandwidth = 125 kHz\n\tCoding rate = 4/5");
  uint8_t sf = 7;                                                     // LoRa spreading factor: 7
  uint32_t bw = 125000;                                               // Bandwidth: 125 kHz
  uint8_t cr = 5;                                                     // Coding rate: 4/5
  LoRa.setLoRaModulation(sf, bw, cr);

  // Configure packet parameter including header type, preamble length, payload length, and CRC type
  // The explicit packet includes header contain CR, number of byte, and CRC type
  // Receiver can receive packet with different CR and packet parameters in explicit header mode
  Serial.println("Set packet parameters:\n\tExplicit header type\n\tPreamble length = 12\n\tPayload Length = 15\n\tCRC on");
  uint8_t headerType = SX126X_HEADER_EXPLICIT;                        // Explicit header mode
  uint16_t preambleLength = 12;                                       // Set preamble length to 12
  uint8_t payloadLength = 15;                                         // Initialize payloadLength to 15
  bool crcType = true;                                                // Set CRC enable
  LoRa.setLoRaPacket(headerType, preambleLength, payloadLength, crcType);    
  
  // Set syncronize word for public network (0x3444)
  Serial.println("Set syncronize word to 0x3444");
  LoRa.setSyncWord(0x3444);

  Serial.println("\n-- LORA TRANSMITTER --\n");

  // Initialize I2C and Sensors
  Wire.begin(36, 0); // SDA = GPIO36, SCL = GPIO0
  analogReadResolution(10); // Set ADC resolution (0-4095)

  if (lightMeter1.begin()) {
      Serial.println("Sensor 1 initialized at 0x23");
  } else {
      Serial.println("Sensor 1 initialization failed!");
  }

  Serial.println("Oxygen Sensor Preheating... Wait 20-30 minutes for stable readings.");
}

void loop() {

  float lux = readLightLevel();
  float o2Concentration = readOxygenConcentration();

  printSensorData(lux, o2Concentration);
  sendLoRaData(lux, o2Concentration);

  delay(5000);
}

// Function to Read Light Sensor
float readLightLevel() {
  return lightMeter1.readLightLevel(); // Get Lux reading
}

// Function to Read Oxygen Sensor Voltage
float readO2Vout() {
  long sum = 0;
  for (int i = 0; i < 32; i++) {
      sum += analogRead(o2SensorPin);
  }
  sum >>= 5; // Divide sum by 32 (bit-shifting)
  Serial.print("ADC Value "); Serial.print(sum); Serial.println(" analog");
  return sum * (VRef / ADCResolution); // Convert ADC to voltage
}

// Function to Convert Sensor Voltage to Oxygen Concentration
float readOxygenConcentration() {
  float Vout = readO2Vout();
  float o2Concentration = (Vout * 0.21 / 2.0)*100; // Scale to % O₂

  return o2Concentration;
}

// Function to Print Sensor Data
void printSensorData(float lux, float o2Concentration) {
  Serial.print("Light: "); Serial.print(lux); Serial.println(" lux");
  Serial.print("Oxygen Concentration: "); Serial.print(o2Concentration); Serial.println(" %");
}

// Function to Send LoRa Data
void sendLoRaData(float lux, float o2Concentration) {
  LoRa.beginPacket();

  // Create the message as a String
  String dataString = "Light: " + String(lux, 2) + " lux, O2: " + String(o2Concentration, 2) + "%";

  // Convert String to char array
  uint8_t nBytes = dataString.length();
  char data[nBytes + 1]; // Add space for null-terminator
  dataString.toCharArray(data, nBytes + 1);

  // Send data over LoRa
  LoRa.write((uint8_t*)data, nBytes);
  LoRa.write(counter);

  LoRa.endPacket();
  // Print message and counter in serial
  Serial.print(data);
  Serial.print("  ");
  Serial.println(counter++);

  LoRa.wait();
  Serial.print("Transmit time: ");
  Serial.print(LoRa.transmitTime());
  Serial.println(" ms");
  Serial.println();

  Serial.println("Data Sent via LoRa!");
  // Don't load RF module with continous transmit
  delay(5000);
}

