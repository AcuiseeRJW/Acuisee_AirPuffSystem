/*
09/09/2019   rjw  original shell
09/10/2019   rjw  turn pump off before air solenoid(s) close to ensure air purge
 */

// add pump before solenoid end
// add max timer to limit the total time animal gets air 

#define       MAXrunTime   12000    // maximum time to air puff animal
#define       surgeDelay   100      // slight delay btw solenoids to supress surge currents


const int     ledPin     = 13;    // onboard LED
const int     controlPin =  7;    // control pin to pump control Arduino
const int     left       =  9;    // solenoid 1
const int     right      = 10;    // solenoid 2
const int     vent       = 11;    // solenoid 3 - used for extra or vent or gauge
const int     pump       =  8;    // solenoid for air pump

unsigned long runTime           = millis();
bool          disableSolenoids  = false;
int           reading;
int           lastreading;

void setup() {
  pinMode(controlPin, INPUT);
  pinMode(left , OUTPUT);   digitalWrite( left,  LOW);
  pinMode(right, OUTPUT);   digitalWrite( right, LOW);
  pinMode(vent , OUTPUT);   digitalWrite( vent,  LOW);
  pinMode(pump , OUTPUT);   digitalWrite( pump,  LOW);
  pinMode(ledPin, OUTPUT);  digitalWrite(ledPin, LOW);
  reading     = digitalRead(controlPin);
  lastreading = reading;

  Serial.begin(9600);             // serial comms
  delay(3000);                    // wait to start
  Serial.println("AIR PUFF SYSTEM . . . . . SLAVE ");
  Serial.println("Version 1.2.1 ");
  Serial.print("ControlPin IS D"); Serial.println(controlPin);
  Serial.print("MAXrunTime is (in millisecs) : "); Serial.println(MAXrunTime);
} // of setup

void loop() {

  // read the signal from the master MCU
  reading = digitalRead(controlPin);

  // code below runs once when controlPin changes state ************************************************************************
  if (reading != lastreading) {  
    lastreading = reading;
    
    // turn everything on or off based on control input from master MCU  
    // IF LOW
    if ((reading == LOW)) {
      Serial.println("Input signal transition to LOW");
      disableSolenoids = false;
      digitalWrite(ledPin, LOW);
      digitalWrite(  pump, LOW); delay(surgeDelay);
      digitalWrite(  left, LOW); delay(surgeDelay);
      digitalWrite( right, LOW); delay(surgeDelay);
    }    
    // IF HIGH
    if (reading == HIGH) {
    Serial.println("Input signal transition to HIGH");  
    runTime  = millis();
    disableSolenoids = false;
    digitalWrite(ledPin, HIGH);
    digitalWrite(  left, HIGH); delay(surgeDelay);  
    digitalWrite( right, HIGH); delay(surgeDelay);  
    digitalWrite(  pump, HIGH); delay(surgeDelay);    
    } 
  } 
  // code above runs once when controlPin changes state ************************************************************************

 // check if system has been blowing air for more than "MAXrunTime" seconds AND DISABLE IF TRUE
 // DON'T WANT TO HARM ANIMAL
  if (((millis() - runTime) > MAXrunTime) 
         && !(disableSolenoids) 
             && (reading == HIGH)){
    disableSolenoids = true;
    Serial.println("Disable solenoids due to timeout");
  }
  
  if ((disableSolenoids)) {
      digitalWrite(ledPin, LOW);
      digitalWrite(  pump, LOW);  delay(surgeDelay);
      digitalWrite(  left, LOW);  delay(surgeDelay);
      digitalWrite(  right, LOW); delay(surgeDelay);
  } 
} // of loop
