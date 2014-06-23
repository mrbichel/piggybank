#include <Adafruit_NeoPixel.h>

#define PIN 5

Adafruit_NeoPixel strip = Adafruit_NeoPixel(10, PIN, NEO_GRB + NEO_KHZ800);


/*
 * WiFlyHQ Example httpclient_progmem.ino
 *
 * This sketch implements a simple Web client that connects to a 
 * web server, sends a GET, and then sends the result to the 
 * Serial monitor.
 *
 * This example uses PROGMEM to reduce the amount of RAM needed
 * for the sketch. Most of the strings are stored in flash rather than RAM.
 *
 * This sketch is released to the public domain.
 *
 */

#include <avr/pgmspace.h>
/* Work around a bug with PROGMEM and PSTR where the compiler always
 * generates warnings.
 */
#undef PROGMEM 
#define PROGMEM __attribute__(( section(".progmem.data") )) 
#undef PSTR 
#define PSTR(s) (__extension__({static prog_char __c[] PROGMEM = (s); &__c[0];})) 

#include <WiFlyHQ.h>

#include <SoftwareSerial.h>
SoftwareSerial wifiSerial(8,9);

//#include <AltSoftSerial.h>
//AltSoftSerial wifiSerial(8,9);

WiFly wifly;

/* Change these to match your WiFi network */
const char mySSID[] = "ID";
const char myPassword[] = "PASSWORD";

const char site[] = "gris.jit.su";

void terminal();
void print_P(const prog_char *str);
void println_P(const prog_char *str);


int percent = 0;

unsigned long start_time; // Timer stuff
unsigned long current_time;
unsigned long elapsed_time;


// LED stuff
#include "LPD8806.h"
#include "SPI.h"

// Number of RGB LEDs in strand:
int nLEDs = 10;


// First parameter is the number of LEDs in the strand.  The LED strips
// are 32 LEDs per meter but you can extend or cut the strip.  Next two
// parameters are SPI data and clock pins:

void setup()
{

  // Start up the LED strip
  strip.begin();

  // Update the strip, to start they are all 'off'
  strip.show();

  char buf[32];

  Serial.begin(115200);
  println_P(PSTR("Starting"));
  print_P(PSTR("Free memory: "));
  Serial.println(wifly.getFreeMemory(),DEC);

  wifiSerial.begin(9600);
  if (!wifly.begin(&wifiSerial, &Serial)) {
    println_P(PSTR("Failed to start wifly"));
    terminal();
  }

  /* Join wifi network if not already associated */
  if (!wifly.isAssociated()) {
    /* Setup the WiFly to connect to a wifi network */
    println_P(PSTR("Joining network"));
    wifly.setSSID(mySSID);
    wifly.setPassphrase(myPassword);
    wifly.enableDHCP();

    if (wifly.join()) {
      println_P(PSTR("Joined wifi network"));
    } 
    else {
      println_P(PSTR("Failed to join wifi network"));
      terminal();
    }
  } 
  else {
    println_P(PSTR("Already joined network"));
  }

  //terminal();

  print_P(PSTR("MAC: "));
  Serial.println(wifly.getMAC(buf, sizeof(buf)));
  print_P(PSTR("IP: "));
  Serial.println(wifly.getIP(buf, sizeof(buf)));
  print_P(PSTR("Netmask: "));
  Serial.println(wifly.getNetmask(buf, sizeof(buf)));
  print_P(PSTR("Gateway: "));
  Serial.println(wifly.getGateway(buf, sizeof(buf)));
  print_P(PSTR("SSID: "));
  Serial.println(wifly.getSSID(buf, sizeof(buf)));

  wifly.setDeviceID("Wifly-WebClient");
  print_P(PSTR("DeviceID: "));
  Serial.println(wifly.getDeviceID(buf, sizeof(buf)));

  if (wifly.isConnected()) {
    println_P(PSTR("Old connection active. Closing"));
    wifly.close();
  }



  start_time = millis();  


}


void checkData() {

  if (wifly.open(site, 80)) {
    print_P(PSTR("Connected to "));
    Serial.println(site);

    /* Send the request */
    wifly.println("GET http://gris.jit.su/data HTTP/1.1");
    wifly.println("Host: gris.jit.su");
    wifly.println("");
  } 
  else {
    println_P(PSTR("Failed to connect"));
  }

}

// Fill the dots progressively along the strip.
void fillBar(int p) {

  float fleds = p*nLEDs/100.0;

  Serial.println("");
  Serial.println("Trying to light up");
  Serial.println(fleds);

  int leds = floor(fleds);
  float rem = fleds-leds;

  int i = 0;

  int intensity = 0;

  for(i=0; i<nLEDs; i++) strip.setPixelColor(i, 0);

  for (i=0; i < leds; i++) {
    intensity = 255;
    // For each LED that needs to light up
    strip.setPixelColor(i, strip.Color( intensity, intensity, intensity ) );
  }

  // Set the last LED on less than maximum brightness to simulate higher resolution of data
  strip.setPixelColor(leds, strip.Color( intensity*rem, intensity*rem, intensity*rem) );
  strip.show(); 
}


char ch;
char chl;

void loop()
{

  current_time = millis();
  elapsed_time = current_time - start_time;

  if(elapsed_time > 2000)
  {
    checkData(); 
    start_time = millis();
  }

  while (wifly.available() > 0) {
    ch = wifly.read();
    Serial.print(ch);

    if(ch == 'P' && chl == '\n') {
      percent = wifly.parseInt();
      Serial.println("Found Number:");
      Serial.println(percent);
      fillBar(percent);
    }

    if (ch == '\n') {
      /* add a carriage return */
      Serial.write('\r');
    }

    chl = ch;

  }

  if (Serial.available() > 0) {
    wifly.write(Serial.read());
  }



}

/* Connect the WiFly serial to the serial monitor. */
void terminal()
{
  while (1) {
    if (wifly.available() > 0) {
      Serial.write(wifly.read());
    }


    if (Serial.available() > 0) {
      wifly.write(Serial.read());
    }
  }
}

/* Print a string from program memory */
void print_P(const prog_char *str)
{
  char ch;
  while ((ch=pgm_read_byte(str++)) != 0) {
    Serial.write(ch);
  }
}

void println_P(const prog_char *str)
{
  print_P(str);
  Serial.println();
}


