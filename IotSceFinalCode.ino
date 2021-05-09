#include <TinyGPS.h>
#include <SoftwareSerial.h>
SoftwareSerial SIM900(7, 8);//rx,tx

TinyGPS gps;  //Creates a new instance of the TinyGPS object

int trigPin_l = 13;
int trigPin_r = 11;
int trigPin_f = 12;
int forcePin = A0;
int echoPin_l=4;
int echoPin_r=2;
int echoPin_f=3;
#define kpa2atm 0.00986923267


// pin defs
int pressurePin = 0;

// variables
int val;
float pkPa; // pressure in kPa
float pAtm; // pressure in Atm

unsigned long time;
void setup() {
  // put your setup code here, to run once:
  Serial.begin (9600);
  SIM900.begin(9600);
  pinMode(trigPin_l, OUTPUT);
  pinMode(trigPin_f, OUTPUT);
  pinMode(trigPin_r, OUTPUT);
  pinMode(echoPin_l, INPUT);
  pinMode(echoPin_f, INPUT);
  pinMode(echoPin_r, INPUT);
  pinMode(forcePin, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  long time_l,time_r,time_f,dist_l,dist_r,dist_f;
  
  digitalWrite(trigPin_l, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPin_l, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_l, LOW); 
  time_l = pulseIn(echoPin_l, HIGH);
  dist_l = (time_l/2) / 29.1;
  if(dist_l<400)
  {
    Serial.println("WARNING!!!!!!!!!!!!!!!!!OBJECT AT LEFT SIDE IS TOO CLOSE ");
    //Serial.println(dist_l);    
  }

  
  digitalWrite(trigPin_f, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin_f, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_f, LOW);
  time_f = pulseIn(echoPin_f, HIGH);
  dist_f = (time_f/2) / 29.1;
  if(dist_f<400)
  {
      //Serial.println(dist_f);
      Serial.println("WARNING!!!!!!!!!!!!!!!!!OBJECT AT FRONT IS TOO CLOSE ");
  }

  
  digitalWrite(trigPin_r, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin_r, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_r, LOW);
  time_r = pulseIn(echoPin_r, HIGH);
  dist_r = (time_r/2) / 29.1;
  if(dist_r<400)
  {
    //Serial.println(dist_r);
    Serial.println("WARNING!!!!!!!!!!!!!!!!!OBJECT AT RIGHT SIDE IS TOO CLOSE ");
  }
  /////////CODE FOR GPS AND GSM MODULE///////////////

  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (Serial.available())
    {
      char c = Serial.read();
      //Serial.print(c);
      if (gps.encode(c)) 
        newData = true;  
    }
  }
  
  

  //////////////CODE FOR PRESSURE SENSOR////////////////
  /* get the pressure */
  val = analogRead(pressurePin);
  pkPa = ((float)val/(float)1023+0.095)/0.009;
  pAtm = kpa2atm*pkPa;
  Serial.print(pkPa);
  Serial.print("kPa ");
  Serial.print(pAtm);
  Serial.println("Atm ");
    
    
    if(pkPa > 80)
    {
      //sound=300;
      //tone(buzzer,sound);
      if (newData)      //If newData is true
      {
        float flat, flon;
        unsigned long age;
        gps.f_get_position(&flat, &flon); //, &age);   
        SIM900.print("AT+CMGF=1\r"); 
        delay(400);
        SIM900.println("AT + CMGS = \"+91xxxxxxxxxx\"");// recipient's mobile number with country code
        delay(300);
        SIM900.print("Latitude = ");
        SIM900.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
        SIM900.print(" Longitude = ");
        SIM900.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
        delay(200);
        SIM900.println((char)26); // End AT command with a ^Z, ASCII code 26
        delay(200);
        SIM900.println();
        }
    }
  }
