#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_ADS1015.h>
#include <SimpleTimer.h>
#define SCREEN_WIDTH 128 //--> Lebar OLED Display
#define SCREEN_HEIGHT 64 //--> Tinggi OLED Display
#define OLED_RESET     -1 // Reset pin

// We are going to include the strings, file read, stream and standard output
#include <fstream>
#include <iostream>
#include <sstream> //std::stringstream
#include <string>

//inlude the ESP wireless and webserver headers
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
//ESP Web Server Library to host a web page
#include <ESP8266WebServer.h>


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
unsigned long previousMillisGetHR = 0; //--> menyimpan waktu millis ketika sudah diperbaharui (heartbeat)
unsigned long previousMillisResultHR = 0; //--> menyimpan waktu millis ketika sudah diperbaharui (BPM)
const long intervalGetHR = 20; //--> Interval pebacaan.
const long intervalResultHR = 10000; //--> Interval pembacaan setiap 10 detik
int PulseSensorSignal; 
const int PulseSensorHRWire = A0; 
int UpperThreshold = 540; 
int LowerThreshold = 520; 
int cntHB = 0; 
boolean ThresholdStat = true; //--> variabel untuk mentriger detak jantung
int BPMval = 0; 
int x=0; 
int y=0; 
int lastx=0;
int lasty=0;
int hour_ = 12;
int minute_ = 45;
int seconds_ = 0;
unsigned long previousMillis = 0;
unsigned long interval = 1000;
int mode = 0; // 0 = Time; 1 = Temp, 2 = Pulse
unsigned long previousMillisBTN = 0;
int btnTimeHigh = 900;
int btnTimeLow = 300;

SimpleTimer timer;

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
//Adafruit_ADS1015 ads;     /* Use thi for the 12-bit version */
float RefVoltage = 3.2;
int16_t adc0, adc1, adc2, adc3;
const int PulseWire = A0;

boolean BUT1, BUT2, Both, BUT1SENSE, buttonState, BUT2SENSE;
int BTNholder = 0;
bool buttonHeld = false;
unsigned long buttonDownTime = 0;

//local network details
//SSID and Password of your WiFi router
const char* ssid = "wlan1";
const char* password = "00001111";
//Declare a global object variable from the ESP8266WebServer class.
ESP8266WebServer server(80); //Server on port 80
//std::string main_page = "";

//---------------------------------------------------------------
//Our HTML webpage contents in program memory
char main_page[] = R"=====(
<!DOCTYPE html>
<html lang="en">
    <head>
        <meta charset="utf-8">
        <meta http-equiv="X-UA-Compatible" content="IE=edge">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">


        <!-- fontawesome cdn -->
        <link rel="stylesheet" href="https://pro.fontawesome.com/releases/v5.10.0/css/all.css"
            integrity="sha384-AYmEC3Yw5cVb3ZcuHtOA93w35dYTsvhLPVnYs9eStHfGJvOvKxVfELGroGkvsg+p" crossorigin="anonymous" />
    
        <meta name="description" id="meta_description"
            content="Local area connection to collect the data from various Tempulse devices" />
        <meta name="keywords"
            content="Tempulse Data Dashboard " />
    
    <title>Tempulse Data Center</title>

    <script src="https://www.gstatic.com/firebasejs/4.5.0/firebase.js"></script>
    <script>
    // Initialize Firebase
    // TODO: Replace with your project's customized code snippet
    var config = {
        apiKey: "<API_KEY>",
        authDomain: "<PROJECT_ID>.firebaseapp.com",
        databaseURL: "https://<DATABASE_NAME>.firebaseio.com",
        storageBucket: "<BUCKET>.appspot.com",
        messagingSenderId: "<SENDER_ID>",
    };
    firebase.initializeApp(config);
    </script>
</head>
<body>
    <style>
        .the_tables{
            border: 2px solid #000;
        }
        .the_tables tr{
            width: 100%;
        }
        .the_tables tr td{
            border: 1px solid #000;
            max-width: 50%;
            min-width: 12%;
        }
        h3 i {
            vertical-align: text-top;
        }
    </style>
    <center>
        <div>
            <canvas>
    
            </canvas>
            <h3> <i class="fa fa-heart" id="j268i67" style="font-size: 54px;"></i> Pulse:  ###Pulse </h3>
        </div>
    
        <div class="the_temp">
            <h3> <i class="fa fa-thermometer-half" id="z6fejk4"  style="font-size: 54px;"> </i> Temperature: ###Temp </h3>
        </div>
    
    
        <div class="the_bat">
            <h3> <i class="fa fa-battery-full" id="sh97z3r" style="font-size: 54px" ></i> Battery: ###Battery </h3>
        </div>
    
        <!-- <div class="the_tables">
            <table>
                <tr>
                    <td>Patient id</td>
                    <td>Temp</td>
                    <td>Pulse</td>
                    <td>Date</td>
                    <td>Time</td>
                    <td>Status</td>
                </tr>
            </table>
        </div> -->
        
    </center>

