/*
                                   *****************************
                                   
                                         X P L O R A  1.0
                                         
                                   LoRa Communication Microsuite
                                   
                                   *****************************

         LoRa Public Chat and Messageing system for Lilygo TTGO lora32 oled0.96 V2/1.6.1
             
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

*/

// so here we go...
// init LORA ! ----------------------------------------------------------------------------------

#include <SPI.h>
#include <LoRa.h>

#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 23 
#define DIO0 26


//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 866E6 // notice it's 866, not 868... these people are weird...

#define LORA_V1_0_OLED  0 // set according to your board version
#define LORA_V1_2_OLED  0
#define LORA_V1_6_OLED  1
#define LORA_V2_0_OLED  0

// #define LORA_PERIOD 433  
 #define LORA_PERIOD 868  
// #define LORA_PERIOD 915     

// end of stuff that must be configured correctly else you may end up in jail - no kidding.

// end of init LORA ---------------------


// init OLED screen ! Kudos to the makers of the library, see below
/**
   The MIT License (MIT)

   Copyright (c) 2018 by ThingPulse, Daniel Eichhorn
   Copyright (c) 2018 by Fabrice Weinberg

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.

   ThingPulse invests considerable time and money to develop these open source libraries.
   Please support us by buying our products (and not the clones) from
   https://thingpulse.com

*/

// Include the correct display library

// For a connection via I2C using the Arduino Wire include:
#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"
// OR #include "SH1106Wire.h"   // legacy: #include "SH1106.h"

// For a connection via I2C using brzo_i2c (must be installed) include:
// #include <brzo_i2c.h>        // Only needed for Arduino 1.6.5 and earlier
// #include "SSD1306Brzo.h"
// OR #include "SH1106Brzo.h"

// For a connection via SPI include:
// #include <SPI.h>             // Only needed for Arduino 1.6.5 and earlier
// #include "SSD1306Spi.h"
// OR #include "SH1106SPi.h"


// Optionally include custom images (unused, but just so you see how it works)
#include "images.h"
#include "myimage.h"


// Initialize the OLED display using Arduino Wire:
SSD1306Wire display(0x3c, SDA, SCL);   // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h e.g. https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h
// SSD1306Wire display(0x3c, D3, D5);  // ADDRESS, SDA, SCL  -  If not, they can be specified manually.
// SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_128_32);  // ADDRESS, SDA, SCL, OLEDDISPLAY_GEOMETRY  -  Extra param required for 128x32 displays.
// SH1106Wire display(0x3c, SDA, SCL);     // ADDRESS, SDA, SCL

// Initialize the OLED display using brzo_i2c:
// SSD1306Brzo display(0x3c, D3, D5);  // ADDRESS, SDA, SCL
// or
// SH1106Brzo display(0x3c, D3, D5);   // ADDRESS, SDA, SCL

// Initialize the OLED display using SPI:
// D5 -> CLK
// D7 -> MOSI (DOUT)
// D0 -> RES
// D2 -> DC
// D8 -> CS
// SSD1306Spi display(D0, D2, D8);  // RES, DC, CS
// or
// SH1106Spi display(D0, D2);       // RES, DC

// end of init oled----------------------------------------------------------------------------


//my XPLORA globals etc
const float myDegRel=(360.0/3.1415927)/2.0; // used for converting between degree systems Radians and Euler
const float halfPi=3.1415927/2.0;
int myscroll=0; // OS mainmenu scroll index

int touch_baselevel1=0; // x distance sensor // touch pin base level when untouched
int touch_baselevel2=0; // y distance sensor
int touch_baselevel3=0; // touchpad physical contact sensor
int touch_baselevel4=0; // "leftclick" sensor

int approx1=0;// actual touch pin reads (x ramp)
int approx2=0; //y ramp
int approx3=0; // pad contact sensor for ramp algo
int approx4=0; // for leftclick separate button
int lastapprox1=0;// to remember previous touch pin reads (for ramp detection)
int lastapprox2=0;

int mytouchpin1=4; // Actual user input touchpad GPIO assignement !
int mytouchpin2=12;
int mytouchpin3=14; // these 2 we share with the sd-card, may have to set again as input after card access.
int mytouchpin4=15; //14=touch_sd_sclk, 15=touch_sd_mosi, btw: 25=adc_led, 34,35=adc;

int mousex=64; // calculated mouse coords, also set initial mouse coords here
int mousey=52;
int realmousex=0; // smoothed mouse coords (rubberband-follower)
int realmousey=0;
int buttdown1=0; // for graphical button state indicator, with release delay (currently unused)
int buttdown2=0;
int buttdown3=0;
int butt1T=0;
int butt2T=0;
int butt3T=0;
long int screensaverT=0; // stores moment (RE millis()) of last user action or msg reception (to detect idle)
int screensaverAfter=30000;  // idle-timeout to start screensaver

const int starn=50; // starfield animation stuff for screensaver
float starx[starn];
float stary[starn];
float starxs[starn];
uint32_t chipId = 0;

String username="WXYZ"; // user name will later in setup() be generated based on mac address

const int baconn=10; // stuff for auto-sending frequent test-beacon messages
String bacon[baconn+1];
int baconTestT=0; //MilliSecs()+3000 // send 1st beacon n ms after booting

const int chatn=100; // array that stores the last 100 LoRa Chat messages
String chat[chatn+1]; 
String my_inp=""; // String that was entered by onscreen keyboard
 
String keyboard[13]; // onscreen keyboard keys in 4 shift states (10x3 x4)
int myShift=0; // keyboard shift state 0 to 3

String menu[13]; // main OS menu (this works solely with short/long clicks on button 4, the "LMB" button)
int menushow=2;
int menuscroll=0;
int menuscrollmax=10;
int menu_click_event=0;
int button1_state=0; //init weird
int button1_lastT=0;

String gametitles[17]; // for games submenu
int cgrid[1+(16*16)]; // some vars for 3d maze game
String gridstring[17];
float px=8.0;  //startposition, middle of the map
float pz=8.0;
float angle=0;
float pxold=0.0;
float pzold=0.0;
float premind=0;
String info="";
int hiscore=0; // for pong game

int chatxo=0; // used in chat screen horizontal msg scrolling

// lora globals
const int pck_stacksize=100;
int pck_stack[pck_stacksize]; // stack for packet IDs that were sent
int pck_stackpointer=0;
const int pck_jobsize=100;
int    pck_jobT[pck_jobsize]; // stack for messages that must be re-broadcasted (yell-type)
String pck_job [pck_jobsize];
int pck_jobcount=0;

const int lastsent_max=10;
String lastsent[lastsent_max];

int is_beaconsender=0; // 0 or 1, auto-send frequent beacon  messages for test and debugging purposes

// SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS Setup
void setup() {
 Serial.begin(115200);
 Serial.println();
 Serial.println();
 
 // init lora ---------------
 SPI.begin(SCK, MISO, MOSI, SS);
 LoRa.setPins(SS, RST, DIO0); 
 if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed! System halted.");
    while (1);
 }
 // Setting the LoRa Sync Word - there's a whole science by it's own going on about this, and practically
 // nobody really understands it. Fact is certain words lead to lower data delivery reliability, and also 
 // 0x34 is reserved for LoRaWAN networks. Official reccomendation by semtech for private networking is
 // 0x12.
 // See also https://blog.classycode.com/lora-sync-word-compatibility-between-sx127x-and-sx126x-460324d1787a
  LoRa.setSyncWord(0x12);           // in orig sample: 0xF3, 0x34= lorawan, 12=private, ranges from 0-0xFF, default 0x34, see API docs
  Serial.println("LoRa init succeeded.");
 // eo init lora ------------
  
 // create unique user name from the chip ID / MAC address
 int i=0, j=0;;
 for(i=0; i<17; i=i+8) {
  chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
 }
 username =(char)(65+(((chipId >> 16) & 255)/10));
 username+=(char)(65+(((chipId >> 8 ) & 255)/10));
 username+=(char)(65+(((chipId      ) & 255)/10));

 // the following sets the main menu entry that will be started after booting.
 menuscroll=2;//1 games, 2 chat; // all other menu entries are placeholders for now.
 menu_click_event=2; // simulate "Enter" click (long tap), so the chat screen opens after booting, useful for devices without buttons etc. attached.

 // test messages that will be sent randomly, in case is_beaconsender was set to 1.
 bacon[0]="XPLORA 1.0 test";
 bacon[1]="test...";
 bacon[2]="Paraphrasing: Whazzup??";
 bacon[3]="CQ CQ OPERATOR This is a test.test 1 2 3!";
 bacon[4]="hello world ";
 bacon[5]="I see trees are green, red roses too, I see them blue, 4 me and you..";
 bacon[6]="HELLO? ";
 bacon[7]="Greetings...";
 bacon[8]="This is a unnecessarily long XPLORA test beacon.";
 bacon[9]="Hi!"; //String(chipId);
 bacon[10]="LoRa!";

 keyboard[0]="QWERTYUIOP";
 keyboard[1]="ASDFGHJKL?";
 keyboard[2]="ZXCV BNM!.";
  
 keyboard[3]="qwertyuiop";
 keyboard[4]="asdfghjkl,";
 keyboard[5]="zxcv bnm;:";
  
 keyboard[6]="1234567890";
 keyboard[7]=".+-/*=()<>";
 keyboard[8]="@#%& $\\{}_";
  
 keyboard[9]= "~^'¦[]üöä£";
 keyboard[10]="ç§°|¢¬éèà" + (char)34;
 keyboard[11]="©®™»«±¥¶ßµ";
 keyboard[12]="";

// main menu entries
 menu[0]="Files";
 menu[1]="Games"; // active
 menu[2]="Chat";  // active
 menu[3]="Notepad";
 menu[4]="BASIC";
 menu[5]="LoRa P2P";
 menu[6]="Tunes";
 menu[7]="Paint";
 menu[8]="Calculator";
 menu[9]="Configuration";
 menu[10]="Deep Sleep";

// for input repeating lines, can also be used as canned messages
lastsent[0]="Here will be...";
lastsent[1]="your last Messages...";
lastsent[2]="that you've sent.";
lastsent[3]="...";
lastsent[4]="Hello World";
lastsent[5]="Hack the planet!";
lastsent[6]="Test... abcd...";
lastsent[7]="XPLORA V1.0 test message";
lastsent[8]="Find us on Github";
lastsent[9]="The sky is the limit";

// games menu
for(i=0;i<16;i++){
 gametitles[i]="";
}
 gametitles[0]="Pong";
 gametitles[1]="DooM";
 gametitles[15]="EXIT";
//gametitles[16]

// 3D maze game map (0=air, rest=walls, must close outter walls, else board will crash)
// (the various numbers except zero are from a color version, here they could all be anything non-zero)
gridstring[0]=  "1,2,1,2,1,2,1,2,1,2,1,2,1,2,1,2";
gridstring[1]=  "2,9,0,0,1,0,0,0,0,0,0,0,0,0,0,1";
gridstring[2]=  "1,0,0,0,0,0,4,3,5,6,5,6,0,0,0,2";
gridstring[3]=  "2,0,1,3,4,2,2,0,0,0,0,2,3,0,0,1";
gridstring[4]=  "1,2,1,0,0,0,2,1,4,0,0,1,2,2,0,2";
gridstring[5]=  "2,0,0,0,4,0,0,0,7,0,0,0,1,0,0,1";
gridstring[6]=  "1,0,8,4,3,0,0,0,0,0,0,0,1,2,0,2";
gridstring[7]=  "2,0,7,0,0,0,0,0,0,4,5,4,1,0,0,1";
gridstring[8]=  "1,0,6,0,0,1,0,0,0,1,0,0,2,2,0,2";
gridstring[9]=  "2,0,5,1,2,1,3,0,1,1,0,0,0,0,0,1";
gridstring[10]= "1,0,4,0,0,0,0,0,2,4,2,4,2,2,0,2";
gridstring[11]= "2,0,3,1,0,0,0,0,0,0,3,0,0,1,0,1";
gridstring[12]= "1,0,2,1,1,0,0,0,0,0,0,0,0,1,0,2";
gridstring[13]= "2,0,1,2,3,4,5,4,5,4,5,5,6,3,0,1";
gridstring[14]= "1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2";
gridstring[15]= "2,1,2,1,2,1,2,1,2,1,2,1,2,1,2,1";
// I managed to get 2D arrays is JS, but in CPP? Boah ej, welcome to the 21th century. so here we go,
// squeezing our 2D logic into a 1D array...
for(j=0;j<16;j++){// For j=0 To 15
 for(i=0;i<16;i++){//  For i=0 To 15
    cgrid[i+(j*16)] = (gridstring[j].substring((i*2) ,(i*2)+1)).toInt();
 }//  Next
}// Next

// some begnner notes:
// VAL() : mystring.toInt();

// ASC() : char character = '3'; // the letter '3'
//         int asciiValue = character; 

// or as a function for testing Strings rather than a char:
// int myASC(String s){
// char c=s.charAt(0);
// int ascii = c;
// return ascii;
// }

 // I wasn't even sure whether int arrays are nulled when declared, so just to be sure...
 for(i=0;i<pck_jobsize;i++){pck_jobT[i]=0;}

  //-------------------------------------------------------------------------------------


  // Initialising the UI will init the display too.
  display.init();
  //display.setContrast(255);

  display.flipScreenVertically();
