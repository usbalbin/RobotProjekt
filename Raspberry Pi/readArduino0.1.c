#include <stdio.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <stdbool.h>

void loop(void);
void setup(void);
bool recieveSerial(void);

int fd;
int angl;
float avst;
char buff[255];

int main(void){
  printf("Hejhopp");
  setup();
  for (;;)
  {
    loop();
  }
  return 0 ;
}

void setup(void){
  wiringPiSetup();
  printf("Hej");
  delay(1000);
  fd = serialOpen("/dev/ttyS0", 9600);
  printf("Startar.....");
}

void loop(void){
  if(recieveSerial()){
    printf("%.4f", avst);
    printf("%d",angl);
  }
}

bool recieveSerial(){
  const int maxTime = 10;//tid att köra funktionen i. Ex maxTime 10 gör att funktionen automatiskt avbryts efter 10ms om ej redan färdig
  char c;
  static int i;
  static bool started;
  unsigned long long startTime=millis();
  while(serialDataAvail(fd) && millis() <= startTime + maxTime){//Ger den "maxTime" millisekunder att köra funktionen, för att den ej ska ta upp för mycket tid
    c = serialGetchar(fd);
    if(started && c == '<'){
      started = false;
      sscanf( buff, "%d  %e", &angl, &avst );// %e=float, %d=int
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