</body>
</html>
)=====";
int once_done = 0;
//String main_page = "<DOCTYPE html> \
//<html> \
//<head> \
//  <title>ESP8266_Temp</title> \
//</head> \
//<body> \
// The Temperature is 34.9 \
//</body> \
//</html>";

//----------------------------------------'HardyIndustriesLogo', 128x64px
const unsigned char HardyIndustriesLogo [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x1f, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x3f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x7f, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x7f, 0xfe, 0x7f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x3f, 0xfc, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x1f, 0xc0, 0x03, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x0f, 0xc0, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x1f, 0xe0, 0x0f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x3f, 0xfc, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x7f, 0xfe, 0x7f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x7f, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x3f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x18, 0x1f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf8, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x0f, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf8, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xf0, 0x1f, 0x03, 0x80, 0x03, 0x00, 0x00, 0x00, 0xf0, 0xf8, 0x7c, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xf0, 0x0f, 0xc3, 0xff, 0x83, 0x7e, 0x3c, 0xf8, 0x71, 0xfc, 0x3f, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xf0, 0x3d, 0xc3, 0xff, 0xc3, 0xff, 0x1d, 0xb9, 0xf1, 0xfc, 0xf7, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xf0, 0x3d, 0xe3, 0xcf, 0xe3, 0xc7, 0xbf, 0xb8, 0xf1, 0xc0, 0xf7, 0x80, 0x00, 
  0x00, 0x00, 0x00, 0xf0, 0x3f, 0xc7, 0x81, 0xc3, 0xff, 0x9f, 0x38, 0xf0, 0xf0, 0xff, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0xf0, 0x7c, 0x07, 0x81, 0xe3, 0xef, 0x9f, 0x79, 0xe3, 0x79, 0xf0, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0xf0, 0x3c, 0x07, 0x8d, 0xc7, 0xcf, 0x1c, 0x78, 0xe7, 0xbc, 0xf0, 0x00, 0x00, 
  0x00, 0x00, 0x01, 0xf0, 0x3e, 0x07, 0x9f, 0xc7, 0xe7, 0x1e, 0xf8, 0xe7, 0x38, 0xf8, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x30, 0x1e, 0x37, 0x8d, 0xf6, 0x7e, 0x0f, 0xf9, 0xe7, 0x38, 0x78, 0xc0, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x07, 0x00, 0x00, 0x78, 0x01, 0xf0, 0x3f, 0xc0, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
//---------------------------------------------------Logo Hati-----------------------------------
const unsigned char Heart_Icon [] PROGMEM = {
  0x00, 0x00, 0x18, 0x30, 0x3c, 0x78, 0x7e, 0xfc, 0xff, 0xfe, 0xff, 0xfe, 0xee, 0xee, 0xd5, 0x56, 
  0x7b, 0xbc, 0x3f, 0xf8, 0x1f, 0xf0, 0x0f, 0xe0, 0x07, 0xc0, 0x03, 0x80, 0x01, 0x00, 0x00, 0x00
};

void handleRoot() {
 Serial.println("You called root page");
 String s = main_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}

void setup() {
 // Serial.begin(9600); 
 Serial.begin(115200);
 Wire.begin();
 //boolean BUT1;
 //boolean BUT2;



//  std::ifstream inFile;
// inFile.open("ESP8266.txt"); //open the input file
// std::stringstream strStream;
// strStream << inFile.rdbuf(); //read the file
// main_page = strStream.str(); //str holds the content of the file
// Serial.print("Now is the time: ");
// Serial.print(main_page.c_str());
 

  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)    // activate this if you are using a 5V sensor, this one should  be used with Arduino boards
  ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV               // As the sensor is powered up using 3.3V, this one should be used with 3.3v controller boards
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); //--> Jangan proses, loop selamanya
  }  
  //----------------------------------------
  display.clearDisplay(); 
  display.drawBitmap(0, 0, HardyIndustriesLogo, 128, 64, WHITE);
  display.display();
  delay(3000); 
  //----------------------------------------
 display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  display.setCursor(0, 12);
  display.print(" Loading, Please wait");
  
  /*display.setCursor(0, 22);
  display.print("     10  seconds");
  
  display.setCursor(0, 32); 
  display.print("       to get");
  
  display.setCursor(0, 42);
  display.print(" the Heart Rate value");*/
  
  display.display(); 
  delay(3000);

  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  // Wait for connection
  int times = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if(times == 10){
      Serial.print("could not connect in setup");
      break;
    }
     times = times + 1;
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
 
  server.on("/", handleRoot);      //Which routine to handle at root location. This is display page

  server.begin();                  //Start server
  Serial.println("HTTP server started");  

  
}

