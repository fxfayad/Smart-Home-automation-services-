#define BLYNK_TEMPLATE_ID "TMPL6xc1GrXnG"
#define BLYNK_TEMPLATE_NAME "Home automation"
#define BLYNK_AUTH_TOKEN "xDDsyDbr-RtIrTEXs5pMjesmLjjYoRK6"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <ESP32Servo.h>

// Wi-Fi credentials
char auth[] = "xDDsyDbr-RtIrTEXs5pMjesmLjjYoRK6";
char ssid[] = "Tor abbu k bol Wifi lagate";
char pass[] = "Amrabacelor5";

// Sensor/Actuator pins
#define DHTPIN 4
#define DHTTYPE DHT11
#define smokePin 35
#define rainPin 32
#define flamePin 19
#define TRIGPIN 5
#define ECHOPIN 18
#define servoPin 23
#define buzzerPin 13   

DHT dht(DHTPIN, DHTTYPE);
Servo myservo;

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);

  dht.begin();
  myservo.attach(servoPin);
  myservo.write(0);

  pinMode(rainPin, INPUT);
  pinMode(flamePin, INPUT);
  pinMode(TRIGPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);

  pinMode(buzzerPin, OUTPUT);   
  digitalWrite(buzzerPin, LOW);
}

// Servo control from Blynk
BLYNK_WRITE(V3) {
  int angle = param.asInt();
  myservo.write(angle);
}

void loop() {
  Blynk.run();

  digitalWrite(buzzerPin, LOW);  //Reset buzzer each loop

  // Smoke
  int smokeVal = analogRead(smokePin);
  Blynk.virtualWrite(V0, smokeVal);
  if (smokeVal > 2000) {
    digitalWrite(buzzerPin, HIGH);
  }

  // Flame
  int flameVal = digitalRead(flamePin);
  Blynk.virtualWrite(V1, flameVal == LOW ? 255 : 0);
  if (flameVal == LOW) {
    digitalWrite(buzzerPin, HIGH);
  }

  // Rain + Servo mechanism
  int rainVal = digitalRead(rainPin);
  if (rainVal == LOW) {
    myservo.write(90);
    Blynk.virtualWrite(V2, 255);
    digitalWrite(buzzerPin, HIGH);
  } else {
    myservo.write(0);
    Blynk.virtualWrite(V2, 0);
  }

  // Temperature and Humidity
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  if (!isnan(temp)) {
    Blynk.virtualWrite(V4, temp);
    if (temp > 35) {   // optional alert
      digitalWrite(buzzerPin, HIGH);
    }
  }
  if (!isnan(hum)) {
    Blynk.virtualWrite(V5, hum);
  }

  // Ultrasonic
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGPIN, LOW);

  long duration = pulseIn(ECHOPIN, HIGH, 30000);
  long distance = duration * 0.034 / 2;

  if (distance > 0 && distance < 400) {
    Blynk.virtualWrite(V7, distance);

    if (distance < 5) {   // alert if object too close
      digitalWrite(buzzerPin, HIGH);
    }
  }

  delay(1000);
}