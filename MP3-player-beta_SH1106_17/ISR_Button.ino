
void Button_ISR() {
  if (micros() - buttonspressedtime > 500000  ) {
    buttonflag = 1;
    buttonspressedtime = micros();
  }
}

void ButtonL_ISR() {
  if (millis() - buttonspressedtimeL > 500  ) {
    buttonflagL = 1;
    buttonspressedtimeL = millis();
  }
}

void ButtonR_ISR() {
  if (millis() - buttonspressedtimeR > 500  ) {
    buttonflagR = 1;
    buttonspressedtimeR = millis();
  }
}

void button_startup() {
  pinMode(Button, INPUT_PULLDOWN);
  attachInterrupt(Button, Button_ISR, RISING );

  pinMode(ButtonL, INPUT_PULLDOWN);
  attachInterrupt(ButtonL, ButtonL_ISR, RISING );
  pinMode(ButtonR, INPUT_PULLDOWN);
  attachInterrupt(ButtonR, ButtonR_ISR, RISING );
}

void clearbuttons() {
  buttonflag = 0;
  buttonflagL = 0;
  buttonflagR = 0;
}

void Call_buttonflag() { // middle button
idletimer = millis();
  longbuttonflag = 0;
  long deltatime = millis();
  digitalWrite(LED2, 1);
  bool exitflag = 0;
  while ( digitalRead(Button)  && exitflag == 0 ) {
    if (millis() - deltatime > 500) {
      longbuttonflag = 1;
      exitflag = 1;
    }
  }
  digitalWrite(LED2, 0);
  if (mp3isplaying && !longbuttonflag) {
    pauseflag = !pauseflag;
    musicPlayer.pausePlaying(pauseflag);
  }
  else {
    pauseflag = 0;
    VS1053_stopplay();
    mp3TrackCurrent = 0;
    if (!longbuttonflag) {
      mp3defaultdir();
      int callback =  0;
      callback =   oled_choosedir();
       idleflag = 1;
      if (callback != 9999) {
        VS1053_startplay();
      }
    }
    else {
      idleflag = 1;
    }
    
  }
  setEncoderVol();
  buttonflag = 0;
}
void Call_buttonflagL() {
  idletimer = millis();
  buttonflagL = 0;
  longbuttonflagL = 0;
  long deltatime = millis();
  digitalWrite(LED2, 1);
  bool exitflag = 0;
  while ( digitalRead(ButtonL)  && exitflag == 0 ) {
    if (millis() - deltatime > 500) {
      longbuttonflagL = 1;
      exitflag = 1;
    }
  }
  digitalWrite(LED2, 0);

  if (mp3isplaying) {
    if (mp3TrackCurrent  > 0) {
      VS1053_stopplay();
      mp3TrackCurrent--; // previous track
      VS1053_startplay();
    }
  }
  else
  {
    if (longbuttonflagL)
      startmenu();
  }
}
void Call_buttonflagR() {
  idletimer = millis();
  buttonflagR = 0;
  if (mp3isplaying) {
    do { // manual next track
      mp3TrackCurrent++; // next track
    }   while ( mp3TrackCurrent < mp3Dirmax   && mp3DirSort[mp3TrackCurrent] == 0 ) ;
    VS1053_stopplay();
    if ( mp3TrackCurrent < mp3Dirmax ) {
      VS1053_startplay();
    }
    else // finished playing folder
    {
      mp3isplaying = 0;
      mp3TrackCurrent = 0;
      VS1053_stopplay();
    }
  }
}