//  display.setFont(ArialMT_Plain_10);

// XPLORA boot intro... show off...
  display.setFont(ArialMT_Plain_24);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.clear();
//  display.drawXbm(-16, 0, myimage_width, myimage_height, myimage_bits); // notice these constants are defined in the include images
// images are included as example, but not used.
  display.setColor(WHITE);

  display.setBrightness(50);

  // an interesting function, unused but left here for experimenting, xywh , float xdotstep, ydotstep
  //drawPattern(0,0,128,64,2.0,1.0);
  //for(i=0;i<15;i++)
  //{
  //  // drawPattern(0,i*4,128,4,1.5+((float)i/3.0),2.0);
  //}
  display.setColor(BLACK);
  display.drawString(62,36,"XPLORA");
  display.setColor(WHITE);
  display.drawString(64,38,"XPLORA");
  display.setFont(ArialMT_Plain_16);
  // display.setColor(BLACK);
  display.drawString(64,3,"   Initializing...");
  display.setFont(ArialMT_Plain_10);
  // display.setColor(BLACK);
  display.drawString(111,32,"V.1.0");
  display.display();
  display.setColor(BLACK);

 // completely unneccessary progressbar, so the user can read the logo
  for(i=0;i<101;i+=10){
   display.drawProgressBar(4, 24, 120, 8, i);
   delay(50);
   display.display();
  }
  delay(500);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.setColor(WHITE);

  // user input nitty gritty...
  pinMode(LED_BUILTIN, OUTPUT);
  //    pinMode(LED_BUILTIN, INPUT);  // funny story, the LED actually works as a light sensor with analogRead(), lol, but it's lowres and drifting
  pinMode(mytouchpin1, INPUT); //  used as touch sensor x-motion
  pinMode(mytouchpin2 , INPUT); // used as touch sensor y-motion
  pinMode(mytouchpin3 , INPUT); // used as touch sensor finger contact
  pinMode(mytouchpin4 , INPUT); // used as touch sensor separate "secure" leftclick button
  // calbrate touch pins (note, as the board heats up, actual baselevel my drift 10 or so points. 
  // In case of issues Reboot once the board reached operational temperature.)

  // IMPORTANT NOTE: USER MAY NOT TOUCH THE TOUCH-BUTTONS DURING BOOTING !!!
  
  // The device does a 40 ms calibration of the sensor baselevels during booting,
  // so don't touch (including the GPIOs on the board), or if you did, reboot. 
  
  for(int i=0;i<10;i++)
  {
   touch_baselevel1+=touchRead(mytouchpin1);
   touch_baselevel2+=touchRead(mytouchpin2);
   touch_baselevel3+=touchRead(mytouchpin3);
   touch_baselevel4+=touchRead(mytouchpin4);
   //  todo: find way to auto-calibrate frequently (aka how to know the user doesn't touch anything)
   delay(4);
  }
  touch_baselevel1/=10;
  touch_baselevel2/=10;
  touch_baselevel3/=10;
  touch_baselevel4/=10;
  // eo touch pins calibration
  // init screensaver fx
  for(int i=0;i<starn;i++) // init screensaver gfx
  {
    starx[i]=random(1,64)*((   floor(random(0,195)/100) *2)-1);
    stary[i]=random(1,32)*((   floor(random(0,195)/100) *2)-1);
    starxs[i]=1.01+(random(0,100.0)/1000.0);
  }
  // set display brightness
  display.setBrightness(50); // That is my personal choice, quite dim compared to default. 
  // However, default has huge overexposure, with white text on black being much beefier than 
  // black text on white (the latter almost disappearing). I think the range is 0 to 255.

  // Initialize true random numbers for packet IDs (so devices don't generate the same sequences)
  // Utilize noise on a touchpin to get some real randomness.... (second thought: may also use MAC address for random seed, or a mix of these two methods)
  int myrandomness=0;
  for(i=0;i<1000;i++)
  {
    lastapprox1=approx1;
    approx1=touchRead(mytouchpin1);
    if(approx1>lastapprox1) myrandomness+=random((approx1-lastapprox1)*random(5,15));
  }
  randomSeed(myrandomness);
  approx1=0;
  lastapprox1=0;
  // say hi to the user, telling him his mac-addres-based username that should be unique on every board (well, there are 12500 variations)
  display.clear();
  display.drawString(36,28, "Welcome "+username);
  display.display();
  delay(1000);
}
//--------------------------------------------------------------------------- end of setup()


// some lines kept here to lookup the synhax (see also keywords.txt in library folder)
//  display.setTextAlignment(TEXT_ALIGN_LEFT);
//  display.setFont(ArialMT_Plain_10);
//  display.drawString(0, 0, "Hello world");
//  display.drawStringMaxWidth(0, 0, 128, "Lorem ipsum\n dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore." );
//  display.setPixel(i, i);
//  display.setPixel(10 - i, i);
//  display.drawRect(12, 12, 20, 20);

//  display.fillRect(14, 14, 17, 17);
//  display.drawHorizontalLine(0, 40, 20);

//  display.drawVerticalLine(40, 0, 20);
//  display.setColor(WHITE);
//  display.drawCircle(32, 32, i * 3);
//  display.fillCircle(96, 32, 32 - i * 3);
//  display.drawProgressBar(0, 32, 120, 10, progress);

//  display.drawString(64, 15, String(progress) + "%");
//  display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
//  display.drawXbm(myscroll, 0, myimage_width, myimage_height, myimage_bits);
//  if(myscroll>-32){myscroll--;}



// LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL Loop
// LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL Loop
// LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL Loop
// LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL Loop
// LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL Loop
// LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL Loop

// main OS menu...
void loop() {
 int i=0; 
 long int ms=millis();
 myUpdateMouse();
 if(menu_click_event==0){menu_click_event=GETmenu_click_event();} // allows autostart of chat via setup()
 if(menu_click_event==1) // short click scroll menu items
 {
  screensaverT=ms+screensaverAfter;
  menuscroll++;
  if(menuscroll>menuscrollmax) {menuscroll=0;}
 }
 if(menu_click_event==2) // long click, start a specific app
 {
  screensaverT=ms+screensaverAfter;
  if(menuscroll==1) {myGames();} // games submenu
  if(menuscroll==2) {myLoraChat();} // chat control screen
  // here one can easily add his own apps, try use myGames() as a template.
 }
 
 display.clear();
 if(screensaverT > ms)
 { // draw main OS menu
  for(i=0;i<=menushow;i++)
  {
    display.drawString(2, 3+(i*20), menu[  i+max(0,(menuscroll-menushow))]  );
  }
  display.drawRect(   0, (menuscroll-max(0,(menuscroll-menushow)))*20, 120, 20);

 }// from if not in screensaver mode
 else
 {
  // myScreensaver(); // used here if main menu should not show background stars animation
 } // from else screensavermode?
 myScreensaver();// used here so main menu has background stars animation anyway.
 display.display();
 mydelay(10);
 menu_click_event=0;
}



// FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF functions








void myKeybTextInput(){
 // -------------------------------------------------------------------                myKeybTextInput()

 my_inp="";
 int inp_exit=0;
 int ms=0;
 int mhit2=0;
 int mhit=0;
 String mychar="";
 int mx=0;
 int my=0;
 int chx=0;
 int chy=0;
 int i=0;
 int j=0;
 int butt1state=0;
 int butt2state=0;
 int butt3state=0;
 int butt4state=0;
 int butt1T=0;
 int butt2T=0;
 int butt3T=0;
 int butt4T=0;
 int lastsent_pointer=0; // to parse last 10 sent messages and copy them to input for repeating a message
 int found_tmp=0; // used by LAS last typed message handler

 while(inp_exit==0){
  myUpdateMouse();
  ms=millis();//   ms=MilliSecs()
  mhit=0;
  if(approx4<(touch_baselevel4-150)){mhit=1;} // detect "mouseclick" on pad 4
  mychar="";
  mx=realmousex;
  my=realmousey;
  if((my>=22)&&(my<=63)){//   hovering over keyboard?
   if((mx>=0)&&(mx<=127)){//    
    chx=fmin( 9,fmax(0,floor((mx-4)/12)));// determine key under mouse
    chy=fmin(2,max(0, (my-22)/12));//     
    mychar=keyboard[chy+myShift].substring(chx,chx+1);
    if(mhit==1){//     If clicked
     my_inp=my_inp+mychar; // add to input string
     mhit=0;
     mydelay(200);
    }//     EndIf
   }//    EndIf
  }//   EndIf
 
  display.clear();

  // draw screen keyboard...
  display.setTextAlignment(TEXT_ALIGN_CENTER);

  for(i=0;i<3;i++){
   for(j=0;j<keyboard[0].length();j++){ 
    display.drawString(9+j*12, 47-(((2-i)*12)), keyboard[i+myShift].substring(j,j+1 ) );
   }
  }
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(128-display.getStringWidth(my_inp),13,my_inp); // actual line entered so far
  //display.drawString(16,0,"UP   DWN    OK   BCKS");
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(2, 0,"X"); // button labels
  display.drawString(16+7, 0,"LAS"); 
  display.drawString(16+37,0,"SHF");
  display.drawString(16+66,0,"OK");
  display.drawString(16+97,0,"BCKS");
  // 
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  
  //   ; manage button effects and exec button actions
  if(mhit==1){// button 4 "clicked"?
   if(my<12){// mouse over top row of buttons?
    if((mx>=0)&&(mx<8)){//  ------------------------------   // CANCEL / X
     inp_exit=1;
     // Cancel sending message, yet still allow user to store a newly typed message in LAS:
     // check whether this message is new, and if so, add it to LAS last typed messages array
     found_tmp=0;
     for(i=0;i<lastsent_max;i++){
      if(lastsent[i]==my_inp){ found_tmp=1; }
     }
     if((found_tmp==0) && (my_inp!=""))
     {
      // move old messages, lose the last, to copy the new one to the first
      for(i=lastsent_max;i>0;i--){ lastsent[i]=lastsent[i-1]; }
      lastsent[0]=my_inp;
     }
     mydelay(200);     
     my_inp=""; // prevent sending anything after returning to chat screen
    }
    if((mx>=8)&&(mx<38)){//  ------------------------------   // LAS last messages sent
      // copies one of the last 10 typed messages to the text input field of the keyboard screen
     butt1T=ms+200;
     butt1state=1;
     my_inp=lastsent[lastsent_pointer];
     lastsent_pointer++;
     if(lastsent_pointer>=lastsent_max){lastsent_pointer=0;}
     mydelay(300);
    }//     EndIf
    if((mx>=38)&&(mx<68)){//  --------------------------   // SHF shift for the 4 shift states
     butt2T=ms+200;
     butt2state=1;
     myShift=myShift+3;
     mydelay(300);
     if(myShift >9){
      myShift=0;
     }//      EndIf
    }//     EndIf
    if((mx>=68)&&(mx<96)){//   ----------------------------- // send / ok
     butt3T=ms+200;
     butt3state=1;
     inp_exit=1;
     // check whether this message is new, and if so, add it to LAS last sent messages array
     found_tmp=0;
     for(i=0;i<lastsent_max;i++){
      if(lastsent[i]==my_inp){ found_tmp=1; }
     }
     if((found_tmp==0) && (my_inp!=""))
     {
      // move old messages, lose the last, to copy the new one to the first
      for(i=lastsent_max;i>0;i--){ lastsent[i]=lastsent[i-1]; }
      lastsent[0]=my_inp;
     }
     mydelay(200);     
    }//     EndIf ok-button clicked
    if((mx>=98)&&(mx<128)){// ---------------------------------- // backspace
     butt4T=ms+200;
     butt4state=1;
     if(my_inp!=""){
      if(my_inp.length()==1){
       my_inp="";
      }else{
       my_inp=my_inp.substring(0,my_inp.length()-1);
      }//       EndIf
     }//      EndIf
     mydelay(300);
    }//     EndIf bckspace clicked
   }//    EndIf
  }//   EndIf
  // 
  if(my>22){display.drawRect(chx*12+3,23+chy*12,13,13);} // mouseover: rectangle over letter of keyboard
  if(ms>butt1T){butt1state=0;}
  if(ms>butt2T){butt2state=0;}
  if(ms>butt3T){butt3state=0;}
  if(ms>butt4T){butt4state=0;}

  // buttons on keyboard screen
  myShadedRect( 0,0, 7,12);
  myShadedRect( 8,0,29,12);
  myShadedRect(38,0,29,12);
  myShadedRect(68,0,29,12);
  myShadedRect(98,0,29,12);
  /*
  display.drawRect( 0,0, 7,12);
  display.drawRect( 8,0,29,12);
  display.drawRect(38,0,29,12);
  display.drawRect(68,0,29,12);
  display.drawRect(98,0,29,12);
  */
  myDrawMouse();
  display.display();
  mydelay(10);
  // 
 }//  Wend
 mydelay(100);
}// End Function


void myShadedRect(int x, int y, int w, int h)
{
display.drawHorizontalLine(x+2,y,w-2);
display.drawVerticalLine(x+w,y+1,h-2);
for(int i=1;i<w-1;i+=2){display.setPixel(x+i+1,y+h-1);}
}







