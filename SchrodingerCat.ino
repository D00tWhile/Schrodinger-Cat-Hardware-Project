//Schrödinger's cat Group Project 

#include <EEPROM.h>

//Data
const unsigned int flagAddress = 0;
const unsigned int notFirst = 128;
byte value = 0;

String password = "";
String passwordMatch = "";

char passwordArray[5] = {'0', '0', '0', '0', '\0'};
char passwordRead[5] = {'0', '0', '0', '0', '\0'};
char matchArray[5] = {'0', '0', '0', '0', '\0'};

const unsigned int passwordAddress = 1;//Desired location of address

int matchCount = 0;

//const int buttonPin1 = 7;     // the number of the pushbutton pin
//int buttonState = 0;         // variable for reading the pushbutton status

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

#include <Wire.h>  // Comes with Arduino IDE
#include <LiquidCrystal_I2C.h>

//Liquid Crystal
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

int trigPin = A1;    //Trig - green Jumper
int echoPin = 12;    //Echo - yellow Jumper
float duration, distance, distanceCheck;

const int buttonPin = 2;     // the number of the pushbutton pin

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
long randNumber;
int catLife;
int check1;
int pos = 0;    // variable to store the servo position

void setup() {

  Serial.begin(9600);
  //Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(11, OUTPUT);//11=red
  pinMode(8, OUTPUT);//8=yellow
  pinMode(4, OUTPUT);//4=green

  myservo.attach(9);  // attaches the servo on pin 9 to the servo object

  pinMode(buttonPin, INPUT_PULLUP); // initialize the pushbutton pin as an input

  lcd.begin(16, 2);  // initialize the lcd for 16 chars 2 lines, turn on backlight
  lcd.backlight(); // finish with backlight on
  lcd.clear();
  unlock();//Unlock at start

  //EEPROM
  value = EEPROM.read(flagAddress);//Gets value @ first address

  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {//Check if it needs to be reset
    Serial.println("Reset option selected");

    newPassword();
    matchCheck();
  }
  else if (value == 128) { //Check if this is first time use or not
    Serial.println("Welcome back...");
    matchCheck();
  }
  else  //Check if this is first time use
  {
    Serial.println("This is the first time powered up...");

    newPassword();
    matchCheck();
  }

}

