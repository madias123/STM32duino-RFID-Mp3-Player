void call_RFID_int_handler() {
      digitalWrite(LED2, 1);
    VS1053_stopplay();
    mp3TrackCurrent = 0;
    if (cardhandler()) {
      VS1053_startplay();
    }
    RFID_int_handler = 0;
    setEncoderVol();
    delay(300);
    digitalWrite(LED2, 0);
    idleflag = 1;
}


// blank tag
void writeNewTag() { // new card inserted writing tag to file on SD card
   clearbuttons();
  display.println("Neues Verzeichnis?");
  display.display_alt(); // getting the directory for new mp3 folder
  delay(500);
  while (!buttonflag) {}
   delay(100);
   clearbuttons();
  oled_choosedir();
  oled_initdisplay();
  display.println(UIDstring);
  display.println("Neuer Pfad:");
  display.println(MP3filePath);
  display.println("korrekt?");
  display.println("(rechts=JA)");
  display.display_alt();
  buttonflag = 0;
  buttonflagL = 0;
  buttonflagR = 0;
  while (!buttonflag && !buttonflagL && !buttonflagR) {}
  oled_initdisplay();
  if (buttonflagR) {
    display.println("schreibe..");
    display.display_alt();
    writeTagSD();
  }
  else {
    display.println("Abbruch");
    display.display_alt();
  }
clearbuttons();
}
//UIDstring

// ************** read TAG and check status
boolean cardhandler() {
  boolean returnflag = 0;
  oled_initdisplay();
  if (getnewUID())
  {
    byte status = readTagSD() ;
    if (status == 1) {
      sortDir();
      opentest() ;
      if (!dirFile.open(MP3filePath, O_READ)) {
        notfounderror(1) ;
      }
      //#DEBUG# CompositeSerial.println( );
      //#DEBUG# CompositeSerial.print ("***");
      //#DEBUG# CompositeSerial.print (MP3filePath);
      //#DEBUG# CompositeSerial.println("***");
      if ( !file.open(&dirFile, mp3DirSort[0], O_READ))
        display.println("ID3 tags:");
      //#DEBUG# CompositeSerial.println("ID3 tags:");
      char  test[30];
      getTrackInfo(TRACK_TITLE, test);
      display.println(test);
      //#DEBUG# CompositeSerial.println(test);
      getTrackInfo(TRACK_ARTIST, test);
      display.println(test);
      //#DEBUG# CompositeSerial.println(test);
      getTrackInfo(TRACK_ALBUM, test);
      display.println(test);
      //#DEBUG# CompositeSerial.println(test);
      file.close();
      dirFile.close();
      display.display_alt();
      returnflag = 1;
    }
    if (status == 2) {
      close_SD_all();
      mp3defaultdir();
      //display.println("TAG zuweisen?");
      //#DEBUG# CompositeSerial.println("New TAG file?");
      //display.display_alt();
       clearbuttons();
      writeNewTag();
      returnflag = 0;
    }
  }
  return returnflag;
}

void readCard() {
  RFID_int_handler = true;
}

// ************** setup MFRC522
void activateRec(MFRC522 mfrc522) {
  mfrc522.PCD_WriteRegister(mfrc522.FIFODataReg, mfrc522.PICC_CMD_REQA);
  mfrc522.PCD_WriteRegister(mfrc522.CommandReg, mfrc522.PCD_Transceive);
  mfrc522.PCD_WriteRegister(mfrc522.BitFramingReg, 0x87);
}

void clearInt(MFRC522 mfrc522) {
  mfrc522.PCD_WriteRegister(mfrc522.ComIrqReg, 0x7F);
}

void MFRC522setup() {
  mfrc522.PCD_Init(); // Init MFRC522 card
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);
  pinMode(IRQ_PIN, INPUT_PULLUP);
  regVal = 0xA0; //rx irq
  mfrc522.PCD_WriteRegister(mfrc522.ComIEnReg, regVal);
  RFID_int_handler = false; //interrupt flag
  attachInterrupt(digitalPinToInterrupt(IRQ_PIN), readCard, FALLING);
  RFID_int_handler = false;
}

// ************** check TAG status
bool getnewUID() {
  oled_initdisplay();
  bool newcard = 0;
  /*
    if ( ! mfrc522.PICC_IsNewCardPresent())
    {
    display.println("Please try again (no card)");
    // //#DEBUG# CompositeSerial.println("Please try again (no card)");
    clearInt(mfrc522);
    mfrc522.PICC_HaltA();
    return 0;
    } */
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    display.println("Please try again (read card error)");
    //  //#DEBUG# CompositeSerial.println("Please try again (read card error)");
    clearInt(mfrc522);
    mfrc522.PICC_HaltA();
    return 0;
  }
  byte *buffer = mfrc522.uid.uidByte;
  byte bufferSize = mfrc522.uid.size;

  memset(oldUIDstring, 0, sizeof(oldUIDstring));
  strcpy(oldUIDstring, UIDstring);
  memset(UIDstring, 0, sizeof(UIDstring));
  for (byte i = 0; i < bufferSize; i++) {
    sprintf(&UIDstring[i * 2], "%02X", buffer[i]);
  }
  strcat (UIDstring, ".txt");

  
//  if (!strcmp(oldUIDstring, UIDstring)) {
//    display.println("Same card inserted!");
//    newcard = 0;
//  }
//  else {
//
//    newcard = 1;
//  }

 newcard = 1;
  
  clearInt(mfrc522);
  mfrc522.PICC_HaltA();
  display.display_alt();
  return newcard;
}


boolean deletecardhandler() {
  boolean returnflag = 0;
  oled_initdisplay();
  if (getnewUID())
  {
    byte status = readTagSD() ;

    if (status == 1) {
      sortDir();
      //  opentest() ;
      if (!dirFile.open(MP3filePath, O_READ)) {
        notfounderror(1) ;
      }

      if ( file.open(&dirFile, mp3DirSort[0], O_READ))
        display.println("----TAG LOESCHEN---");
      display.println(MP3filePath);
      display.println("-------WIRKLICH?-----");
      display.println("NEIN               JA");
      display.println("---------------------");
      display.display_alt();
      file.close();
      clearbuttons();
      while (!buttonflagL && !buttonflagR) {}
      oled_initdisplay();
      if (buttonflagL)
        display.println("Abbruch");
      else
      {
        display.println("TAG geloescht!");
        display.display_alt();
        char tempfile[255];
        memset(tempfile, 0, sizeof(tempfile));
        strcat (tempfile, TAGidPath);
        strcat (tempfile, UIDstring);
        File myFile = audio_SD.open(tempfile, FILE_WRITE);
        myFile.remove();
        delay(1000);
      }


      dirFile.close();
      display.display_alt();
      returnflag = 1;
    }
    if (status == 2) {
      display.println("---------------------");
      display.println("TAG ist leer!");
      display.println("---------------------");
      display.display_alt();
      delay(1000);

      returnflag = 0;
    }

  }
  display.display_alt();
  return returnflag;
}
