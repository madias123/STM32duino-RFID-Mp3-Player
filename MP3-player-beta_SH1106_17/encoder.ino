void encodercheck() {
  if ((lastEncoderPos != encoderpos)) {
    //#DEBUG# CompositeSerial.print(" Value ");
    //#DEBUG# CompositeSerial.println(encoderpos);
    encflag = LOW;
    lastEncoderPos = encoderpos;
    //   musicPlayer.setVolume(encoderpos, encoderpos);
  }
}

// ********encoder function
void readEncoders() {
  lastEncoderPos = encoderpos;
  if ( (gpio_read_bit(PIN_MAP[encoderpinA].gpio_device, PIN_MAP[encoderpinA].gpio_bit) ? HIGH : LOW) != A_set )
  {
    A_set = !A_set;
    if ( A_set && !B_set )
    {
      if (millis() - encodertimer > 1)
        encoderpos += 1;
      else
        encoderpos += 5;
    }
    encodertimer = millis();
  }
  if ( (gpio_read_bit(PIN_MAP[encoderpinB].gpio_device, PIN_MAP[encoderpinB].gpio_bit) ? HIGH : LOW) != B_set )
  {
    B_set = !B_set;
    if ( B_set && !A_set )
      if (millis() - encodertimer > 1)
        encoderpos -= 1;
      else
        encoderpos -= 5;
    encodertimer = millis();
  }
  if (encoderpos < encoderMIN) encoderpos = encoderMIN;
  if (encoderpos > encoderMAX) encoderpos = encoderMAX;
  if (lastEncoderPos != encoderpos) {
    encflag = HIGH;
    lastEncoderPos = encoderpos;
  }
}

void initEncoders()
{
  encodertimer = millis(); // acceleration measurement

  encstate = HIGH;
  encflag = HIGH;
  A_set = false;
  B_set = false;
  encoderpos = 0;
  pinMode(encoderpinA, INPUT_PULLUP);
  pinMode(encoderpinB, INPUT_PULLUP);
  lastEncoderPos = 1;

  // timer setup for encoder
  timer.pause();
  timer.setPeriod(ENCODER_RATE); // in microseconds
  timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  timer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  timer.attachCompare1Interrupt(readEncoders);
  timer.refresh();
  timer.resume();
  encoderpos = 0;
}