void loop() {

  buttonState = digitalRead(buttonPin);  // read the state of the pushbutton value
  delay(15);

  if (digitalRead(buttonPin) == HIGH) {//Button Pressed
    digitalWrite(8, LOW);
    lock();//If button pressed lock the box
    delay(15);//Wait 10 seconds

    check1 = doStuff();//Match the sensor input to unlock, green light
    if (check1 == 1) {//PASS
      delay(10);
      for ( int i = 0; i < 10; i++) {
        allBlink();
      }
      unlock();
      delay(15);//Wait 10 seconds
      theCat(); //Outcome cat
    }
    else {//FAIL
      digitalWrite(11, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(75);
      digitalWrite(11, LOW);
      delay(75);
      digitalWrite(11, HIGH);
      delay(75);
      digitalWrite(11, LOW);
      delay(75);
      digitalWrite(11, HIGH);
      delay(75);
      digitalWrite(11, LOW);
      delay(75);
    }

  }
  else {
    digitalWrite(8, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(10);
  }

  delay(10);

}

void theCat() {
  randNumber = random(1000);  //create random number
  if ((randNumber % 3 ) == 0) { //The outcome of the cat.
    lcd.clear(); //Clear the screen
    lcd.setCursor(0, 0);
    lcd.print("    DEAD");//output Dead
    lcd.setCursor(0, 1);
    lcd.print("     ...RIP");//output Dead
  }
  else {
    lcd.clear(); //Clear the screen
    lcd.setCursor(0, 0);
    lcd.print("     ALIVE");//output Dead
    lcd.setCursor(0, 1);
    lcd.print(" Meow Meow Meow");//output Dead
  }

}

void lock() {
  //Serial.println("lock");
  for (pos = 180; pos >= 85; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}

void unlock() {
  // Serial.println("unlock");
  for (pos = 85; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}


int doStuff() {
  //Solid Green light
  digitalWrite(4, HIGH);   // turn the green LED on while checking ok
  delay(10);
  lcd.clear(); //Clear the screen
  lcd.setCursor(0, 0);
  lcd.print("Do Stuff");//output Dead

  for (int i = 0; i < 10; i++) {
    distanceCheck = theDistance();
    // Serial.println(distanceCheck);
    delay(1000);
    lcd.clear();

    if (distanceCheck > 1 && distanceCheck < 7) {
      digitalWrite(4, LOW);
      delay(50);
      digitalWrite(4, HIGH);   // turn the green LED on while checking ok
      delay(50);
      digitalWrite(4, LOW);
      delay(50);
      digitalWrite(4, HIGH);   // turn the green LED on while checking ok
      delay(50);
      digitalWrite(4, LOW);
      delay(50);
      digitalWrite(4, HIGH);   // turn the green LED on while checking ok
      delay(50);
      digitalWrite(4, LOW);   // turn the green LED on while checking done
      delay(10);
      return 1;
    }
  }

  digitalWrite(4, LOW);   // turn the green LED on while checking done
  delay(10);
  return 0;
}

float theDistance() { // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);//read in duration

  distance = ( duration * .0066929 );//Convert to inches

  // NOTE: Cursor Position: (CHAR, LINE) start at 0
  lcd.setCursor(0, 0);
  lcd.print("in: ");//Outout in
  lcd.print(distance);

  //distance = ( duration * .017 );//Convert to cm
  //lcd.setCursor(0, 1);
  //lcd.print("cm:");//output cm
  //lcd.print(distance);

  //delay(250);
  //lcd.clear();
  delay(10);
  return distance;
}


void allBlink() { //Waiting lights

  digitalWrite(11, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(250);                       // wait for a second
  digitalWrite(11, LOW);    // turn the LED off by making the voltage LOW
  delay(250);                       // wait for a second
  digitalWrite(8, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(250);                       // wait for a second
  digitalWrite(8, LOW);    // turn the LED off by making the voltage LOW
  delay(250);                       // wait for a second
  digitalWrite(4, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(250);                       // wait for a second
  digitalWrite(4, LOW);    // turn the LED off by making the voltage LOW
  delay(250);                       // wait for a second

}

void newPassword() { //Gets the users new password

  Serial.println("Please enter a new 4 digit password, followed by Enter.");
  while (!Serial.available()) {
    // just loop, maybe delay a small amount
    delay(1);
  }

  while (Serial.available()) {
    password = Serial.readStringUntil('\n');//read in the string till endline
  }
  if (password != "") {
    password.toCharArray(passwordArray, 5);//string to char array

    for (int i = 0; i < 5; i++) {//Store the char array to the eeprom
      EEPROM.write((i + 1), passwordArray[i]);
    }
    Serial.println("Password has been saved to the EEPROM.");//print out the string
    EEPROM.update(flagAddress, 128);
  }
  else
    Serial.println("Error @ line 40.");
}

void matchCheck() {

  Serial.println("Please enter a 4 digit password, followed by Enter."); //print out the string

  while (!Serial.available()) {
    delay(1);// just loop, maybe delay a small amount
  }

  while (Serial.available()) { //take in a string passwordMatch
    passwordMatch = Serial.readStringUntil('\n');//read in the string till endline
  }
  if (passwordMatch != "") {//If there is confirmed data in then

    for (int i = 0; i < 5; i++) {//Store the eeprom to char array
      passwordRead[i] = EEPROM.read(i + 1);
    }
  }

  passwordMatch.toCharArray(matchArray, 5);//string to char array
  //compare matchArray to passwordRead

  for (int i = 0; i < sizeof(passwordRead); i++) {
    if (matchArray[i] != passwordRead[i]) {
      //if not match tell user to restart and try again.
      Serial.println("Incorrect password. Please restart Arduino and try again.");//print out the string
      Serial.println("To reset password hold down button at startup.");
      delay(500);
      exit(0);
    }
    else {//check if match with password

      matchCount ++;//increment counter

      if (matchCount == sizeof(passwordRead)) {

        Serial.println("Password match. Now entering Schrödinger's cat...");
        matchCount = 0;//reset counter
        loop();//if match go to loop()

      }
    }
  }
}


