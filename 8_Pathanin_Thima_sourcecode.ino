#include <AntoIO.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int sensorPIN = A0;
int sensorValue = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2); // Change the I2C address and dimensions if different

const char* ssid = "Galaxy888"; //ชื่อwifi
const char* pass = "ylzz3835"; //รหัสwifi
const char* user = "Protile"; //cloudของ Anto
const char* token = "Zx19FXn9Xk8PpYUUEBX1XuyPgAfebCfXD31DvrM7"; //key จาก Anto
const char* thing = "DHT11"; //อุปกรณ์ที่ใช้
AntoIO anto(user, token, thing); //กำหนดตัวแปรให้ anto

void setup() {
  Serial.begin(115200);

  lcd.begin(); // Initialize the LCD with 16 columns and 2 rows
  lcd.backlight();
  lcd.home();
  lcd.print("Hello, world!");
  lcd.clear();

  delay(10);

  Serial.println();
  Serial.println();
  Serial.println("Anto library version: ");
  Serial.println(anto.getVersion());

  Serial.print("\nTrying to connect ");
  Serial.print(ssid);
  Serial.println("...");

  anto.begin(ssid, pass, messageReceived);
  Serial.println("\nConnected Anto done");

  pinMode(D7, OUTPUT);
  dht.begin();
}

void loop() {
  anto.mqtt.loop();
  if (!anto.mqtt.isConnected()) {
    Serial.println("Disconnected"); //เช็คถ้าไม่ได้Connect ให้แจ้งเตือน
  }

  sensorValue = digitalRead(analogRead(sensorPIN));

  float humid = dht.readHumidity();
  float temp = dht.readTemperature();

  if (isnan(humid) || isnan(temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  //LCD display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(temp);
  lcd.print("C ");
  lcd.print("LDR: ");
  lcd.setCursor(0, 1);
  lcd.print("Humid:");
  lcd.print(humid);
  lcd.print("%  ");
  
  lcd.print(sensorValue);


  //serial
  Serial.print(F("Humidity: "));
  Serial.print(humid);
  Serial.print(F("% Temperature: "));
  Serial.print(temp);
  Serial.print(F("C "));
  Serial.print("LDR: ");
  Serial.print(sensorValue);
  Serial.println();

  //Publish to Cloud
  if (temp >= 0) {
    anto.pub("Temp", temp);
  }
  if (humid >= 0) {
    anto.pub("Humid", humid);
  }
  if (sensorValue == 0) {
    anto.pub("LDR_1", "Light");
  }
  if (sensorValue == 1) {
    anto.pub("LDR_1", "No Light");
  }
 
  delay(500);
}

void messageReceived(String thing, String channel, String payload) {
  Serial.print("Recieved: ");
  Serial.print(thing);
  Serial.print("/");
  Serial.print(channel);
  Serial.print("-> ");
  Serial.println(payload);
}