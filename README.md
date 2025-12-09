# XPLORA-Microsuite
LoRa text communication system for TTGO oled 0.96 with tutorial for making ultra-lowcost DIY mouse device.





                                   *****************************
                                   
                                         X P L O R A  1.0
                                         
                                   LoRa Communication Microsuite
                                   
                                   *****************************

         LoRa Public Chat and Messageing system for Lilygo TTGO lora32 oled0.96 V2/1.6.1
                                     compiled in Arduino IDE
                                    Written by Dieter Marfurt
                                         Nov. Dec. 2025
                                        
Notice: before any testing, scroll to the LoRa initialization and set the frequency according
to your local laws (eg. US=915 MHz, EU= 868, China=433).

This Project is meant to provide a simple means of LoRa text communication. The device chosen
for prototyping is a TTGO lora board that lacks of any user input features. The software hence
expects a certain user input device that can be constructed in a very simple way. It uses
4 of the capacitive touch pins of the ESP32 to facilitate a mouse control pad and a separate
user button. The construction of such a pad is explained in detail on hackster.io under the
title "Mouse on a TTGO". In that instance the sheetmetal bottom of a potato chips roll was
used to create the capacitive touch pads required, plus some wires and nothing else, making 
this potentially the cheapest user input device ever.

The main OS provides an app menu whereof currently everything is a placeholder, except
"Chat" and "Games". Two games were added, for fun and as a framework for further additions.
The focus however is on the Chat screen of XPLORA, which among its features manages to:

-list the last 5 chat messages onscreen.
-scroll the last 100 chat messages.
-scroll every line horizontally if it is longer than the screen is wide.
-send a message in "speak" mode (locally broadcasted without re-broadcast-request)
-send a message in "yell" mode (broadcasted with re-broadcast-request so it spreads beyond local reach)

When clicking "speak" or "yell", an onscreen keyboard allows to enter simple, short messages, and 
send them over LoRa. Letters are chosen by the mouse. 

The keyboard screen features 4 shift states with 120 letters, a memory for previously typed
messages that people may want to re-send and a backspace button to correct typos. When the
keyboard screen is left, by "OK" or "X", any newly typed text will be stored in the "LAS"
last typed messages memory. This keyboard screen can be used as a generic text input interface in
future additions to the project, for configuration screens etc.


However, by default the device starts up the chat screen, so incoming messages are displayed
even without user input.

In order to prevent oled display burn in, a screensaver is used after a certain time of inactivity.
User action or incoming messages bring the chat screen back. (Even just touching the GPIOs)

In order to create unique packet IDs, a true random seed was implemented, using noise from
capacitive touch pins. So even without user interaction each device generates a unique sequence
of random numbers.

The MAC address is used to create a unique station name (12500 variations) consisting of 3 
capital letters. If these turn out to be anything nasty, we're sorry, it's pure coincidence.

Besides, the MAC address could also be used to individually create a true random seed.


Notice: Touch buttons are auto-calibrated at bootup. Do not touch them or their GPIOs
while booting. Also don't lay them on the ground during booting. Capacitive touch pads can 
be very touchy - here we pay a moderate price for the ultra low material costs.

Also the values measured may drift slightly when a device warms up. In case of touch issues do a reboot.

Notice: before any testing, scroll to the LoRa initialization and set the frequency according
to your local laws (eg. US=915 MHz, EU= 868, China=433).


Compling and Portability
************************
This code may compile on other Versions of the TTGO lora32 oled0.96 board fine, except
for Version 1.0, which has one pin swapped. Please go to the TTGO lora32 oled WIKI of
www.lilygo.cc to see the differences between the 4 versions. Also find the two links
at the bottom of the page to the libraries that you must install. (arduino-LoRa-master.zip
and esp8266-oled-ssd1306-master.zip) Simply copy their content to documents/Arduino/libraries/ .
Restart Arduino so it lists their Examples.
The Board must be supported by Arduino IDE (Lilygo TTGO lora32 oled), AFAIR it comes with
the expressif ESP32 package. Notice: after choosing the board in the IDE, one must additionally
select the correct board version (as printed on the PCB) in the IDE tools menu, near "Port".
Compiling time is actually quite fast, compared to other boards and libraries.

Porting this to other systems having a similar or same 128x64 pixel oled display seems straight 
forward. However the system may run just as fine on bigger displays, allowing for more onscreen 
features. We higly welcome any such projects.


Future Plans
************
Besides some utilities we are mainly focussing on a point to point routing protocol, that will
allow peer to peer packet delivery over its mesh network without hops limit, featuring
simplified autonomous dynamic routing. Current simulation show up to 95% connectivity with
200 stations randomly distributed over 3500 square kilometers. Whether this will be implemented
in this project, or when, time will tell.


Broader purpose of the software
*******************************
This sourcecode uses a monolithic and simple style that doesn't use much of the more sophisticated
C++ syntax. It is therefor ideal for people new to C++. Other than the ready-to-go communication
it provides, the system can act as a framework to incorporate multiple apps, whereas
together with its mouse interface adding powerful features becomes a piece of cake.



License
*******

This work is Open Source under MIT License (see below). Provisions given by the original creators of 
the mentioned used libraries persist as well.
