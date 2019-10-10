#include "Arduino.h"
// USB composite
#define PRODUCT_ID 0x29
SdFatEX sd;
const uint32_t speed = SPI_CLOCK_DIV2 ;
const uint8_t SD_CHIP_SELECT = SS;
bool enabled = false;
uint32 cardSize;

// ****OLED
#define OLED_RESET 255 // no reset pin
// *********** PAM
#define PAM_SHUTDOWN PB3 // connected with shutdown pin on PAM  
#define BT_POWER PB1 // Bluetooth main power
#define BAT_CHK PB0 // Battery Input (analog in )   between 0 and 4095 -> 2.9V = Maximum = 4.22V
// *********** BT Module
#define BT_LED_IN PA9  // only with new version of the PCB!
#define BT_USB_V_CHK PA15 // USB Power check

// ***** LED
#define LED1 PA10 // LED 1 on front plate
#define LED2 PC13 // LED 2 on front plate
// ****************  VS1003
#define BREAKOUT_RESET  PA2 // VS1053 reset pin (output)
#define BREAKOUT_CS     PA3 // VS1053 chip select pin (output)
#define BREAKOUT_DCS    PA1 // VS1053 Data/command select pin (output)
#define CARDCS          PA4 // Card chip select pin
#define DREQ            PA0 // VS1053 Data request, ideally an Interrupt pin

// **************** MP3 tags
#define TRACK_TITLE              3
#define TRACK_ARTIST            33
#define TRACK_ALBUM             63

// **************** buttons
#define Button PB9 // encoder button
volatile int buttonflag = LOW;
volatile long buttonspressedtime = millis();
boolean longbuttonflag = 0;

#define ButtonL PB11 // left button
volatile int buttonflagL = LOW;
volatile long buttonspressedtimeL = millis();
boolean longbuttonflagL = 0;

#define ButtonR PB10 // right button0
volatile int buttonflagR = LOW;
volatile long buttonspressedtimeR = millis();
boolean longbuttonflagR = 0;

// **************** Encoder setup
volatile int encstate;
volatile int encflag;
boolean A_set;
boolean B_set;
volatile int16_t encoderpos = 0;
volatile int16_t encoderMIN = 0;
volatile int16_t encoderMAX = 100;
volatile int  encodertimer = millis(); // acceleration measurement
int encoderpinA = {PB5}; // pin array of all encoder A inputs
int encoderpinB = {PB4}; // pin array of all encoder B inputs
unsigned int lastEncoderPos;
#define ENCODER_RATE 1000    //timer in microseconds; 
HardwareTimer timer(1);

// **************** SD CARD
const uint8_t SD_CS_PIN = SS;
const char TAGidPath[] = "PLAYER/TAGS/"; // path on SD where ID Tags stored
const char ICONSPath[] = "PLAYER/ICONS/"; // path on SD where ID Tags stored
const char ICONSmenuPATH[] = "PLAYER/ICONS/startmenu/"; // start menu path
//const char defaultMP3DirPath[] = "MP3/"; // MP3 path
char MP3filePath[255]; // subdir of each MP3 folder: beware: should be <100 bytes long or raise this value
#define  MAXFILES 255
int dirIDlist[MAXFILES];
int dir_numbers_entries = 0;
int numbersoftracks = 0;
int currenttrack = 0;
SdFat audio_SD;
SdFile file;
SdFile dirFile;

// ****************  MFRC522
#define RST_PIN         PA8          // Configurable, see typical pin layout above
#define SS_PIN          PB12         // Configurable, see typical pin layout above
#define IRQ_PIN         PB8           // Configurable, depends on hardware

volatile boolean RFID_int_handler = false;
byte regVal = 0x7F;

//********system
boolean idleflag = 1;
boolean mp3isplaying = 0; // flag if music is playing
boolean oldisplaying = 0;
byte main_volume = 40; // master volume
char UIDstring[20]; // RF Card ID as hex char array (needed for tag filenames) todo: convert it into a long saving RAM
char oldUIDstring[20];
byte mp3DirSort[100]; // sort array for the mp3 files
byte mp3Dirmax = 0; // max Numbers of tracks in folder in mp3DirSort array
byte mp3TrackCurrent = 0; // current mp3 in folder
byte unsortedflag = 0; // if mp3's are not correctly named like "01 title.mp3" folder will be played unsorted via "opennext"
char actualTrackTitel[31];
char actualInterpret[31];
bool pauseflag = 0;
float batVolt; // LiIon Battery voltage
int batBar; // Battery remain percent (3.5V-4.22V)
long idletimer=millis();
boolean idlesleepflag=0;
// for BT Audio
// Plugin: admix right
#ifndef SKIP_PLUGIN_VARNAME
#define PLUGIN_SIZE 105
const unsigned short plugin[105] = { /* Compressed plugin */
#endif
  0x0007, 0x0001, 0x8f00, 0x0006, 0x0064, 0x2803, 0xc2c0, 0x0030, /*    0 */
  0x0697, 0x0fff, 0xfdc0, 0x3700, 0x4024, 0xb100, 0x0024, 0xbc82, /*    8 */
  0x3c00, 0x0030, 0x1297, 0x3f10, 0x0024, 0x3f00, 0x0024, 0x2803, /*   10 */
  0xc540, 0x0003, 0xc795, 0x0000, 0x0200, 0x3700, 0x4024, 0xc100, /*   18 */
  0x0024, 0x3f00, 0x0024, 0x0000, 0x0040, 0x0004, 0x07c1, 0x0003, /*   20 */
  0xc7d5, 0x0030, 0x1097, 0x3f70, 0x0024, 0x3f00, 0x4024, 0xf400, /*   28 */
  0x5540, 0x0000, 0x08d7, 0xf400, 0x57c0, 0x0007, 0x9257, 0x0000, /*   30 */
  0x0000, 0x3f00, 0x0024, 0x0030, 0x0297, 0x2000, 0x0000, 0x3f00, /*   38 */
  0x0024, 0x2a08, 0x2bce, 0x2a03, 0xc80e, 0x3e12, 0xb817, 0x3e14, /*   40 */
  0xf806, 0x0030, 0x0317, 0x3701, 0x3804, 0x0030, 0x1117, 0xf148, /*   48 */
  0x1c06, 0x0030, 0x1293, 0x2803, 0xcad1, 0xa64c, 0x0024, 0xf168, /*   50 */
  0x0024, 0x464c, 0x0024, 0x3b11, 0x8024, 0x3b01, 0x9bc4, 0x36f4, /*   58 */
  0xd806, 0x3602, 0x8024, 0x0030, 0x0717, 0x2100, 0x0000, 0x3f05, /*   60 */
  0xdbd7,
#ifndef SKIP_PLUGIN_VARNAME
};
#endif
