/*Nytt sedan 0.63:
	* Turning penalty
	* Bug:
		Can not run when compiled using gcc
	* To Do:
		* Check that turnPenaltyCalc is working correctly
		
*/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
//#include <wiringPi.h>
//#include <wiringSerial.h>

void setup(void);
void loop(void);
bool findPath(void);
int hCalc(int x,int y);
bool checkAndAddNodesStraight(int x,int y,int parentG,int p);
bool checkAndAddNodesDiagonal(int incX,int incY,int parentX, int parentY,int parentG,int p);
bool checkAndAddNodesSemiDiagonal(int incX,int incY,int parentX, int parentY,int parentG,int p);
bool noCornersCrossed(int incX,int incY,int parentX,int parentY);
int obstNearbyCost(int x, int y);
bool isGoal(int x, int y);
bool placeIsDrivable(int x, int y,int g);
bool placeIsClear(int x, int y);
bool findLowestFval(void);
bool addNewNodes(int parentX,int parentY);
void createPath(void);
void writeToOpenlist(int x, int y, int g, int p);
void writeToClosedlist(int x, int y, int g, int p);
void moveToClosedlist(int x, int y);
void printMap(void);

//-----------------------------------------------------







//inställningar:
bool debugg = true;// true skapar debugg output
//#define width 10
//#define height 10
const unsigned int width=50,height=50;
unsigned int startX=0,startY=0;
unsigned int goalX=49,goalY=49;
const unsigned char gCostNondiagonal=10;
const unsigned char gCostDiagonal=14;
const unsigned char gCostSemiDiagonal=22;//Så som upp-upp-höger (hästen i schack)

const bool allowDiagonal = true;
const bool allowSemiDiagonal = true;
const unsigned char obstNearbyWeight=0;
const float turnWeight= 0.5;//Där 1 betyder 1 straffpoäng / graders sväng;
const unsigned char hWeight=5;

