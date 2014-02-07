#include <EasyTransfer.h>

#include <EEPROM.h>

#define ROCK 1
#define PAPER 2
#define SCISSORS 3

#define TIE 4
#define AI_WINS 5
#define AI_LOSES 6

#define FALSE 0
#define TRUE 1

#define TEST_MODE
#include <Wire.h>
#include "nunchuck_funcs.h"

#define SHK1 3
#define SHK2 4
#define SHK3 5

#define CPIN 7
#define ZPIN 6
#define NO_MEM
const int RED_PIN = 9;
const int GREEN_PIN = 10;
const int BLUE_PIN = 11;

const int outSignalPin0 = 2;
const int outSignalPin1 = 13;

const int buttonPin =12;
int loop_cnt=0;

byte accx,accy,zbut,cbut;
byte nextPlay;


int shakeCount;
int tSinceShook=0;
int accData[5];
byte pastForms[200];
byte starterSeq[] = {
  3, 2, 1, 2, 3, 2, 1, 1, 1, 2, 1, 2, 2, 3, 3, 2, 1, 1, 3, 2, 1, 3, 2, 1, 3, 3, 3, 3, 3, 3, 1, 1, 3, 2, 1, 3, 2, 2, 1, 1, 3, 3, 1, 2, 1, 2, 1, 3, 3, 3, 1, 3};
short totalPlays;
short playsAtStart;
EasyTransfer ET; 
typedef struct TRANSMIT_STRUCT{
  boolean isNew;
  byte shake;
  byte aiPlay;
  byte humanPlay;
  byte result;
  int totalGames;
  int humanWins;
  int aiWins;
  int ties;
}
transmission;

transmission gameInfo;

void setup()
{
  gameInfo.totalGames=0;
  gameInfo.humanWins=0;
  gameInfo.aiWins=0;
  gameInfo.ties=0;
  gameInfo.isNew=FALSE;   
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  Serial.begin(9600);
  nunchuck_setpowerpins();
  nunchuck_init(); // send the initilization handshake

  //Serial.print("WiiChuckDemo ready\n");
  pinMode(SHK1, OUTPUT);
  pinMode(SHK2, OUTPUT);
  pinMode(SHK3, OUTPUT);
  pinMode(CPIN, OUTPUT);
  pinMode(ZPIN, OUTPUT);
  for(int i = 0; i<5; i++){
    accData[i]=83;
  }
  shakeCount =0;

  pinMode(buttonPin, INPUT);
  if(digitalRead(buttonPin)==HIGH){
    totalPlays=50;
    playsAtStart=50;
    gameInfo.humanWins=12;
    gameInfo.aiWins=21;
    gameInfo.ties=17;
    for(int i=2; i<52; i++){
      pastForms[i]=starterSeq[i];
    }
    for(int i=52;i<200;i++){
      pastForms[i]=0;
    }
  }
  else {
    totalPlays=0;
    playsAtStart=0;
     for(int i=0;i<200;i++){
      pastForms[i]=0;
    }
  }

  ET.begin(details(gameInfo), &Serial);
  Serial.print(totalPlays);
  Serial.println(" games in memory.");


  nextPlay =selectPlay();
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(RED_PIN, LOW);
}

void loop()
{

  if( loop_cnt > 100 ) { // every 100 msecs get new data
    loop_cnt = 0;

    nunchuck_get_data();

    accx  = nunchuck_accelx(); // ranges from approx 70 - 182
    accy  = nunchuck_accely(); // ranges from approx 65 - 173
    zbut = nunchuck_zbutton();
    cbut = nunchuck_cbutton(); 

    for(int i=4; i>=0; i--){
      accData[i+1]= accData[i];
    }
    accData[0]=accy;
    /*Serial.print("accx: "); 
     Serial.print((byte)accx,DEC);
     Serial.print("\taccy: "); 
     Serial.print((byte)accy,DEC);
     Serial.print("\tzbut: "); 
     Serial.print((byte)zbut,DEC);
     Serial.print("\tcbut: "); 
     Serial.println((byte)cbut,DEC);
     Serial.println(shakeCount);
     */
    if((accData[0]-accData[4])>20&tSinceShook>1){
      shook(cbut,zbut);
      gameInfo.shake=shakeCount;
      tSinceShook =0;
      ET.sendData();
    }


    else{
      tSinceShook++;
    }
    if(tSinceShook>15){
      shakeCount=0;
      tSinceShook=0; 
      gameInfo.shake=0;
      ET.sendData();
    }
    digitalWrite(CPIN, cbut);
    digitalWrite(ZPIN, zbut);
    if(shakeCount==0){
      digitalWrite(SHK1, LOW);
      digitalWrite(SHK2, LOW);
      digitalWrite(SHK3, LOW);
    }
    else if(shakeCount==1){
      digitalWrite(SHK1, HIGH);
      digitalWrite(SHK2, LOW);
      digitalWrite(SHK3, LOW);
    }
    else if(shakeCount==2){
      digitalWrite(SHK1, HIGH);
      digitalWrite(SHK2, HIGH);
      digitalWrite(SHK3, LOW);
    }
    else if(shakeCount==3) {
      digitalWrite(SHK1, HIGH);
      digitalWrite(SHK2, HIGH);
      digitalWrite(SHK3, HIGH);
    }


  }
  loop_cnt++;
  delay(1);
}



void shook(int c, int b){  
  if(shakeCount>=3){
    shakeCount =0;

    digitalWrite(SHK1, LOW);
    digitalWrite(SHK2, LOW);
    digitalWrite(SHK3, LOW);

    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    humanPlayed(c,b);

    nextPlay = selectPlay();
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
  }
  else
    shakeCount++;

}

