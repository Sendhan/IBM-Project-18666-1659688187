// C++ code
//
#include <Adafruit_LiquidCrystal.h>
#include <IRremote.h>
#include <Servo.h>

#define en1 10
#define in1 9
#define in2 8
#define button 7

int value = 0;

Adafruit_LiquidCrystal lcd_1(0);

int RECV_PIN = 11;

Servo s;

//global variable for controlling the devices connected with IR Remote
bool flag_bulb1 = false;
bool flag_bulb2 = false;
bool pir_sensor = false;
bool window_open = false;
bool dc_motor_fan = false;

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{
  Serial.begin(9600);
  
  //for LCD
  lcd_1.begin(16, 2);
  lcd_1.print("Temperature:");
  
  //for IR sensor
  Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Enabled IRin");
  pinMode(7, OUTPUT);
  pinMode(2, OUTPUT);
  
  //For security alarm pizo buzzer
  pinMode(13, OUTPUT);
  pinMode(4, INPUT);
  
  pinMode(6, OUTPUT);
  
  //For DC Motor Fan
  pinMode(en1, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  
}

void loop()
{
  analogWrite(9, 1000);
  
  // pir module (for security alarm system)
  int a = digitalRead(4); 
  if(a){
    digitalWrite(13, HIGH);
  }
  
  // temperature module
  double t = analogRead(A0); //Analog in from Temperature Sensor at A0
  double temp = (((t/1024)*5)-0.5)*100; // analog to temperature conversion
  //Serial.println(temp);
  lcd_1.setCursor(0, 1);
  lcd_1.print(temp);
  
  // Gas Detector Module
  float gas = analogRead(A1);
  float gas_val = map(gas, 300, 750, 0, 100);
  if(gas_val > 50 and window_open == false){
    windowOpener();
    window_open = true;
  }
  
  //IR module
  if (irrecv.decode(&results)) {
    Serial.println(results.value);
    
    switch(results.value){
      
      case 16593103: //DC Motor Fan (Button 0)
       if(dc_motor_fan == false){
         dcMotorFanOn();
         dc_motor_fan = true;
       }
       else{
         dc_motor_fan = false;
         dcMotorFanOff();
       }
       break;
      
      case 16582903: //Light 1 (Button 1)
       if(flag_bulb1 == false){
         digitalWrite(7,HIGH); 
         flag_bulb1 = true;
       }
       else{
         digitalWrite(7,LOW); 
         flag_bulb1 = false;
       }
       break;
      
      case 16615543: //Light 2 (Button 2)
       if(flag_bulb2 == false){
         digitalWrite(2,HIGH); 
         flag_bulb2 = true;
       }
       else{
         digitalWrite(2,LOW); 
         flag_bulb2 = false;
       }
       break;
      
      case 16599223: //Window (Button 3)
       if(window_open){
         windowCloser();
         window_open = false;
       }
       else{
         windowOpener();
         window_open = true;
       }
       break;
    }
  
    irrecv.resume(); // Receive thxt value
  }
  
  
  
}

void initiateServo(){
  s.attach(3);
}

void endServo(){
  s.detach();
}

void windowOpener(){
  s.attach(3);
  s.write(0);
  delay(500);
  s.write(30);
  delay(500);
  s.write(75);
  delay(500);
  s.write(135);
  delay(500);
  s.write(180);
  delay(500);
  s.detach();
}

void windowCloser(){
  s.attach(3);
  s.write(0);
  delay(500);
  s.write(-30);
  delay(500);
  s.write(-75);
  delay(500);
  s.write(-135);
  delay(500);
  s.write(-180);
  delay(500);
  s.detach();
}

void dcMotorFanOn(){
  analogWrite(en1, 750);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
}

void dcMotorFanOff(){
  analogWrite(en1, 0);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}