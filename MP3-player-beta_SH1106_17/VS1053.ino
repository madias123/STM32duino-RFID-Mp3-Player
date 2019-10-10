void VS1053_startplay() {
  digitalWrite (PAM_SHUTDOWN, 1);
  mp3isplaying = 0;
  pauseflag = 0;
  idleflag = 0;
  if (!dirFile.open(MP3filePath, O_READ)) {
    notfounderror(1) ;
  }
  if (mp3DirSort[mp3TrackCurrent] != 0) {
    file.open(&dirFile, mp3DirSort[mp3TrackCurrent], O_READ); // open  media file
    oled_initdisplay();
    char  longtext[31];
    display.setCursor(0, 16);
    display.setTextSize(2);
    getTrackInfo(TRACK_ARTIST, actualInterpret);
  //  for (int x = 0; x < 20; x++)
    //  display.print(longtext[x]);
 
    display.setTextSize(1);
    display.setCursor(0, 34);
    getTrackInfo(TRACK_ALBUM, longtext);
    for (int x = 0; x < 21; x++)
      display.print(longtext[x]);

    getTrackInfo(TRACK_TITLE, actualTrackTitel);



    //    display.println(test);
    //oled_invert_entry(0, 1);
    display.display_alt();
    //#DEBUG# CompositeSerial.println();
    //#DEBUG# CompositeSerial.println("****************");
    //#DEBUG# CompositeSerial.println ("Index: ");
    //#DEBUG# CompositeSerial.print (mp3TrackCurrent);
    //#DEBUG# CompositeSerial.println ("Sort Number: ");
    //#DEBUG# CompositeSerial.print (mp3TrackCurrent);
    //#DEBUG# CompositeSerial.println(mp3DirSort[mp3TrackCurrent]);
    char existmp3[100];
    file.getName(existmp3, 99); // get file name into char array
    //#DEBUG# CompositeSerial.println(existmp3);
    //#DEBUG# CompositeSerial.println("****************");
    file.close();
    dirFile.close();
    //#DEBUG# CompositeSerial.println();
    int filesize = sizeof(MP3filePath) + sizeof(existmp3) + 1;
    char tempfile[filesize];
    memset (tempfile, 0, sizeof(tempfile));
    strcat(tempfile, MP3filePath);
    strcat(tempfile, existmp3);
    printfreestack();
    //#DEBUG# CompositeSerial.println(tempfile);
    // if (! musicPlayer.startPlayingFile(tempfile)) { // old version vs1053 lib
    if (! musicPlayer.startPlayingFile(MP3filePath, mp3DirSort[mp3TrackCurrent])) { // new version - not real names but ID - Umlaute! stable?
      notfounderror(11) ;
    }
    //#DEBUG# CompositeSerial.println("start music now");
    musicPlayer.playingMusic;
    //#DEBUG# CompositeSerial.println("back from plaingMusic");
    mp3isplaying = 1;
  }
}

void VS1053_stopplay() {
  digitalWrite (PAM_SHUTDOWN, 0);
  mp3isplaying = 0;
  idleflag = 0;
  musicPlayer.stopPlaying();
  if (dirFile.isOpen())
    dirFile.close();
  if (file.isOpen())
    file.close();
}

void dump_byte_array(byte * buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    //#DEBUG# CompositeSerial.print(buffer[i] < 0x10 ? " 0" : " ");
    //#DEBUG# CompositeSerial.print(buffer[i], HEX);
  }
}


void VS1053setup() {
  // initialise the music player
  if (! musicPlayer.begin()) { // initialise the music player
    notfounderror(10);
    while (1);
  }
  // musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate VS1053 is working
  musicPlayer.setVolume(main_volume, main_volume);
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);
}
