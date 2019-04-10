//defining musical notes to be used for Buzzer

#define NOTE_A1  55
#define NOTE_C2  65
#define NOTE_F2  87
#define NOTE_D3  147
#define NOTE_F3  175
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_B5  988
#define NOTE_E7  2637

#include <stdlib.h> //include c standard library

//Initialize all memory adresses to be used

volatile int *IO_7SEGEN = (int*)0xbf800010;
volatile int *IO_7SEG0  = (int*)0xbf800014;
volatile int *IO_7SEG1  = (int*)0xbf800018;
volatile int *IO_7SEG2  = (int*)0xbf80001c;
volatile int *IO_7SEG3  = (int*)0xbf800020;
volatile int *IO_7SEG4  = (int*)0xbf800024;
volatile int *IO_7SEG5  = (int*)0xbf800028;
volatile int *IO_7SEG6  = (int*)0xbf80002c;
volatile int *IO_7SEG7  = (int*)0xbf800030;
volatile int *IO_MCNTR = (int*)0xbf800034;
volatile int *IO_LEDR = (int*)0xbf800000;
volatile int *IO_LEDG = (int*)0xbf800004;
volatile int *IO_PB	 = (int*)0xbf80000c;
volatile int *IO_SW = (int*)0xbf800008;
volatile int *bz = (int*)0xbf800038;


int startgame = 0;      //Game Mode Selection variable

int GoS_I_rt, GoS_O_rt; //User input, System output ready for IoT

//Initialize all psuedocode modules to be used

void delay();
void gameOL();
void stscreen();
void winleds();
void loseleds();
void GameOfLEDs();
void correctsong();
void wrongsong();
void startupsong();
void tone();
void notone();
void sadsong();

//-------------------------
// main() Main body of code
//-------------------------

int main() {
  

  GameOfLEDs();
  notone();                 //initialise buzzer tone 0     

  *IO_LEDG = 1;
  *IO_7SEGEN = 0xFF;        //turn off all 7-segment display

  while(1){                 //infinite loop allows for re-entering game startscreen

  startupsong();            //use startup song
  stscreen();               //begin startscreen animations, contains pushbutton to start game
   
  while(startgame == 1){    //if startgame is equal to 1 begin the game
      notone();             //set buzzer to no tone
      gameOL();             //begin game
    }
  }
  
  
  return 0;                 //return 0 expected by int main code, however this should not be reached
}

//-------------------------------------------
// creation of delay function.
// delay(n) allows for delay to be created
// and value of delay (n) to be decided when
// delay is called
//-------------------------------------------

void delay(volatile unsigned int n) {

volatile int *IO_MCNTR = (int*)0xbf800034;
int finish = *IO_MCNTR + n;

while(*IO_MCNTR < finish)
{
  //while the value of *IO_MCNTR is less than finish
  //this will do nothing
}
}

//-------------------------------------------
// Creation of mips handler, in case of fatal
// errors in code
//-------------------------------------------

void _mips_handle_exception(void* ctx, int reason) {
  volatile int *IO_LEDR = (int*)0xbf800000;

  *IO_LEDR = 0x8001;  // Display 0x8001 on LEDs to indicate error state
  while (1) ;
}

//--------------------------------------------
// Creation of tone(freq) function, which lets
// buzzer function with frequency values
// assigned when called
//--------------------------------------------

void tone(unsigned int freq){
  
  volatile unsigned int T, T_final; //initialising values for Period alteration

  T = 1000000/freq;                 //find period of 'freq' in microseconds    
  T_final = T/2;                    //divide period by 2
  *bz = T_final;                    //set T_final to buzzer
}

//--------------------------------------------
// Creation of tone(freq) function, which lets
// buzzer function with frequency values
// assigned when called, and displays the
// frequencies on the 7 seg
//--------------------------------------------

void tonewithseg(unsigned int freq){
  
  volatile unsigned int T, T_final; //initialising values for Period alteration

  T = 1000000/freq;                 //find period of 'freq' in microseconds    
  T_final = T/2;                    //divide period by 2
  *bz = T_final;                    //set T_final to buzzer
  *IO_7SEGEN = 0xF8;
  *IO_7SEG0  = freq % 10;
  *IO_7SEG1  = freq/10 % 10;
  *IO_7SEG2  = freq/100 % 10;
}

