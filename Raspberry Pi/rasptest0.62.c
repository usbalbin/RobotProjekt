/*Nytt sedan 0.6:
	* Klarar diagonaler utan att korsa h�rn
	  �n s� l�nge buggfritt
	* Bugg:
		* 
*/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
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







//inst�llningar:
bool debugg = true;// true skapar debugg output
//#define width 10
//#define height 10
const unsigned int width=7,height=7;
unsigned int startX=0,startY=0;
unsigned int goalX=0,goalY=3;
const unsigned char gCostNondiagonal=10;
const unsigned char gCostDiagonal=14;
const unsigned char gCostSemiDiagonal=22;//S� som upp-upp-h�ger (h�sten i schack)

const bool allowDiagonal = true;
const bool allowSemiDiagonal = true;
const unsigned char obstNearbyWeight=0;
const unsigned char hWeight=5;

//karta_typAvPkt[height][width]//  D�r 0 �r �vrigt, 1 �r v�gg, 2 �r openlist, 3 �r closedlist
unsigned int karta_typAvPkt[7][7]={
/* 	{0,0,0,0,0,0,0},
	{1,1,1,1,1,0,0},
	{0,0,0,0,0,0,0},
	{0,0,0,1,0,0,0},
	{0,0,0,1,0,0,1},
	{0,1,1,1,1,1,0},
	{0,0,0,0,0,0,0} */
	
	{0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0},
	{1,0,0,0,0,0,0},
	{0,1,1,1,0,0,0},
	{0,0,0,0,1,0,0},
	{0,0,0,0,0,1,0},
	{0,0,0,1,0,0,0}
	
/* 	{#,#,#,#,#,#,#},
	{0,0,0,0,0,0,0},
	{1,0,0,0,0,0,0},
	{0,1,1,1,0,x,0},
	{0,x,x,0,1,0,0},
	{0,0,0,0,0,1,0},
	{0,0,0,1,0,0,0}
	
	  0 1 2 3 4 5 6
	0  , , , , , , ,
    1  , , , , , , ,
	2 #, , , , , , ,
	3  ,#,#,#, , , ,
	4 0,1,2, ,#, , ,
	5  , , ,3, ,#, ,
	6  , , ,#, , , ,
 */
};
	
unsigned int karta_gVarde[7][7]; //karta_gVarde[height][width]//obs kartor > 5*5 b�r ha unsigned short h�r ist f�r byte(=unsigned char) (troligen funkar byte p� kartor upp till ca 7*7)
unsigned int karta_parent[7][7]; //karta_parent[height][width]//obs kartor > 16*16 m�ste ha unsigned short ist f�r byte(=unsigned char). parent ber�knas parentY*width+parentX
unsigned int parentX,parentY;
unsigned int startID;
int* pathX = 0;
int* pathY = 0;//M�ste vara = 0 !!!
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

