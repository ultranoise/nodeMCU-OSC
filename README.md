# nodeMCU-OSC
Transform your nodeMCU into an OpenSoundControl device

1) Copy all these source files to a folder in your computer

2) Run Arduino and install the library WiFi (Standard arduino library)

3) If it is the first time you work with a nodeMCU board and Arduino:

    Open Arduino IDE
    Open Preferences
    Add http://arduino.esp8266.com/stable/package_esp8266com_index.json to Additional Boards Manager URLs. 
    Save
    Restart Arduino IDE
    Go to Tools > Boards > Boards Manager
    Scroll down and click on esp8266 by ESP8266 Community (This contains ESP8266wifi.h)
    Click install
    Click close
    Restart Arduino IDE


4) Open the .ino file at your folder

Notes:
By default, this code creates an access point with no password. Find a  WiFi called "kike &&&" and connect to it, this is your nodeMCU. It will have the IP: 192.168.4.1

As you will see in the source code, the example transmits OSC data to 192.168.4.2 (port 9999) which is the IP of the first device connected to the nodeMCU. 

Thus if you type http://192.168.4.1 into your browser you will be able to configure a bit your nodeMCU (still in process). 


