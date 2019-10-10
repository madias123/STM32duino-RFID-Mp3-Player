
#include <USBComposite.h>
USBMassStorage MassStorage;
USBCompositeSerial CompositeSerial;
#include "itoa.h"
#include "SdFat.h"
#include "FreeStack.h"
#include "init.h"
#define SD_SPI_CONFIGURATION 1

#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306_STM32.h>


Adafruit_SSD1306 display(OLED_RESET);
Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
void activateRec(MFRC522 mfrc522);
void clearInt(MFRC522 mfrc522);

// ********* testing variables
byte teststate = 0;
byte testtimer = millis();

void setup() {
  //afio_cfg_debug_ports(AFIO_DEBUG_NONE);
  afio_cfg_debug_ports(AFIO_DEBUG_SW_ONLY);
  pinMode (BAT_CHK, INPUT);
  pinMode (BT_USB_V_CHK, INPUT);
  pinMode (BT_LED_IN, INPUT);
  pinMode (BT_POWER, OUTPUT);
  pinMode (LED1, OUTPUT);
  pinMode (LED2, OUTPUT);
  analogWrite (LED1, 64);
  digitalWrite (BT_POWER, 0);
  pinMode (PAM_SHUTDOWN, OUTPUT);
  digitalWrite (PAM_SHUTDOWN, 0);


  mp3defaultdir();
  USBComposite.setProductId(PRODUCT_ID);
  //pinMode (LED1, OUTPUT); // PAM Mute!
  //digitalWrite(LED1, LOW); //active LOW - so switch is on

  delay(100);
  oled_startupSHORT();
  oled_initdisplay();

  button_startup();
  initEncoders();

  MFRC522setup();

  if (!audio_SD.begin(SD_CS_PIN, 8000000)) {
    notfounderror(0);
  }

  //audio_SD.errorPrint(&CompositeSerial);
  //oled_startup();
  // while (!CompositeSerial);
  if (digitalRead(ButtonR)) { // entering the "hidden" startmenu (BT, SD-USB...)
    startmenu();
  }
  //#DEBUG# CompositeSerial.registerComponent();
  //#DEBUG# CompositeSerial.begin(9600);
  // future: power on VS1053, ...

  VS1053setup();

  encoderpos = main_volume;
}

void loop() {

  if (! mp3isplaying ) {
    oled_IDLEscreen();
  }

  if (buttonflag) {
    Call_buttonflag();
  }
  if (buttonflagR) {
    Call_buttonflagR();
  }
  if (buttonflagL) {
    Call_buttonflagL();
  }
  if ( RFID_int_handler) {
    call_RFID_int_handler();
  }
  delay(100);
  activateRec(mfrc522);
  encodercheck();
  oled_batteryshow();
  if (mp3isplaying) {
    if (pauseflag) {
      scrollText( "*** PAUSE ***" ,  2,  6, 1, 10,40 );
    }
    else {
      scrollText( actualInterpret,  2,  2, 0, 2,-1 );
      scrollText( actualTrackTitel,  2,  6, 1, 5,-1 );
    }
  }
  if (musicPlayer.stopped() == 1 && mp3isplaying == 1) { // end of mp3 while playing status =1 ?
    do {
      mp3TrackCurrent++; // next track
    }   while ( mp3TrackCurrent < mp3Dirmax   && mp3DirSort[mp3TrackCurrent] == 0 ) ;

    // next track
    VS1053_stopplay();
    if ( mp3TrackCurrent < mp3Dirmax ) {
      VS1053_startplay();
    }
    else // finished playing folder
    {
      mp3isplaying = 0;
      mp3TrackCurrent = 0;
      idleflag=1;
       display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print ("fertig.");
 
  display.display_alt();
    }
  }

  if (musicPlayer.stopped() != oldisplaying) {
    oldisplaying = musicPlayer.stopped();
  }

  if (encflag) {
    encflag = 0;
    main_volume =  encoderMAX - encoderpos;
    musicPlayer.setVolume(main_volume, main_volume);
    oled_vol_display();
  }

}
