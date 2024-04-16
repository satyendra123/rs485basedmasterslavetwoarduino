//EXAMPLE-1 this is the sender code. it sends the count data through rs485tottl. to check this whether it is working or not. what we do is we connect the rs485tousb to the rs485tottl A to A and B to B. and what we see is when i check from the docklight
// then we can see the data in the docklight software in decimal.
/*
  Welcome to JP Learning
*/
//sender.ino
#include <SoftwareSerial.h>
// GPIO Pins
byte TX_PIN = 7, RX_PIN = 8;
byte DE_RE_PIN = 16, LED_PIN = 2;

SoftwareSerial Soft_Serial(RX_PIN, TX_PIN);

long counter = 0;
void setup() {
  Serial.begin(115200);
  Soft_Serial.begin(9600);

  pinMode(DE_RE_PIN, OUTPUT);     //DE/RE Controling pin of RS-485
  digitalWrite(DE_RE_PIN, HIGH);  //DE/RE=HIGH Transmit Enabled
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  Serial.println("\n\nWelcome to JP Learning\n");
  Serial.println("Sender Start\n");
}
void loop() {
  counter++;
  Serial.println("counter = " + String(counter));
  Soft_Serial.write(counter);
  digitalWrite(LED_PIN, LOW);
  delay(100);
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
}
// now we connect the rs485tottl with arduino. and we connect both the rs485tottl module and check the receiver arduino. whether we receive the data into the arduino or not
//receiver.ino
/*
  Welcome to JP Learning
*/
#include <SoftwareSerial.h>

// GPIO Pins
byte TX_PIN = 4, RX_PIN = 5;
byte DE_RE_PIN = 16, LED_PIN = 2;

SoftwareSerial Soft_Serial(RX_PIN, TX_PIN);

void setup() {
  Serial.begin(115200);
  Soft_Serial.begin(9600);

  pinMode(DE_RE_PIN, OUTPUT);    //DE/RE Controling pin of RS-485
  digitalWrite(DE_RE_PIN, LOW);  //DE/RE=LOW Receive Enabled
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  Serial.println("\n\nWelcome to JP Learning\n");
  Serial.println("Receiver Start\n");
}
void loop() {
  //If Serial Data is available
  while (Soft_Serial.available() > 0) {
    Serial.println(Soft_Serial.read());
    digitalWrite(LED_PIN, LOW);
    delay(100);
    digitalWrite(LED_PIN, HIGH);
    Soft_Serial.flush();
  }
}
NOTE- remember one thing is we use the twisted pair wire to connect with the rs485tottl module so that noise will be removed.
  
//EXAMPLE-2 now we will use the master slave device. slave arduino will receive the temperature data and it will send this data to master arduino. 
  slave.ino
  
/*
  Welcome to JP Learning
*/
#include <SoftwareSerial.h>
#include <DHT.h>

// GPIO Pins
byte TX_PIN = 4, RX_PIN = 5;
byte DE_RE_PIN = 16, LED_PIN = 2;
byte DHT_PIN = 14;

SoftwareSerial Soft_Serial(RX_PIN, TX_PIN);

#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);
void setup() {
  Serial.begin(115200);
  Soft_Serial.begin(9600);
  dht.begin();

  pinMode(DE_RE_PIN, OUTPUT);    //DE/RE Controling pin of RS-485
  digitalWrite(DE_RE_PIN, LOW);  //DE/RE=LOW Receive Enabled
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);


  Serial.println("\n\nWelcome to JP Learning\n");
  Serial.println("Receiver Start\n");
}
void loop() {
  //If Serial Data is available
  while (Soft_Serial.available() > 0) {
    char req = Soft_Serial.read();
    Serial.println("\nreq = " + String(req));
    if (req == 'D') {
      Serial.println("Request Received for Data");
      digitalWrite(DE_RE_PIN, HIGH);  //DE/RE=HIGH Transmit Enabled

      float DHT_Temperature = dht.readTemperature();
      if (isnan(DHT_Temperature)) {
        Serial.println("Failed to read from DHT11 sensor!");
        delay(1000);
      } else {
        Serial.println("DHT Temperature: " + String(DHT_Temperature) + " °C");

        Soft_Serial.write(String(DHT_Temperature).c_str());

        digitalWrite(DE_RE_PIN, LOW);  //DE/RE=LOW Receive Enabled
      }
    }
    digitalWrite(LED_PIN, LOW);
    delay(100);
    digitalWrite(LED_PIN, HIGH);
    Soft_Serial.flush();
  }
}
// code master.ino
/*
  Welcome to JP Learning
*/
#include <SoftwareSerial.h>

// GPIO Pins
byte TX_PIN = 4, RX_PIN = 5;
byte DE_RE_PIN = 16, LED_PIN = 2;

SoftwareSerial Soft_Serial(RX_PIN, TX_PIN);

// Data Read Timeout
unsigned long CurrentMillis, PreviousMillis, DataReadTimeout = (unsigned long)1000 * 10;
void setup() {
  Serial.begin(115200);
  Soft_Serial.begin(9600);

  pinMode(DE_RE_PIN, OUTPUT);     //DE/RE Controling pin of RS-485
  digitalWrite(DE_RE_PIN, HIGH);  //DE/RE=HIGH Transmit Enabled
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  Serial.println("\n\nWelcome to JP Learning\n");
  Serial.println("Sender Start\n");
}
void loop() {
  // Sending Data Request
  digitalWrite(DE_RE_PIN, HIGH);  //DE/RE=HIGH Transmit Enabled
  char req = 'D';
  Soft_Serial.write(req);
  Serial.println("\nRequest Send for Data");

  // Receiving Data After Request
  digitalWrite(DE_RE_PIN, LOW);  //DE/RE=LOW Receive Enabled
  while (true) {
    if (Soft_Serial.available() > 0) {
      Serial.println("Request Data Received");
      String data = Soft_Serial.readString();
      Serial.println("data = " + data);

      digitalWrite(LED_PIN, LOW);
      delay(100);
      digitalWrite(LED_PIN, HIGH);
      PreviousMillis = millis();
      break;
    }

    CurrentMillis = millis();
    if (CurrentMillis - PreviousMillis > DataReadTimeout) {
      PreviousMillis = CurrentMillis;
      Serial.println("Request Data Timout");
      break;
    }
  }
  delay(1000);
}

//
