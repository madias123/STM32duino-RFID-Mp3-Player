/* power saving
  most function for PCB version 2.0!
  2 mosfets:
  PB1: BT-Module Power Mosfet
  PB3: PAM-Amp Power Mosfet

  MFRC522 power savings:
  void PCD_Init(byte resetPowerDownPin);
  void PCD_Init(byte chipSelectPin, byte resetPowerDownPin);
  void PCD_Reset();
  void PCD_AntennaOn();
  void PCD_AntennaOff();
  byte PCD_GetAntennaGain();
  void PCD_SetAntennaGain(byte mask);
  bool PCD_PerformSelfTest();

  VS1053
  Yeah, pulling XRESET low should shut the VS1053 down as far as it will go.
  When you send XRESET high again, the VS1053 will reboot, which apparently takes about 2ms.
  Then you could call the vs1053.begin() method to get the chip ready to play files again.
  #define BREAKOUT_RESET  PA2
*/

//#DEBUG# CompositeSerial.println(  mfrc522.PCD_GetAntennaGain() );
////#DEBUG# CompositeSerial.println("Power Antenna off in 1 second");
//delay(1000);
//mfrc522.PCD_AntennaOff();
////#DEBUG# CompositeSerial.println("Antenna off");
//delay(2000);
////#DEBUG# CompositeSerial.println("VS 1053 off");
//digitalWrite(PA2,LOW);
//delay(10000);

/* power save code bis jetzt:
    mfrc522.PCD_AntennaOff();
   digitalWrite (BREAKOUT_RESET,0);
   systick_disable();
   asm("wfi");

   */
