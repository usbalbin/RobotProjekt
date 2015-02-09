int avst;
float angl;
char buff[50];

boolean recieveSerial(){
  const byte maxTime = 10;//tid att köra funktionen i. Ex maxTime 10 gör att funktionen automatiskt avbryts efter 10ms om ej redan färdig
  char c;
  static byte i;
  static boolean started;
  unsigned long long startTime=millis();
  while(Serial.available() && millis() <= startTime + maxTime){//Ger den "maxTime" millisekunder att köra funktionen, för att den ej ska ta upp för mycket tid
    c = Serial.read();
    if(started && c == '<'){
      started = false;
      sscanf( buff, "%e  %d", &angl, &avst );// %e=float, %d=int
      return true;
    }
      
    if(started && c != '>' && c != '<'){
      buff[i] = c;
      i++;
    }
    else if(c == '>'){
      started = true;
      for(i = 0; i < 32; i++){//Empty the buff
        buff[i] = ' ';
      }
    }
  }
  return false;
}
