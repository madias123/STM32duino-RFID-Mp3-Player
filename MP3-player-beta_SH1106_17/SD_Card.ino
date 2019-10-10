// ************** get ID3 TAG
/*
  Offset Länge Bedeutung
  0       3 Kennung „TAG“ zur Kennzeichnung eines ID3v1-Blocks
  3       30  Titel des Musikstücks
  33      30  Künstler/Interpret
  63      30  Album
  93      4 Erscheinungsjahr
  97      30  Beliebiger Kommentar
  127     1 Genre
*/

void close_SD_all() {
  if (file.isOpen())
    file.close();
  if (dirFile.isOpen())
    dirFile.close();
}

void  getTrackInfo(uint8_t offset, char* infobuffer) {
  //skip to end
  file.seekEnd((-128 + offset));
  //read 30 bytes of tag informat at -128 + offset
  file.read(infobuffer, 30);
  infobuffer = strip_nonalpha_inplace(infobuffer);
}

char* strip_nonalpha_inplace(char *s) {
  for ( ; *s && !isalpha(*s); ++s)
    ; // skip leading non-alpha chars
  if (*s == '\0')
    return s; // there are no alpha characters

  char *tail = s + strlen(s);
  for ( ; !isalpha(*tail); --tail)
    ; // skip trailing non-alpha chars
  *++tail = '\0'; // truncate after the last alpha

  return s;
}

// ************** get MP3 directory
void getMP3dir(bool dironly) {
  dir_numbers_entries = 0;
  int dirfilecounter = 0;
  char xstring[21]; // temp string
  memset (xstring, 0, sizeof(xstring));
  memset (dirIDlist, 0, sizeof(dirIDlist));
  if (!dirFile.open(MP3filePath, O_READ)) {
    notfounderror(1) ;
  }

  while ( file.openNext(&dirFile, O_READ) && dirfilecounter < MAXFILES) { // count  MP3 subfolders
    if (  !file.isHidden()  ) { // avoid hidden files
      if (!(dironly && !file.isDir()))
        dirfilecounter++;
    }
    file.close();
  }

  close_SD_all();
  char *fileName[dirfilecounter + 1]; // array with size of counted dir files +1
  dirFile.open(MP3filePath, O_READ); // reopen dir (rewind doesn't work?)

  while ( file.openNext(&dirFile, O_READ) && dir_numbers_entries < dirfilecounter) { // getting number of MP3 subfolders
    if (  !file.isHidden()) { // avoid hidden files
      if (!(dironly && !file.isDir())) {
        file.getName(xstring, 20); // get only 20 chars from each file name
        // file.getSFN(xstring ); // better alternative get 8.3 filename? saves RAM
        dirIDlist[dir_numbers_entries] = file.dirIndex();
        xstring[0] = toupper(xstring[0]); // upper case the first char
        dir_numbers_entries++;
        fileName[dir_numbers_entries - 1] = (char *)malloc(22);
        sprintf(fileName[dir_numbers_entries - 1], "%s", xstring); // adding the temp. xstring to the char array
      }
    }
    file.close();
  }

  close_SD_all();
  // sorting the file list (only first 20 chars)
  for (int j = 0; j < dir_numbers_entries - 1; j++)
  {
    for (int i = 0; i < dir_numbers_entries - 1; i++)
    {
      if (strncmp(fileName[i], fileName[i + 1], 20) > 0)
      {
        char *  temp = fileName[i];
        int temp2  = dirIDlist[i];
        fileName[i] = fileName[i + 1];
        dirIDlist[i] = dirIDlist[i + 1];
        fileName[i + 1] = temp;
        dirIDlist[i + 1] = temp2;
      }
    }

  }
  /*
    // output file list
    //#DEBUG# CompositeSerial.println();
    //#DEBUG# CompositeSerial.println("####### sorted list #########");
    for (int x = 0; x < dirfilecounter; x++) {
    if (x < 10) {
      //#DEBUG# CompositeSerial.print("0");
    }
    //#DEBUG# CompositeSerial.print (x);
    //#DEBUG# CompositeSerial.print(" ID: ");
    //#DEBUG# CompositeSerial.print (dirIDlist[x]);
    //#DEBUG# CompositeSerial.print(" Entry: ");
    //#DEBUG# CompositeSerial.println (fileName[x]);
    }
    //#DEBUG# CompositeSerial.println();
    //#DEBUG# CompositeSerial.print  ("MP3 sub dir in Dir: ");
    //#DEBUG# CompositeSerial.println(dirfilecounter);
  */
  for (int x = 0; x < dir_numbers_entries; x++) { // free memory!
    free(fileName[x]);
    fileName[x] = 0;
  }
}

