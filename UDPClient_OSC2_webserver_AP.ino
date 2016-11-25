/*
 Udp Client
 */


 /*
  *  TODO: 
  *  1) scanning all available wifis
  *  2) dialog for connecting to a specific wifi: user & password
  *  
  */

  
#include <stdio.h>
#include <strings.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "common.h"
#include "osc.h"


char ssid[] = "myWifiName";  //  your network SSID (name)
char pass[] = "myWifiPassw";       // your network password


unsigned int localPort = 2390;      // local port to listen for UDP packets
unsigned int sendPort = 9999;       //port to transmit to computer

/* Don't hardwire the IP address or we won't get the benefits of the pool.
 *  Lookup the IP address for the host name instead */

IPAddress timeServerIP(192, 168, 4, 2); // time.nist.gov NTP server
//IPAddress timeServerIP; // time.nist.gov NTP server address
//const char* ntpServerName = "time.nist.gov";

const int UDP_PACKET_SIZE = 16; // UDP message size

byte packetBuffer[UDP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

//FROM RIOT
byte mac[6];
IPAddress DestIP = IPAddress(192,168,4,2);  //computer is assigned 192.168.4.2
uint16_t DestPort = 9999;
uint8_t ModuleID = 0;
unsigned long SampleRate = 5;
int status = WL_IDLE_STATUS;

WiFiUDP UdpPacket;
WiFiUDP ConfigPacket;
OscBuffer Kick;

char StringBuffer[MAX_STRING];
/////


//physical inputs
const int pinButton = 5; //GPIO 5 is D1
const int potentiometer = A0;
float val = 0;
int ledPin = 16; // GPIO13

//wifi server
WiFiServer server(80);

//Access Point password
const char WiFiAPPSK[] = "12345678"; //wifi password should be 8 characters long 

//WiFi Status of the Board
int WiFiStatus = -1;

void setup()
{
  delay(2000);
  //JUST INIT THE PINS FOR A SPECIFIC CONFIGURATION
  initHardware();

  //TO SCAN ALL WIFIS AROUND 
  //scanWifi();

  //TO CONNECT TO OTHER WIFI WITH SSID + PASSWORD
  //initWifi();

  //TO SETUP THE BOARD AS ACCESS POINT
  setupWiFiAP(); 

  //INIT OPEN SOUND CONTROL COMMUNICATION
  initUdpOsc();

  //INIT WEB SERVER TO COMMUNICATE/CONFIGURE VIA BROWSER
  initWebServer();

}

void loop()
{
  //READ SENSORS
  val = analogRead(potentiometer);
  //val = digitalRead(pinButton);
  
  //SEND AN UDP packet to a server (THIS WORKS)  
  //sendUDPpacket(timeServerIP); 
  
  ///SEND OSC 
  char *pData;  //Data Container
  pData = Kick.pData;
  
  FloatToBigEndian(pData, &val); //Convert a variable (val) into OSC format and store it into pData
  pData += sizeof(float);

  FloatToBigEndian(pData, &val); //Again, convert a variable (val) into OSC format, this is a demo, and store it into pData
  pData += sizeof(float);
  
  UdpPacket.write((uint8_t*)Kick.buf, Kick.PacketSize);  //transmit an OSC package and end
  UdpPacket.endPacket();


  //WIFI SERVER
  // Check if a client has connected
  WiFiClient client = server.available();
  
  if (client) {
    // Wait until the client sends some data
    //Serial.println("new client");
    if(client.available()){
      // Read the first line of the request
      String request = client.readStringUntil('\r');
      Serial.println("client says...");
      Serial.println(request);
      client.flush();

      // Match the request
      int value = LOW;
      if (request.indexOf("/LED=ON") != -1)  {
        digitalWrite(ledPin, HIGH);
        value = HIGH;
      }
      if (request.indexOf("/LED=OFF") != -1)  {
        digitalWrite(ledPin, LOW);
        value = LOW;
      }
      if (request.indexOf("/connect_form.asp?") != -1)  {
        Serial.println(" ");
        Serial.println("I have received a connect request");
        //find its content
        String s1 = request.substring(30); //string starting in wifi's name
        //Serial.println(s1);
        int amp = s1.indexOf('&');
        String wifi2connect = s1.substring(0,amp); //the ssid of the new wifi
        Serial.println("I received a ssid");
        Serial.println(wifi2connect);
        int equal = s1.indexOf('=');
        String s2 = s1.substring(equal+1);
        //Serial.println(s2);
        int space = s2.indexOf("HTTP");
        String pass2connect = s2.substring(0,space);
        Serial.println("I received a passw");
        Serial.println(pass2connect);
        
      }
      
      // Return the response
      GUImanager(client);
    
      delay(1);
      //Serial.println("Client disconnected");
      Serial.println("");
   }   
  }
  




  ///

  
  delay(10);  //try different rates




  //receive packages
  int cb = udp.parsePacket();
  if (!cb) {
    //Serial.println("no packet yet");
  }
  else {
    Serial.print("packet received, length=");
    Serial.println(cb);
    // We've received a packet, read the data from it
    udp.read(packetBuffer, UDP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:
    Serial.println(packetBuffer[0]);
    Serial.println(packetBuffer[1]);
    Serial.println(packetBuffer[2]);
    Serial.println(packetBuffer[3]);
    //etc
    
    /*
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    */
  }

}

// send an UDP request to the time server at the given address
unsigned long sendUDPpacket(IPAddress& address)
{
  //Serial.println("sending NTP packet...");
  
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, UDP_PACKET_SIZE);
  
  // Initialize values needed to form UTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = digitalRead(pinButton);   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, sendPort); //NTP requests are to port 123
  udp.write(packetBuffer, UDP_PACKET_SIZE);
  udp.endPacket();

}

void GUImanager(WiFiClient& cl) {
  // Return the response
      cl.println("HTTP/1.1 200 OK");
      cl.println("Content-Type: text/html");
      cl.println(""); //  do not forget this one
      cl.println("<!DOCTYPE HTML>");
      cl.println("<html>");
      cl.print("<body bgcolor=\"#F4E242\">");
      
      
      cl.print("<strong>NodeMCU - nodeMCU-OSC Controller: </strong>");
      if(WiFiStatus == 1){
        cl.println("<br><br>");
        cl.print("<strong>WiFi Status:  </strong>");
        cl.println("Access Point");
      }
      if(WiFiStatus == 2){
        cl.println("<br><br>");
        cl.print("<strong>WiFi Status:  </strong>");
        cl.println("Station");
      }
      /*
      if(value == HIGH) {
        cl.print("On");
      } else {
        cl.print("Off");
      }
      */
    cl.println("<br><br>");
    cl.println("<a href=\"/LED=ON\"\"><button>Turnnn On </button></a>");
    cl.println("<a href=\"/LED=OFF\"\"><button>Turnnn Off </button></a><br />");
    cl.println("<a href=\"/OSC=ON\"\"><button>OSC On </button></a>");
    cl.println("<a href=\"/OSC=OFF\"\"><button>OSC Off </button></a><br />");   

    //OSC Configuration
    cl.println("<br><br>");
    cl.println("<form action=\"OSC_form.asp\" id=\"usrform\"> OSC Configuration <input type=\"text\" name=\"usrname\" value=\"Destination IP\"> <input type=\"text\" name=\"passw\" value=\"Destination Port\"> <input type=\"submit\" value=\"Submit\"> </form>");
    

    //Connect to WiFi
    cl.println("<br><br>");
    cl.println("<form action=\"connect_form.asp\" id=\"usrform\"> Connect Board to Wifi <input type=\"text\" name=\"usrname\" value=\"Wifi Name (SSID)\"> <input type=\"text\" name=\"passw\" value=\"password\"> <input type=\"submit\" value=\"Connect\"> </form>");
    
    cl.println("</html>");
}

void initHardware() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  pinMode(pinButton, INPUT);
  pinMode(potentiometer, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  /*
   pinMode(16, OUTPUT);
   pinMode(5, OUTPUT);
   pinMode(4, OUTPUT);
   pinMode(0, OUTPUT);
   pinMode(2, OUTPUT);
   pinMode(14, OUTPUT);
   pinMode(12, OUTPUT);
   pinMode(13, OUTPUT);
   pinMode(15, OUTPUT);

   digitalWrite(16, LOW);
   digitalWrite(5,LOW);
   digitalWrite(4,LOW);
   digitalWrite(0,LOW);
   digitalWrite(2,LOW);
   digitalWrite(14,LOW);
   digitalWrite(12,LOW);
   digitalWrite(13,LOW);
   digitalWrite(15,LOW);
  */
}

void initWifi(){
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void initUdpOsc(){
  Serial.println("Starting UDP/OSC");
  //udp.begin(localPort);
  //Serial.print("Local port: ");
  //Serial.println(udp.localPort());

  // Prep the OSC packet
  UdpPacket.begin(DestPort);
  UdpPacket.beginPacket(DestIP, DestPort);
  Serial.print("Dest port: ");
  Serial.println(UdpPacket.localPort());
  Serial.println("UDP/OSC Ready");

  sprintf(StringBuffer, "/%u/kick\0", ModuleID);
  PrepareOSC(&Kick, StringBuffer, 'f', 2); // 1 float, was:2 floats: kick intensity, boolean kick detected
}

void initWebServer(){
  //start a wifi server
  server.begin();
  Serial.println("Web/Configuration Server started");
}

void scanWifi(){
  
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0){
        Serial.println(" no networks found");
  }
  else
  {
    Serial.println("networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i +1);
      Serial.print( " : ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
    }
  }
    Serial.println("");
}

void setupWiFiAP() {
  WiFi.mode(WIFI_AP);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "ssid your name":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "nodeMCU-OSC " + macID;

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  boolean result = WiFi.softAP(AP_NameChar);// , WiFiAPPSK); //if not opened
  Serial.println("");
  if(result == true)
  {
    Serial.println("Access Point ON");;
  }
  else
  {
    Serial.println("Access Point Failed");
  }
  
  Serial.println("My IP is:");
  Serial.println(WiFi.softAPIP());

  WiFiStatus = 1;
}