void myDrawMouse()
{
  int x=realmousex;
  int y=realmousey;
  if((millis()& 128)==0){
    display.setColor(WHITE);
  }else{
    display.setColor(BLACK);
  }
  display.drawLine(x,y,x+3,y+6);
  display.drawLine(x,y,x+6,y+3);
  display.setColor(WHITE);
}

int myHexToInt(String h) // for up to 7 hex digits
{
  if(h=="") return 0;
  int i;
  int val=0;
  int val2=0;
  for(i=0;i<h.length();i++)
  {
    String hex1=h.substring(i,i+1);
    val=0;
    // yeah I know this is very unelegant. But hey, it works.
    if(hex1=="0") val=0;
    if(hex1=="1") val=1;
    if(hex1=="2") val=2;
    if(hex1=="3") val=3;
    if(hex1=="4") val=4;
    if(hex1=="5") val=5;
    if(hex1=="6") val=6;
    if(hex1=="7") val=7;
    if(hex1=="8") val=8;
    if(hex1=="9") val=9;
    if(hex1=="a" || hex1=="A") val=10;
    if(hex1=="b" || hex1=="B") val=11;
    if(hex1=="c" || hex1=="C") val=12;
    if(hex1=="d" || hex1=="D") val=13;
    if(hex1=="e" || hex1=="E") val=14;
    if(hex1=="f" || hex1=="F") val=15;
    val2=val2*16 + val;
  }
  return val2;
}




void myLoraChat() // ---------------------------------------------------------------- myLoraChat()
{
 int packet_id=0;
 String pckid_string= "";
 String msg="";
 int chat_scrollmode=0;
 int chat_exit=0;
 int ms=0;
 int i=0;
 int mx=0;
 int my=0;
 chatxo=0;
 int mhit=0;
 int myrow=0;
 int old_myrow=0;
 int scroll_offset=0;
 int bacon_rand;
 baconTestT=millis()+30000+random(0,10000); // send beacon every 30 to 40 secs (if is_beaconsender is 1)

 while(chat_exit==0){
  myUpdateMouse();
  mhit=0;
  if(approx4<touch_baselevel4-150){mhit=1;}
  mx=realmousex;
  my=realmousey;
  ms=millis();
  if(ms>baconTestT){//  ; auto-send LORA bacon messages
   baconTestT=ms+30000+random(0,10000); // schedule next beacon

  if(is_beaconsender==1){
  // send random bacon lora........................
      // generate unique packet ID
      packet_id=random(0xFFFFFFF);
      pckid_string= String(packet_id, HEX); // turn into 7 byte hex string
      while(pckid_string.length()<7) {pckid_string="0"+pckid_string;} // add leading zeros if neccessary
      pck_stack[pck_stackpointer]=packet_id; // remember this ID in "sent packets" stack
      pck_stackpointer++;
      if(pck_stackpointer>=pck_stacksize){ pck_stackpointer=0;} // we just "carrouel" through this stack, kind of "first in, last driven over, or something"
      bacon_rand=random(0,baconn);
      msg="XPL0"+pckid_string+">"+username+":"+bacon[bacon_rand]; // assembling a proper XPLORA data packet (of type 0, "speak")
      LoRa.beginPacket();
      LoRa.print(msg);
      LoRa.endPacket();

      for(i=0;i<chatn;i++){//      add line to local chat array
       chat[i]=chat[i+1];
      }
      chat[chatn]=username+">"+bacon[bacon_rand];
      screensaverT=ms+screensaverAfter; // prevent screensaver ...
   }// EndIf beaconsender?
  }//   EndIf time for beacon?
 
  if(mhit==1){//     ; buttons check
   screensaverT=ms+screensaverAfter; // prevent screensaver ...
  
   if(my>55){//   mouse over bottom buttons row?
    if((mx>0)&&(mx<=30)){//                         button 1, speak
     mydelay(300);
     mousex=64;
     mousey=32;

     myKeybTextInput();
     screensaverT=millis()+screensaverAfter;
     if(my_inp!=""){//
      for(i=0;i<chatn;i++){//
       chat[i]=chat[i+1];//
      }//      Next
      chat[chatn]=username+">"+my_inp;//
     
      //-----------------------------------  send user input!
      packet_id=random(0xFFFFFFF); // create uniqie packet id
      pckid_string= String(packet_id, HEX);
      while(pckid_string.length()<7) {pckid_string="0"+pckid_string;}
      pck_stack[pck_stackpointer]=packet_id;
      pck_stackpointer++;
      if(pck_stackpointer>=pck_stacksize){ pck_stackpointer=0;}

      // int to hex:
      // int n = 255;
      // String hexString = String(n, HEX); 
      // hex to int:
      // int myHexToInt(String h)
     
      msg="XPL0"+pckid_string+">"+username+":"+my_inp;
      LoRa.beginPacket();
      LoRa.print(msg);
      LoRa.endPacket();
      blinkLED();
      mydelay(50);
      blinkLED();
      //-----------
      chatxo=0;
      chat_scrollmode=0;
     }//     EndIf
     mousex=64;
     mousey=52;
    }//    EndIf
    if((mx>32)&&(mx<=62)){//                   button 2, yell
     //
     // this identical like the previous button, except for "XPL1" rather than "XPL0" in the packet header.
     mydelay(300);
     mousex=64;
     mousey=32;

     myKeybTextInput();
     // glowLED();
     screensaverT=millis()+screensaverAfter;
     if(my_inp!=""){//
      for(i=0;i<chatn;i++){//
       chat[i]=chat[i+1];//
      }//      Next
      chat[chatn]=username+">"+my_inp;//
     
      //-----------------------------------  send YELL user input! (notice "XPL1" in header)
      packet_id=random(0xFFFFFFF);
      pckid_string= String(packet_id, HEX);
      while(pckid_string.length()<7) {pckid_string="0"+pckid_string;}
      //const int pck_stacksize=100;
      pck_stack[pck_stackpointer]=packet_id;
      pck_stackpointer++;
      if(pck_stackpointer>=pck_stacksize){ pck_stackpointer=0;}
     
      msg="XPL1"+pckid_string+">"+username+":"+my_inp;
      LoRa.beginPacket();
      LoRa.print(msg);
      LoRa.endPacket();
      blinkLED();
      mydelay(50);
      blinkLED();
      //-----------
      chatxo=0;
      chat_scrollmode=0;
     }//     EndIf
     mousex=64;
     mousey=52;
    }//    EndIf
    if((mx>64)&&(mx<=94)){//                       button 3, scroll toggle
     chat_scrollmode=(chat_scrollmode+1) & 1; // toggle scrollmode, mouse over top or bottom line scrolls
     mydelay(200);
     screensaverT=ms+screensaverAfter;
    }//    EndIf
    if((mx>96)&&(mx<=128)){//                       button 4, exit chat screen, return to main OS menu
     chat_exit=1;
     screensaverT=ms+screensaverAfter;
    }//    EndIf 
   }//   EndIf
  }//  EndIf
 
  display.clear();
  // List last chat messages...
  // handle chat history scrolling: when scroll is toggled on, user
  // can scroll through the last 100 chat messages
  old_myrow=myrow;
  myrow=(min(5,max(0,my/9)) ); // determine chat row under mouse
  // 
  if(chat_scrollmode==1){//
   if(myrow==0){//               scroll up
    scroll_offset=scroll_offset+1;
    if(scroll_offset>chatn-6){// scroll down
     scroll_offset=chatn-6;
    }else{
     for(i=0;i<15;i++){myUpdateMouse();mydelay(10);}
    }//   EndIf
   }//  EndIf
   if(my>55){// 
    scroll_offset=scroll_offset-1;
    if(scroll_offset<0){
     scroll_offset=0;
    }else{
     for(i=0;i<15;i++){myUpdateMouse();mydelay(10);}
    }//   EndIf
   }//  EndIf
  }else{// chat_scrollmode is not 1
   scroll_offset=0;
  }// EndIf
 
  if(old_myrow!=myrow){chatxo=0;}//  register change of line under mouse (in case of incoming message)
  //   ; allow horizontal scrolling of long (longer than screen width) textlines (if mouseover)
  for(i=chatn;i>=chatn-5;i--){//
   int real_chatxo=0;
   if((5-(chatn-i))==myrow){//
    int striwi=display.getStringWidth(chat[i-scroll_offset]);//
    if(striwi>128){//
     chatxo=chatxo+1;
     if(chatxo> striwi+4){chatxo= -128;}//
    }else{
     chatxo=0;
    }//     EndIf
    real_chatxo=chatxo;
   }//    EndIf
   // draw actual chat screen text lines
   if(screensaverT>ms){  display.drawString(-real_chatxo, 45-(((chatn-i))*9), chat[i-scroll_offset]  );}
  }//   Next


  //   ; buttons gfx
  if(screensaverT>ms){ 
   for(i=0;i<=3;i++){//
    display.fillRect(i*32,57,30,9);
   }//   Next
   display.setColor(BLACK);

   if(chat_scrollmode==1){//
    display.drawRect(65,57,28,8);//
   }//   EndIf
   display.drawString(0, 54, "Speak   Yell   Scroll   EXIT" );
   display.setColor(WHITE);

   myDrawMouse();
  } // endif non screen saver?
  else {myScreensaver();}
  display.display();//
  mydelay(30);

 }//  Wend
 mydelay(200);
} // end function




