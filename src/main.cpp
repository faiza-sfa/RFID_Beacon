#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 5                  // Slave Select Pin
#define RST_PIN 4                 // Reset Pin
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

// Define the custom payload
uint8_t customPayload[24];

// String customPayload[24];

// std::string customPayload[24];

void setup()
{
  Serial.begin(9600);
  // Initialize BLE
  // Convert the custom payload byte array to a string
  while (!Serial)
    ;                 // Wait for serial port to connect
  SPI.begin();        // Initialize SPI communication
  mfrc522.PCD_Init(); // Initialize MFRC522 RFID reader
  
  Serial.println("Ready to read RFID card...");


  BLEDevice::init("RFID_Beacon");
}

void loop()
{
  // Nothing to do here

  if (mfrc522.PICC_IsNewCardPresent())
  {
    if (!mfrc522.PICC_ReadCardSerial())
    {
      Serial.println("Error reading card data!");
      return;
    }
    // Get UID of the card
    String uid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
      uid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      uid += String(mfrc522.uid.uidByte[i], HEX);
      customPayload[i] += mfrc522.uid.uidByte[i];
      
      Serial.println(customPayload[i], HEX);
    }
    // Create an advertising object
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();

    // Configure the advertising parameters
    // pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);

    // Set the Eddystone-UID data
    BLEAdvertisementData oAdvertisementData;
    oAdvertisementData.setFlags(0x04); // BLE general discoverable mode
    std::string customPayloadString(reinterpret_cast<char *>(customPayload), sizeof(customPayload));
    oAdvertisementData.setManufacturerData(customPayloadString);
    std::string test = "hello";
    oAdvertisementData.addData(test);

    // Set the advertisement data
    pAdvertising->setAdvertisementData(oAdvertisementData);

    // Start advertising
    pAdvertising->start();

    Serial.print("UID: ");
    // Serial.println(customPayloadString.c_str());
    // for(int i = 0; i < customPayloadString.size(); i++)
    // {
    //   Serial.print(customPayloadString[i]);
    // }
    Serial.println();

    mfrc522.PICC_HaltA();      // Halt PICC
    mfrc522.PCD_StopCrypto1(); // Stop encryption on PCD
  }
}
