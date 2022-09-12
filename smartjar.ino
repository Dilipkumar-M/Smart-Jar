#include <ESP8266WiFi.h>

#include <DHT.h>
#define DHTPIN 4 //Connect Out pin to D2 in NODE MCU
#define DHTTYPE DHT11  
DHT dht(DHTPIN, DHTTYPE);

#define BLYNK_TEMPLATE_ID ""
#define BLYNK_DEVICE_NAME "Smart Jar"
#define BLYNK_AUTH_TOKEN ""
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>



char auth[] = BLYNK_AUTH_TOKEN;

char ssid1[] = "";  // type your wifi name
char pass1[] = "";  // type your wifi password

BlynkTimer timer;






const int trigPin = D5;
const int echoPin = D6;
long duration;
int f=0;
int distance; 
float level;
const char* ssid = ""; 
const char* password = "";
void send_event(const char *event);
const char *host = "maker.ifttt.com";
const char *privateKey = "";
WiFiServer server(80);
void setup() {
 pinMode(trigPin, OUTPUT);
 pinMode(echoPin, INPUT);
 Serial.begin(115200);

 
 Blynk.begin(auth, ssid1, pass1);
 //timer.setInterval(100L, sendSensor);

 
 Serial.print("Connecting to Wifi Network");
 Serial.println(ssid);
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 }
 Serial.println("");
 Serial.println("Successfully connected to WiFi.");
 Serial.println("IP address is : ");
 Serial.println(WiFi.localIP());
 server.begin();
 Serial.println("Server started");
 dht.begin();
}
void loop() {

  float h = dht.readHumidity();
  float t = dht.readTemperature();


  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.0340 / 2;
  Serial.println("Distance");
  Serial.println(distance);
  level =((11-distance)/11.0)*100;
  Serial.println("level");
  Serial.println(level);



  Serial.print("Temperature : ");
    Serial.print(t);
    Serial.print("    Humidity : ");
    Serial.println(h);

 
  delay(1000);
  WiFiClient client = server.available();
  Blynk.virtualWrite(V0, level);
  Blynk.virtualWrite(V1, t);
  Blynk.virtualWrite(V2, h);
  Blynk.run();
  timer.run();
  if ( level <= 10 && f==0 ) {
        send_event("jar_event");
        }
  f=1;
}
void send_event(const char *event)
{
  Serial.print("Connecting to "); 
  Serial.println(host);
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("Connection failed");
    return;
  }
  // We now create a URI for the request
  String url = "/trigger/";
  url += event;
  url += "/with/key/";
  url += privateKey;
  Serial.print("Requesting URL: ");
  Serial.println(url);
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  while(client.connected())
  {
    if(client.available())
    {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    } else {
      // No data yet, wait a bit
      delay(50);
    };
  }
  Serial.println();
  Serial.println("closing connection");
  client.stop();
}