bool findLowestFval(void){
    unsigned int f, bestF=0;
    for(unsigned int y = 0; y < height; y++){
        for(unsigned int x = 0; x < width; x++){
            if(karta_typAvPkt[y][x] == 2){//Kontrollerar att punkten finns p� openlist
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
    if(debugg)printf("\nLägger till nod %d;%d\n",parentX,parentY);
    long x,y;
	int incX;
	int incY;
    const unsigned long p = parentY * width + parentX;// f�r�lderns "id" eller nummer r�knat fr�n 0;0
    const unsigned long parentG=karta_gVarde[parentY][parentX];
    //Upp
    x=parentX;
    y=parentY-1;
	if(checkAndAddNodesStraight(x,y,parentG,p))
		return true;
    //H�ger
    x=parentX+1;
    y=parentY;
	if(checkAndAddNodesStraight(x,y,parentG,p))
		return true;
    
    //Ner
    x=parentX;
    y=parentY+1;
	if(checkAndAddNodesStraight(x,y,parentG,p))
		return true;

    //V�nster
    x=parentX-1;
    y=parentY;
	if(checkAndAddNodesStraight(x,y,parentG,p))
		return true;
	
    if(allowDiagonal){
		//H�ger-Upp
		incX=1;
		incY=-1;
		//printf("Lägger till H�ger-Upp nod %d;%d\n",parentX+incX,parentY+incY);
		if(checkAndAddNodesDiagonal(incX,incY,parentX,parentY,parentG,p))
			return true;
	
		//H�ger-Ner
		incX=1;
		incY=1;
		//printf("Lägger till H�ger-Ner nod %d;%d\n",parentX+incX,parentY+incY);
		if(checkAndAddNodesDiagonal(incX,incY,parentX,parentY,parentG,p))
			return true;

	
		//V�nster-Ner
		incX=-1;
		incY=1;
		//printf("Lägger till V�nster-Ner nod %d;%d\n",parentX+incX,parentY+incY);
		if(checkAndAddNodesDiagonal(incX,incY,parentX,parentY,parentG,p))
			return true;

	
		//V�nster-Upp
		incX=-1;
		incY=-1;
		//printf("Lägger till V�nster-Upp nod %d;%d\n",parentX+incX,parentY+incY);
		if(checkAndAddNodesDiagonal(incX,incY,parentX,parentY,parentG,p))
			return true;
    }
            
    if(allowSemiDiagonal){
		//Upp-Upp-H�ger
		incX=1;
		incY=-2;
		if(checkAndAddNodesSemiDiagonal(incX,incY,parentX,parentY,parentG,p))
			return true;
    
		//Upp-H�ger-H�ger
		incX=2;
		incY=-1;
		if(checkAndAddNodesSemiDiagonal(incX,incY,parentX,parentY,parentG,p))
			return true;
    
		//Ner-H�ger-H�ger
		incX=2;
		incY=1;
		if(checkAndAddNodesSemiDiagonal(incX,incY,parentX,parentY,parentG,p))
			return true;

		//Ner-Ner-H�ger
		incX=1;
		incY=2;
		if(checkAndAddNodesSemiDiagonal(incX,incY,parentX,parentY,parentG,p))
			return true;
    
		//Ner-Ner-V�nster
		incX=-1;
		incY=2;
		if(checkAndAddNodesSemiDiagonal(incX,incY,parentX,parentY,parentG,p))
			return true;
    
		//Ner-V�nster-V�nster
		incX=-2;
		incY=1;
		if(checkAndAddNodesSemiDiagonal(incX,incY,parentX,parentY,parentG,p))
			return true;
    
		//Upp-V�nster-V�nster
		incX=-2;
		incY=1;
		if(checkAndAddNodesSemiDiagonal(incX,incY,parentX,parentY,parentG,p))
			return true;
    
		//Upp-Upp-V�nster
		incX=-1;
		incY=2;
		if(checkAndAddNodesSemiDiagonal(incX,incY,parentX,parentY,parentG,p))
			return true;
    }

     
    /* if(x >= 0 && x < width && y >= 0 && y < height){//H�r kontrolleras att punkten finns innom kartan
        if(karta_typAvPkt[y][x]==0 || (karta_typAvPkt[y][x]==2 && g < karta_gVarde[y][x])){
          	//printf("L�gger till pkt: %d;%d\n",x,y);
			writeToOpenlist(x,y,g,p);
    	}
    }  */
    return false;
}

void createPath(void){
    pathLength=1;
    
    //Ber�knar l�ngden p� path genom att g� fr�n m�l till start
    unsigned int x = goalX, y = goalY; 
	while(x != startX || y != startY){
        y = karta_parent[y][x];
        x = y%width;
        y = (y-x)/width;
		if(debugg)printf("Skapar v�g %d;%d \n",x,y);
		pathLength++;
    }
	 if(debugg)printf("pathLength %d\n",pathLength);
    
    //S�tter l�ngden p� pathX-arrayen
    if (pathX != 0) {
        pathX = (int*) realloc(pathX, pathLength * sizeof(int));
    }else {
        pathX = (int*) malloc(pathLength * sizeof(int));
    }
	
	//S�tter l�ngden p� pathY-arrayen
    if (pathY != 0) {
        pathY = (int*) realloc(pathY, pathLength * sizeof(int));
    }else {
        pathY = (int*) malloc(pathLength * sizeof(int));
    }
    
    //stoppar in alla v�rden i path fr�n closedlist
    for(int i = pathLength-1, x = goalX, y = goalY; i >= 0; i--){
        pathX[i] = x;
		pathY[i] = y;
        y = karta_parent[y][x];
        x = y % width;
        y = (y - x) / width;
    }
   
}

bool checkAndAddNodesStraight(int x,int y,int parentG,int p){
	int g=parentG+gCostNondiagonal+obstNearbyCost(x,y);
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
	int g=parentG+gCostDiagonal+obstNearbyCost(parentX+incX,parentY+incY);
	if(noCornersCrossed(incX,incY,parentX,parentY) && placeIsDrivable(parentX+incX,parentY+incY,g)){
		writeToOpenlist(parentX+incX,parentY+incY,g,p);
		if(isGoal(parentX+incX,parentY+incY)) return true;
	}
	return false;
}

bool checkAndAddNodesSemiDiagonal(int incX,int incY,int parentX, int parentY,int parentG,int p){
	int g=parentG+gCostSemiDiagonal+obstNearbyCost(parentX+incX,parentY+incY);
	if(noCornersCrossed(incX,incY,parentX,parentY) && placeIsDrivable(parentX+incX,parentY+incY,g)){
		writeToOpenlist(parentX+incX,parentY+incY,g,p);
		if(isGoal(parentX+incX,parentY+incY)) return true;
	}
	return false;
}

bool noCornersCrossed(int incX,int incY,int parentX,int parentY){
	if(debugg)printf("  Kollar v�gen fr�n %d;%d till %d;%d\n",parentX,parentY,parentX+incX,parentY+incY);
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
	}while( y != parentY + incY + stepY);	//"+ stepY" �r p.g.a. raden ovan "y += stepY;" d� detta v�rde ej �r testat.
											//senaste testade v�rde (om vi �r f�rdiga) �r  "parentY + incY" men d� vi
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
    //H�ger
    x=parentX+1;
    y=parentY;
	if(!placeIsDrivable(x,y,0))
		nodes++;
    
    //Ner
    x=parentX;
    y=parentY+1;
	if(!placeIsDrivable(x,y,0))
		nodes++;

    //V�nster
    x=parentX-1;
    y=parentY;
	if(!placeIsDrivable(x,y,0))
		nodes++;
	//printf("%d\n",nodes);
	return obstNearbyWeight*nodes;
}

bool placeIsDrivable(int x, int y,int g){
	if(x >= 0 && x < width && y >= 0 && y < height){//H�r kontrolleras att punkten finns innom kartan
		if(karta_typAvPkt[y][x]==0 || (karta_typAvPkt[y][x]==2 && g < karta_gVarde[y][x])){//Kolla att inte �r v�gg/closed list, om openlist skriv bara �ver om nya v�gen har l�gre g
			return true;
		}
	}
	return false;
}

bool placeIsClear(int x, int y){
	if(x >= 0 && x < width && y >= 0 && y < height){//H�r kontrolleras att punkten finns innom kartan
		if(karta_typAvPkt[y][x] != 1){//Kolla att inte �r v�gg
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
					if(pathX[i] == x && pathY[i] == y){//Del av v�g
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
			/* if(karta_typAvPkt[y][x] == 1){//v�gg
				
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