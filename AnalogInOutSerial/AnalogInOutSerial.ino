/*

  The following is based off the code made by Tom Igoe created on Dec. 29th 2008 and modified on April 9th 2012:
  Analog input, analog output, serial output

  Reads an analog input pin, maps the result to a range from 0 to 255 and uses
  the result to set the pulse width modulation (PWM) of an output pin.
  Also prints the results to the Serial Monitor.

  The circuit:
  - potentiometer connected to analog pin 0.
    Center pin of the potentiometer goes to the analog pin.
    side pins of the potentiometer go to +5V and ground
  - LED connected from digital pin 9 to ground through 220 ohm resistor

  This example code is in the public domain.

  https://docs.arduino.cc/built-in-examples/analog/AnalogInOutSerial/
*/
#include <WiFi.h>

// change the ssid and password below with the wifi you're connecting to (make sure it's 2.4 GHz!)
// ssid is the username
const char *ssid = "username";
// password is the password of your WiFi
const char *password = "password";


// These constants won't change. They're used to give names to the pins used:
const int analogInPin = 38;  // Analog input pin that the potentiometer is attached to
const int analogOutPin = 5;  // Analog output pin that the LED is attached to

int sensorValue = 0;  // value read from the pot
int outputValue = 0;  // value output to the PWM (analog out)

NetworkServer server(80);
void getData(){
    sensorValue = analogRead(analogInPin);
    // map it to the range of the analog out:
    outputValue = map(sensorValue, 0, 1023, 0, 255);
    // client.println(outputValue);
}
void setup() {
  // initialize serial communications at 115200 bps:
    Serial.begin(115200);
    pinMode(5, OUTPUT);  // set the LED pin mode
    delay(10);

  // We start by connecting to a WiFi network

    Serial.print("\r\n");
    Serial.print("\r\n");
    Serial.print("Connecting to \r\n");
    Serial.print(ssid);
    Serial.print("\r\n");

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print("Not Connected\r\n");
  }
    int count = 0;
    while(count < 1000){
    Serial.print("WiFi connected.\r\n");
    Serial.print("IP address: \r\n");
    Serial.println(WiFi.localIP());
    count++;
    }
    server.begin();

}

void loop() {

  NetworkClient client = server.accept();  // listen for incoming clients
  // read the analog in value:
  sensorValue = analogRead(analogInPin);
  // map it to the range of the analog out:
  outputValue = map(sensorValue, 0, 1023, 0, 255);
  // change the analog out value:
  // analogWrite(analogOutPin, outputValue);
  // if (outputValue == 0){
    // digitalWrite(5, LOW); 
  // }
  // else if (outputValue == 160){
  // digitalWrite(5, HIGH); 
  // }
  // print the results to the Serial Monitor:
  Serial.print("sensor = ");
  Serial.print(sensorValue);
  Serial.print("\t output = ");
  Serial.println(outputValue);

  // wait 2 milliseconds before the next loop for the analog-to-digital
  // converter to settle after the last reading:
  delay(2);
  if (client) {                     // if you get a client,
    Serial.print("New Client.\r\n");  // print a message out the serial port
    String currentLine = "";        // make a String to hold incoming data from the client
    String request = "";

    while (client.connected()) {    // loop while the client's connected
      if (client.available()) {     // if there's bytes to read from the client,
        char c = client.read();     // read a byte, then
        Serial.write(c);            // print it out the serial monitor
        request += c;

        if (c == '\n') {            // if the byte is a newline character
          
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {

            if (request.indexOf("GET /data") >= 0) {
              sensorValue = analogRead(analogInPin);
              outputValue = map(sensorValue, 0, 1023, 0, 255);

              client.print("HTTP/1.1 200 OK\r\n");
              client.print("Content-type:text/plain\r\n");
              client.print("Connection: close\r\n");
              client.print("\r\n");
              client.print(String(outputValue));
            } else {

              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              client.print("HTTP/1.1 200 OK\r\n");
              client.print("Content-type:text/html \r\n");
              client.print("\r\n");

              // the content of the HTTP response follows the header:
              client.print("<!DOCTYPE html><html><head><title>Live Sensor</title></head><body>");
              client.print("<h1>Live Sensor Value</h1>");
              client.print("<div id='value'>Loading...</div>");
              client.print("<script>");
              client.print("setInterval(getData(){");
              client.print("fetch('/data').then(r=>r.text()).then(t=>document.getElementById('value').innerText = t);");
              client.print("}, 1000);");  // fetch every second
              client.print("</script>");
              client.print("</body></html>");
          
            }

            // The HTTP response ends with another blank line:
            client.print("\r\n");
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // // Check to see if the client request was "GET /H" or "GET /L":
        // if (currentLine.endsWith("GET /H")) {
        //   digitalWrite(5, HIGH);  // GET /H turns the LED on
        // // }
        // if (currentLine.endsWith("GET /L")) {
        // }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
