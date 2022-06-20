#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ESP8266Wifi.h>

//mudar para o nome de seu wifi e a sua senha
const char *ssid = "esp";
const char *password =  "12341234";
const int dns_port = 53;
const int http_port = 80;
const int ws_port = 1337;
const int motor1Pin1 = 15; 
const int motor1Pin2 = 12; 
const int enable1Pin = 13;
const int motor2Pin1 = 14; 
const int motor2Pin2 = 4; 
const int enable2Pin = 5;

//servo
int px, py, v1, v2 = 0;
float d = 0;

// tempo atual
unsigned long currentTime = millis();
// tempo passado
unsigned long previousTime = 0; 
// definir tempo de timeout (exemplo: 2000ms = 2s)
const long timeoutTime = 2000;

// Globals
AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(1337);
char msg_buf[10];
char *strtokindex;

void moverRobo(){
  
  v1 = 0;
  v2 = 0;
  
  delay(20);
  
  if (py < 0) {
    
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin1, HIGH);
    digitalWrite(motor2Pin2, LOW);
    
    //Aumenta a velocidade do motor
    v1 = map(py, -10, -100, 150, 255);
    v2 = map(py, -10, -100, 150, 255);
  }
  else if (py > 0) {
    
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, HIGH);
    
    // Aumenta a velocidade do motor
    v1 = map(py, 10, 100, 143, 255);
    v2 = map(py, 10, 100, 143, 255);
  }
    //Faz com que os motores fiquem parados caso o joystick esteje centralizado
    else {
    v1 = 0;
    v2 = 0;
  }

  if (px > 10){
    d = map(px, 10, 100, 100, 80);
    d = d/100;
    v1 = v1 * d;
  }else if (px < -10){
    d = map(px, -10, -100, 100, 80);
    d = d/100;
    v2 = v2 * d;
  }

  analogWrite(enable1Pin, v1);
  analogWrite(enable2Pin, v2);
}

// Callback: receiving any WebSocket message
void onWebSocketEvent(uint8_t client_num,
                      WStype_t type,
                      uint8_t * payload,
                      size_t length) {

  // Figure out the type of WebSocket event
  switch (type) {

    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", client_num);
      break;

    // New client has connected
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(client_num);
        Serial.printf("[%u] Connection from ", client_num);
        Serial.println(ip.toString());
      }
      break;

    // Handle text messages from client
    case WStype_TEXT:
    {
      // Print out raw message
      //Serial.printf("%s\n", payload);
    strtokindex = strtok((char*) payload, " ");
    px = atoi(strtokindex);
    
    strtokindex = strtok(NULL, " ");
    py = atoi(strtokindex);
    
    Serial.print("X: ");
    Serial.print(px);
    Serial.print("  Y: ");
    Serial.println(py);
    }
    break;

    // For everything else: do nothing
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
     break;
  }
}

void setup() {
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(enable2Pin, OUTPUT);

  // Start Serial port
  Serial.begin(115200);
  
  // Start access point
  WiFi.softAP(ssid, password);

  // Print our IP address
  Serial.println();
  Serial.println("My IP address: ");
  Serial.println(WiFi.softAPIP());

  // Start web server
  server.begin();

  // Start WebSocket server and assign callback
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);

}

void loop() {

  // Look for and handle WebSocket data
  webSocket.loop();
  moverRobo();
}
