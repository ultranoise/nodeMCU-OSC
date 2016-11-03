# nodeMCU-OSC
Transform your nodeMCU into an OpenSoundControl device

1) Copy all these source files to a folder in your computer

2) Run Arduino and install the following libraries: ESP8266WiFi and WiFiUdp

3) Open the .ino file at your folder

By default, this code creates an access point with no password. Find a  WiFi called "kike &&&" and connect to it, this is your nodeMCU. It will have the IP: 192.168.4.1

As you will see in the source code, the example transmits OSC data to 192.168.4.2 (port 9999) which is the IP of the first device connected to the nodeMCU. 

Thus if you type http://192.168.4.1 into your browser you will be able to configure your nodeMCU (still in process). 


