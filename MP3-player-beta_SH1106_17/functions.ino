void mp3defaultdir() {
  memset(MP3filePath, 0, sizeof(MP3filePath));
  strcat (MP3filePath, "MP3/");
}

//********** callback for all dir or file not found messages
// error 0: No SD card
// error 1: directory not found
// error 2: TAG ID path ("PLAYER/TAGS/") missing
// error 10: VS1053 not found
// error 11: cannot find media file
void notfounderror(byte error) //
{
  oled_initdisplay();
  if (error == 0) { // no SD card

    display.println("No SD Card found!");
    display.println("Please insert Card and");
    display.println("press Encoder for reset");
    display.display_alt();
    while (!buttonflag) {
    }
     musicPlayer.reset();
    nvic_sys_reset();
  }
  else {
    display.println("Error:");
    display.println(error);
    display.println("Press Encoder");
    display.display_alt();
    buttonflag = 0;
    while (!buttonflag) {
    }
    oled_initdisplay();
  }

}

void calculateBat() { //4.20* (5.1/(2.2+5.1) = 2.9V    between 0 and 4095 -> 2.9V = Maximum = 4.22V
   long calcvalue=0;
   for (int a=0;a<16;a++) {
     calcvalue+=analogRead(BAT_CHK);
     delay(10);
   }
 
 float batlevel = calcvalue/16 * (3.3 / 4095.0);
 batVolt=batlevel/0.698; //  (5.1/(2.2+5.1) -> Resistor divider
 int temp=batVolt*100;
 batBar=map(temp,350,420,0,100);
 if (batBar>95)
 batBar=100;
}


//*********** print freestack #DEBUG
void printfreestack() {
  //#DEBUG# CompositeSerial.print(F("FreeStack: "));
  //#DEBUG# CompositeSerial.println(FreeStack());
  //#DEBUG# CompositeSerial.println();
}

void setEncoderVol() {
  encoderpos = main_volume;
  encoderMIN = 0;
  encoderMAX = 127;
}
