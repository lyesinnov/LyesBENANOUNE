
#include <EthernetInterface.h>
#include <SPI.h>
#include <PortentaEthernet.h>
#include <Ethernet.h>
#include <Arduino_MachineControl.h>
#define SENSE_RES 1000


// The IP address will be dependent on your local network:
IPAddress ip(10, 146, 97, 77);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);
using namespace machinecontrol;

#define SENSE_RES 1000

float reference = 24.0;
int counter;
float output;
// PID Constants
float KP = 1.0; 
float KI = 0.0;
float KD = 0.0; 
// Variables 
float setpoint = 400.0;   
float prev_error; 
float integral; 
float derivative; 
int prev_time; 
int current_time; 
float temp_ch0;
int years = 23;
int months = 2;
int days = 27;
int hours = 12;
int minutes = 38;
int seconds = 31;
 

using namespace machinecontrol;
void setup(){
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
  Serial.begin(9600);
  
  //analog_out.period_ms(CHANNEL, PERIOD_MILLISECONDS);
  analog_out.period_ms(0, 4);
  
  
  
  Serial.println("Analog out test");
  // Communication 
  analogReadResolution(16);
  analog_in.set4_20mA();
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect.
  }

  Serial.println("Initialization");
  if(!rtc_controller.begin()) {
    Serial.println("Initialization fail!");
  }
  Serial.println("Initialization Done!");

  // APIs to set date's fields: years, months, days, hours, minutes and seconds
  // The RTC time can be set as epoch, using one of the following two options:
  // - Calendar time: rtc_controller.setEpoch(years,  months,  days, hours, minutes, seconds);
  // - UTC time: rtc_controller.setEpoch(date_in_seconds);
  rtc_controller.setYears(years);
  rtc_controller.setMonths(months);
  rtc_controller.setDays(days);
  rtc_controller.setHours(hours);
  rtc_controller.setMinutes(minutes);
  rtc_controller.setSeconds(seconds);
  rtc_controller.setEpoch();
}



void loop() {

  
  float raw_voltage_ch0 = analog_in.read(0);
  float voltage_ch0 = (raw_voltage_ch0 * reference) / 65535;
  float current_ch0 = (voltage_ch0 / SENSE_RES) * 1000;
  float temperature = (((current_ch0) -4 )/16)*1000; 

  // APIs to get date's fields.
  Serial.print("Date: ");
  Serial.print(rtc_controller.getYears());
  Serial.print("/");
  Serial.print(rtc_controller.getMonths());
  Serial.print("/");
  Serial.print(rtc_controller.getDays());
  Serial.print(" - ");
  Serial.print(rtc_controller.getHours());
  Serial.print(":");
  Serial.print(rtc_controller.getMinutes());
  Serial.print(":");
  Serial.println(rtc_controller.getSeconds());
  time_t utc_time = time(NULL);
  Serial.print("Date as UTC time: ");
  Serial.println(utc_time);
  Serial.println();
  

  //Take CH0 measurement
  Serial.print("Temperature CH0 [°C]: ");
  Serial.print(temp_ch0);
  Serial.println();

  
  
  output = output + 0.1;
  //Maximum output value is 10.4V
 if (output >= 10.5){
   
   output = 0;
   //Additional 100 ms delay introduced to manage 10.5V -> 0V fall time of 150 ms
   
  }
  
  // Read Input 
  float input = temperature; 
   
  // Calculate error 
  float error = setpoint - input; 
   
  // Calculate integral and derivative 
  current_time = millis(); 
  float dt = (current_time - prev_time )/ 1000.0; 
  float integral = error * dt; 
  float derivative = (error - prev_error)/ dt; 
   
  // Calculate Output 
  float output = ((KP * error) + (KI * integral) +(KD * derivative))/100; 
  
  //analog_out.write(CHANNEL, OUTPUT_VOLTAGE_VALUE);
  analog_out.write(0, output);
 
  // Update machine control
   
  
 delay(5000); 
 // Print values if we have monitor 
 Serial.print("KP = ");
 Serial.print(KP);
 Serial.print(" / KI = ");
 Serial.print(KI);
 Serial.print(" / KD = ");
 Serial.println(KD);
 Serial.print("Setpoint: ");
 Serial.println(setpoint);  
 Serial.print("Error: ");
 Serial.println(error);  
 Serial.print("Input: "); 
 Serial.println(temp_ch0);
 Serial.print("Output: ");
 Serial.print(output);
   
 // update previous values 
 prev_time = current_time; 
  prev_error = error; 
  Serial.println();
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
        client.print("Current Value is ");
        client.print(current_ch0);
        client.println(" mA");
        client.print("TC temperature is ");
        client.print(temperature);
        client.println("deg C"); 
        client.print("KP = ");
        client.print(KP);
        client.print(" / KI = ");
        client.print(KI);
        client.print(" / KD = ");
        client.println(KD);
        client.print("Setpoint: ");
        client.println(setpoint); 
        client.print("Error: ");
        client.println(error); 
        client.print("Input: "); 
        client.println(temperature);
        client.print("Output: ");
        client.println(output);
        client.print("Date: ");
        client.print(rtc_controller.getYears());
        client.print("/");
        client.print(rtc_controller.getMonths());
        client.print("/");
        client.print(rtc_controller.getDays());
        client.print(" - ");
        client.print(rtc_controller.getHours());
        client.print(":");
        client.print(rtc_controller.getMinutes());
        client.print(":");
        client.println(rtc_controller.getSeconds());
        time_t utc_time = time(NULL);
        client.print("Date as UTC time: ");
        client.println(utc_time);
        
        
        return;
          
       
      } 
    }
  }    
}




