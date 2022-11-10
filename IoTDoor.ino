#include "DHT.h"
#include "SoftwareSerial.h"
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN   9   
#define SS_PIN    10

#define LED 3

SoftwareSerial btSerial(4, 5);


MFRC522 mfrc522(SS_PIN, RST_PIN);

String msg = ""; 

bool lockOn;

void setup( )
{
  Serial.begin( 9600);
  pinMode(LED, OUTPUT);
  Serial.print("Temperature and Humidity Monitoring Application\n\n");
  delay(1000);

  btSerial.begin(9600);  
  //btSerial.println("AT");
  //getBTReply();
  //btSerial.println("AT+ROLE0");
  //getBTReply();
  //btSerial.println("AT+NAMEtempSens");  
  //Serial.println("Check if name is assigned");
  //getBTReply();

  SPI.begin(); 
  mfrc522.PCD_Init();
  Serial.println("Lock initialised");

  lockOn = true;
}

void loop( )
{
  if(lockOn == true) {
    if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
      Serial.println("Application locked, waiting for RFID key to unlock");
    } else {
      if(checkRFID("BC 38 AA 39") || checkRFID("DO EF 81 A")) {
        Serial.println("RFID Tag detected");
        Serial.println("Application unlocked, performing monitoring");
        digitalWrite(LED, HIGH);
        lockOn=false;
      }
    }
  } else {
    if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
      if(btSerial.available()) {
        btSerial.print("Temperature: ");
        btSerial.println("");
        Serial.print(" Message sent on Bluetooth! ");
      }
    } else {
      if(checkRFID("BC 38 AA 39") || checkRFID("DO EF 81 A")) {
        Serial.println("RFID Tag detected");
        Serial.println("Application locked, stopping monitoring");
        digitalWrite(LED, LOW);
        lockOn=true;
      }
    }
  }

  delay(5000);
}

boolean checkRFID(String rightID) {
  Serial.print("Detected RFID with UUID = ");
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  content.toUpperCase();
  Serial.println(content);
  return (content.substring(1) == rightID);
}
