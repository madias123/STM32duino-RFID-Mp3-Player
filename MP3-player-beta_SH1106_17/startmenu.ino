// soft reset: nvic_sys_reset();

void startmenu() {
  oled_initdisplay();
  encoderMIN = 1; // -1 = previous page
  encoderMAX = 4;// +8 = next page
  lastEncoderPos = 0;
  byte lastinvpage = 0;
  encoderpos = 0;
  encflag = 0;
  char picPath[30];
  // strcpy (picPath,"PLAYER/ICONS/startmenu/1.bmp"); // starting with bluetooth picture
  //  getbmp(picPath) ;
  /*
    display.println("Choose:");
    display.println("Bluetooth Audio");
    display.println("SD-USB");
    display.println("delete RFID-tags");
    display.println("Parameters");
    display.println("exit");
    oled_invert_entry(0, 0); // invert first entry
    display.display_alt();

    char str[80];
    strcpy (str,"these ");
    strcat (str,"strings ");
    ICONSmenuPATH[]
  */
  char test = 'A';
  while (!buttonflag) {
    if (encflag) {
      //oled_invert_entry(encoderpos, lastinvpage);
     // OLED_fadein();

      sprintf(picPath, "%s%d.bmp", ICONSmenuPATH, encoderpos);
      getbmp(picPath) ;
     // OLED_fadeout();
      encflag = 0;
      lastinvpage = encoderpos;
    }
  }
  // oled_initdisplay();
  display.setCursor(0, 48);
  switch (encoderpos) {
    case 1: // BT Audio
      BT_AUDIO();
      break;
    case 2: // SD-USB
      display.println("Ende = Knopf druecken");
      display.display_alt();
      buttonflag = 0;
      SD_USB();
      break;
    case 3: // delete RF TAGS
      delete_RFTAGS();
      break;
    case 4: // Parameter
      Parameters();
      break;
    case 5: // exit
      return;
      break;
  }

}

void BT_AUDIO() {
  //display.println("BT-Audio");
  digitalWrite (BT_POWER, 1);
    digitalWrite (PAM_SHUTDOWN, 1);
  display.println("Ende = Knopf druecken");
  display.display_alt();
  musicPlayer.begin();
  musicPlayer.setVolume(main_volume, main_volume);
  musicPlayer.applyPatch(plugin, PLUGIN_SIZE);// Add here ...
  musicPlayer.sciWrite(VS1053_SCI_AIADDR, 0);
  // disable all interrupts except SCI
  // musicPlayer.sciWrite(VS1053_REG_WRAMADDR, VS1053_INT_ENABLE);
  // musicPlayer.sciWrite(VS1053_REG_WRAM, 0x02);
  //musicPlayer.sciWrite(VS1053_REG_CLOCKF, 0xC000);  // set max clock
  //musicPlayer. sciWrite(VS1053_REG_MODE ,    VS1053_MODE_SM_SDINEW);

  // musicPlayer.sciWrite(VS1053_SCI_AICTRL0, 0xfffd);
  musicPlayer.sciWrite(VS1053_SCI_AICTRL0, 1024); // mittelwert
  //musicPlayer.sciWrite(VS1053_SCI_AICTRL1, 512);
  //musicPlayer.sciWrite(VS1053_SCI_AICTRL2, 0);
  //musicPlayer.sciWrite(VS1053_SCI_AICTRL3, 0);
  musicPlayer.sciWrite(VS1053_SCI_AIADDR, 0x0f00);
  encoderMIN = 0;
  encoderMAX = 64;
  encoderpos = 128;
  buttonflag = 0;
  while (!buttonflag) {
    if (encflag) {
      encflag = 0;
      // musicPlayer.setVolume(encoderpos, encoderpos);
      musicPlayer.sciWrite(VS1053_SCI_AICTRL0, 128 + 64 + encoderpos);
    }
  }
  digitalWrite (BT_POWER, 0);
  nvic_sys_reset();
}

void SD_USB() {

  sd.begin(SD_CHIP_SELECT);
  initReader();

  while (!buttonflag) {
    MassStorage.loop();
  }
  nvic_sys_reset();
}

void delete_RFTAGS() {
  //#DEBUG# CompositeSerial.registerComponent();
  //#DEBUG# CompositeSerial.begin(9600);
  VS1053setup();
  boolean displayupdate = 1;
  clearbuttons();
  while (!buttonflag && !buttonflagR && !buttonflagL) {

    activateRec(mfrc522);
    delay(100);
    if ( RFID_int_handler) {

      digitalWrite(LED2, 1);
      deletecardhandler();
      delay(500);
      digitalWrite(LED2, 0);
      //   clearInt(mfrc522);
      RFID_int_handler = 0;
      displayupdate = 1;
      clearbuttons();
    }
    if (displayupdate) {
      oled_initdisplay();
      display.println("--- TAG LOESCHEN ----");
      display.println("---------------------");
      display.println(" Bitte Sticker/Karte/");
      display.println(" Figur auflegen!");
      display.println("---------------------");
      display.println(" Knopf: Beenden ");
      display.println("---------------------");
      oled_invert_entry(0,0);
      display.display_alt();
      displayupdate = 0;
    }
  }
  nvic_sys_reset();
}
void Parameters() {
nvic_sys_reset();
}


bool write(const uint8_t *writebuff, uint32_t startSector, uint16_t numSectors) {
  return sd.card()->writeBlocks(startSector, writebuff, numSectors);
}

bool read(uint8_t *readbuff, uint32_t startSector, uint16_t numSectors) {
  return sd.card()->readBlocks(startSector, readbuff, numSectors);
}

void initReader() {

  cardSize = sd.card()->cardSize();
  MassStorage.setDriveData(0, cardSize, read, write);
  MassStorage.registerComponent();
  //#DEBUG# CompositeSerial.registerComponent();
  USBComposite.begin();
  enabled = true;
  display.println("Bitte kurz warten...");

  display.display_alt();
}
