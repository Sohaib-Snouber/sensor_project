#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter1(0x23); // Sensor 1 with ADDR → GND
BH1750 lightMeter2(0x5C); // Sensor 2 with ADDR → VCC

void setup() {
    Serial.begin(115200);
    Wire.begin(21, 22); // SDA = GPIO21, SCL = GPIO22

    if (lightMeter1.begin()) {
        Serial.println("Sensor 1 initialized at 0x23");
    } else {
        Serial.println("Sensor 1 initialization failed!");
    }

    if (lightMeter2.begin()) {
        Serial.println("Sensor 2 initialized at 0x5C");
    } else {
        Serial.println("Sensor 2 initialization failed!");
    }
}

void loop() {
    float lux1 = lightMeter1.readLightLevel();
    float lux2 = lightMeter2.readLightLevel();

    Serial.print("Sensor 1 Light Level: ");
    Serial.print(lux1);
    Serial.println(" lux");

    Serial.print("Sensor 2 Light Level: ");
    Serial.print(lux2);
    Serial.println(" lux");

    delay(1000);
}