void blinkLED(){
  digitalWrite(LED_BUILTIN, HIGH);
  mydelay(100);
  digitalWrite(LED_BUILTIN, LOW);
  }


void glowLED(){ // using some pseudo-PWM to dim the LED, but it doesn't get very dim
  int i=0;
  for(i=0;i<13;i++)
  {
  digitalWrite(LED_BUILTIN, HIGH);
  mydelay(1);
  digitalWrite(LED_BUILTIN, LOW);
  mydelay(7);
  }
}

void myLEDon(){digitalWrite(LED_BUILTIN, HIGH);}
void myLEDoff(){digitalWrite(LED_BUILTIN, LOW);}




// The following function lies at the core of the OS, it substitutes the delay() function,
// but rather than to just "delay" it listens to LoRa packets, and in case one is received,
// writes it into the local chat messages array. If it receives a "yell" type message, it
// checks whether it re-broadcasted this packet already, and if not, it will schedule it
// for re-broadcasting once, and write the packet ID to the stack of already sent packets.
// Additionally it checks whether any packet is scheduled for re-broadcasting at the current moment,
// and if so it sends this packet and removes its scheduling from the packet jobs stack.

// Note: do not use any function call inside this function that by itself calls this function
// (example: blinkLED() ), as it would be a recursive call and probably cause a stack overflow crash.
void mydelay(int t)
{  
 int pck_id=0;
 int found=0;
 int pck_type=0; 
 int i=0;
 int packetSize = 0;
 int ms=millis()+t;
 String msg="";
 while(millis()<ms){
  packetSize = LoRa.parsePacket();
  if (packetSize) {
   // Serial.print("Received packet '");
   msg="";
   while (LoRa.available()) {
     msg+=(char)LoRa.read();
   }
   // investigte packet...
   if(msg.length()>12) // minimal length for XPLORA packet
   {
    if(msg.substring(0,3)=="XPL") // is XPLORA data packet header
    {
     pck_type=0; 
     if(msg.substring(3,4)=="0") pck_type=0; // packet is of type "speak"
     if(msg.substring(3,4)=="1") pck_type=1; // or "yell", unlike "speak" to be re-broadcasted once.

     if(pck_type==1)
     {
      // search sent-packets ID stack, did we send this already?
      pck_id=myHexToInt(msg.substring(4,11));
      found=0;
      for(i=0;i< pck_stacksize; i++)
      {
        if(pck_stack[i]==pck_id) found=1;
      }
      if(found==0) // not sent already, re-broadcast!
      {
        // we have found out we have to re-broadcast this packet, but sending it right now
        // would cause havoc when multiple stations would repeat it right now, at the same time.
        // So we schedule it for sending, using a random delay.
        pck_jobT[pck_jobcount]=millis()+100+random(2000);  // point in time to send it
        pck_job[pck_jobcount]=msg; // make copy of the packet
        pck_jobcount++;
        if(pck_jobcount>(pck_jobsize-1)) pck_jobcount=0; // yet another "stack" we just cycle through, overwriting oldest entries as they should be obsolete anyway.
        // (that is currently 100 packets scheduled at max)
        
        pck_stack[pck_stackpointer]=pck_id;  // and remember it in a other stack, so we re-broadcast it only once
        pck_stackpointer++;
        if(pck_stackpointer>=pck_stacksize){ pck_stackpointer=0;}
      }else{ // found=1, ignore this packet as we sent it already
       //... hence do nothing here
      }
    } // endif pck type 1?
    if((pck_type==0) || ( (pck_type==1) && (found==0) ) ) // is it type 0 or type 1 and new? Then show it onscreen etc.
      {
       for(i=0;i<chatn;i++){//
        chat[i]=chat[i+1];//
       }//    Next
       chat[chatn]=msg.substring(12);//    add received msg to chat string array
       chatxo=0;
       // print RSSI of packet, radio signal strength indicator?
       // Serial.print("' with RSSI ");
       // Serial.println(LoRa.packetRssi());
       screensaverT=millis()+screensaverAfter;
      }
  } // proper XPLn packet header? 
 } // packet length >12?


 } // endif whether lora received any packet

  // check schedule whether we must re-broadcast a packet...------------------------------------------------
  int ms2=millis();
  String msg2="";
  for(i=0;i<pck_jobsize;i++)
  {
   if((pck_jobT[i]!=0)&&(ms2 > pck_jobT[i])) // if T=0 then it's nothing. Else, if millis>T then it's time for sending this msg.
   {
    // rebroadcast!
    msg2=pck_job[i];
    myLEDon();
    LoRa.beginPacket();
    LoRa.print(msg2); // actually re-broadcast it!
    LoRa.endPacket();
    delay(1);
    myLEDoff();
    pck_jobT[i]=0; // "delete" this job from packet-sending-schedule
   }
  }
  delay(1);
 } // wend
}
//-------------------------------------------------------------------------------------------------  