//--------------------------------------------
// Creation of notone() allowing buzzer mute
//--------------------------------------------

void notone(){
  *bz = 0;    //assigning 0 to buzzer
}

//--------------------------------------------
// Creation of gameOL()game functionality code
// Containing user inputs and system outputs
//--------------------------------------------

void gameOL(){
  //start the game of LED's this will include only the functionality of the game
  
  int ledoff, x, lives, level, input_time, lifeline, levelloss; //initialise variables used in module
  
      GoS_O_rt = 0;                                             //set system output to 0 which means user is wrong if it is 1
	                                                            //user has gotten level correct, 2 game is won, 3 game was lost

  lives = 3;													//begin with 3 lives
  level = 1;													//being in level 1
  lifeline = 0;													//lifeline used to add lives when 3 level passed in a row
  levelloss = 0;                                                //leveloss used to move level down if lost 3 level in a row
  ledoff = 5000;												//begin with toggle LEDs off in 5 seconds
  input_time = 3000;											//begin with allowing 3 seconds for user to input answer
  *IO_7SEGEN = 0x00;											//enable all 7 segs
  *IO_7SEG7 = 20; 												//display L
  *IO_7SEG6 = 27; 												//        V
  *IO_7SEG5 = 20; 												//        L
  *IO_7SEG4 = 1;  												//        1 this will change throughout game

  *IO_7SEG3 = 20; 												//display L
  *IO_7SEG2 = 15; 												//        F
  *IO_7SEG1 = 14; 												//        E
  *IO_7SEG0 = 3;  												//        3 this will change throughout game
  *IO_LEDG = 0b111001;											//set colored LEDs to show lives white, blue.
  delay(1000);													//wait one second

  
  while(lives != 0 && level != 11 && *IO_PB == 0){  			//run while lives are valid, levels not complete and PB not pressed
																//if PB is pressed a soft rest occurs, after losing state occurs
  
  if(lifeline == 2){											//if statement allowing life addition
    lives++;
    lifeline = 0;
  }
  
  if(level != 10){												//if statement displaying level value on SEG0 unless game is won.
    *IO_7SEG7 = 20; 											//display L
    *IO_7SEG6 = 27; 											//        V
    *IO_7SEG5 = 20; 											//        L
    *IO_7SEG4 = level;

    *IO_7SEG3 = 20; 											//display L
    *IO_7SEG2 = 15; 											//        F
    *IO_7SEG1 = 14; 											//        E

  }

  delay(1000);										//wait 1 second

  srand(*IO_MCNTR);									//Srand used to seed rand() at *IO_MCNTR allowing for
													//'true' random number
  x = (rand() % 254) + 1;							//assign a random number to x between 1-255
  *IO_LEDR = x;										//show value of x on LEDs
  delay(ledoff);                               		//wait 'ledoff' seconds
  *IO_LEDR = 0;										//turn LEDs off
  delay(input_time);								//wait 'input_time' seconds
  GoS_I_rt = *IO_SW;								//assign switch values, set by user, to GoS_I_rt
  if(GoS_I_rt != x){                         		//if user switches were not correct
    GoS_O_rt = 0;									//assign GoS_O_rt to 0
    wrongsong();									//play incorrect inut song wrongsong()
    lifeline = 0;									//set lifeline to 0 dissalowing life gain from 3 level correct in a row
    lives--;                     					//take away a life
    levelloss++;									//add one to value to toggle level down if 3 incorrects in a row
    *IO_7SEG0 = lives;								//display lives on  SEG0
    GoS_I_rt = 0;									//set user input GoS_I_rt to 0
  }
  else{												//if user input is correct
    GoS_O_rt = 1;									//system output GoS_O_rt = 1
    correctsong();									//play correct song
    level++;										//move up level
    lifeline++;										//add to lifeline
    levelloss = 0;									//set levelloss to 0
    *IO_7SEG0 = lives; 								//SEG0 displays lives
    GoS_I_rt = 0;									//User input set to 0 again					
  }

  if(levelloss == 2){								//if statement for level deduction if 3 incorrects in a row
    level--;										//deduct level
    levelloss = 0;
  }

  switch(level){									//switch statement using level values to set difficulty
													//altering ledoff time and input_time harder as level go up
    case 1: ledoff = 5000;
            input_time = 3000;
            break;

    case 2: ledoff = 4500;
            input_time = 3000;
            break;

    case 3: ledoff = 4000;
            input_time = 2800;
            break;

    case 4: ledoff = 3500;
            input_time = 2600;
            break;

    case 5: ledoff = 3000;
            input_time = 2400;
            break;

    case 6: ledoff = 2500;
            input_time = 2200;
            break;

    case 7: ledoff = 2000;
            input_time = 2000;
            break;

    case 8: ledoff = 1500;
            input_time = 1700;
            break;

    case 9: ledoff = 1000;
            input_time = 1500;
            break;

   case 10: ledoff =  750;								//not only setting final level dicciculty but also showing
            input_time = 1300;							//FINAL LVL on seven seg display
            *IO_7SEG7 = 15;
            *IO_7SEG6 = 18;
            *IO_7SEG5 = 21;
            *IO_7SEG4 = 10;
            *IO_7SEG3 = 20;
            *IO_7SEG2 = 20;
            *IO_7SEG1 = 27;
            *IO_7SEG0 = 20;
            break;

  }
  switch(lives){										//setting RGB leds to colors for lives
    case 0: *IO_LEDG = 0b100100;
            break;    
    case 1: *IO_LEDG = 0b111100;
            break;
    case 2: *IO_LEDG = 0b111101;
            break;
    case 3: *IO_LEDG = 0b111001;
            break;
    case 4: *IO_LEDG = 0b101001;
            break;
    case 5: *IO_LEDG = 0b001001;
            break;
    case 6: *IO_LEDG = 0b010010;
            break;
    case 7: *IO_LEDG = 0b011011;
            break;
  }

  }
if(level == 11){										//if the game loop is broken because max level is reached (won)
  GoS_O_rt = 2;											//set system output to 3, commence winning sequence
  *IO_7SEGEN = 0xF8;									//enable SEG2-0
  *IO_7SEG2 = 28;										//Y
  *IO_7SEG1 = 10;										//A
  *IO_7SEG0 = 28;										//Y
  winleds();											//winning LED sequence
  GameOfLEDs();											//play the winning song, GameOfLEDs theme tune
  startgame = 0;										//startgame back to 0 ending loop in int main

}else{													//if the game loop is broken because of lives lost (lost)
   GoS_O_rt = 3;										//set system outpt to 3, commence losing sequence
   *IO_7SEG7 = 16;										//G
   *IO_7SEG6 = 10;										//A
   *IO_7SEG5 = 30;										//M
   *IO_7SEG4 = 14;										//E
   *IO_7SEG3 = 0;										//O
   *IO_7SEG2 = 27;										//V
   *IO_7SEG1 = 14;										//E
   *IO_7SEG0 = 23;										//R
  sadsong();											//play sad song
  loseleds();											//losing LED sequence
  startgame = 0;										//break int main loop
}
}

