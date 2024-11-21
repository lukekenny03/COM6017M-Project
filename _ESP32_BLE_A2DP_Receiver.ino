#include <btAudio.h>
#include <Wire.h>
#define ESP32_ADDRESS 8 // I2C address of this ESP32 device
btAudio audio = btAudio("Luke's ESP32"); //set the name of the ESP32 device

void setup() {
  Serial.begin(9600); //Initialize Serial on 9600 baud
  Wire.begin(ESP32_ADDRESS); // Initialize I2C as slave with defined address
  Wire.onRequest(sendData);   // Attach function to handle request for data
  Wire.onReceive(receiveData); //Attach function to handle receive of data  
  audio.begin(); // streams audio data to the ESP32 
 
 //  outputs the received data to an I2S DAC 
 int bck = 26;  //Output bit clock data to pin 26 on ESP32
 int ws = 27; //Output word select (left-right clock) to pin 27 on ESP32
 int dout = 25; //Output data to pin 25 on ESP32
 audio.I2S(bck, dout, ws); //Call function in btAudio library to send data to DAC
}

void loop() {

}
void sendData() {
  Wire.write((const uint8_t*)"Hello", 5); // Send response to master
}
void receiveData(int byteCount) {
  while (Wire.available()) {  // While there are bytes to read
    byte receivedByte = Wire.read();  // Read a byte from the master
    if (receivedByte == 42) { //Verify if byte received is equal to numeric value 42
      Serial.println("\nTemperature exceeded safety threshold. Shutting down..."); //Output string to Serial monitor
      abort(); //Abort all processes, restart device
    }
  }
}