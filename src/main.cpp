//
// Template for Arduino Projects
//

#include <Arduino.h>
#include "PinsTTGO.h"
#include "BluetoothSerial.h"
#include "LoRa.h"

//433000000 for Asia
//866000000 for Europe
//915000000 for North America
#define LORA_BAND 866000000

BluetoothSerial SerialBT;

void onReceive(int packetSize) {

    if (packetSize) {

        //TODO: Crashing somewhere here !

        // received a packet
        if (SerialBT.available()) SerialBT.print("Received packet '");

        //read packet
        String LoRaData;
        while (LoRa.available()) {
            LoRaData = LoRa.readString();
            if(SerialBT.available()) SerialBT.print(LoRaData);
        }

        //print RSSI of packet
//        int rssi = LoRa.packetRssi();
//        float snr = LoRa.packetSnr();
//
//        // print RSSI of packet
//        if (SerialBT.available()) {
//            SerialBT.print("' with RSSI ");
//            SerialBT.println(rssi);
//            SerialBT.print(" and SNR ");
//            SerialBT.println(snr, 3);
//        }
    }
}

void receivePacket() {
    int packetSize = LoRa.parsePacket();
    onReceive(packetSize);
}

void initLoRa() {
    LoRa.setPins(TTGO_PIN_NSS, TTGO_PIN_RST, TTGO_PIN_DIO0); // set CS, reset, IRQ pin

    if (!LoRa.begin(LORA_BAND)) {         // initialize ratio at 915 MHz
        SerialBT.println("LoRa init failed. Check your connections.");
        while (true);                   // if failed, do nothing
    }

    // Sync Words will not receive each other's transmissions. This is one way you can filter out radios you want to ignore, without making an addressing scheme.
    //LoRa.setSyncWord(0xF3);           // ranges from 0-0xFF, default 0x34, see API docs

    // Spreading factor affects reliability of transmission at high rates, however, avoid a large spreading factor when you're sending continually.
    //LoRa.setSpreadingFactor(8);           // ranges from 6-12,default 7 see API docs
    LoRa.onReceive(onReceive);
    LoRa.receive();
    SerialBT.println("LoRa started.");
}

void setup() {
    Serial.begin(9600);
    SerialBT.begin("LoRaGate");
    Serial.println("Serial BT started. You can connect your client now.");
}

String message = "";


void parseAndHandleBT() {
    if (SerialBT.available()){
        char incomingChar = SerialBT.read();
        if (incomingChar != '\n'){
            message += String(incomingChar);
        }
        else{
            message = "";
        }
        Serial.write(incomingChar);
    }

    if(message.equalsIgnoreCase("lora on")) {
        initLoRa();
    }
    else if(message.equalsIgnoreCase("lora off")) {
        LoRa.end();
    }

}

void loop() {
    receivePacket();
    parseAndHandleBT();
}
