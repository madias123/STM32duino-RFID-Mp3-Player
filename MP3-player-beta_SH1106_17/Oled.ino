
// ************** OLED startup
void oled_startupSHORT() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display_alt();
  oled_initdisplay();
  display.println("V: Beta 1.17");
  display.println("Player ready!");
  display.display_alt();
}
void oled_startup() {
  byte lm[][4] = {
    {7, 50, 55, 50},
    {55, 50, 59, 43},
    {59, 43, 62, 57},
    {62, 57, 66, 57},
    {66, 57, 77, 24},
    {77, 24, 86, 59},
    {86, 59, 90, 59},
    {90, 59, 99, 37},
    {99, 37, 119, 37}
  };
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display_alt();
  oled_initdisplay();
  //display.println("SW Version: 0.95");
  //display.println("Player ready!");


  for (int x = 0; x < 255; x++) {
    display.ssd1306_command(SSD1306_SETCONTRAST);

    display.ssd1306_command(x);
  }
  for (int x = 0; x < 9; x++) {
    display.drawLine(lm[x][0], lm[x][1], lm[x][2], lm[x][3], 1);
    display.drawLine(lm[x][0] + 1, lm[x][1] + 1, lm[x][2] + 1, lm[x][3] + 1, 1);
    display.display_alt();
    for (int xx = 0; xx < 255; xx++) {
      display.ssd1306_command(SSD1306_SETCONTRAST);

      display.ssd1306_command(xx);
    }
    delay(30);
  }
  OLED_fadein();
  getbmp("PLAYER/ICONS/splash.bmp") ;
  display.setCursor(85, 20);
  display.println("V0.97");
  display.display_alt();
  for (int x = 0; x < 255; x++) {
    display.ssd1306_command(SSD1306_SETCONTRAST);
    delay(5);
    display.ssd1306_command(x);
  }
  delay (1000);

  for (int y = 0; y < 64; y++) {
    display.ssd1306_command(SSD1306_SETSTARTLINE | y); // line #0
    display.ssd1306_command(SSD1306_SETCONTRAST);

    display.ssd1306_command(255 - y * 4);
    delay(10);
    oled_initdisplay();
  }
}

void oled_batteryshow() {
  oled_deleterow(0, 1);
  calculateBat();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print ("Bat: ");
  //display.print (batBar );
 // display.print  ("% ");
    display.print (batVolt );
  display.print  ("V ");

    display.drawRect(64, 0 , 63, 8, 1);
   byte tempbar = map(batBar, 0, 100, 0, 63);
    display.fillRect(64, 0 , tempbar, 8, 1);
  if (digitalRead(BT_USB_V_CHK))
    display.print("      *USB*");
  display.display_alt();
}

void oled_IDLEscreen() {
  if (idleflag) {
    getbmp("PLAYER/ICONS/sequence/Pause01.bmp") ;
    idleflag = 0;
    idletimer = millis();
   
  }

if (millis()-idletimer>100000) {
  getbmp("PLAYER/ICONS/sequence/Pause02.bmp") ;
        mfrc522.PCD_AntennaOff();
     digitalWrite (BREAKOUT_RESET,0);
     OLED_fadein();
      digitalWrite (PAM_SHUTDOWN, 0);
      digitalWrite (BT_POWER, 1);
     mfrc522.PCD_WriteRegister(mfrc522.Powerdown,0x10);
     mfrc522.PCD_WriteRegister(mfrc522.Powerdown,0x00);
   //  display.ssd1306_command(SSD1306_DISPLAYOFF); 
  //    noInterrupts();
         systick_disable();
           analogWrite (LED1, 1); // dark PWM
        // asm("cpsid i");  
     asm("wfi");
     
 
  while (!buttonflag && !buttonflagL && !buttonflagR ) {     }
  nvic_sys_reset();
}
  /* power save code bis jetzt:
      mfrc522.PCD_AntennaOff();
     digitalWrite (BREAKOUT_RESET,0);
     systick_disable();
     asm("wfi");

  */
}



void oled_vol_display() {
  if (mp3isplaying) {
    display.fillRect(0, 48 , 128, 16, 0);
    display.drawRect(64, 50 , 63, 12, 1);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 54);
    display.print("Volume: ");
   // display.print( 128 - main_volume );
    byte mapping;
    mapping = main_volume;
    map(mapping, encoderMIN, encoderMAX, 0, 64);
    //  display.fillRect(64, 48 , 48+mapping, 14, 1);
    display.fillRect(64, 50 , 63 - main_volume / 2, 12, 1);
    display.display_alt(6, 7);
  }
}

