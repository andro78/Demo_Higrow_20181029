#include "DHT.h"
#include <ESP.h>

//#include "SSD1306.h"
//#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "esp_deep_sleep.h"

#define DHTTYPE DHT11   // DHT 22  (AM2302), AM2321
#define uS_TO_S_FACTOR 1000000

 
unsigned long now;
int DEEPSLEEP_SECONDS = 1800;

uint64_t chipid;

long timeout;

const int dhtpin = 22;
const int soilpin = 32;
const int POWER_PIN = 34;
const int LIGHT_PIN = 33;

// Initialize DHT sensor.
DHT dht(dhtpin, DHTTYPE);

// Temporary variables
static char celsiusTemp[7];
static char humidityTemp[7];

// Client variables
char linebuf[80];
int charcount = 0;

char deviceid[21];

int fixvaue_for_water = 1000;
int water_value;
int MAX_water;
int min_water;

//SSD1306 display(0x3c, 21, 22);
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

void setup() {
  dht.begin();

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  //esp_deep_sleep_enable_timer_wakeup(1800 * uS_TO_S_FACTOR);
  //esp_deep_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);

  pinMode(16, OUTPUT);
  pinMode(POWER_PIN, INPUT);
  digitalWrite(16, LOW);

  timeout = 0;

  chipid = ESP.getEfuseMac();
  sprintf(deviceid, "%" PRIu64, chipid);
  //Serial.print("DeviceId: ");
  //Serial.println(deviceid);
  
//  display.init();
  //display.flipScreenVertically();
}

void loop() {

  char body[1024];
  digitalWrite(16, LOW); //switched on

  //display.clear();
  sensorsData(body);
  //display.drawString(55, 0, "H: ");
  //display.display();
  //esp_deep_sleep_enable_timer_wakeup(DEEPSLEEP_SECONDS * uS_TO_S_FACTOR);
  //esp_deep_sleep_start();

}

int new_value;

void sensorsData(char* body) {

  //This section read sensors
  timeout = millis();

  int waterlevel = analogRead(soilpin);
  int lightlevel = analogRead(LIGHT_PIN);

  waterlevel = map(waterlevel, 0, 4095, 0, 1023);
  waterlevel = constrain(waterlevel, 0, 1023);
  lightlevel = map(lightlevel, 0, 4095, 0, 1023);
  lightlevel = constrain(lightlevel, 0, 1023);

  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float temperature = dht.readTemperature();

  float hic = dht.computeHeatIndex(temperature, humidity, false);
  dtostrf(hic, 6, 2, celsiusTemp);
  dtostrf(humidity, 6, 2, humidityTemp);

  String did = String(deviceid);
//  String water = String((int)waterlevel);
  int water = (int)waterlevel;

  new_value = map(water, 300, 700, 0, 100);
  //water_value = fixvaue_for_water - water;
  String light = String((int)lightlevel);
/*
  strcpy(body, "{\"deviceId\":\"");
  strcat(body, did.c_str());
  strcat(body, "\",\"water\":\"");
  strcat(body, water.c_str());
  strcat(body, "\",\"light\":\"");
  strcat(body, light.c_str());
  strcat(body, "\",\"humidity\":\"");
  strcat(body, humidityTemp);
  strcat(body, "\",\"temperature\":\"");
  strcat(body, celsiusTemp);
  strcat(body, "\"}");
*/
  
  //Serial.println(body);

  //Serial.println(0);
  Serial.print("WTR: "); Serial.print(100 - new_value); Serial.print("  ");
  Serial.print("TMP: "); Serial.print(celsiusTemp); Serial.print("  ");
  Serial.print("HMD: "); Serial.print(humidityTemp); Serial.print("  ");
  Serial.println("uT");
/*
  if((100 - new_value) < 10) {
    display.clear();

    display.drawFastImage(30, 16, 16, 16, logo16_glcd_bmp);
    display.display();
  } else {
    display.clear();
    display.drawCircle(50,50,20);
    display.display();
  }
  */

 // Serial.println(",");
 /*
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);
  display.drawString(40, 0, "W:");
  display.setFont(ArialMT_Plain_24);
  display.drawString(50, 10, String(100 - new_value));
  display.display();
  */
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("W:");
  display.setTextSize(2);display.print(String(100 - new_value)); display.setTextSize(1);display.println("%");

  display.setCursor(1,22);
  display.print("W:");
  display.setTextSize(2);display.print(String(100 - new_value)); display.setTextSize(1);display.println("%");

  display.setCursor(2,44);
  display.print("W:");
  display.setTextSize(2);display.print(String(100 - new_value)); display.setTextSize(1);display.println("%");
  
//  display.setCursor(0,22);
//  display.setTextSize(2);display.println("");
//  display.setCursor(0,2);
//  display.setTextSize(1);display.println(". ");display.println(" .");
//  display.setTextSize(1);display.print("H:");
//  display.setTextSize(2);display.print(String(humidityTemp)); display.setTextSize(1);display.println("%");

//  display.setCursor(0,44);
//  display.setTextSize(1);display.print("T:");
//display.setTextSize(2);display.print(String(celsiusTemp)); display.setTextSize(1);display.println("%");
  display.display();
  delay(100);
  
  //if (lightlevel < 100) {
  //  DEEPSLEEP_SECONDS = 10800;
  //}
}
