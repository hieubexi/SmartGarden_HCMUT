#include <Adafruit_Sensor.h> //Library for Adafruit sensors , we are using for DHT
#include <DHT_U.h> //DHT library which uses some func from Adafruit Sensor library
#include <ESP8266WiFi.h> //library for using ESP8266 WiFi 
#include <PubSubClient.h> //library for MQTT
#include <ArduinoJson.h> //library for Parsing JSON

//defining Pins
#define DHTPIN D1
//#define LED_WIFI D6
//#define LED_MQTT D7
#define LED D5
#define PUMP D6
#define SUONG D7
//DHT parameters

#define DHTTYPE    DHT11     // DHT 11
#define AUTO      11
#define MAN       12
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;
//!ID:<data>:<data>:<data>#
//MQTT Credentials
const char* ssid = "mot chiec Xoai";//setting your ap ssid
const char* password = "hieu301203";//setting your ap psk
const char* mqttServer = "172.17.39.168"; //MQTT URL
const char* mqttUserName = "hieu";  // MQTT username
const char* mqttPwd = "12345678";  // MQTT password
const char* clientID = "hieu0001"; // client id username+0001
const char* topic = "Garden/data_1"; //publish topic
//parameters for using non-blocking delay
unsigned long previousMillis = 0;
const long interval = 5000;
int time_soil = 20 ;
// MQTT message buffer
int status_mode = 0 ;
float temp, hum, soil;
unsigned long time_mode1 = 0;
int pump_time = 0;
float tb_humi = 0;
float tb_soil  = 0;
String strs[20];
int StringCount = 0;
//setting up wifi and mqtt client
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  //  digitalWrite(LED_WIFI, HIGH);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect(clientID, mqttUserName, mqttPwd)) {
      Serial.println("MQTT connected");
      //      digitalWrite(LED_MQTT, HIGH);
      client.subscribe("Device/lights_1");
      Serial.println("Topic lights 1 Subscribed");
      client.subscribe("Device/pump_1");
      Serial.println("Topic pump 1 Subscribed");
      client.subscribe("Device/mist_1");
      Serial.println("Topic quat 1 Subscribed");
      client.subscribe("Device/time_1");
      Serial.println("Topic time 1 Subscribed");
      client.subscribe("Garden/mode_1");
      Serial.println("Topic garden/mode_1 Subscribed");
      status_mode = MAN ;
      //
      // getMessToMQTT();
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);  // wait 5sec and retry
      status_mode = AUTO ;
    }
  }

}


//subscribe call back
void callback(char*topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  String data = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    data += (char)payload[i];
  }
  Serial.println();
  Serial.print("Message size :");
  Serial.println(length);
  Serial.println();
  Serial.println("-----------------------");
  Serial.println(data);
  //---light---
  if (strcmp(topic, "Device/lights_1") == 0) {
    if (data == "ON_L") {
      Serial.print("LIGHT: ");
      digitalWrite(LED, HIGH);
      status_mode = MAN;
    }
    else if (data == "OFF_L") {
      Serial.print("LIGHT: ");
      digitalWrite(LED, LOW);
      status_mode = MAN;
    }
  }
  //--- pump----
  else if (strcmp(topic, "Device/pump_1") == 0) {
    if (data == "ON_P") {
      Serial.print("PUMP: ");
      digitalWrite(PUMP, HIGH);
      status_mode = MAN;
    } else if (data == "OFF_P") {
      Serial.print("PUMP: ");
      digitalWrite(PUMP, LOW);
      status_mode = MAN;
    }
  }
  //---- phun suong ----
  else if (strcmp(topic, "Device/mist_1") == 0) {
    if (data == "ON_S") {
      Serial.print("PHUN SUONG: ");
      digitalWrite(SUONG, HIGH);
      status_mode = MAN;
    } else if (data == "OFF_S") {
      Serial.print("PHUN SUONG: ");
      digitalWrite(SUONG, LOW);
      status_mode = MAN;
    }
  }
  //------------time----------------------

  else if (strcmp(topic, "Device/time_1") == 0) {
    Serial.print("PUMP Time: ");
    digitalWrite(PUMP, HIGH);
    status_mode = MAN;
    time_soil = data.toInt();
    for (int i = 0 ; i < time_soil; i++) {
      time_soil--;
      delay(1000);
    }
    digitalWrite(PUMP, LOW);
  }

  //----------mode------------------------
  else if (strcmp(topic, "Garden/mode_1") == 0) {
    if (data == "AUTO") {
      status_mode = AUTO;
    } else if (data == "MAN") {
      status_mode = MAN;
    }
  }
  /*
    else{
     Serial.print("PUMP Time: ");
    digitalWrite(PUMP, HIGH);
    time_soil = data.toInt();
    for(int i = 0 ; i < time_soil; i++){
      time_soil--;
      delay(1000);
    }
     digitalWrite(PUMP,LOW);

    }
  */

}