//--------------------------------------------
// Creation of stscreen() code creating home
// screen animations
//--------------------------------------------

void stscreen(){

*IO_7SEGEN = 0x00;             	// enable 7-segment display
*IO_LEDG = 0b000000;			//set LEDG to 0 and all SEG to triple dash
*IO_7SEG7 = 29; 
*IO_7SEG6 = 29; 
*IO_7SEG5 = 29; 
*IO_7SEG4 = 29; 
*IO_7SEG3 = 29; 
*IO_7SEG2 = 29; 
*IO_7SEG1 = 29; 
*IO_7SEG0 = 29; 


//creation of SEG array to toggle Seg enable 
int SEG_ARRAY[17] = {0b11111110, 0b11111100, 0b11111000, 0b11110000, 0b11100000, 0b11000000, 0b10000000, 0b00000000,
					0b00000000, 0b10000000, 0b11000000, 0b11100000, 0b11110000, 0b11111000, 0b11111100, 0b11111110, 0b11111111};
int i = 1;
int x = 1;
int y = 1;


  while(*IO_PB == 0){			//while loop to loop until pushbutton pushed altering graphics as it goes
								//toggles through SEG_ARRAY per loop
    *IO_7SEGEN = 0x00;
    *IO_7SEGEN = SEG_ARRAY[i];
    *IO_LEDR = x;
    delay(75);
    x = x *2;
	*IO_LEDG=y;
    y++;
    i++;
    if(i == 17){				
      i = 1;
    }
    if(x == 65536){
      x = 1;
    }
  }
  if(*IO_PB != 0){				//When PB pressed commence game start animations and sounds
      startgame = 1;			//set startgame to 1 to enable game loop in int main
      notone();
      *IO_LEDR = 0;
      *IO_7SEGEN = 0x00;
      *IO_7SEG7 = 29; 
      *IO_7SEG6 = 29; 
      *IO_7SEG5 = 29; 
      *IO_7SEG4 = 29; 
      *IO_7SEG3 = 16;
      *IO_7SEG2 = 10;
      *IO_7SEG1 = 30;
      *IO_7SEG0 = 14;
      *IO_LEDG = 0b100100;
      tone(NOTE_E4);
      delay(1000);
      notone();
      *IO_7SEGEN = 0x00;
      *IO_7SEG7 = 31; 
      *IO_7SEG6 = 31; 
      *IO_7SEG5 = 31; 
      *IO_7SEG4 = 31; 
      *IO_7SEG3 = 31;
      *IO_7SEG2 = 31;
      *IO_7SEG1 = 0;
      *IO_7SEG0 = 15;
      *IO_LEDR = 0b1111111111111111;
      *IO_LEDG = 0b110110;
      tone(NOTE_A4);
      delay(1000);
      notone();
      *IO_7SEGEN = 0x00;
      *IO_7SEG7 = 29; 
      *IO_7SEG6 = 29; 
      *IO_7SEG5 = 29; 
      *IO_7SEG4 = 29; 
      *IO_7SEG3 = 20;
      *IO_7SEG2 = 14;
      *IO_7SEG1 = 13;
      *IO_7SEG0 = 24;
      *IO_LEDR = 0;
      *IO_LEDG = 0b010010;
      tone(NOTE_E4);
      delay(1000);
      notone();
      }

}

