const int ledPin = 13; // indicator led output pin
const int relayPin = 12; // relay output pin
const int buttonPin = 10; // button input pin
const int potPin = A0; // potentiometer input pin

const int minTime = 5; // minimum timer delay in seconds
const int maxTime = 300; // maximum timer delay in seconds. 5 minutes

int buttonState = LOW;
int potState = LOW;
bool relayState = false;
float relayOnStart = 0;
float relayOnEnd = 0;
float delayTime = 0;

void setup() {
  // Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  pinMode(potPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  // startup led sequence
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay(500);

  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay(100);
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay(500);

  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay(100);
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay(100);
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
}

void loop() {
  buttonState = digitalRead(buttonPin);

  // delay is activated when button is pressed
  if (buttonState == HIGH) {
    relayState = true;
  }

  if (relayState) {
    potState = analogRead(potPin);

    // relayOnStart 0 means we're starting up
    if (relayOnStart == 0) {
      // calculate the end time using millis
      relayOnStart = millis();
      relayOnEnd = map(potState, 1023, 0, minTime, maxTime) * 1000 + relayOnStart;
      // Serial.println((relayOnEnd - relayOnStart) / 1000);

      // drive the relay and led indicator on
      digitalWrite(ledPin, HIGH);
      digitalWrite(relayPin, HIGH);

    } else if (relayOnStart < relayOnEnd) {
      // delay continuation. simply count up towards the end times
      relayOnStart += millis() - relayOnStart;

    // relayOnStart is gt relayOnEnd means it's time for tubby byebye
    } else {
      // drive the relay and led indicator orff
      digitalWrite(ledPin, LOW);
      digitalWrite(relayPin, LOW);
      // Serial.println((relayOnEnd - relayOnStart) / 1000);
      relayOnEnd = 0;
      relayOnStart = 0;
      relayState = false;
    }
  }
}