void myScreensaver()
{
     // screensaver starfield
   for(int i=0;i<starn;i++)
   {
    int ox=starx[i];
    int oy=stary[i];
    starx[i]*=(starxs[i]*1.02);
    if((starx[i]<-80)||(starx[i]>80)){ starx[i]=random(1,64)*((   floor(random(0,199)/100) *2)-1);stary[i]=random(1,32)*((   floor(random(0,195)/100) *2)-1);ox=starx[i];oy=stary[i];}
    stary[i]*=(starxs[i]*1.0);
    if((stary[i]<-48)||(stary[i]>48)){ starx[i]=random(1,64)*((   floor(random(0,195)/100) *2)-1);stary[i]=random(1,32)*((   floor(random(0,195)/100) *2)-1);ox=starx[i];oy=stary[i];}
    display.drawLine(64+starx[i],32+stary[i],64+ox,32+oy);
   }
   mydelay(30);
}


void myUpdateMouse()
{
  // user input, touchpin 12, 14, 4 ,15, see deinitions in globals section
  lastapprox1=approx1;
  lastapprox2=approx2;
  approx1=touchRead(mytouchpin1);
  approx2=touchRead(mytouchpin2);
  approx3=touchRead(mytouchpin3);
  approx4=touchRead(mytouchpin4);
  int mi=1;
  int ma=150;

  // mouse navigation...  
  // finger pad contact? In case of "jump-back-jerkiness" problems of the mouse try values between 25 and 45.
  if(approx3 <(touch_baselevel3-35)) // finger contact?
  {
   screensaverT=millis()+screensaverAfter;
   // any ramp up or down of distance x sensor?
   if ( (approx1 < (touch_baselevel1-10)) && (abs(approx1-lastapprox1) >mi) && (abs(approx1-lastapprox1) <ma)  )
   {
    int tmpx=(approx1-lastapprox1)/4;
    if(tmpx>0){tmpx*=1.4;}
    mousex-=tmpx; // this divisor affects mouse sensitivity...
//    mousex-=(approx1-lastapprox1)/4; // this divisor affects mouse sensitivity...
    if(mousex>129) mousex=129;
    if(mousex<-2)   mousex=-2;
   }
   // any ramp up or down of distance y sensor?
   if ( (approx2 < (touch_baselevel2)-10) &&   (abs(approx2-lastapprox2) >mi) && (abs(approx2-lastapprox2) <ma)  )
   {
    int tmpy=(approx2-lastapprox2)/6;
    if(tmpy>0){tmpy*=1.4;}
     mousey-=tmpy; // this one too
//     mousey-=(approx2-lastapprox2)/8; // this one too
    if(mousey>65) mousey=65;
    if(mousey<-2)   mousey=-2;
   }
  }  
  else // no finger contact
  {
  }
  // smoothing mouse movement
  realmousex-=((realmousex-mousex)/5); // this divisor affects the speed at which the smoothed mouse catches up with the real mouse coords
  realmousey-=((realmousey-mousey)/5); // (so the name may be confusing, but anyway :-) ) 
}








int GETmenu_click_event()
{
 // only used in main OS menu so far.
 //  ; one button input, short=1, long=2, very long=3 press detection, returns menu_click_event 0,1,2,3
 int menu_click_event=0;
 int mynow=0;
 if(approx4 < touch_baselevel4-150)
 {
  if(button1_state==0)
   {
     button1_lastT=millis();
     button1_state=1;
   }
 }
 else 
 {
  if((button1_state == 1)&&(button1_lastT != 0))
   {
    mynow=millis();
    if((mynow-button1_lastT)>300) // is long or extremely long click
     {
      if((mynow-button1_lastT)>2000) {menu_click_event=3;} //is extremely long-click
      else{menu_click_event=2;} // long click
     }
     else
     {
      menu_click_event=1; // is short click
     }
     button1_state=0;
  }
 }
 return menu_click_event;
}


void drawPattern(float x,float y,float w,float h,float p, float p2)
{ // left here for experimentation, unused
  float i_f=0;
  float j_f=0;
  if(p<=0)p=2;
  for(j_f=y;j_f<(y+h);j_f+=p2){
//   for(i_f=x;i_f<(x+w);i_f+=p){  display.setPixel(((int)j_f & 1)+ (int)i_f, (int)j_f );}  
   for(i_f=x;i_f<(x+w);i_f+=p){  display.setPixel( (int)i_f, (int)j_f );}  
  }
}

void myGames()
{
 // a simple 4x4 choice menu screen
 int myexit=0;
 int i;
 int j;
 int game_hover=0;
 String tit="";
 while(myexit != 1)
 {
  myUpdateMouse();
  display.clear();
  for(j=0;j<4;j++)
  {
   for(i=0;i<4;i++)
   {
    tit=gametitles[i+(j*4)];
    if(tit!="")
    {
     display.drawRect(i*32,j*16,32,16);
     display.drawString(2+i*32,2+j*16,tit);
    } // endif
   } // next i
  } // next j

  myDrawMouse();
  display.display();
  mydelay(10);
  game_hover=floor(realmousex/32)+( floor(realmousey/16)*4  );
  if(approx4<touch_baselevel4-150)// yeah, it's cryptic... it means "if user clicked button"
  {
    if(game_hover==0){myGamePong();}
    if(game_hover==1){myGameDoom();}
    if(game_hover==15){myexit=1;}
  }//endif
 }//wend
 mydelay(300);
}// eo function