//--------------------------------------------
// Creation of winfleds() creating LED
// animations for winning the game
//--------------------------------------------

void winleds(){			
  *IO_LEDR = 0b1111111111111111;
  delay(150);
  *IO_LEDG = 0b000000;
  *IO_LEDR = 0b1111111001111111;
  delay(150);
  *IO_LEDG = 0b010010;
  *IO_LEDR = 0b1111110000111111;
  delay(150);
  *IO_LEDG = 0b000000;
  *IO_LEDR = 0b1111100000011111;
  delay(150);
  *IO_LEDG = 0b010010;
  *IO_LEDR = 0b1111000000001111;
  delay(150);
  *IO_LEDG = 0b000000;
  *IO_LEDR = 0b1110000000000111;
  delay(150);
  *IO_LEDG = 0b010010;
  *IO_LEDR = 0b1100000000000011;
  delay(150);
  *IO_LEDG = 0b000000;
  *IO_LEDR = 0b1000000000000001;
  delay(150);
  *IO_LEDG = 0b010010;
  *IO_LEDR = 0b1000000000000001;
  delay(150);
  *IO_LEDG = 0b000000;
  *IO_LEDR = 0b1100000000000011;
  delay(150);
  *IO_LEDG = 0b010010;
  *IO_LEDR = 0b1110000000000111;
  delay(150);
  *IO_LEDG = 0b000000;
  *IO_LEDR = 0b1111000000001111;
  delay(150);
  *IO_LEDG = 0b010010;
  *IO_LEDR = 0b1111100000011111;
  delay(150);
  *IO_LEDG = 0b000000;
  *IO_LEDR = 0b1111110000111111;
  delay(150);
  *IO_LEDG = 0b010010;
  *IO_LEDR = 0b1111111001111111;
  delay(150);
  *IO_LEDG = 0b000000;
  *IO_LEDR = 0b1111111111111111;
  delay(150);
  *IO_LEDG = 0b010010;
}