// ************** just a directory printout
void opentest() {
  //#DEBUG# CompositeSerial.println();
  //#DEBUG# CompositeSerial.println("**** Test output ****");
  printfreestack();
  byte sortnumber = 0;
  dirFile.open(MP3filePath,  O_READ);
  while (mp3DirSort[sortnumber]) {
    file.open(&dirFile, mp3DirSort[sortnumber], O_READ);
    if (sortnumber < 10)
      //#DEBUG# CompositeSerial.print("0");
      //#DEBUG# CompositeSerial.print(sortnumber);
      //#DEBUG# CompositeSerial.print(": ");
      file.printName(&CompositeSerial);
    //#DEBUG# CompositeSerial.println();
    sortnumber++;

    file.close();
  }
  close_SD_all();
  printfreestack();
}

// ************** sorting MP3 files
// todo wenn files nicht mit 01-0xx beginnen - standardsortierung
// more than mp3, ogg, wav, wma files?
bool sortDir() { // returns 1 if successfully sorted media files, 0 if not.
  char searchstring[2];
  bool status = 0;
  memset (searchstring, 0, sizeof(searchstring));
  if (!dirFile.open(MP3filePath, O_READ)) {
    //#DEBUG# CompositeSerial.print("open MP3-dir folder failed");
  }
  else {
    unsortedflag = 0;
    byte mp3DirUnsorted[100]; // catching non sortable audio files
    memset(mp3DirSort, 0, sizeof(mp3DirSort));
    mp3Dirmax = 0;
    byte filecounter = 0;
    byte sorted = 0;
    byte unsorted = 0;
    boolean badflag = 0;
    while ( file.openNext(&dirFile, O_READ)) {
      if (!file.isHidden()) { // avoid hidden files
        char existmp3[100];
        file.getName(existmp3, 99); // get file name into char array
        if (strstr(existmp3, ".mp3") || strstr(existmp3, ".ogg") || strstr(existmp3, ".wav") || strstr(existmp3, ".wma")) { // sort out files
          char firsttwo[2] = {existmp3[0], existmp3[1]}; // get the first two chars, hopefully like "01"
          filecounter++;
          byte sortnumber = atoi (firsttwo); // getting first two chars of file name as number
          if (sortnumber > 0) {
            sorted++;
            //#DEBUG# CompositeSerial.print(existmp3);
            //#DEBUG# CompositeSerial.print (" *** first ***" );
            //#DEBUG# CompositeSerial.println(file.dirIndex());
            mp3DirSort[sortnumber - 1] = file.dirIndex(); // creating index array
          }
          else { // deeper searching: finding track number (xx) in whole file name only works with two digits!
            badflag = 1;
            for (int x = -1; x < 100; x++) {
              itoa(x, searchstring, 10);
              if (x < 10) {
                searchstring[1] = searchstring[0];
                searchstring[0] = '0';
              }
              if (strstr(existmp3, searchstring)) {
                sorted++;
                //#DEBUG# CompositeSerial.print(existmp3);
                //#DEBUG# CompositeSerial.print (" *** secondary ***" );
                //#DEBUG# CompositeSerial.println(file.dirIndex());
                mp3DirSort[x - 1] = file.dirIndex(); // creating index array
                badflag = 0;
              }

            }
            if  (badflag) { // non sortable audio files
              mp3DirUnsorted[unsorted] = file.dirIndex();
              unsorted++;
              //#DEBUG# CompositeSerial.print(existmp3);
              //#DEBUG# CompositeSerial.print (" *** BAD flag ***");
              //#DEBUG# CompositeSerial.println(file.dirIndex());
              badflag = 0;
            }
          }
        }
      }
      file.close();
    }
    mp3Dirmax = sorted + unsorted; // numbers of entries in mp3sort array
    if (file.isOpen()) {// to be save
      file.close();
    }
    //#DEBUG# CompositeSerial.println();
    //#DEBUG# CompositeSerial.print ("Unsorted  ");
    //#DEBUG# CompositeSerial.println (unsorted);
    if (unsorted > 0) { // flag if not all or any files are sorted and fill up gaps with unsorted files
      unsortedflag = 1;
      byte trackstogo = unsorted;
      byte counterA = 0;
      byte counterB = 0;
      //#DEBUG# CompositeSerial.println ("Entering while  ");
      while (trackstogo != 0) {
        while (mp3DirSort[counterA] != 0)   { // find gap between sorted tracks
          counterA++;
        }
        mp3DirSort[counterA] = mp3DirUnsorted[counterB ];
        counterB++;
        trackstogo--;
        //#DEBUG# CompositeSerial.print ("Unsorted left ");
        //#DEBUG# CompositeSerial.print (trackstogo);
        //#DEBUG# CompositeSerial.print ("CounterA ");
        //#DEBUG# CompositeSerial.println (counterA);
      }
      //#DEBUG# CompositeSerial.println ("Leaving while  ");
      //   for (int x = 0; x <= unsorted; x++) { // putting unsorted files to end of sorted list
      //    mp3DirSort[x + sorted ] = mp3DirUnsorted[x ];
      // }

    }
    numbersoftracks = sorted;
    currenttrack = 0;
    //#DEBUG# CompositeSerial.println();
    //#DEBUG# CompositeSerial.print("Total MP3 files: ");
    //#DEBUG# CompositeSerial.println(filecounter);
    //#DEBUG# CompositeSerial.print("Total sorted Files: ");
    //#DEBUG# CompositeSerial.println(sorted);
    //#DEBUG# CompositeSerial.println("Sort Array: ");
    for (int x = 0; x < sizeof(mp3DirSort); x++) {
      if (mp3DirSort[x] != 0) {
        //#DEBUG# CompositeSerial.print("#");
        //#DEBUG# CompositeSerial.print(x);
        //#DEBUG# CompositeSerial.print(":");
        //#DEBUG# CompositeSerial.print(mp3DirSort[x]);
      }
    }
    dirFile.close();
    if (mp3Dirmax > 0)
      status = 1;
  }
  return status;
}

