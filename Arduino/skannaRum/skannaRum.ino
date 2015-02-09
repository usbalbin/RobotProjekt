// ---------------------------------------------------------------------------
// This example shows how to use NewPing's ping_timer method which uses the Timer2 interrupt to get the
// ping time. The advantage of using this method over the standard ping method is that it permits a more
// event-driven sketch which allows you to appear to do two things at once. An example would be to ping
// an ultrasonic sensor for a possible collision while at the same time navigating. This allows a
// properly developed sketch to multitask. Be aware that because the ping_timer method uses Timer2,
// other features or libraries that also use Timer2 would be effected. For example, the PWM function on
// pins 3 & 11 on Arduino Uno (pins 9 and 11 on Arduino Mega) and the Tone library. Note, only the PWM
// functionality of the pins is lost (as they use Timer2 to do PWM), the pins are still available to use.
// NOTE: For Teensy/Leonardo (ATmega32U4) the library uses Timer4 instead of Timer2.
// ---------------------------------------------------------------------------
#include <NewPing.h>
#include <Servo.h> 

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on ping sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on ping sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

Servo myservo;  // create servo object to control a servo 
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

const int yMax=12;
const int xMax=yMax*2-1;
boolean karta[xMax][yMax];




int potpin = 0;  // analog pin used to connect the potentiometer
int vinkel=0;    // Vinkel 
unsigned int pingSpeed = 50; // How frequently are we going to send out a ping (in milliseconds). 50ms would be 20 times a second.
unsigned long pingTimer;     // Holds the next ping time.
unsigned int servoSpeed = 50; // Hur ofta vrida servo
unsigned long servoTimer;     // Holds the next servovridtid.
boolean servoKlar=true;
boolean pingKlar=false;
int vinkelIntervall=1;
float avst;
int x=0,y=0;
int var=0;
int runsBeforeCheck=0;

void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  pingTimer = millis(); // Start now.
  for(int y =0; y<yMax;y++){
    for(int x =0; x<xMax;x++){
      karta[x][y]=false;
    }
  }
  delay(1500);
}

void loop() {
 // if(var>10){
    
 //   var=0;
 // }else{
 //   var++;
 // }
  if(pingKlar&&millis() >= servoTimer){
    myservo.write(vinkel+90);                  // sets the servo position according to the scaled value
    pingTimer += pingSpeed;      // Set the next ping time.
    servoKlar=true;
    pingKlar=false;
    if(vinkel>=90) vinkelIntervall*=-1;
    else if(vinkel<=-90) vinkelIntervall*=-1;
    vinkel+=vinkelIntervall;     // scale it to use it with the servo (value between 0 and 180)
  }
  if (servoKlar&&millis() >= pingTimer) {   // pingSpeed milliseconds since last ping, do another ping.
    //sonar.ping_timer(echoCheck); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status.
    avst=random(1,yMax);
    x=(float)avst*sin((((float)vinkel * 71) / 4068));
    y=(float)avst*cos((((float)vinkel * 71) / 4068));
    karta[x+(xMax/2)][y]=true;
    printMap();
    servoTimer += servoSpeed;
    pingKlar=true;
    servoKlar=false;
  }
  
  
  
  // Do other stuff here, really. Think of it as multi-tasking.
}

void echoCheck() { // Timer2 interrupt calls this function every 24uS where you can check the ping status.
  // Don't do anything here!
  if (sonar.check_timer()) { // This is how you check to see if the ping was received.
    // Here's where you can add code.
    Serial.print("Ping: ");
    Serial.print(sonar.ping_result / US_ROUNDTRIP_CM); // Ping returned, uS result in ping_result, convert to cm with US_ROUNDTRIP_CM.
    Serial.println("cm");
    avst = sonar.ping_result / US_ROUNDTRIP_CM;
  }
  // Don't do anything here!
}

void printMap(){
  if(runsBeforeCheck>90){
    for(int x =0; x<xMax;x++){
      Serial.print("\t");
      Serial.print(x-(xMax/2));
    }
    Serial.println();
    Serial.print("\t|");
    for(int x =0; x<xMax*8-9;x++){
      Serial.print("-");
    }
    Serial.println("|");
    for(int y = yMax-1; y>=0;y--){
      for(int x =0; x<xMax;x++){
        Serial.print("\t|");
      }
      Serial.println();
      Serial.print(y);
      for(int x =0; x<xMax;x++){
        if(karta[x][y]){
          Serial.print("\t|   x");
        }
        else{
          Serial.print("\t|");
        }
      }
      Serial.println(y);
      for(int x =0; x<xMax;x++){
        Serial.print("\t|");
      }
      Serial.println();
      Serial.print("\t|");
      for(int x =0; x<xMax*8-9;x++){
        Serial.print("-");
      }
      Serial.println("|");
      
    }
    for(int x =0; x<xMax;x++){
      Serial.print("\t");
      Serial.print(x-(xMax/2));
    }
    Serial.println();
    Serial.print("Vinkel: ");
    Serial.print(vinkel);
    Serial.print(" Avst: ");
    Serial.print(avst);
    Serial.print(" Pos: ");
    Serial.print(x);
    Serial.print(";");
    Serial.println(y);
    runsBeforeCheck=0;
  }
  runsBeforeCheck++;
}