//--------------------------------------------
// Creation of loseleds() creating LED
// animations for losing the game
//--------------------------------------------

void loseleds(){
  *IO_LEDR = 0b0000000000000000;
  delay(150);
  *IO_LEDG = 0b000000;
  *IO_LEDR = 0b1000000000000001;
  delay(150);
  *IO_LEDG = 0b100100;
  *IO_LEDR = 0b1100000000000011;
  delay(150);
  *IO_LEDG = 0b000000;
  *IO_LEDR = 0b1110000000000111;
  delay(150);
  *IO_LEDG = 0b100100;
  *IO_LEDR = 0b1111000000001111;
  delay(150);
  *IO_LEDG = 0b000000;
  *IO_LEDR = 0b1111100000011111;
  delay(150);
  *IO_LEDG = 0b100100;
  *IO_LEDR = 0b1111110000111111;
  delay(150);
  *IO_LEDG = 0b000000;
  *IO_LEDR = 0b1111111001111111;
  delay(150);
  *IO_LEDG = 0b100100;
  *IO_LEDR = 0b1111111111111111;
  delay(150);
  *IO_LEDG = 0b000000;
  *IO_LEDR = 0b1111111001111111;
  delay(150);
  *IO_LEDG = 0b100100;
  *IO_LEDR = 0b1111110000111111;
  delay(150);
  *IO_LEDG = 0b000000;
  *IO_LEDR = 0b1111100000011111;
  delay(150);
  *IO_LEDG = 0b100100;
  *IO_LEDR = 0b1111000000001111;
  delay(150);
  *IO_LEDG = 0b000000;
  *IO_LEDR = 0b1110000000000111;
  delay(150);
  *IO_LEDG = 0b100100;
  *IO_LEDR = 0b1100000000000011;
  delay(150);
  *IO_LEDG = 0b000000;
  *IO_LEDR = 0b1000000000000001;
  delay(150);
  *IO_LEDG = 0b100100;
  *IO_LEDR = 0b0000000000000000;
  delay(150);
  *IO_LEDG = 0b000000;
}

//--------------------------------------------
// Creation of correctsong() creating tune
// for level pass
//--------------------------------------------

void correctsong()
{
  notone();
  tone(NOTE_B5);
  delay(100);
  notone();
  tone(NOTE_E7+NOTE_B5);
  delay(50);
  notone();
  tone(NOTE_E7);
  delay(300);
  notone();

}

//--------------------------------------------
// Creation of wrongsong() creating tune for
// level fail
//--------------------------------------------

void wrongsong()
{
  notone();
  tone(NOTE_A3);
  delay(400);
  notone();
  tone(NOTE_D3);
  delay(400);
  notone();
}

//--------------------------------------------
// Creation of startupsong() creating tune for
// startscreen recommence
//--------------------------------------------

void startupsong(){	

  notone();
  tone(NOTE_A4);
  delay(100);
  notone();
  tone(NOTE_B4);
  delay(100);
  notone();
  tone(NOTE_C4);
  delay(100);
  notone();
  tone(NOTE_D4);
  delay(100);
  notone();
  tone(NOTE_E4);
  delay(100);
  notone();
  tone(NOTE_F4);
  delay(100);
  notone();
  tone(NOTE_G4);
  delay(100);
  notone();
  tone(NOTE_F4);
  delay(100);
  notone();
  tone(NOTE_E4);
  delay(100);
  notone();
  tone(NOTE_D4);
  delay(100);
  notone();
  tone(NOTE_C4);
  delay(100);
  notone();
  tone(NOTE_B4);
  delay(100);
  notone();
  tone(NOTE_A4);
  delay(100);
  notone();
  
}

//--------------------------------------------
// Creation of sadsong() creating tune for
// game loss
//--------------------------------------------

void sadsong(){

  notone();
  tonewithseg(NOTE_F4);
  delay(100);
  notone();
  tonewithseg(NOTE_C4);
  delay(100);
  notone();
  tonewithseg(NOTE_G3);
  delay(100);
  notone();
  tonewithseg(NOTE_F2);
  delay(100);
  notone();
  tonewithseg(NOTE_C2);
  delay(100);
  notone();
  tonewithseg(NOTE_A1);
  delay(100);
  notone();
  
}

