// Load Wi-Fi library
#include <WiFi.h>

#define RXD2 16
#define TXD2 17

// Replace with your network credentials
const char* ssid = "binh";
const char* password = "12345678";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String response, ip_address;

// Auxiliar variables to store the current output state
String output26State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;
int wait30 = 30000; // time to reconnect when connection is lost.

// This is your Static IP
IPAddress local_IP(192, 168, 153, 189); 
// Gateway IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4); 

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  // Initialize the output variables as outputs
  pinMode(output26, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output26, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  ip_address = WiFi.localIP().toString();
  Serial.println(ip_address);
  server.begin();
}

void loop() {

// If disconnected, try to reconnect every 30 seconds.
  if ((WiFi.status() != WL_CONNECTED) && (millis() > wait30)) {
    Serial.println("Trying to reconnect WiFi...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);
    wait30 = millis() + 30000;
  } 
  // Check if a client has connected..
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
   
  Serial.print("New client: ");
  Serial.println(client.remoteIP());
   
  // Read the information sent by the client.
  String req = client.readStringUntil('\r');
  Serial.println(req);

  // Make the client's request.
  if(req.indexOf("status") != -1) {
    response = "WiFi Connected: " + ip_address;
  }

  if(req.indexOf("forward") != -1) {
    Serial2.write(0x30);
  }

  if(req.indexOf("backward") != -1) {
    Serial2.write(0x31);
  }  

  if(req.indexOf("left") != -1) {
    Serial2.write(0x32);
  }

  if(req.indexOf("right") != -1) {
    Serial2.write(0x33);
  }

  if(req.indexOf("stop") != -1) {
    Serial2.write(0x34);
  }

  if(req.indexOf("stopForward") != -1) {
    Serial2.write(0x35);
  }

  if(req.indexOf("stopBackward") != -1) {
    Serial2.write(0x36);
  }

  if(req.indexOf("stopLeft") != -1) {
    Serial2.write(0x37);
  }

  if(req.indexOf("stopRight") != -1) {
    Serial2.write(0x38);
  }

  if(req.indexOf("turnRight") != -1) {
    Serial2.write(0x39);
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); 
  client.println(response); //  Return status.

  client.flush();
  client.stop();
  Serial.println("Client disconnected.");
}