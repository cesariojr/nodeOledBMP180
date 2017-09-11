/*
   10/SET/2017
   Jose Maria Cesario Jr - cesarioj@br.ibm.com
   Hardware: new NodeMCU LoLin V3 + BMP180 + Display OLED I2C
   https://www.filipeflop.com/produto/display-oled-0-96-i2c-azul-amarelo/
   Library: https://github.com/squix78/esp8266-oled-ssd1306

   Simplified flow:
   1. Connects to WiFi
   2. Acquire temperature, atm pressure and altitude from BMP180 sensor
   3. Connects to IBm Watson IoT Platform
   4. Publish a JSON string to MQTT topic

   References Bluemix e IoTF
   https://developer.ibm.com/recipes/tutorials/connect-an-esp8266-with-the-arduino-sdk-to-the-ibm-iot-foundation/
   http://www.hobbytronics.co.uk/arduino-float-vars
   http://forum.carriots.com/index.php/topic/61-wireless-gardening-with-arduino-cc3000-wifi-modules/page-2 
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "SSD1306.h"
#include "image.h"

// Create objects
SSD1306  display(0x3c, 4, 5);
Adafruit_BMP085 bmp;

//update your SSID e WiFi passwd

const char* ssid = "";
const char* password = "";

//D4 only for Lolin board
#define LED_BUILTIN D4

//Update your Org, device type, device id, token
#define ORG ""
#define DEVICE_TYPE ""
#define DEVICE_ID ""
#define TOKEN ""

//broker messagesight server
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/evt/status/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

float pressao = 0.0;
float altitude = 0.0;
float temperatura = 0.0;

char pressaostr[6];
char altitudestr[6];
char temperaturastr[6];

WiFiClient wifiClient;
PubSubClient client(server, 1883, NULL, wifiClient);

void setup() {
  Serial.begin(115200);
  Wire.pins(4, 5);
  Wire.begin(4,5);
  Serial.println("Iniciando...");
  Serial.print("Conectando na rede WiFi ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("[INFO] Connected WiFi IP: ");
  Serial.println(WiFi.localIP());

  pinMode(LED_BUILTIN, OUTPUT);

  display.init();
  display.flipScreenVertically();
  display.clear();

  display.setFont(Courier_10_Pitch_16);
  display.drawString(47, 27, "IBM");
  
  display.drawXbm(35, 12, Logo_width, Logo_height, Logo_bits);
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(10, 0, "Watson IoT Platform");
  display.display();
  delay(3000);

  //BMP180 init
  if (!bmp.begin())
  {
    Serial.println("BMP 180 sensor not found.Pls reset.");
    while (1) {}
  }
}

void getData() {
  pressao = (float)bmp.readPressure();
  altitude = (float)bmp.readAltitude();
  temperatura = (float)bmp.readTemperature();
  dtostrf(pressao, 6, 0, pressaostr);
  dtostrf(altitude, 6, 0, altitudestr);
  dtostrf(temperatura, 6, 0, temperaturastr);
}

void loop() {

  if (!!!client.connected()) {
    Serial.print("Reconnecting client to ");
    Serial.println(server);
    while (!!!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
    }
    Serial.println();
  }

  //Print data to serial console
  Serial.print("Temperatura = ");
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");

  Serial.print("Pressao = ");
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");

  Serial.print("Altitude = ");
  Serial.print(bmp.readAltitude());
  Serial.println(" m");

  //Print data to Oled LCD
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);

  String label = "Barometro IoT";
  String t = "Temp:" + String(bmp.readTemperature()) + " *C";
  String p = "Pressao:" + String(bmp.readPressure()) + " Pa";
  // 103000pa pressure at sea level
  String a = "Altitude:" + String(bmp.readAltitude(103000)) + " m";

  display.drawString(5, 0, label);
  display.drawString(0, 15, t);
  display.drawString(0, 30, p);
  display.drawString(0, 45, a);
 
  // write the buffer to the display
  display.display();
  delay(1000);

  digitalWrite(LED_BUILTIN, LOW);   
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  getData();

  // Converts Float to String
  char TempString[32];  //  array de character temporario

  // dtostrf( [Float variable] , [Minimum SizeBeforePoint] , [sizeAfterPoint] , [WhereToStoreIt] )
  dtostrf(temperatura, 2, 1, TempString);
  String temperaturastr =  String(TempString);
  dtostrf(pressao, 2, 2, TempString);
  String pressaostr =  String(TempString);
  dtostrf(altitude, 2, 2, TempString);
  String altitudestr =  String(TempString);

  // Prepares JSON to WIOT Platform
  int length = 0;

  String payload = "{\"d\":{\"temperatura\": " + temperaturastr + ",\"pressao\":\"" + String(pressaostr) + "\",\"altitude\":\"" + String(altitudestr) + "\"}}";

  length = payload.length();
  Serial.print(F("\nData length"));
  Serial.println(length);

  Serial.print("Sending payload: ");
  Serial.println(payload);

  if (client.publish(topic, (char*) payload.c_str())) {
    Serial.println("Publish ok");
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
  } else {
    Serial.println("Publish failed");
    Serial.println(client.state());
  }
  
  delay(2000);
}