void humanPlayed(int cButton, int zButton){

  int input;
  byte humanPlay;
  Serial.print("Game no. ");
  Serial.println(totalPlays);
  Serial.print("cButton: ");
  Serial.print(cButton);
  Serial.print("\tzButton: ");
  Serial.println(zButton);


  if(cButton &&zButton)
    humanPlay=(byte)SCISSORS;
  else if(cButton)
    humanPlay= (byte)ROCK;
  else if(zButton)
    humanPlay= (byte)PAPER;

  gameInfo.humanPlay= humanPlay;
  if(humanPlay==0){

    gameInfo.totalGames = totalPlays;
    gameInfo.isNew=TRUE;
    ET.sendData();
    gameInfo.isNew=FALSE;
    return;
  }

  pastForms[totalPlays+2] = humanPlay;
  /*if(humanPlay==ROCK)
   Serial.println("you played ROCK.");
   if(humanPlay==PAPER)
   Serial.println("you played PAPER.");
   if(humanPlay==SCISSORS)
   Serial.println("you played SCISSORS.");
   */
  if(nextPlay ==ROCK){

    Serial.println("I Played ROCK");
    gameInfo.aiPlay = ROCK;


  }
  if(nextPlay ==PAPER){

    Serial.println("I Played PAPER");
    gameInfo.aiPlay = PAPER;

  }
  if(nextPlay ==SCISSORS){

    Serial.println("I Played SCISSORS");
    gameInfo.aiPlay = SCISSORS; 

  }



  if(nextPlay==humanPlay){

    Serial.println("We tied.");
    gameInfo.result = TIE;
    gameInfo.ties++;
  }
  else if((nextPlay ==ROCK && humanPlay== SCISSORS)||(nextPlay ==PAPER && humanPlay== ROCK)||
    (nextPlay ==SCISSORS && humanPlay== PAPER)){

    Serial.println("I WON.");
    gameInfo.result = AI_WINS;
    gameInfo.aiWins++;


  }
  else{ 

    Serial.println("I lost."); 
    gameInfo.result = AI_LOSES;
    gameInfo.humanWins++;

  }

#ifndef NO_MEM
  if(digitalRead(buttonPin)==LOW){
    saveSession();
  }
#endif
  totalPlays++;
  gameInfo.totalGames = totalPlays;
  gameInfo.isNew=TRUE;
  ET.sendData();
  gameInfo.isNew=FALSE;

}

byte selectPlay(){
  Serial.println("Selecting next play based on this sequence:");
  for(int i =2; i<totalPlays+2; i++){
    Serial.print(pastForms[i]);
    Serial.print(" ");
  }
  Serial.println("");
  if(totalPlays <4)
    return (byte)random(1, 3);
  if(totalPlays < 8)
    return markov(3);
  return markov(4);

}

byte markov(int seqLen){
  Serial.print("Searching for sequences of length ");
  Serial.println(seqLen);
  int matchedSoFar=0;
  byte winner;
  byte tally[4];
  int i=0;
  int matchStart= -1;
  int offset=seqLen;
  for(int i=0; i<4; i++)
    tally[i]=0;
  if(seqLen<1){
    Serial.println("No matches. Playing randomly.");
    return (byte)random(1, 3);
  }
  while(i+seqLen<totalPlays+2) {
    if(pastForms[i]!=pastForms[totalPlays+2-offset]){
      matchedSoFar=0;
      offset= seqLen;
      matchStart= -1;
      i++;

      Serial.println("no good");
    }
    else if(matchedSoFar ==0){
      matchedSoFar++;
      offset--;
      matchStart =i;
      i++;
      if(seqLen ==1){
        tally[pastForms[i]]++;
        matchedSoFar =0;
        i = matchStart+1;
        offset=seqLen;
        matchStart = -1;
      }
      Serial.println("found the first symbol");
    }
    else if(matchedSoFar >=seqLen-1){
      tally[pastForms[i+1]]++;
      matchedSoFar =0;
      i = matchStart+1;
      offset=seqLen;
      matchStart = -1;
      Serial.println("found the last symbol");
    }
    else{
      matchedSoFar++;
      i++;
      offset--;
      Serial.println("found the next symbol");
    }
  }
  for(int i = 1; i<4; i++){
    Serial.println(tally[i]);
  }
  if(winner = determineBestPlay(tally[ROCK], tally[PAPER], tally[SCISSORS]))
    return winner;
  return markov(seqLen-1);

}

int determineBestPlay(int rocks, int papers, int scissors){
  int least;
  if(rocks==papers && papers==scissors)
    return 0;
  if( rocks> papers && rocks>scissors)
    return PAPER;
  if(papers>rocks && papers > scissors)
    return SCISSORS;
  if(scissors> rocks && scissors > papers)
    return ROCK;
  if(rocks == papers)
    return (byte)random(PAPER, SCISSORS);
  if(rocks == scissors)
    return (byte)random(ROCK, PAPER);
  if(papers == scissors){
    if(random(0,1))
      return ROCK;
    return SCISSORS;
  }

}

short getHistory(){
  short count;
  Serial.println("Reading play history from EEPROM");
  count = EEPROM.read(1);
  count << 8;
  count += EEPROM.read(0);
  for(int i=2; i<count+2; i++){
    pastForms[i-2] = EEPROM.read(i);
  }
  Serial.print("Read ");
  Serial.print(count);
  Serial.println(" bytes from EEPROM.");
  return count;
}

void saveSession(){
  EEPROM.write(0, highByte(totalPlays));
  EEPROM.write(1, lowByte(totalPlays));
  for(int i= playsAtStart; i<totalPlays; i++){
    EEPROM.write(i, pastForms[i]);
  }
}