boolean writeTagSD() {
  if (!dirFile.open(TAGidPath, O_READ)) {
    notfounderror(2);
  }
  char tempfile[255];
  memset(tempfile, 0, sizeof(tempfile));
  strcat (tempfile, TAGidPath);
  strcat (tempfile, UIDstring);
  File myFile = audio_SD.open(tempfile, FILE_WRITE);
  myFile.print(MP3filePath);
  myFile.close();
  dirFile.close();
}

// ************** read MP3 TAG from SD card
byte readTagSD() { // status 0: nothing (error): status 1: ID exists as file, status 2: ID file doesn't exists
  byte status = 0;
  // Achtung! stop player, close file....interrupts?
  // problem: \r\n nicht erkannt, doch mit while schleife einzeln einlesen? Ignorieren - User soll das mit Player machen
  if (!dirFile.open(TAGidPath, O_READ)) {
    notfounderror(2);
  }
  else {
    if (dirFile.exists(UIDstring)) {
      //  display.println("ID File ready!");
      dirFile.close();
      status = 1;
      char tempfile[255];
      memset(tempfile, 0, sizeof(tempfile));
      strcat (tempfile, TAGidPath);
      strcat (tempfile, UIDstring);
      //#DEBUG# CompositeSerial.println(tempfile);
      File test = audio_SD.open(tempfile);
      //#DEBUG# CompositeSerial.println();
      char temptext[255];
      memset(temptext, 0, sizeof(temptext));
      memset(MP3filePath, 0, sizeof(MP3filePath));
      char delim[] = "\n\r";
      test.fgets(temptext, 150, delim );
      //  strcat (MP3filePath, "MP3/");
      strcat (MP3filePath, temptext );
      // strcat (MP3filePath, "/" );
      //#DEBUG# CompositeSerial.print("'");
      //#DEBUG# CompositeSerial.print(MP3filePath);
      //#DEBUG# CompositeSerial.println("'");
      test.close();
      dirFile.close();

    }
    else {
      display.println("TAG ist unbekannt");
      status = 2;
      dirFile.close();
    }
  }
  if (file.isOpen()) {// to be save
    file.close();
  }
  return status;
}

void getbmp(char* filename) {
  byte header[62];
  boolean invertpix = 0;
  dirFile.open("/", O_READ);
  file.open(&dirFile, filename, O_READ);
  file.read  (header, 62 ) ; // getting header of BMP
  // some checks
  if (header[0x0] != 0x42 && header[0xE] != 0x28 && header[0x12] != 128 && header[0x16]  != 0x64 ) {
    //#DEBUG# CompositeSerial.println ("BAD pix");
  }
  else {
    if (header[0x36] != 0) // significant color in pix! either black or white
      invertpix = 1;
    file.seekSet(header[0x0A]); // 0A = start picture data
    char secondbuffer[16]; // buffer for one row
    for (int x = 0; x < 64; x++) { // rows
      file.read  (secondbuffer, 16 ) ; // get one row
      for (int y = 0; y < 16; y++) {
        for (int z = 0; z < 8; z++) {
          if (invertpix)
            display.drawPixel((y * 8) + z, 63 - x, bitRead(secondbuffer[y], 7 - z));
          else
            display.drawPixel((y * 8) + z, 63 - x, !bitRead(secondbuffer[y], 7 - z));
        }
      }
    }
    display.display_alt();
  }
  close_SD_all();
}