void myGamePong()
{
 // a simple pong-oid game, not finished in any way 
 float ballx=0;
 float bally=0;
 float ballxs=4;
 float ballys=4;
 int playerx=32;
 int score=0;
 int myexit=0;
 int i;
 int j;
 int game_hover=0;
 String tit="";
 mydelay(300);
 while(myexit != 1)
 {
  myUpdateMouse();
  display.clear();

  display.fillCircle(ballx,bally,3);
  display.drawLine(playerx-8,63,playerx+8,63);
  display.drawString(0,0,"SCORE "+(String)score);
  display.drawString(72,0,"HI "+(String)hiscore);
  display.drawRect(0,0,128,65);
  ballx+=(ballxs/10);
  if( (ballx<0) || (ballx>127)) {ballxs=-ballxs; ballys=(ballys*  (1.0+random(100)/200.0)  );}

  bally+=(ballys/10);
  if( bally<0 ) {ballys=-ballys; ballxs=(ballxs*1.0+(random(100)/200.0));}

  if( bally>62 )
  {
    if( (ballx>=(playerx-9)) && (ballx<=(playerx+13))) // catched it
    {
      ballys=-ballys;
      ballxs=(ballxs*1.0+(random(100)/200.0));
      ballxs*=1.02;
      ballys*=1.02;
    }
    else // missed it
    {
      ballx=0;
      bally=0;
      ballxs=4+(float(random(100))/30.0);
      ballys=4+(float(random(100))/30.0);
      if(score>hiscore)hiscore=score;
      score=0;
    }
  }

//  myDrawMouse();
  display.display();
  mydelay(10);
  // game_hover=floor(realmousex/32)+( floor(realmousey/16)*4  );
  if(approx4<touch_baselevel4-150){myexit=1;}
  if(approx2<touch_baselevel2-150) // steer left
  {
   playerx-=2;
   if(playerx<0){playerx=0;}
  }
  if(approx1<touch_baselevel1-150) // steer right
  {
   playerx+=2;
   if(playerx>127){playerx=127;}
  }

//  ballxs*=1.01;
//  ballys*=1.01;
  score++;
 }//wend



 mydelay(300);
}// eo pong









// -------------------------------------------------------------------------------------- 3D Maze Game
void myGameDoom()
{
 int game_T=millis()+30;
 float wspeed=50; //walk speed, the higher, the slower, 5 to 100
 int myexit=0;
 int i;
 int j;
 int game_hover=0;
 String tit="";
 float mxs=0;
 int old_realmousex=realmousex;
 mydelay(300);
 while(myexit != 1)
 {
  myUpdateMouse();
  
  display.clear();

  if(approx3<touch_baselevel3-150)// walk forward
  {
   pxold=px;
   pzold=pz;
   px=px+(mysin(angle)/wspeed);
   pz=pz+(mycos(angle)/wspeed);
   sliding();    
  }
  angle=(angle-mxs);// ; //use mouse to steer
  if(angle<  0)angle+=360;
  if(angle>360)angle-=360;
  raycast(); 

  if(approx1<touch_baselevel1-150){ // turn left
   if(mxs<5)mxs+=.25;
  }
  if(approx2<touch_baselevel2-150){ // turn left
   if(mxs>-5)mxs-=.25;
  }

  mxs*=0.9;
  
  old_realmousex=realmousex;
  display.display();
  mydelay(1);
  while(millis()<game_T){
   mydelay(1);
  }
  game_T=millis()+15;
  game_hover=floor(realmousex/32)+( floor(realmousey/16)*4  );
  if(approx4<touch_baselevel4-150)
  {
    if(game_hover==15){myexit=1;}
    myexit=1;
  }//endif
 }//wend
 mydelay(300);
  
}
// eo doom main ---------------------


void sliding(){
 // sliding collision handler for "doom" 3D maze game.
 float premind=0;
 int pxz_i=0;
  pxz_i=(  floor(px)+(floor(pz)*16)  );
 if(cgrid[pxz_i]!=0){
  premind=px;
  px=pxold;
  pxz_i=(  floor(px)+(floor(pz)*16)  );
  if(cgrid[pxz_i]!=0){
   px=premind;
   pz=pzold;
   pxz_i=(  floor(px)+(floor(pz)*16)  );
   if(cgrid[pxz_i]!=0){
    px=pxold;
   }//     EndIf
  }//    EndIf
 }//   EndIf
}// End Function



 
void raycast(){ // measure distance of camera to whatever a pixel column displays, use that distance for perceived wall height calculation.
 float rayx=0;
 float rayz=0;
 int i=0;
 int i2=0;
 float stepx=0;
 float stepz=0;
 int count=0;
 int touched=0;
 int rayxz_i=0;
 int near_edge=0;
 float h=0;
 float crit=0;
 float rayx_rest=0;
 float rayz_rest=0;
 float old_rayx_rest=0;
 float old_rayz_rest=0;
 for(i=-64;i<64;i++){//  For i=-64 To 63
  rayx=px;
  rayz=pz;
  stepx=mysin(angle+0.375*i)/15.0;
  stepz=mycos(angle+0.375*i)/15.0;
  count=0;
  touched=0;
  while((touched==0) && (count<1000)){//   While touched=0 And count<1000
   rayx=rayx+stepx;
   rayz=rayz+stepz;
   rayxz_i=(int)(rayx)+((int)(rayz)*16);
   count=count+1;
   if(cgrid[rayxz_i]!=0){//    If cgrid(rayxz_i)<>0 Then
    touched=cgrid[rayxz_i];
   }//    EndIf
  }//   Wend
  if(touched!=0){//   If touched<>0 Then 
   near_edge=0; // some attempts to detect wall edges
   rayx_rest=((rayx+.0)-(floor(rayx+.0)));
   rayz_rest=((rayz+.0)-(floor(rayz+.0)));
 
   if(  (  ((old_rayx_rest<=0.4) && (rayx_rest>=0.6))  ||  ((old_rayx_rest>=0.6) && (rayx_rest<=0.4))  )  ||  (   ((old_rayz_rest<=0.4) && (rayz_rest>=0.6))   ||   ((old_rayz_rest>=0.6) && (rayz_rest<=0.4))  )  ) {
    near_edge=1;
   }//    EndIf

   crit=fmax(0.07,(1000.0/count)/1500.0);
   if(  ((rayx_rest< crit) || (rayx_rest> (1.0-crit))) && ((rayz_rest< crit) || (rayz_rest> (1.0-crit)))  ) {
    near_edge=1;
   }//    EndIf
   old_rayx_rest=rayx_rest;
   old_rayz_rest=rayz_rest; 
   h=(8000.0/count)/10.0; //wall height, seen from here
   if(h>33) h=33;//    draw wall, edges only:
   if(near_edge==1){// 
    display.drawVerticalLine(64-i,32-h,h+h+2);
   }else{//    Else
    display.setPixel(64-i,32-h); // floor
    display.setPixel(64-i,33+h); // ceiling
   }//    EndIf
   for(i2=32;i2>=h;i2-=2){//
     display.setPixel(64-i,(33-(i & 1))+i2);
   }//    Next
  }//   EndIf
 }//  Next
}// End Function


float mysin(float a){  // Euler / radians converting angles
  return sin(a/myDegRel);
}
float mycos(float a){
  return cos(a/myDegRel);
}
 
// ----------------------------------------------------------------------- end of 3d maze game