//--------------------------------------------
// Creation of GameOfLEDs() creating tune for
// final game win
//--------------------------------------------

void GameOfLEDs()		//creating the song for game won
  {

    int i,j,k,l,q;

    for(i=0; i<4; i++)	//for loops used to loop through reccurring sections of song
    {
    tonewithseg(NOTE_G4);
    delay(500);
    notone();
    tonewithseg(NOTE_C4);
    delay(500);
    notone();
    tonewithseg(NOTE_DS4);
    delay(250);
    notone();
    tonewithseg(NOTE_F4);
    delay(250);
    notone();
    }
    for(j=0; j<4; j++)
    {
    tonewithseg(NOTE_G4);
    delay(500);
    notone();
    tonewithseg(NOTE_C4);
    delay(500);
    notone();
    tonewithseg(NOTE_E4);
    delay(250);
    notone();
    tonewithseg(NOTE_F4);
    delay(250);
    notone();
    }
        tonewithseg(NOTE_G4);
        delay(500);
        notone();
        tonewithseg(NOTE_C4);
        delay(500);
        tonewithseg(NOTE_DS4);
        delay(250);
        notone();
        tonewithseg(NOTE_F4);
        delay(250);
        notone();
        tonewithseg(NOTE_D4);
        delay(500);
        notone();
    for(k=0; k<3; k++)
    {
    tonewithseg(NOTE_G3);
    delay(500);
    notone();
    tonewithseg(NOTE_AS3);
    delay(250);
    notone();
    tonewithseg(NOTE_C4);
    delay(250);
    notone();
    tonewithseg(NOTE_D4);
    delay(500);
    notone();
    }//
        tonewithseg(NOTE_G3);
        delay(500);
        notone();
        tonewithseg(NOTE_AS3);
        delay(250);
        notone();
        tonewithseg(NOTE_C4);
        delay(250);
        notone();
        tonewithseg(NOTE_D4);
        delay(1000);
        notone();
        
        tonewithseg(NOTE_F4);
        delay(1000);
        notone();
        tonewithseg(NOTE_AS3);
        delay(1000);
        notone();
        tonewithseg(NOTE_DS4);
        delay(250);
        notone();
        tonewithseg(NOTE_D4);
        delay(250);
        notone();
        tonewithseg(NOTE_F4);
        delay(1000);
        notone();
        tonewithseg(NOTE_AS3);
        delay(1000);
        notone();
        tonewithseg(NOTE_DS4);
        delay(250);
        notone();
        tonewithseg(NOTE_D4);
        delay(250);
        notone();
        tonewithseg(NOTE_C4);
        delay(500);
        notone();
    for(l=0; l<3; l++)
    {
    tonewithseg(NOTE_GS3);
    delay(250);
    notone();
    tonewithseg(NOTE_AS3);
    delay(250);
    notone();
    tonewithseg(NOTE_C4);
    delay(500);
    notone();
    tonewithseg(NOTE_F3);
    delay(500);
    notone();
    }
          tonewithseg(NOTE_G4);
          delay(1000);
          notone();
          tonewithseg(NOTE_C4);
          delay(1000);
          notone();
          tonewithseg(NOTE_DS4);
          delay(250);
          notone();
          tonewithseg(NOTE_F4);
          delay(250);
          notone();
          tonewithseg( NOTE_G4);
          delay(1000);
          notone();
          tonewithseg( NOTE_C4);
          delay(1000);
          notone();
          tonewithseg(NOTE_DS4);
          delay(250);
          notone();
          tonewithseg(NOTE_F4);
          delay(250);
          notone();
          tonewithseg(NOTE_D4);
          delay(500);
          notone();
    for(q=0; q<4; q++)
    {
    tonewithseg(NOTE_G3);
    delay(500);
    notone();
    tonewithseg(NOTE_AS3);
    delay(250);
    notone();
    tonewithseg(NOTE_C4);
    delay(250);
    notone();
    tonewithseg(NOTE_D4);
    delay(500);
    notone();
    }
}
