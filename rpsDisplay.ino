#include <EasyTransfer.h>


// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int digitalPin1 = 7;
int digitalPin2 = 8;
int digitalPin3 = 9;
byte aiPlay;
byte result;
#define ROCK 1
#define PAPER 2
#define SCISSORS 3
#define TIE 4
#define AI_WINS 5
#define AI_LOSES 6
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
byte lastShake;

void setup() {
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  Serial.begin(9600);
  ET.begin(details(gameInfo), &Serial);
  lastShake=-1;

}


void loop() {




  while(!ET.receiveData()){
  }
  if(lastShake!=gameInfo.shake||gameInfo.isNew)
    update();






}

void update(){
  lastShake=gameInfo.shake;
  if(gameInfo.isNew){

    lcd.setCursor(10,1);
    lcd.print("SHOOT!");
    delay(250);
    displayResults();
  }
  else if(gameInfo.shake==0){
    lcd.begin(16,2);
    lcd.setCursor(0,0);
    lcd.print("c=Rock, z=Paper,");
    lcd.setCursor(0,1);
    lcd.print("c+z=Scissors.");

  }
  else if(gameInfo.shake == 1){
    lcd.begin(16,2);
    // Prints ROCK
    lcd.setCursor(0,0);
    lcd.print("ROCK");
  }
  else if(gameInfo.shake == 2){
    lcd.begin(16,2);
    // Prints ROCK
    lcd.setCursor(0,0);
    lcd.print("ROCK");

    // Prints PAPER
    lcd.setCursor(11,0);
    lcd.print("PAPER");
  }
  else if(gameInfo.shake== 3){
    lcd.begin(16,2);
    // Prints ROCK
    lcd.setCursor(0,0);
    lcd.print("ROCK");

    // Prints PAPER
    lcd.setCursor(11,0);
    lcd.print("PAPER");

    lcd.setCursor(0,1);
    lcd.print("SCISSORS");
  }
}

void displayScore(){
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Humans: ");
  lcd.setCursor(8,0);
  lcd.print(gameInfo.humanWins);
  lcd.setCursor(0,1);
  lcd.print("AI: ");
  lcd.print(gameInfo.aiWins);
  lcd.print(" ");
  lcd.print("Ties: ");
  lcd.print(gameInfo.ties);
  delay(2000);
}

void displayResults(){
  if(gameInfo.humanPlay==0){
    displayScore();
    delay(3000);
    return;
  }
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("You played: ");
  lcd.setCursor(0,1);
  delay(600);
  lcd.print(makeString(gameInfo.humanPlay));
  delay(700);
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("I played: ");
  lcd.setCursor(0,1);
  delay(600);
  lcd.print(makeString(gameInfo.aiPlay));
  delay(700);

  lcd.begin(16,2);
  lcd.setCursor(3,0);
  lcd.print(makeString(gameInfo.result));
  delay(1500);
  displayScore();


  /*if(gameInfo.result == (byte)AI_WINS)
  {
    lcd.setCursor(5,1);
    lcd.print("YOU LOSE");
    delay(2000);
  }

  else if(gameInfo.result == (byte)AI_LOSES)
  {
    lcd.setCursor(5,1);
    lcd.print("YOU WIN!");
    delay(2000);
  }

  else if (gameInfo.result == (byte)TIE)
  {
    lcd.setCursor(5,1);
    lcd.print("TIE");
    delay(2000);
  }
  */
  
}

// Turn off the display:
//lcd.noDisplay();
//delay(500);
// Turn on the display:
//lcd.display();
//delay(500);


String makeString(byte b){
  if(b==ROCK)
    return "Rock";
  if (b ==PAPER)
    return "Paper";
  if(b==SCISSORS)
    return "Scissors";
   if(b == TIE)
     return "We tied.";
   if(b == AI_WINS)
     return "You Lost!";
   if(b == AI_LOSES)
     return "You Won!";
}