// ************** OLED init display
void oled_initdisplay() {
  display.clearDisplay();
  display.stopscroll();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
}

// ************** OLED directory browser
int oled_choosedir () { // fills up
  // oled has 8 pages * 21 chars (6x6 font)
  bool dironly = 1;
  getMP3dir(dironly) ; // getting the directory into buffer
  encoderMIN = -1; // -1 = previous page
  encoderMAX = 8;// +8 = next page
  lastEncoderPos = 0;
  encoderpos = 0;
  encflag = 0;
  int entry = 9999;
  byte page = 0;
  long fullnametimer = millis(); // delay before scrolling full text
  byte lastinvpage = 0; // last inverted row on display
  oled_printdir(0) ; // print first dir page
  oled_invert_entry(0, 0); // invert first entry
  display.display_alt();
  clearbuttons();
  buttonspressedtime = millis();
  while (!buttonflag) {
    if (encflag) {
      encflag = 0;
      fullnametimer = millis();
      if (encoderpos >= 0 && encoderpos < 8) {
        if (page == dir_numbers_entries / 8)
          encoderMAX = (dir_numbers_entries % 8) ; // count entries on last page
        else
          encoderMAX = 8;
        oled_invert_entry(encoderpos, lastinvpage);
        lastinvpage = encoderpos;
        if (page == 0 && encoderpos == 0)
          entry = 9999; // back flag
        else
          entry = encoderpos - 1 + page * 8;
        display.display_alt();
      }
      else {
        if (encoderpos == 8) {
          page++;
          if (page > dir_numbers_entries / 8)
            page--;
          oled_printdir(page) ;
          encoderpos = 0;
          lastinvpage = 0;
          oled_invert_entry(encoderpos, lastinvpage);
          lastinvpage = encoderpos;
          if (page == 0 && encoderpos == 0)
            entry = 9999; // back flag
          else
            entry = encoderpos - 1  + page * 8;
          display.display_alt();
        }
        if (encoderpos == -1) {

          if (page > 0) {
            page--;
            oled_printdir(page) ;
            encoderpos = 7;
            lastinvpage = 7;
            oled_invert_entry(encoderpos, lastinvpage);
            lastinvpage = encoderpos;
            if (page == 0 && encoderpos == 0)
              entry = 9999; // back flag
            else
              entry = encoderpos - 1 + page * 8;
            display.display_alt();
          }
          else {
            lastEncoderPos = 0;
            encoderpos = 0;
            encflag = 0;
            entry = 9999;
            page = 0;
            lastinvpage = 0; // last inverted row on display
            oled_printdir(0) ; // print first dir page
            oled_invert_entry(0, 0);
            display.display_alt();
          }
        }
      }
    }
    if ((millis() - fullnametimer > 1000) && (encoderpos != -1) && (encoderpos < 8) && (entry != 9999)) {
      uint8_t buffer2[1024]; // copy screen buffer
      for (int i = 0; i < 1024; i++)
        buffer2[i] = display.buffer[i];
      char longfile[128];
      if (!dirFile.open(MP3filePath, O_READ)) {
        notfounderror(1) ;
      }
      file.open(&dirFile, dirIDlist[entry], O_READ);
      file.getName(longfile, 128);
      file.close();
      dirFile.close();
      if (encoderpos > 0)
        display.drawFastHLine(0, (encoderpos * 8) - 1, 127, 1);
      if (encoderpos < 7)
        display.drawFastHLine(0, (encoderpos * 8) + 8, 127, 1);
      display.display_alt();
      scrollText( longfile, 1, encoderpos, 1, 10,40);
      for (int i = 0; i < 1024; i++)
        display.buffer[i] = buffer2[i];
      display.display_alt();
      fullnametimer = millis();
    }
  }
  clearbuttons();
  if (file.isOpen())
    file.close();
  if (dirFile.isOpen())
    dirFile.close();
  oled_initdisplay();
  //  display.println("back from dir...");
  if (entry != 9999) {
    // display.println(dirIDlist[entry] );
    char temp[100];
    bool isdirflag = 0;
    dirFile.open(MP3filePath, O_READ);
    file.open(&dirFile, dirIDlist[entry], O_READ); // -1 because of "<back>" entry!
    file.getName(temp, 100);
    isdirflag = file.isDir();
    file.close();
    dirFile.close();
    //   memset(MP3filePath, 0, sizeof(MP3filePath));
    //   strcat (MP3filePath, "MP3/");
    strcat (MP3filePath, temp );
    if (isdirflag)
      strcat (MP3filePath, "/" );
    display.println(MP3filePath);
    display.display_alt();
    if (sortDir()) { // getting media file list
      opentest();
      return dirIDlist[entry];
    }
    else
    {
      oled_choosedir(); // if no media files: do it again for the next subfolder
    }
  }
  else
  {
    display.println("*** zurueck ***");
    display.display_alt();

    return 9999;
  }
}