//karta_typAvPkt[height][width]//  Där 0 är övrigt, 1 är vägg, 2 är openlist, 3 är closedlist
unsigned int karta_typAvPkt[50][50]={
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,1,0,0,1,1,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,1,0,0,1,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,1,0,0,1,0,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,1,1,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,1,1,1,1,1,1,1,0,1,1,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,1,1,1,1,1,1,1,1,0,0,0,1,0,0,1,0,1,0,0,1,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,1,0,0,0,0,1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{1,1,1,1,1,1,1,0,0,1,0,0,0,1,0,0,1,0,1,1,1,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,1,0,1,0,0,1,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,1,1,1,0,0,1,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0},
	{1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,1,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0},
	{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0},
	{1,0,0,0,0,0,0,1,0,0,0,1,1,1,1,0,0,1,1,0,0,1,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,0,0,1,0,0,1,1,0,0,1,0,0,0,1,0,0,0,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0},
	{1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,1,0,0,1,0,0,0,1,0,0,0,1,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,1,0,0,1,1,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,1,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,1,0,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,1,1,1,0,0,0,1,0,1,0,1,0,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,1,1,1,1,1,1,1,0,1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,1,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,1,1,1,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,1,1,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{1,1,1,1,1,1,1,0,0,1,0,0,0,1,1,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,1,0,1,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,1,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0}

};	
unsigned int karta_gVarde[50][50]; //karta_gVarde[height][width]//obs kartor > 5*5 bör ha unsigned short här ist för byte(=unsigned char) (troligen funkar byte på kartor upp till ca 7*7)
unsigned int karta_parent[50][50]; //karta_parent[height][width]//obs kartor > 16*16 måste ha unsigned short ist för byte(=unsigned char). parent beräknas parentY*width+parentX
unsigned int parentX,parentY;
unsigned int startID;
int* pathX = 0;
int* pathY = 0;//Måste vara = 0 !!!
unsigned int pathLength = 0;

int main(void){
  setup();
  /* for (;;)
  {
    loop();
  } */
  return 0 ;
}

void setup(void){
	if(debugg)printf("started!\n");
    if(findPath()){
        createPath();
	}
	printMap();
}

void loop(void){
     if(debugg)printf("running!\n");
}









bool findPath(void){
	if(debugg)printf("path finding started!\n");
    startID = startY * width + startX;
    writeToClosedlist(startX,startY,0,startID);
    addNewNodes(startX,startY);
    
    for(unsigned long i = 0; i < width * height*20; i++){
        if(!findLowestFval()){
			printf("Fail!\n");
            return false;
		}
		//printf("parent x;y %d;%d\n",parentX,parentY);
        if(addNewNodes(parentX,parentY))
            return true;
		printMap();
    }
    return false;
}

int hCalc(int x,int y){
    
    int deltaX=x-goalX;
    int deltaY=y-goalY;
    if(deltaX<0)
        deltaX*=-1;

    if(deltaY<0)
        deltaY*=-1;

    return hWeight*(deltaX+deltaY);
}

int turnPenaltyCalc(int x, int y, int parentX, int parentY){
	
	int parentsParentY = karta_parent[parentY][parentX];
    int parentsParentX = parentsParentY % width;
    parentsParentY = (parentsParentY - parentsParentX) / width;
	float zero,newAngle;
	if(parentsParentY - parentY == 0 && parentsParentX - parentX > 0)
		zero = 90;
	else if(parentsParentY - parentY == 0 && parentsParentX - parentX < 1)
		zero = 270;
	else
		zero = 57.3*(double)atan((double)(parentsParentX - parentX)/(double)(parentsParentY - parentY));//Riktning fram till parent (57 deg ca = 1 radian)
	
	if(parentY - y == 0 && parentX - x > 0)
		newAngle = 90;
	else if(parentY - y == 0 && parentX - x < 1)
		newAngle = 270;
	else
		newAngle = 57.3*(double)atan((double)(parentX - x)/(double)(parentY - y));							//Riktning mellan parent och nuvarande pkt.
	int turn = newAngle-zero;
	//turn = (turn+180) % 360 -180;
	if(turn < 0)
		turn *= -1;
	if(turn > 180)
		turn = 360-turn;
	//if(turn > 180)
	//	turn = (turn-360)*-1;//Beräknar absolutvärdet av vinkeln
	printf("%d;%d till %d;%d:%fdeg, till %d;%d:%fdeg => %d\n",parentsParentX,parentsParentY,parentX,parentY,zero,x,y,newAngle,turn);	
	return (180-turn)*turnWeight;//Returnerar svängen som krävs(grader) * turnWeight
}

bool findLowestFval(void){
    unsigned int f, bestF=0;
    for(unsigned int y = 0; y < height; y++){
        for(unsigned int x = 0; x < width; x++){
            if(karta_typAvPkt[y][x] == 2){//Kontrollerar att punkten finns på openlist
                f = karta_gVarde[y][x]+hCalc(x,y);
				//printf("Hel lista F:%d %d;%d\n",f,x,y);
                if(bestF==0 || f < bestF){
                    //printf("Kandidat F:%d %d;%d\n",f,x,y);
					parentX=x;
                    parentY=y;
					bestF = f;
                }
            }
        }
    }
    if(bestF==0)
        return false;
    else
        return true;
}

bool addNewNodes(const int parentX,const int parentY){
    moveToClosedlist(parentX,parentY);
    if(debugg)printf("\nLÃ¤gger till nod %d;%d\n",parentX,parentY);
    long x,y;
	int incX;
	int incY;
    const unsigned long p = parentY * width + parentX;// förälderns "id" eller nummer räknat från 0;0
    const unsigned long parentG=karta_gVarde[parentY][parentX];
    //Upp
    x=parentX;
    y=parentY-1;
	if(checkAndAddNodesStraight(x,y,parentG,p))
		return true;
    //Höger
    x=parentX+1;
    y=parentY;
	if(checkAndAddNodesStraight(x,y,parentG,p))
		return true;
    
    //Ner
    x=parentX;
    y=parentY+1;
	if(checkAndAddNodesStraight(x,y,parentG,p))
		return true;

    //Vänster
    x=parentX-1;
    y=parentY;
	if(checkAndAddNodesStraight(x,y,parentG,p))
		return true;
	
    if(allowDiagonal){
		//Höger-Upp
		incX=1;
		incY=-1;
		//printf("LÃ¤gger till Höger-Upp nod %d;%d\n",parentX+incX,parentY+incY);
		if(checkAndAddNodesDiagonal(incX,incY,parentX,parentY,parentG,p))
			return true;
	
		//Höger-Ner
		incX=1;
		incY=1;
		//printf("LÃ¤gger till Höger-Ner nod %d;%d\n",parentX+incX,parentY+incY);
		if(checkAndAddNodesDiagonal(incX,incY,parentX,parentY,parentG,p))
			return true;

	
		//Vänster-Ner
		incX=-1;
		incY=1;
		//printf("LÃ¤gger till Vänster-Ner nod %d;%d\n",parentX+incX,parentY+incY);
		if(checkAndAddNodesDiagonal(incX,incY,parentX,parentY,parentG,p))
			return true;

	
		//Vänster-Upp
		incX=-1;
		incY=-1;
		//printf("LÃ¤gger till Vänster-Upp nod %d;%d\n",parentX+incX,parentY+incY);
		if(checkAndAddNodesDiagonal(incX,incY,parentX,parentY,parentG,p))
			return true;
    }
            
    if(allowSemiDiagonal){
		//Upp-Upp-Höger
		incX=1;
		incY=-2;
		if(checkAndAddNodesSemiDiagonal(incX,incY,parentX,parentY,parentG,p))
			return true;
    
		//Upp-Höger-Höger
		incX=2;
		incY=-1;
		if(checkAndAddNodesSemiDiagonal(incX,incY,parentX,parentY,parentG,p))
			return true;
    
		//Ner-Höger-Höger
		incX=2;
		incY=1;
		if(checkAndAddNodesSemiDiagonal(incX,incY,parentX,parentY,parentG,p))
			return true;

		//Ner-Ner-Höger
		incX=1;
		incY=2;
		if(checkAndAddNodesSemiDiagonal(incX,incY,parentX,parentY,parentG,p))
			return true;
    
		//Ner-Ner-Vänster
		incX=-1;
		incY=2;
		if(checkAndAddNodesSemiDiagonal(incX,incY,parentX,parentY,parentG,p))
			return true;
    
		//Ner-Vänster-Vänster
		incX=-2;
		incY=1;
		if(checkAndAddNodesSemiDiagonal(incX,incY,parentX,parentY,parentG,p))
			return true;
    
		//Upp-Vänster-Vänster
		incX=-2;
		incY=1;
		if(checkAndAddNodesSemiDiagonal(incX,incY,parentX,parentY,parentG,p))
			return true;
    
		//Upp-Upp-Vänster
		incX=-1;
		incY=2;
		if(checkAndAddNodesSemiDiagonal(incX,incY,parentX,parentY,parentG,p))
			return true;
    }

     
    /* if(x >= 0 && x < width && y >= 0 && y < height){//Här kontrolleras att punkten finns innom kartan
        if(karta_typAvPkt[y][x]==0 || (karta_typAvPkt[y][x]==2 && g < karta_gVarde[y][x])){
          	//printf("Lägger till pkt: %d;%d\n",x,y);
			writeToOpenlist(x,y,g,p);
    	}
    }  */
    return false;
}

void createPath(void){
    pathLength=1;
    
    //Beräknar längden på path genom att gå från mål till start
    unsigned int x = goalX, y = goalY; 
	while(x != startX || y != startY){
        y = karta_parent[y][x];
        x = y%width;
        y = (y-x)/width;
		if(debugg)printf("Skapar väg %d;%d \n",x,y);
		pathLength++;
    }
	 if(debugg)printf("pathLength %d\n",pathLength);
    
    //Sätter längden på pathX-arrayen
    if (pathX != 0) {
        pathX = (int*) realloc(pathX, pathLength * sizeof(int));
    }else {
        pathX = (int*) malloc(pathLength * sizeof(int));
    }
	
	//Sätter längden på pathY-arrayen
    if (pathY != 0) {
        pathY = (int*) realloc(pathY, pathLength * sizeof(int));
    }else {
        pathY = (int*) malloc(pathLength * sizeof(int));
    }
    
    //stoppar in alla värden i path från closedlist
    for(int i = pathLength-1, x = goalX, y = goalY; i >= 0; i--){
        pathX[i] = x;
		pathY[i] = y;
        y = karta_parent[y][x];
        x = y % width;
        y = (y - x) / width;
    }
   
}

bool checkAndAddNodesStraight(int x,int y, int parentG,int p){
	
	parentY = karta_parent[y][x];
    parentX = parentY % width;
    parentY = (parentY - parentX) / width;
	
	
	int g=parentG+gCostNondiagonal+obstNearbyCost(x,y)+turnPenaltyCalc(x,y,parentX,parentY);
	if(isGoal(x,y)){
		writeToClosedlist(x,y,g,p);
		return true;
	}
	if(debugg)printf("  Kollar %d;%d\n",x,y);
    if(placeIsDrivable(x,y,g)){
        writeToOpenlist(x,y,g,p);
		if(debugg)printf("    %d;%d OK\n",x,y);
    }else{
		if(debugg)printf("    %d;%d NOT OK\n",x,y);
	}
	return false;
}

bool checkAndAddNodesDiagonal(int incX,int incY,int parentX, int parentY,int parentG,int p){
	int g=parentG+gCostDiagonal+obstNearbyCost(parentX+incX,parentY+incY)+turnPenaltyCalc(parentX+incX,parentY+incY,parentX,parentY);
	if(noCornersCrossed(incX,incY,parentX,parentY) && placeIsDrivable(parentX+incX,parentY+incY,g)){
		writeToOpenlist(parentX+incX,parentY+incY,g,p);
		if(isGoal(parentX+incX,parentY+incY)) return true;
	}
	return false;
}

bool checkAndAddNodesSemiDiagonal(int incX,int incY,int parentX, int parentY,int parentG,int p){
	int g=parentG+gCostSemiDiagonal+obstNearbyCost(parentX+incX,parentY+incY)+turnPenaltyCalc(parentX+incX,parentY+incY,parentX,parentY);
	if(noCornersCrossed(incX,incY,parentX,parentY) && placeIsDrivable(parentX+incX,parentY+incY,g)){
		writeToOpenlist(parentX+incX,parentY+incY,g,p);
		if(isGoal(parentX+incX,parentY+incY)) return true;
	}
	return false;
}

bool noCornersCrossed(int incX,int incY,int parentX,int parentY){
	if(debugg)printf("  Kollar vägen från %d;%d till %d;%d\n",parentX,parentY,parentX+incX,parentY+incY);
	int stepX,stepY;
	if(incX>0){
		stepX=1;
	}else if(incX<0){
		stepX=-1;
	}else{
		stepX=0;
	}
	if(incY>0){
		stepY=1;
	}else if(incY<0){
		stepY=-1;
	}else{
		stepY=0;
	}
	
	int y = parentY;
	int x = parentX;
	 do{
		x = parentX;
		do{
			if(debugg)printf("    Checking %d;%d\n",x,y);
			if(!placeIsClear(x,y)){
				if(debugg)printf("      %d;%d NOT OK\n",x,y);
				return false;
			}
			x += stepX;
		}while(x != parentX + incX + stepX);
		y += stepY;
	}while( y != parentY + incY + stepY);	//"+ stepY" är p.g.a. raden ovan "y += stepY;" då detta värde ej är testat.
											//senaste testade värde (om vi är färdiga) är  "parentY + incY" men då vi
											//laggt till "stepY" kollar vi "parentY + incY + stepY" 
	
	if(debugg)printf("      %d;%d OK\n",x - stepX,y - stepY);
	return true;
}

bool isGoal(int x, int y){
	if(x == goalX && y == goalY)
		return true;
	else
		return false;
}

int obstNearbyCost(int x, int y){
	int nodes = 0;
	//Upp
    x=parentX;
    y=parentY-1;
	if(!placeIsDrivable(x,y,0))
		nodes++;
    //Höger
    x=parentX+1;
    y=parentY;
	if(!placeIsDrivable(x,y,0))
		nodes++;
    
    //Ner
    x=parentX;
    y=parentY+1;
	if(!placeIsDrivable(x,y,0))
		nodes++;

    //Vänster
    x=parentX-1;
    y=parentY;
	if(!placeIsDrivable(x,y,0))
		nodes++;
	//printf("%d\n",nodes);
	return obstNearbyWeight*nodes;
}

bool placeIsDrivable(int x, int y,int g){
	if(x >= 0 && x < width && y >= 0 && y < height){//Här kontrolleras att punkten finns innom kartan
		if(karta_typAvPkt[y][x]==0 || (karta_typAvPkt[y][x]==2 && g < karta_gVarde[y][x])){//Kolla att inte är vägg/closed list, om openlist skriv bara över om nya vägen har lägre g
			return true;
		}
	}
	return false;
}

bool placeIsClear(int x, int y){
	if(x >= 0 && x < width && y >= 0 && y < height){//Här kontrolleras att punkten finns innom kartan
		if(karta_typAvPkt[y][x] != 1){//Kolla att inte är vägg
			return true;
		}
	}
	return false;
}

void writeToOpenlist(int x, int y, int g, int p){
    karta_typAvPkt[y][x]=2;
    karta_gVarde[y][x]=g;
    karta_parent[y][x]=p;
}

void writeToClosedlist(int x, int y, int g, int p){
    karta_typAvPkt[y][x]=3;
    karta_gVarde[y][x]=g;
    karta_parent[y][x]=p;
}

void moveToClosedlist(int x, int y){
    karta_typAvPkt[y][x]=3;
}

void printMap(){
	printf("\n");
	bool partOfWay = false;
	for(int y = 0; y < height; y++){
		for(int x = 0; x < width; x++){
			if(pathX != 0 && pathY != 0){
				for(int i = 0; i < pathLength; i++){
					if(pathX[i] == x && pathY[i] == y){//Del av väg
						printf("o");
						partOfWay = true;
						break;
					}
				}
			}if(!partOfWay){
				switch(karta_typAvPkt[y][x]){
					case 0:
						printf(" ");
						break;
					case 1:
						printf("#");
						break;
					case 2:
						printf(".");
						break;
					case 3:
						printf(":");
						break;
				}
			}
			/* if(karta_typAvPkt[y][x] == 1){//vägg
				
			}
			else if(!partOfWay){//Tomt
				printf(" ");
			} */
			printf(",");
			partOfWay = false;
		}
		printf("\n");
	}
}