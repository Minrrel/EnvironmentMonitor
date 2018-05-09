# EnvironmentMonitor
Project: Environment Monitor
By: Olivia Howard, Newnham College, ofyh2

-Summary-
This project is an environment monitor that uses a SensorTag's sensors to read various attributes (temperature, humidity and pressure) and sends the data to a mobile phone with the environment monitor app installed on it; allowing the data to be read. In addition to this there is also functionality allowing for alarms to be set if certain criteria are met (such as it reaches above a certain temperature) which cause the SensorTag to 'beep' and flash an LED, with the reason being stated on the phone and a button to switch off the alarm on the phone. There is also a basic chance of rain calculator allowing for the percentage chance of rain to be viewed and alarms to be set for if this percentage exceeds a certain value.

To compile the firmware code composer studio and the BLE stack from below need to be installed.
http://www.ti.com/tool/ble-stack?DCMP=wbu-blestack&HQS=ble-stack 

-Layout- 
In this repository there are two base level folders corresponding to the phone application files and the SensorTag firmware (and a pdf of the final report (FinalReport.pdf).

-PhoneAppFiles-
Inside the "EnvironmentMonitor/PhoneAppFiles/", there is the .apk (envmonitor.apk) for the application which can be directly installed onto Android phones, and a folder of the project files for building the application using Evothings Studio.

To develop this application the only code created is in the index.html file (located at: EnvironmentMonitor/PhoneAppFiles/EnvMonitor)
This was based upon code found: https://evothings.com/doc/tutorials/evothings-ble-api-guide.html
and using basic Javascript and html5 documentation, to allow the slider to work properly and be able to be dragged the following library also had to be used: http://touchpunch.furf.com/.
To build you need to install evothings from here:
http://evothings.com/

-SensorTagFirmwareFiles-
In this section of the repository there are two folders, one corresponding to the Code Composer workspace for the firmware, and the original files that were edited to incorporate the functionality required.

-Bluetooth functionality-
The base bluetooth functionality for connections between the phone and the SensorTag is provided by the contents in the project_zero_app_cc2650stk

All the modified code for the firmware is in the emon_app_cc2650stk, the files edited are:
(The original unedited files can be found in EnvironmentMonitor/SensortagFirmwareFiles/OriginalFilesThatWereEdited/)

->EnvironmentMonitor/SensortagFirmwareFiles/workspace_v7/emon_app_cc2650stk/PROFILES/envservice.h
Modified from the led_service.h
Defines the UUID (unique identifier) for the bluetooth servies for ES_COMMAND - // Env service command and value. (Command's single character followed by a comma then the value)
ES_RESULT - The returned value/result due to the command

->EnvironmentMonitor/SensortagFirmwareFiles/workspace_v7/emon_app_cc2650stk/PROFILES/envservice.c
Modified from the led_service.c
Handles the functions used in setting up the bluetooth communications between the phone and sensortag
(both reading and writing)

->EnvironmentMonitor/SensortagFirmwareFiles/workspace_v7/emon_app_cc2650stk/Application/project_zero.c was edited
Modified file from a template to provide the actual communications.
Various functions e.g LEDs commented out to reduce memory use

->EnvironmentMonitor/SensortagFirmwareFiles/workspace_v7/emon_app_cc2650stk/Application/emon.c
Modified from a pwmled file 
To bring everything together

->EnvironmentMonitor/SensortagFirmwareFiles/workspace_v7/emon_app_cc2650stk/Application/emon.h
Header for emon.c

->EnvironmentMonitor/SensortagFirmwareFiles/workspace_v7/emon_app_cc2650stk/Startup/main.c
Initialises everything, was edited to include call for initialising the buzzer for the alarm and start threads for monitoring sensors