void loop() {
  //
  if(WiFi.status() == WL_CONNECTED and once_done == 0){
      Serial.print("Connected to ");
      Serial.println(ssid);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());  //IP address assigned to your ESP
      //Serial.print(main_page.c_str());
    
      //server.on("/", handleRoot);      //Which routine to handle at root location. This is display page
      //server.on("/ledOn", handleLEDon); //as Per  <a href="ledOn">, Subroutine to be called
      //server.on("/ledOff", handleLEDoff);

      //server.begin();                  //Start server
      //Serial.println("HTTP server started");

      

      once_done = 1;
  }

  server.handleClient();          //Handle client requests
  // BUT1, BUT2
  timer.run();
  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);
  
  unsigned long currentMillis = millis();
  
  //TEMP();
  //display.clearDisplay();
  //GetHeartRate(); 
  if(mode == 0){
    //display.clearDisplay();
    MainPage();
  }else if(mode == 1){
    TEMP();
  }else if(mode == 2){
    GetHeartRate(); 
  }
 
  //Serial.print(adc1);
  if (adc1 >= 20000){
    Serial.print("single tap");
    BUT1 = true;
    if ( (currentMillis - previousMillisBTN) >= btnTimeLow ){
      if(mode == 0){
        mode = 1;
        display.clearDisplay();
      }else if (mode == 1){
        mode = 2;
        display.clearDisplay();
      }else if (mode == 2){
        mode = 0;
        display.clearDisplay();
      }
      
   }
    
    Serial.println(currentMillis - previousMillisBTN);
    //ignore debounce rule
    if ( ((currentMillis - previousMillisBTN)/1000) >= btnTimeLow and ((currentMillis - previousMillisBTN)/1000) <= btnTimeHigh){
      Serial.println("double tap");
    }
    previousMillisBTN = currentMillis;
  }else{
    BUT1 = false;
  }
  if (adc3 >= 20000){
    
    Serial.print("single tap");
    BUT2 = true;
    if ( (currentMillis - previousMillisBTN) >= btnTimeLow ){
      if(mode == 0){
        mode = 2;
        display.clearDisplay();
      }else if (mode == 1){
        mode = 2;
        display.clearDisplay();
      }else if (mode == 2){
        mode = 0;
        display.clearDisplay();
      }
    }
    previousMillisBTN = currentMillis;
  }else{
    BUT2 = false;
  }
  if (adc1 >= 20000 and adc3 >= 20000){
    Both = true;
  }

  
  //Serial.print(currentMillis/1000);
  //unsigned long previousMillis = 0UL;
  //unsigned long interval = 1000;
  if (currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;
    seconds_ = seconds_ + 1;
    //minute_ = minute_ + 1;
    if(minute_ >= 60){
      hour_ = hour_ + 1;
      minute_ = 0;
      if(hour_ > 23){
        hour_ = 0;
      }
    }else if(seconds_ >= 60){
      minute_ = minute_ + 1;
      seconds_ = 0;
    }
  }

  
}



//void handleRoot() {
// Serial.println("You called root page");
// //std::string main_page = strStream.str(); //str holds the content of the file
// //String s = main_page.c_str();
// //String^ s = gcnew String(main_page.c_str());
// String s = main_page; //Read HTML contents
// 
// server.send(200, "text/html", s); //Send web page
//}

float calibrator(float Ntc) {
  float ret = 0;
  float r = 1;
  //ret = Ntc * 0.05882 * r;
  ret = Ntc * 0.051 * r;
  return ret;
}

