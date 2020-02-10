#include <avr/sleep.h>//this AVR library contains the methods that controls the sleep modes
#include <Password.h>//Library for Password
#include <Keypad.h>//Library used to access keypad


#define interruptPin 2 //Pin we are going to use to wake up the Arduino
#define Gled 3 //Green Led pin
#define Bled 4 //Blue led pin
#define Rled 5 //Red led pin
#define Relaypin 13//Relay pin
#define Odroidpin 14// Odroid pin for interrupt

Password password = Password( "221016" ); //Password setup
Password pass = Password( "1994" ); //reset password
char pass2[7] = "";

const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {7, 12, 11, 9};
byte colPins[COLS] = {8, 6, 10};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup() {
  Serial.begin(9600);
  //pinMode(LED_BUILTIN,OUTPUT);//We use the led on pin 13 to indecate when Arduino is Asleep
  pinMode(interruptPin, INPUT_PULLUP); //Set pin d2 to input using the buildin pullup resistor
  pinMode(Gled, OUTPUT);
  pinMode(Bled, OUTPUT); //Declare all 3 leds as outputs
  pinMode(Rled, OUTPUT);
  pinMode(Relaypin, OUTPUT); //Declare status of relay pin
  pinMode(Odroidpin, OUTPUT);
  //digitalWrite(LED_BUILTIN,HIGH);//turning LEDs on (just for testing when not a led in circuit)
  digitalWrite(Bled, HIGH);
  digitalWrite(Relaypin, HIGH);
  keypad.addEventListener(keypadEvent);
}
void Going_To_Sleep() {
  sleep_enable();//Enabling sleep mode
  attachInterrupt(0, wakeUp, LOW);//attaching an interrupt to pin d2
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);//Setting the sleep mode, in our case full sleep
  //digitalWrite(LED_BUILTIN,LOW);//turning LED off (just for testing when not a led in circuit)
  digitalWrite(Bled, LOW); //Turn off Blue LED
  digitalWrite(Odroidpin, HIGH); //Turn off Odroid
  delay(1000);
  digitalWrite(Odroidpin, LOW);
  digitalWrite(Relaypin, LOW);
  delay(100); //wait a second to allow the led to be turned off before going to sleep
  sleep_cpu(); //activating sleep mode
}
void wakeUp() {
  sleep_disable();//Disable sleep mode
  //digitalWrite(LED_BUILTIN,HIGH);//turning LED on (just for testing when not a led in circuit)
  digitalWrite(Bled, HIGH); //Turn on blue led
  delay(200);
  digitalWrite(Relaypin, HIGH);
  delay(500);
  Serial.println("Woke Up!!");
  detachInterrupt(0);

}
void loop() {
  int state = digitalRead(interruptPin);
  switch (state)
  {
    case 0:
      keypad.getKey();
      break;
    case 1:
      Serial.println("Going to sleep in 5 sec!");
      delay(1000);
      Serial.println("Going to sleep in 4 sec!");
      delay(1000);
      Serial.println("Going to sleep in 3 sec!");
      delay(1000);
      Serial.println("Going to sleep in 2 sec!");
      delay(1000);
      Serial.println("Going to sleep in 1 sec!");
      delay(1000);
      Serial.println("Going to sleep now!");
      Going_To_Sleep();
      break;
  }
}
void keypadEvent(KeypadEvent eKey) {
  switch (keypad.getState()) {
    case PRESSED:

      switch (eKey) {
        case '#': checkPassword(); delay(1); break;  //"Enter" key

        case '*': password.reset(); delay(1); break;  //"Reset" key

        default: password.append(eKey); delay (1);
      }
  }
}
void checkPassword() { //Checks to see if entered password is correct

  if (pass.evaluate()) { //if password is the reset password you can reset the main password
    digitalWrite(Gled, HIGH);
    digitalWrite(Rled, HIGH);
    for (byte i = 0; i < 7; i++) {
      char key = NO_KEY;
      while (key == NO_KEY || key == '*' || key == '#') key = keypad.getKey();
      pass2[i] = key;
      delay(15);
    }
    pass2[7] = '\0';
    password.set(pass2);
    delay(1);
    digitalWrite(Gled, LOW);
    digitalWrite(Rled, LOW);
    delay(5);
    password.reset();
    pass.reset();
  }
  else if (password.evaluate()) { //If it is, the relayPin is set Low for 2.5 seconds
    digitalWrite(Bled, LOW);// Turn off Blue led
    digitalWrite(Gled, HIGH); //turn on green led to show access granted
    digitalWrite(Relaypin, LOW);
    delay(2500);
    digitalWrite(Relaypin, HIGH);
    digitalWrite(Gled, LOW);
    digitalWrite(Bled, HIGH);  //Turning Blue LED on again
    password.reset();
    pass.reset();//Prevents the situation where after the correct code
    //is entered, the "Enter" key can be pressed and the relay
    //be activated again until the reset (or any other button)
    //is pressed.
  }
  else { //If it isn't, the chip prevents access.
    digitalWrite(Bled, LOW);
    digitalWrite(Rled, HIGH);
    password.reset();
    pass.reset(); //NOTE: If the wrong code is entered or the wrong button is
    //accidentally pressed, the reset button must still be pressed
    //before it will accept the correct sequence.
    delay(1000);
    digitalWrite(Rled, LOW);
    digitalWrite(Bled, HIGH);
  }
}
