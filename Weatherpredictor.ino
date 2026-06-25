#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

#define DHTPIN D4
#define DHTTYPE DHT11

#define GREEN_LED D5
#define YELLOW_LED D6
#define RED_LED D7
#define BUZZER D8

const char* ssid = "Sadhana";
const char* password = "123456789";

ESP8266WebServer server(80);
DHT dht(DHTPIN, DHTTYPE);

String lastData[5];
int indexData = 0;

void storeData(String data)
{
  lastData[indexData] = data;
  indexData = (indexData + 1) % 5;
}

void handleRoot()
{
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  String weather = "";

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);

  if(temp > 30 && temp < 60)
  {
    weather = "Sunny";
    digitalWrite(GREEN_LED, HIGH);
  }
  else if(temp >= 25 && temp <= 30)
  {
    weather = "Cloudy";
    digitalWrite(YELLOW_LED, HIGH);
  }
  else
  {
    weather = "Rainy";
    digitalWrite(RED_LED, HIGH);
    digitalWrite(BUZZER, HIGH);
  }

  String record =
      "Temp: " + String(temp) +
      " C | Humidity: " + String(hum) +
      "% | " + weather;

  storeData(record);

  String html = "<html><head>";
  html += "<meta http-equiv='refresh' content='5'>";
  html += "<title>Weather Prediction</title>";
  html += "</head><body>";

  html += "<h1>Weather Prediction System</h1>";
  html += "<h2>Temperature: " + String(temp) + " C</h2>";
  html += "<h2>Humidity: " + String(hum) + " %</h2>";
  html += "<h2>Prediction: " + weather + "</h2>";

  html += "<h3>Last 5 Readings</h3><ul>";

  for(int i=0;i<5;i++)
  {
    html += "<li>" + lastData[i] + "</li>";
  }

  html += "</ul>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void setup()
{
  Serial.begin(115200);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  dht.begin();

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
}

void loop()
{
  server.handleClient();
}