void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  // Initialize device.
  dht.begin();
  // get temperature sensor details.

  /*  pinMode(LED_WIFI, OUTPUT);
    pinMode(LED_MQTT, OUTPUT);
    digitalWrite(LED_WIFI, LOW);
    digitalWrite(LED_MQTT, LOW);
  */
  pinMode(LED, OUTPUT);
  pinMode(PUMP, OUTPUT);
  pinMode(SUONG, OUTPUT);
  digitalWrite(LED, LOW);
  digitalWrite(PUMP, LOW);
  digitalWrite(SUONG, LOW);

  setup_wifi();

  client.setServer(mqttServer, 1883); //setting MQTT server
  client.setCallback(callback); //defining function which will be called when message is received.

}

void loop() {
  if (!client.connected()) { //if client is not connected
    reconnect(); //try to reconnect

  }
  client.loop();
  if (Serial.available()) {
    String msgStr = "";
    msgStr = Serial.readString();
    msgStr = "!1:" +  msgStr  + "#";
    process_msg(msgStr);
    byte arrSize = msgStr.length() + 1;
    char msg[arrSize];
    Serial.print("PUBLISH DATA:");
    Serial.println(msgStr);
    msgStr.toCharArray(msg, arrSize);
    client.publish(topic, msg);
    delay(50);
  }
  control_mode();
  if (status_mode == AUTO) {

    if (tb_soil <= 40) {
      if (digitalRead(PUMP) == LOW) {
        digitalWrite(PUMP, HIGH) ;
        client.publish("Device/pump_1", "ON_P");
      }
    }
    if (tb_soil >= 60) {
      if (digitalRead(PUMP) == HIGH) {
        digitalWrite(PUMP, LOW);
        client.publish("Device/pump_1", "OFF_P");
      }

    }

    if (tb_humi >= 80 ) {
      if (digitalRead(SUONG) == LOW) {
        digitalWrite(SUONG, HIGH) ;
        client.publish("Device/mist_1", "ON_S");
      }

    }
    if (tb_humi >= 50 && tb_humi <= 80) {
      if (digitalRead(SUONG) == HIGH) {
        digitalWrite(SUONG, LOW);
        client.publish("Device/mist_1", "OFF_S");
      }

    }
    if (tb_soil <= 40 && tb_humi <= 50 ) {
      if (digitalRead(SUONG) == LOW) {
        digitalWrite(SUONG, HIGH) ;
        client.publish("Device/mist_1", "ON_S");
      }
      if (digitalRead(PUMP) == LOW) {
        digitalWrite(PUMP, HIGH) ;
        client.publish("Device/pump_1", "ON_P");
      }
    }
  }
}


void process_msg(String str) {
  str.remove('!');
  str.remove('#');
  str.remove('1');
  while (str.length() > 0)
  {
    int index = str.indexOf(':');
    if (index == -1) // No ':' found
    {
      strs[StringCount++] = str;
      break;
    }
    else
    {
      strs[StringCount++] = str.substring(0, index);
      str = str.substring(index + 1);
    }
  }
  tb_humi = strs[1].toInt();
  tb_soil = (strs[2].toInt() + strs[3].toInt() + strs[4].toInt()) / 3;
}

void control_mode() {
  if (status_mode == MAN ) {
    if ((unsigned long) (millis() - time_mode1) > 30000) {
      status_mode = AUTO;
      client.publish("Garden/mode_1", "AUTO");
      time_mode1 = millis();
    }
  }
}