void GetHeartRate() {
  /*display.clearDisplay(); 
  display.drawBitmap(0, 47, Heart_Icon, 16, 16, WHITE);
  
  display.drawLine(0, 43, 127, 43, WHITE);
  
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(20, 48); 
  display.print(": 0 BPM");
  display.display(); 
  Serial.println();
  Serial.println("Please wait 10 seconds to get the BPM Value");*/
  
  //----------------------------------------
  unsigned long currentMillisGetHR = millis();

  if (currentMillisGetHR - previousMillisGetHR >= intervalGetHR) {
    previousMillisGetHR = currentMillisGetHR;

    PulseSensorSignal = analogRead(PulseSensorHRWire);
    Serial.print(PulseSensorSignal);
    Serial.print("   ");

    if (PulseSensorSignal > UpperThreshold && ThresholdStat == true) {
      cntHB++;
      ThresholdStat = false;
    }

    if (PulseSensorSignal < LowerThreshold) {
      ThresholdStat = true;
    }
    
    DrawGraph();
  }
  //----------------------------------------
  unsigned long currentMillisResultHR = millis();

  if (currentMillisResultHR - previousMillisResultHR >= intervalResultHR) {
    previousMillisResultHR = currentMillisResultHR;

    BPMval = cntHB * 6;
    Serial.print("BPM : ");
    Serial.println(BPMval);
    
    display.fillRect(20, 48, 108, 18, BLACK);
    
    display.drawBitmap(0, 47, Heart_Icon, 16, 16, WHITE); 
    display.drawLine(0, 43, 127, 43, WHITE); 
  
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(20, 48);
    display.print(": ");
    display.print(BPMval);
    display.print(" BPM");
    display.display(); 
    
    cntHB = 0;
  }
}
//--------------------------------------------------------------------------------membuat grafik
void DrawGraph() {
  if (x > 127) {
    display.fillRect(0, 0, 128, 42, BLACK);
    x = 0;
    lastx = 0;
  }
  int ySignal = PulseSensorSignal; 
  
  if (ySignal > 850) ySignal = 850;
  if (ySignal < 350) ySignal = 350;

  int ySignalMap = map(ySignal, 350, 850, 0, 40);    
  
  y = 40 - ySignalMap;
  Serial.println(y);
  display.writeLine(lastx,lasty,x,y,WHITE);
  display.display();   
  lastx = x;
  lasty = y;
  
  x++;
}


void TEMP(){

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(1, 1);
  display.print("Temperature");
  display.setCursor(80, 1);
  display.print("BAT:");
  float higher = 5.65;
  int percent = ((((adc2 * RefVoltage) / 1023.0) * 0.4) * 100) / higher;
  if(percent > 100) {
    percent = 100;
  }
  display.print(percent);
  display.print("%");
  
  display.setTextSize(3);
  display.setCursor(10, 20);
  Serial.print(calibrator(((adc0 * RefVoltage) / 1023.0) * 13.7), 1);
  display.print(calibrator(((adc0 * RefVoltage) / 1023.0) * 13.7), 1);
  //display.print((((adc0 * RefVoltage) / 1023.0) * 0.9), 1);
  display.setTextSize(1);
  display.print("  o");
  display.setTextSize(3);
  display.print("C");
  //display.print();// normal BPM 30 - 100
  display.setTextSize(1);
  display.setCursor(1, 55);
  display.print("Home");
  display.setCursor(90, 55);
  display.print("Pulse");
  display.display();

}


void MainPage(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(1, 1);
  display.print(seconds_);
  display.print("secs");
  display.setCursor(80, 1);
  display.print("BAT:");
  float higher = 5.65;
  int percent = ((((adc2 * RefVoltage) / 1023.0) * 0.4) * 100) / higher;
  if(percent > 100) {
    percent = 100;
  }
  display.print(percent);
  display.print("%");
  
  display.setTextSize(4);
  display.setCursor(5, 20);
  //char* d_hour = int(hour_);
  if(hour_ < 10){
    display.print("0");
    display.print(hour_);
  }else{
    display.print(hour_);
  }
  display.print(":");
  if(minute_ < 10){
    display.print("00");    
    display.print(minute_);
  }else{
    display.print(minute_);
  }
  
  //display.print();// normal BPM 30 - 100
  display.setTextSize(1);
  display.setCursor(1, 55);
  display.print("Temp");
  display.setCursor(90, 55);
  display.print("Pulse");
  display.display();
}


void myFunction() {
  // Code to execute when the button has been held down for 1 second
  // This is just an example - replace with your own code
  Serial.println("BUT1 pressed: ");
}
