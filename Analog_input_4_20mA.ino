/*
  Machine Control - Analog in 4 - 20 mA Example

  This example provides the current value acquired by the
  Machine Control. For each channel of the ANALOG IN
  connector, there is a 120 ohm resistor to GND. The current
  of the 4-20mA sensor flows through it, generating a voltage
  which is sampled by the Portenta's ADC.
  To use the 4-20mA functionality, a 24V supply on
  the PWR SUPPLY connector is necessary.

  The circuit:
   - Portenta H7
   - Machine Control

  This example code is in the public domain.
*/
#include <Arduino_MachineControl.h>
#include <EthernetInterface.h>
#include <SPI.h>
#include <PortentaEthernet.h>
#include <Ethernet.h>

// The IP address will be dependent on your local network:
IPAddress ip(10, 146, 97, 77);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);
using namespace machinecontrol;

#define SENSE_RES 1000

float reference = 24.0;

void setup() {
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Ethernet WebServer Example");

  // start the Ethernet connection and the server:
  Ethernet.begin(ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start the server
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  analogReadResolution(16);

  Serial.begin(9600);
  analog_in.set4_20mA();
}

void loop() {
  float raw_voltage_ch0 = analog_in.read(0);
  float voltage_ch0 = (raw_voltage_ch0 * reference) / 65535;
  float current_ch0 = (voltage_ch0 / SENSE_RES) * 1000;
  float temperature = (((current_ch0) -4 )/16)*1000;
  Serial.print("Measured Current CH0: ");
  Serial.print(current_ch0);
  Serial.println("mA");

  float raw_voltage_ch1 = analog_in.read(1);
  float voltage_ch1 = (raw_voltage_ch1 * reference) / 65535;
  float current_ch1 = (voltage_ch1 / SENSE_RES) * 1000;
 
  Serial.print("Measured Current CH1: ");
  Serial.print(current_ch1);
  Serial.println("mA");
  float raw_voltage_ch2 = analog_in.read(2);
  float voltage_ch2 = (raw_voltage_ch2 * reference) / 65535;
  float current_ch2 = (voltage_ch2 / SENSE_RES) * 1000;
  Serial.print("Measured Current CH2: ");
  Serial.print(current_ch2);
  Serial.println("mA");

  Serial.println();
  delay(250);
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        client.print("Current Value is ");
        client.print(current_ch0);
        client.println(" mA");
        client.print("TC temperature is ");
        client.print(temperature);
        client.println("deg C"); 
        
        break;
          
       if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
        }
        
        
        
      }

    }
    
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}
