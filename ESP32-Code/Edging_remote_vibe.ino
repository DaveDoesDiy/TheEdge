#include <Arduino.h>
#include <NimBLEDevice.h> //version 1.4.2 (h2zero)
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>


#include <Wire.h>
#include <Adafruit_BMP085.h>

//bmp180 vars
Adafruit_BMP085 bmp;
int oldpres=0,newpres=0;

// WiFi Credentials
const char *ssid = "";
const char *password = "";

// Web Server Port
const int webPort = 80;
int getmode=0;

unsigned long previousMillis = 0;
const unsigned long interval = 10000; // Interval in milliseconds
int currentLevel=0;

// Web Server Object
WebServer server(webPort);

static uint16_t companyId = 0xFFF0;

#define MANUFACTURER_DATA_PREFIX 0x6D, 0xB6, 0x43, 0xCE, 0x97, 0xFE, 0x42, 0x7C

// Updated list of manufacturer data options with both speed and vibration modes
uint8_t manufacturerDataList[][11] = {
    {MANUFACTURER_DATA_PREFIX, 0xE5, 0x00, 0x00}, // Off (E50000)
    {MANUFACTURER_DATA_PREFIX, 0xF4, 0x00, 0x00}, // Mode 1 (F40000)
    {MANUFACTURER_DATA_PREFIX, 0xF7, 0x00, 0x00}, // Mode 2 (F70000)
    {MANUFACTURER_DATA_PREFIX, 0xF6, 0x00, 0x00}, // Mode 3 (F60000)
    {MANUFACTURER_DATA_PREFIX, 0xF1, 0x00, 0x00}, // Mode 4 (F10000)
    {MANUFACTURER_DATA_PREFIX, 0xF3, 0x00, 0x00}, // Mode 5 (F30000)
    {MANUFACTURER_DATA_PREFIX, 0xE7, 0x00, 0x00}, // Mode 6 (E70000)
    {MANUFACTURER_DATA_PREFIX, 0xE6, 0x00, 0x00}, // Mode 7 (E60000)
    {MANUFACTURER_DATA_PREFIX, 0xE1, 0x00, 0x00}, // Vibration 1 (E10000)
    {MANUFACTURER_DATA_PREFIX, 0xD0, 0x00, 0x00}, // Vibration 2 (D00000)
    {MANUFACTURER_DATA_PREFIX, 0xD3, 0x00, 0x00}, // Vibration 3 (D30000)
    {MANUFACTURER_DATA_PREFIX, 0xE2, 0x00, 0x00}, // Vibration 4 (E20000)
    {MANUFACTURER_DATA_PREFIX, 0xED, 0x00, 0x00}, // Vibration 5 (ED0000)
    {MANUFACTURER_DATA_PREFIX, 0xDC, 0x00, 0x00}, // Vibration 6 (DC0000)
    {MANUFACTURER_DATA_PREFIX, 0xE9, 0x00, 0x00}  // Vibration 7 (E90000)
};

const char *deviceName = "MuSE_Advertiser";

volatile uint8_t currentManufacturerDataIndex = 0;
volatile bool modeChanged = false;

void advertiseManufacturerData(uint8_t index) {
  NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->stop();
  uint8_t *manufacturerData = manufacturerDataList[index];
  pAdvertising->setManufacturerData(std::string((char *)&companyId, 2) + std::string((char *)manufacturerData, 11));
  pAdvertising->setScanResponse(true);
  pAdvertising->setMaxPreferred(0x12);
  pAdvertising->setMinPreferred(0x02);
  pAdvertising->start();

  Serial.print("Advertising mode: ");
  Serial.println(index);
}

void connectWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}




void  setStop() {currentManufacturerDataIndex = 0; modeChanged = true; Serial.print("Mode changed to: 0"); }

void  setSpeedOne() {currentManufacturerDataIndex = 1; modeChanged = true; Serial.print("Mode changed to: 1"); }
void  setSpeedTwo() {currentManufacturerDataIndex = 2; modeChanged = true; Serial.print("Mode changed to: 2");  }
void  setSpeedThr() {currentManufacturerDataIndex = 3; modeChanged = true; Serial.print("Mode changed to: 3");  }
void  setSpeedFou() {currentManufacturerDataIndex = 4; modeChanged = true; Serial.print("Mode changed to: 4");  }
void  setSpeedFiv() {currentManufacturerDataIndex = 5; modeChanged = true; Serial.print("Mode changed to: 5");  }
void  setSpeedSix() {currentManufacturerDataIndex = 6; modeChanged = true; Serial.print("Mode changed to: 6");  }
void  setSpeedSev() {currentManufacturerDataIndex = 7; modeChanged = true; Serial.print("Mode changed to: 7");  }

void  setModeOne() {currentManufacturerDataIndex = 8; modeChanged = true; Serial.print("Mode changed to: 8");  }
void  setModeTwo() {currentManufacturerDataIndex = 9; modeChanged = true; Serial.print("Mode changed to: 9");  }
void  setModeThr() {currentManufacturerDataIndex = 10; modeChanged = true; Serial.print("Mode changed to: 10");  }
void  setModeFou() {currentManufacturerDataIndex = 11; modeChanged = true; Serial.print("Mode changed to: 11");  }
void  setModeFiv() {currentManufacturerDataIndex = 12; modeChanged = true; Serial.print("Mode changed to: 12");  }
void  setModeSix() {currentManufacturerDataIndex = 13; modeChanged = true; Serial.print("Mode changed to: 13");  }
void  setModeSev() {currentManufacturerDataIndex = 14; modeChanged = true; Serial.print("Mode changed to: 14");  }






void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
	  Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
  }

  connectWiFi();
  NimBLEDevice::init(deviceName);
  advertiseManufacturerData(currentManufacturerDataIndex);
  Serial.println("BLE advertising started.");

// Initialize mDNS
  if (!MDNS.begin("LSProCon")) {   // Set the hostname to "esp32.local"
    Serial.println("Error setting up MDNS responder!");
    while(1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  

}




void loop() 
{
    if (modeChanged) {
    modeChanged = false;
    advertiseManufacturerData(currentManufacturerDataIndex);
  }
  oldpres=newpres;
  newpres=bmp.readPressure();
  //Serial.println(bmp.readPressure());
  
    if ((newpres-100) > oldpres)
    {
      Serial.print("gotone");
      setStop();
      currentLevel=0;
    }
  
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    Serial.println("10secs");
      switch(currentLevel)
      {
        case 0:
        setSpeedOne();
        currentLevel=1;
        break;
        case 1:
        setSpeedTwo();
        currentLevel=2;
        break;
        case 2:
        setSpeedThr();
        currentLevel=3;
        break;  
        case 3:
        setSpeedFou();
        currentLevel=4;
        break;
        case 4:
        setSpeedFiv();
        currentLevel=5;
        break;
        case 5:
        setSpeedSix();
        currentLevel=6;
        break;
        case 6:
        setSpeedSev();
        currentLevel=7;
        break;
      }
    previousMillis = currentMillis;
  }
}