// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFiS3.h>
#include <ThingSpeak.h>
#include <Wire.h>

#define ESP32_ADDRESS 8 // I2C address of the ESP32 slave device

const int buzzer = 7; //Sets pin on Arduino for piezo
int baselineTemp = 28; //sets baseline temperature to set alert threshold
float timeSet = 0; //declares variable for use as internal timer
float UploadSet = 0; //declares variable to be used as a counter
float AlertSet = 0; //declares variable to be used as a counter
int alert = 0; //declares variable to be used as a flag for checking if the device is currently above temperature threshold
int AlertFlag = 0; //declares variable to be used as a flag for if temperature has exceeded the threshold by 5 degrees celcius
const char* ssid = ""; //WiFi network name
const char* password = ""; //Wifi network password
int Flag = 0; //Used as a flag for looping until slave device is connected

unsigned long Channel = ; //Channel ID of ThingSpeak channel
const char* APIKey = ""; //Write API key for ThingSpeak channel
WiFiClient client; //Variable for connecting to ThingSpeak channel

#define ONE_WIRE_BUS 2 //Data wire is plugged into port 2 on the Arduino

OneWire oneWire(ONE_WIRE_BUS); //Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)

DallasTemperature sensors(&oneWire); //Pass oneWire reference to Dallas Temperature. 

DeviceAddress insideThermometer; //arrays to hold device address

void setup(void)
{
  Wire.begin(); //Initialize Wire

  Serial.begin(9600); //Initialize Serial with 9600 baud
  WiFi.begin(ssid, password); //Initialize WiFi connectivity
  ThingSpeak.begin(client); //Initialize connectivity to ThingSpeak channel
  ThingSpeak.setField(3, 1); //Sets field to 3, to write numeric value 1
  ThingSpeak.writeFields(Channel, APIKey); //Write data to ThingSpeak channel with write API key
  pinMode(4, OUTPUT); //Initialize pin 4
  pinMode(8, OUTPUT); //Initialize pin 8
  pinMode(buzzer, OUTPUT); //Initialize pin 7
  sensors.begin(); //locate devices on the bus
  Serial.print(sensors.getDeviceCount(), DEC); //Output no. devices found to Serial monitor

  // report parasite power requirements
  Serial.print("Parasite power is: "); //Output string to Serial monitor
  if (sensors.isParasitePowerMode()) Serial.println("ON"); //Check if parasite power is on and if true, output string to Serial monitor
  else Serial.println("OFF"); //ELSE, output string to Serial monitor
  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0"); //If address cannot be found, output string to Serial monitor

  Serial.print("Device 0 Address: "); //Output device address to Serial monitor
  Serial.println(); //Output newline to Serial monitor

  sensors.setResolution(insideThermometer, 9); //Sets resolution of thermo sensor
 
  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(insideThermometer), DEC);  //Output sensor resolution to Serial monitor
  Serial.println();
}
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress); //Get current temperature from sensor
  float timeNow = millis(); //Stores the current time
  if(tempC == DEVICE_DISCONNECTED_C) //If temperature data cannot be found
  {
    Serial.println("Error: Could not read temperature data"); //Output string to Serial monitor
    return;
  } 
  if (tempC < baselineTemp) { //If current temperature is less than temp threshold
    alert = 0; //Set alert flag to 0 (no alert)
    AlertFlag = 0; //Set alertFlag to 0 (no alert shutdown)
    digitalWrite(4, LOW); //Turn off red LED
    noTone(buzzer); //Turn off piezo
    digitalWrite(8, HIGH); //Turn on Green LED
  }
  if (tempC >= baselineTemp) { //If current temperature is greater than or equal to temp threshold
    if (AlertFlag == 0) { //If no alert shutdown
      if (alert == 0) { //If no alert already present
        int num = 1; //Sets numeric counter variable
        ThingSpeak.setField(2, num); //Sets ThingSpeak field to 2, to write variable "num"
        ThingSpeak.writeFields(Channel, APIKey); //Write data to channel with write API key
        delay(2000); //Wait 2 seconds before continuing
        alert = 1; //Set active alert so multiple writes to ThingSpeak aren't made for the same alert
      }
      if (tempC >= baselineTemp + 5) { //If current temperature is greater than or equal to temp threshold + 5 degrees celcius
        int num = 1; //Sets numeric counter variable
        ThingSpeak.setField(4, num); //Set ThingSpeak field to 4, and data to "num" variable
        ThingSpeak.writeFields(Channel, APIKey); //Write data to channel with write API key
        delay(2500); //Wait for 2.5 seconds before proceeding
        Serial.println("Temperature has exceeded the safety threshold. Initiating shutdown on audio device."); //Output string to Serial monitor
        Serial.println("Shutting down thermo-safety system."); //Output string to Serial monitor
        while (timeNow < timeNow + 5000ul) { //For the next 5 seconds, do this:
          Wire.beginTransmission(ESP32_ADDRESS);  // Begin transmission to ESP32
          byte dataToSend = 42;  // Data to send to ESP32
          Wire.write(dataToSend);  // Send the data
          Wire.endTransmission();  // End the transmission 
        }
        AlertFlag = 1; //Set alert shutdown flag to true
      }
    }
    digitalWrite(8, LOW); //Turn green LED off
    digitalWrite(4, HIGH); //Turn red LED on
    tone(buzzer, 1000); //Turn piezo on with frequency value
    delay(1000); //Wait for one second
    digitalWrite(4, LOW); //Turn off red LED
    noTone(buzzer); //Turn off piezo
    delay(1000); //Wait 1 second
  }

  if (timeNow - timeSet >= 5000ul) { //Don't process unless 5 seconds has passed
    Serial.print("Temp C: "); //Output string to Serial monitor
    Serial.print(tempC); //Output current temperature value to Serial monitor
    Serial.print('\n'); //New line in Serial monitor
    timeSet = timeNow; //reset variable timer
  }
  if (timeNow - UploadSet >= 20000ul) { //Don't process unless 20 seconds has passed
    ThingSpeak.setField(1, tempC); //Set ThingSpeak field to 1, with data as current temperature
    ThingSpeak.writeFields(Channel, APIKey); //Write data to ThingSpeak channel with write API key
    UploadSet = timeNow; //Update variable timer
  }
}
void loop(void) { 
  if (Flag == 0) { //If connection has not already been made
    Serial.print("Attempting Connection...\n"); //Output string to Serial monitor
    Wire.requestFrom(ESP32_ADDRESS, 5); // Request 5 bytes from ESP32
    String Characters = ""; //Declare string variable
    while (Wire.available()) { //While data is available from ESP32
      char c = Wire.read(); //Read each byte
      Characters.concat(c); //Concatenate each byte received into the string variable
    }
    if (Characters == "Hello") { //Verify bytes received is equal to expected string value.
      Serial.print("Starting Thermo-Safety System"); //Output string to Serial monitor
      Serial.println(); //Newline in Serial monitor
      Flag = 1; //Change variable flag to show the two devices are connected
    }
    delay(1000); //Wait 1 second
  }
  if (Flag == 1) { //If device is connected:
    sensors.requestTemperatures(); //Get current temperature data from sensor
    printTemperature(insideThermometer); //Call function with current temperature data
  }
}
