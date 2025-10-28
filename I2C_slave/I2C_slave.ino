

#include <Wire.h>
#include <DHT.h>

#define DHTPIN 12
#define DHTTYPE DHT11
#define SLAVE_ADDR 9

DHT dht(DHTPIN, DHTTYPE);

const int leds[3] = {2, 3, 4};  // LED1, LED2, LED3
String dataToSend = "";

void setup() {
  Wire.begin(SLAVE_ADDR);
  Wire.onRequest(sendData);
  Wire.onReceive(receiveData);

  dht.begin();
  for (int i = 0; i < 3; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }

  Serial.begin(9600);
  Serial.println("UNO (Slave I2C) sẵn sàng!");
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) dataToSend = "Loi DHT11";
  else dataToSend = String(t, 1) + " " + String(h, 1);
  Serial.println(dataToSend);

  delay(2000);
}

void sendData() {
  Wire.write(dataToSend.c_str());
}

void receiveData(int howMany) {
  static String buffer = "";
  while (Wire.available()) {
    char c = Wire.read();
    if (c == '<') buffer = "";
    else if (c == '>') {
      //Serial.println("Nhan I2C: " + buffer);
      int sep = buffer.indexOf(':');
      if (sep != -1) {
        int ledNum = buffer.substring(0, sep).toInt();
        String state = buffer.substring(sep + 1);
        if (ledNum >= 1 && ledNum <= 3) {
          digitalWrite(leds[ledNum - 1], state == "ON" ? HIGH : LOW);
          //Serial.println("LED" + String(ledNum) + " -> " + state);
        }
      }
    } else buffer += c;
  }
}