// ************** software scrolling long text
void scrollText(String text, byte txtsize, byte page, boolean invers, byte speeding, int spaces )
{
  if (invers)
    display.setTextColor(0, 1);
  else
    display.setTextColor(1, 0);
  display.setTextSize(txtsize);
  display.setTextWrap(false);
  int x = display.width();
  if (text.length()<10) {
    int fillchars=11-text.length();
    for (int x=0;x<fillchars;x++)
    text=text+" ";
  }
  int minX = -6 * txtsize * text.length(); // 6 = 6 pixels/character * text size
  for (int y = 0; y < 6 * txtsize * text.length() +spaces   * txtsize; y++) {  
    activateRec(mfrc522); // check RFID while looping text
    if (encflag || buttonflag || buttonflagR || buttonflagL || RFID_int_handler ) { // exit scrolling
      display.setTextWrap(true);
      return;
    }
    if (invers)
      display.fillRect(0, page * 8 , 128, 8 * txtsize, 1);
    else
      display.fillRect(0, page * 8 , 128, 8 * txtsize, 0);
    display.setCursor(x,  (0 + page * 8) );
    display.print(text);
    display.display_alt(page, page + txtsize - 1 );
    delay(speeding);
    if (--x < minX) x = display.width();
  }
  display.setTextWrap(true);
}

// ************** invert a page
void oled_invert_entry(byte actual, byte last) {
  for (int z = 0; z < 128; z++) {
    display.buffer[z + actual * 128] = ~display.buffer[z + actual * 128]; // invert page from position 8
    if (last != actual)
      display.buffer[z + last * 128] = ~display.buffer[z + last * 128]; // re-invert old page from position 8
  }
}

/*
  Text Size 1: 8 Zeilen, 21 Zeichen
  Text Size 2: 4 Zeilen,10 Zeichen
  Text Size 3: 2.7 Zeilen,7 Zeichen
  Text Size 4: 2 Zeilen,5 Zeichen
  fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color),
*/

// ************** delete a row
void oled_deleterow(byte row, byte fontsize) {
  display.fillRect(0, row * fontsize * 8, 127, fontsize * 8, 0);
}

// ************** OLED directory printout
void oled_printdir(byte page  ) {
  int offset =  page * 8;
  char mp3name21[21];
  bool firstpageflag = 0;
  if (page == 0)
    firstpageflag = 1;
  oled_initdisplay();
  if (firstpageflag)
    display.println("<*** zurueck ***>");
  if (!dirFile.open(MP3filePath, O_READ)) {
    notfounderror(1) ;
  }
  for (int x = 0 + firstpageflag; x < 8; x++) {
    file.open(&dirFile, dirIDlist[x + offset - 1], O_READ); // -1 because of "<back>" entry!
    file.getName(mp3name21, 21);
    file.close();
    display.setCursor(0, 8 * x);
    display.println(mp3name21);
  }
  // display.display_alt();
  if (file.isOpen())
    file.close();
  dirFile.close();

}

void OLED_fadein() {
  for (int x = 0; x < 255; x++) {
    display.ssd1306_command(SSD1306_SETCONTRAST);

    display.ssd1306_command(255 - x);
    // display.ssd1306_command(SSD1306_SETSTARTLINE | (x/2)); // line #0
    delay(1);
  }

}
void OLED_fadeout() {
  for (int x = 0; x < 255; x++) {
    display.ssd1306_command(SSD1306_SETCONTRAST);

    display.ssd1306_command(x);
    // display.ssd1306_command(SSD1306_SETSTARTLINE | 255-(x/2)); // line #0
    delay(1);
  }

}

void OLED_scrollUP() {
  for (int y = 0; y < 64; y++) {
    display.ssd1306_command(SSD1306_SETSTARTLINE | y); // line #0
    delay(10);
  }
}

void OLED_scrollDOWN() {
  for (int y = 64; y > -1; y--) {
    display.ssd1306_command(SSD1306_SETSTARTLINE | y); // line #0
    delay(10);
  }
}
