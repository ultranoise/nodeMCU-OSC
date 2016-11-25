# nodeMCU-OSC
Transform your nodeMCU into an OpenSoundControl device

Setup Instructions:

1) Copy all these source files to a folder in your computer

2) Only for MAC OSX users: 
- For uploading code to your nodeMCU you will need the SiLabs driver for mcu. It is a USB to UART Bridge that you need to install or the operating system won't create a serial port to upload code from Arduino. It can be found here: https://www.silabs.com/products/mcu/Pages/USBtoUARTBridgeVCPDrivers.aspx

3) Run Arduino. Go to Sketch > Include Library and install the "WiFi (Built-in)"

4) If it is the first time you work with a nodeMCU board and Arduino:

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
    Select your board from Tools > Board > NodeMCU 1.0 (ESP-12E Module) 


5) Open the .ino file at your folder

6) When you Save or Compile your project, Arduino needs to copy your .ino file into a folder of the same name but it does not copy the files you downloaded from this respository. Please copy or move all the .h and .cpp files downloaded from this repository to the new folder created by Arduino (namely common.h, osc.cpp, osc.h). 

7) Select your Port at Tools > Port (OSX users it will be something like cu.SLAB_USBtoUART) 

8) Compile and Upload to your nodeMCU board. Wait until the uploading process has finished (look at Arduino's text terminal and wait until it reaches [ 100% ] )

Notes:

By default, this code creates an access point with no password. Find a  WiFi called "kike &&&" and connect to it, this is your nodeMCU. It will have the IP: 192.168.4.1

As you will see in the source code, the example transmits OSC data to 192.168.4.2 (port 9999) which is the IP of the first device connected to the nodeMCU. 

Thus if you type http://192.168.4.1 into your browser you will be able to configure a bit your nodeMCU (still in process). 


