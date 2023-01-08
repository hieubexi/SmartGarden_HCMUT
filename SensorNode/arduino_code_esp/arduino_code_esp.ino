#include <Adafruit_Sensor.h> //Library for Adafruit sensors , we are using for DHT
#include <DHT_U.h>
#include <SoftwareSerial.h>
#define DHTPIN 4
// Uncomment whatever type you're using!
#define DHTTYPE DHT11 // DHT 11
//#define DHTTYPE DHT22 // DHT 22 (AM2302), AM2321
//#define DHTTYPE DHT21 // DHT 21 (AM2301)
#define SOIL1 A0
#define SOIL2 A1
#define SOIL3 A2
SoftwareSerial espSerial(5, 6);
DHT_Unified dht(DHTPIN, DHTTYPE);
String str = "";
String msgStr = "";
float humi_buffer = 0 ; ;
float soil_buffer = 0 ;
float tb_humi = 0 ;
float tb_soil = 0 ;
int time_soil = 0 ;
String status_a = "AUTO";
unsigned long previousMillis = 0;
const long interval = 15000;
unsigned long pre = 0;
unsigned long pre_2 = 0 ;
const long count = 1000;
float temp, hum, soil1, soil2, soil3;
int i = 0 ;
void setup() {
  Serial.begin(115200);
  espSerial.begin(115200);
  dht.begin();
  // get temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
}
void loop()
{
  //  process_come();
  read_dht();
  //  if(status_a = "AUTO"){
  //      automatic();
  //  }
}


//---------------------
void read_dht() {
  unsigned long currentMillis = millis(); //read current time
  if (currentMillis - previousMillis >= interval) { //if current time - last time > 5 sec
    previousMillis = currentMillis;
    //read temp and humidity
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
      //   Serial.println(F("Error reading temperature!"));
    }
    else {
      //      Serial.print(F("Temperature: "));
      temp = event.temperature;
      //    Serial.print(temp);
      //   Serial.println(F("°C"));
    }
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
      //    Serial.println(F("Error reading humidity!"));
    }
    else {
      //      Serial.print(F("Humidity: "));
      hum = event.relative_humidity;
      //   Serial.print(hum);
      //   Serial.println(F("%"));
    }
    soil1 = analogRead(SOIL1);
    soil2 = analogRead(SOIL2);
    soil3 = analogRead(SOIL3);
    int soil1_percent = map(soil1, 0, 1023, 0, 100);
    int soil2_percent = map(soil2, 0, 1023, 0, 100);
    int soil3_percent = map(soil3, 0, 1023, 0, 100);
    msgStr =  String(temp) + ":" + String(hum) + ":" + String(soil1_percent) + ":" + String(soil2_percent) + ":" + String(soil3_percent) ;
    byte arrSize = msgStr.length() + 1;
    char msg[arrSize];
    msgStr.toCharArray(msg, arrSize);
    espSerial.print(msg);
  }
}



    //!ID:<Temp>:<Humi>:<soil1>:<soil2>:<soil3>#
