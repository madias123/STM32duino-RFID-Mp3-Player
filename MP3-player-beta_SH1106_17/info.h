/*
BMP Header monochrome
Hex
00: muss 42 sein
01: muss 4D sein (00+01 = "BM")
0A: Startadresse des Bildes SOLLTE "3E" sein!
0E: Größe Header in Byte (+0E also: 28h + 0E = 36h = Startadresse) SOLLTE "28h" sein
12: Breite in pixel
16: Höhe in pixel
1C: colors per pixel, muss "01" sein bei monochrome!
1E: compression: muss null sein!

Wichtig!
36,37,38: WENN Zeichenfarbe vorwiegend weiss: alle drei "FF", wenn Schwarz "00"

Programmierung:
Filelänge überprüfen (unter 2kb)

Header einlesen also bis Position (inklusive) "3D" 61 (dec) 
00,01, 0A,0E überprüfen
12 Einlesen (breite in Pixel)
16 Einlesen (höhe in Pixel)
36,37,38 (eigentlich nur 36) einlesen für default black oder white

buffer erzeugen (B pixel * H pixel)
if (default black oder white):
Bild in buffer einlesen  

SD FAT Funktionen:

uint32_t File::fileSize  ()  
Returns The total number of bytes in a file.

uint32_t File::position ()  
Returns the current file position.

int FatFile::read ( void *  buf, size_t   nbyte )   
Read data from a file starting at the current position.
Parameters
[out] buf Pointer to the location that will receive the data.
[in]  nbyte Maximum number of bytes to read.
Returns For success read() returns the number of bytes read. A value less than nbyte, including zero, will be returned if end of file is reached. If an error occurs, read() returns -1. Possible errors include read() called before a file has been opened, corrupt file system or an I/O error occurred.

bool File::seek ( uint32_t  pos ) 
Seek to a new position in the file, which must be between 0 and the size of the file (inclusive).
Parameters
[in]  pos the new file position.
Returns true for success else false.
 */
