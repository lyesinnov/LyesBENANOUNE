
#include <Arduino_MachineControl.h>
// PID Constants
float KP = 1.0; 
float KI = 0.1;
float KD = 0.01; 
 
// Variables 
 
float setpoint = 0.0; 
float input = 0.0; 
float output = 0.0;
float error = 0.0; 
float prev_error = 0.0; 
float integral = 0.0; 
float derivative = 0.0; 
int prev_time = 0; 
int current_time = 0; 
 

using namespace machinecontrol;

void setup() {
  Serial.begin(9600);
  
  //analog_out.period_ms(CHANNEL, PERIOD_MILLISECONDS);
  analog_out.period_ms(0, 4);
  analog_out.period_ms(1, 4);
  analog_out.period_ms(2, 4);
  analog_out.period_ms(3, 4);
  
  Serial.begin(9600);
  Serial.println("Analog out test");
  // Communication 
   
  analogReadResolution(16);

  Serial.begin(9600);
  analog_in.set4_20mA();
   

   
  void updateControl(float output) { 
  // code to update machine control using output value 
   
  
}

void loop() {
  int counter;
  float raw_voltage_ch0 = analog_in.read(0);
  float voltage_ch0 = (raw_voltage_ch0 * reference) / 65535;
  float current_ch0 = (voltage_ch0 / SENSE_RES) * 1000;
  float temperature = (((current_ch0) -4 )/16)*1000; 
  //Set CH0, has internal 150 ms delay
  temp_probes.selectChannel(0);
  //Take CH0 measurement
  Serial.print("Temperature CH0 [°C]: ");
  Serial.print(temp_ch0);
  Serial.println();

  //Set CH1, has internal 150 ms delay
  temp_probes.selectChannel(1);
  //Take CH1 measurement
  float temp_ch1 = temp_probes.tc.readTemperature();
  Serial.print("Temperature CH1 [°C]: ");
  Serial.print(temp_ch1);
  Serial.println();
  //analog_out.write(CHANNEL, OUTPUT_VOLTAGE_VALUE);
  analog_out.write(0, counter);
  analog_out.write(1, counter);
  analog_out.write(2, counter);
  analog_out.write(3, counter);
  Serial.println("All channels set at "+String(counter)+"V");
  
  counter = counter + 0.1;
  //Maximum output value is 10.4V
  if (counter >= 10.5){
   
    counter = 0;
    //Additional 100 ms delay introduced to manage 10.5V -> 0V fall time of 150 ms
    delay(100);
  }
  delay(100);
  // Read Input 
  Input = float temp_ch0; 
   
  // Calculate error 
  error = setpoint - input; 
   
  // Calculate integral and derivative 
  current_time = millis(); 
  float dt = (current_time - prev_time )/ 1000.0; 
  integral = error * dt; 
  derivative = (error - prev_error)/ dt; 
   
  // Calculate Output 
  output = KP * error + KI * integral + KD * derivative; 
   
  // Update machine control
   
  updateControl(output); 
   
 // Print values if we have monitor 
 Serial.print("KP = ");
 Serial.print(KP);
 Serial.print(" / KI = ");
 Serial.print(KI);
 Serial.print( / KD = );
 Serial.println(KD);
 Serial.print("Setpoint: ");
 Serial.println(setpoint);  
 Serial.print("Input: "); 
 Serial.println(temp_ch0);
 Serial.print("Output: ");
 Serial.print(output: );
   
 // update previous values 
 prev_time = current_time; 
  prev_error = error; 
   
  float readSensor() { 
  // code to read machine sensor ad return value 
  return 0.0; 
 
  
}



