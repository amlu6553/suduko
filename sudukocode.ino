#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>


#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8  // can be a digital pin

// Revised Touch Callibration numbers
#define TS_MINX 130
#define TS_MINY 88
#define TS_MAXX 915
#define TS_MAXY 880

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4

// Assign human-readable names to some common 16-bit color values:
int  BLACK =  0x0000;
int BLUE  =  0x001F;
int MAGENTA = 0xF800;
int GREEN  = 0x07E0;
int CYAN = 0x07FF;
int RED = 0xF81F;
int YELLOW = 0xFFE0;
int WHITE = 0xFFFF;


Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define BOX 118  // Block to clear screen
#define BOXSIZE 120
#define BUTTON 20

boolean debounce = false;

// Array with following Structure: {value , X , Y , Solved }

const byte colcoord[4] = { 20, 80, 140, 218 };
const byte rowcoord[4] = { 20, 80, 140, 218 };

byte location = 1;

byte puzzlenum = 1; // These identifier for 5 puzzles stored in memory

byte sudoku[17][14];

byte touchlocation = 0; // Used to track the array value that the stylis is closest to in the Sudoku 4x4 framework

int delay1 = 400;  // Pause input into screen

byte tempreading = 0;


void setup(void) {

  tft.reset();

  uint16_t identifier = tft.readID();
  identifier = 0x9341;

  tft.begin(identifier);
  tft.setRotation(1);
  tft.fillScreen(BLACK);

  // **** Intro Screen


  tft.drawRoundRect(0, 0, 320, 240, 20, BLUE);
  tft.drawRoundRect(1, 1, 318, 238, 20, BLUE);

  byte g = 70;

  tft.drawCircle(46, g, 25, GREEN);
  tft.fillCircle(46, g, 20, GREEN);
  tft.setTextColor(BLACK);  tft.setTextSize(3);
  tft.setCursor(39, g - 10);   tft.println("S");

  tft.drawCircle(91, g + 30, 25, BLUE);
  tft.fillCircle(91, g + 30, 20, BLUE);
  tft.setTextColor(BLACK);  tft.setTextSize(3);
  tft.setCursor(84, g + 20);   tft.println("U");

  tft.drawCircle(137, g, 25, YELLOW);
  tft.fillCircle(137, g, 20, YELLOW);
  tft.setTextColor(BLACK);  tft.setTextSize(3);
  tft.setCursor(130, g - 10);   tft.println("D");

  tft.drawCircle(183, g + 30, 25, RED);
  tft.fillCircle(183, g + 30, 20, RED);
  tft.setTextColor(BLACK);  tft.setTextSize(3);
  tft.setCursor(176, g + 20);   tft.println("O");

  tft.drawCircle(229, g, 25, GREEN);
  tft.fillCircle(229, g, 20, GREEN);
  tft.setTextColor(BLACK);  tft.setTextSize(3);
  tft.setCursor(222, g - 10);   tft.println("K");

  tft.drawCircle(274, g + 30, 25, YELLOW);
  tft.fillCircle(274, g + 30, 20, YELLOW);
  tft.setTextColor(BLACK);  tft.setTextSize(3);
  tft.setCursor(267, g + 20);   tft.println("U");

  tft.setTextColor(WHITE);  tft.setTextSize(2);
  tft.setCursor(25, 170);   tft.println("Wake up the right way!");

  tft.setTextColor(GREEN);  tft.setTextSize(1);
  tft.setCursor(25, 200);   tft.println("Boulder Bro's");


  delay(4000);

  tft.fillScreen(BLACK);

  drawscreen(); // Clearscreen and setup Sudoku matrix
  resetmatrix(); // Initialize the sudoku matrix by setting all locations to zero and loading in the coordinates for each touch location
  loadtestpuzzle1(); // Load specifc Panel identification Data into the Array for each of the 81 locations

  // Test Display by showing all values in the puzzle - White are Solved , Blue are others

  //cursor for each box


  drawbuttons();

  pinMode(13, OUTPUT);

  //  testText();


  tft.setTextSize(2);

  for (byte a = 1; a < 17; a++) {

    //Test solve or set condition
    if  (sudoku[a][1] != 0) {
      if (sudoku[a][0] != 0) {
        tft.setTextColor(WHITE);
      } else {
        tft.setTextColor(GREEN); // Set this colour if not a genuine set clue
      }
      tft.setCursor( sudoku[a][3], sudoku[a][2]);
      tft.println(sudoku[a][1]);
    }
  }



}





#define MINPRESSURE 5
#define MAXPRESSURE 1000

void drawscreen() {

  //    tft.fillScreen(BLACK);

  // Setup Screen

  GREEN  = 0x07E0;


  tft.fillRect(1, 1, 239, 239, BLACK);

  tft.drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);
  tft.drawRect(0 + BOXSIZE, 0, BOXSIZE, BOXSIZE, WHITE);
  tft.drawRect(0, BOXSIZE, BOXSIZE, BOXSIZE, WHITE);
  tft.drawRect(0 + BOXSIZE, BOXSIZE, BOXSIZE, BOXSIZE, WHITE);

  // const byte colcoord[4] = { 20, 80, 140, 218 };


  tft.drawRect(0, 60, BOXSIZE, BOXSIZE, WHITE);
  tft.drawRect(0 + BOXSIZE, 60, BOXSIZE, BOXSIZE, WHITE);
  tft.drawLine(60, 0, 60, 239, WHITE);
  tft.drawLine(180, 0, 180, 239, WHITE);



}





void loop () {



  // Solve button pressed

  // if ( ((p.y > 255)&&(p.y < 300))&& ((p.x > 140)&&(p.x < 160)))


  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  Serial.print(" Px="); Serial.print(p.x);
  Serial.print(" Py="); Serial.print(p.y);
  Serial.print(" Pz="); Serial.println(p.z);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {

    // scale from 0->1023 to tft.width
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320); // Original Code

    if ((p.x > 0 ) && (p.x < 60)) { // col 1

      if ((p.y > 0) && (p.y < 60)) {           // Row 1
        touchlocation = 1;
      }
      if ((p.y > 60) && (p.y > 120)) {  // col 1 row 2

        touchlocation = 2;
      }
      if ((p.y > 120) && (p.y > 180)) { // col 1 row 3

        touchlocation = 3;
      }
      if ((p.y > 180) && (p.y > 239)) { // col 1 row 4

        touchlocation = 4;
      }

    } else if ((p.x > 60) && (p.x < 120)) { // col 2

      if ((p.y > 0) && (p.y < 60)) {           // col 2 Row 1
        touchlocation = 5;
      }
      if ((p.y > 60) && (p.y > 120)) {  // col 2 row 2

        touchlocation = 6;
      }
      if ((p.y > 120) && (p.y > 180)) { // col 2 row 3

        touchlocation = 7;
      }
      if ((p.y > 180) && (p.y > 239)) { // col 2 row 4

        touchlocation = 8;
      }

    } else if ((p.x > 120) && (p.x < 180)) { // col 3

      if ((p.y > 0) && (p.y < 60)) {           // col 3 Row 1
        touchlocation = 9;
      }
      if ((p.y > 60) && (p.y > 120)) {  // col 3 row 2

        touchlocation = 10;
      }
      if ((p.y > 120) && (p.y > 180)) { // col 3 row 3

        touchlocation = 11;
      }
      if ((p.y > 180) && (p.y > 239)) { // col 3 row 4

        touchlocation = 12;
      }


      // Calculate the position of the screen touch based on the input values of p.x and p.y

    } else if ((p.x > 180) && (p.x < 239)) { // col 4

      if ((p.y > 0) && (p.y < 60)) {  // col 3 Row 1
        touchlocation = 13;
      }
      if ((p.y > 60) && (p.y > 120)) {  // col 3 row 2

        touchlocation = 14;
      }
      if ((p.y > 120) && (p.y > 180)) { // col 3 row 3

        touchlocation = 15;
      }
      if ((p.y > 180) && (p.y > 239)) { // col 3 row 4

        touchlocation = 16;
      }
    }
  }

  if (debounce == false) {
    touchlocation = 0;
    debounce = true;
  }


  if ( ((p.x < 235) && (p.y < 230)) && (touchlocation != 0)) {

    Serial.print("Y = "); Serial.print(p.x);
    Serial.print("\tX = "); Serial.print(p.y);

    //   tft.fillRect(250, 80, 15, 10, BLACK);

    /*
          Serial.print("Y = "); Serial.print(p.x);
          Serial.print("\tX = "); Serial.print(p.y);
          Serial.print("\tLocation = "); Serial.println(touchlocation);
    */

    // Calculate the incremental changes to the data array
    // Array Structure: {value , X , Y , Solved }

    // Only increment if has not been solved, Debounce by checking if this is the second time the same range is selected
    if ((tempreading == touchlocation) && (sudoku[touchlocation][0] == 0) || (tempreading == touchlocation) && (sudoku[touchlocation][0] == 2)) {
      sudoku[touchlocation][1]++;
      if (sudoku[touchlocation][1] > 9) {
        sudoku[touchlocation][1] = 0;
      }
      // Test to see if changing an item can be classified as solved
      if (sudoku[touchlocation][1] != 0) {
        sudoku[touchlocation][0] = 2;    // Set to Solved if a manually changed number however flag as manual change with va,ue of 2
      } else {
        sudoku[touchlocation][0] = 0;    // Set to Not Solved if 0
      }
      // Finally reset all of the data values in this location that have been manually changed to unsolved
      for (byte u = 1; u < 82; u++) {
        // If preprogrammed from a game then leave values otherwise reset data to baseline
        if (sudoku[u][0] != 1) {
          for (byte q = 4; q < 13; q++) {
            sudoku[u][q] = q - 3;
          }
        }
      }
    }

    //     tft.setTextColor(WHITE);  tft.setTextSize(1);
    //      tft.setCursor(250, 80);   tft.println(touchlocation);



    // Refresh only the location concerned with new value and show colour coded
    //First Clear Location


    tft.fillRect(sudoku[touchlocation][3], sudoku[touchlocation][2], 15, 15, BLACK);
    tft.setTextSize(2);

    if (sudoku[touchlocation][0] != 0) { // Do not draw 0 just leave blank square
      if (sudoku[touchlocation][0] == 1) {
        tft.setTextColor(WHITE);
      } else if (sudoku[touchlocation][0] == 2) {
        tft.setTextColor(GREEN); // Set this colour if not a genuine set clue
      }
      tft.setCursor( sudoku[touchlocation][3], sudoku[touchlocation][2]);
      tft.println(sudoku[touchlocation][1]);
    }

    if (tempreading == touchlocation) {
      delay(delay1);
    }


    tempreading = touchlocation; // take a first sample then repeat and compare to debounce

  }


  if ( ((p.y > 255) && (p.y < 300)) && ((p.x > 80) && (p.x < 110))) {


    //PLAY Button pressed
    //Button 2
    tft.fillCircle(280, 90, 20, WHITE);
    tft.setTextColor(BLACK);  tft.setTextSize(1);
    tft.setCursor(268, 87);   tft.println("PLAY");



    delay(delay1 / 2);

    drawscreen(); // Clearscreen and setup Sudoku matrix
    resetmatrix(); // Initialize the sudoku matrix by setting all locations to zero:

    loadtestpuzzle1(); // Loads test puzzle into the location array



    solvealigndata();  // Sanitize test puzzle data with correct data format


    refreshdisplay(); // Only display testpuzzle data locations that are
    drawbuttons(); // Redraw buttons togive push button effect
    tft.setCursor(277, 97);   tft.println(puzzlenum);

    // Manage Puzzle Number
    puzzlenum++; // increment puzzle number
    if (puzzlenum > 5) {
      puzzlenum = 1;
    }

  }





} // void loop end


void drawbuttons() {
  // Setup Buttons
  GREEN  = 0x07E0;

  //Button 1
  tft.drawCircle(280, 30, 24, GREEN);
  tft.fillCircle(280, 30, 20, GREEN);
  tft.setTextColor(BLACK);  tft.setTextSize(1);
  tft.setCursor(268, 27);   tft.println("HOME");

  //Button 2
  tft.drawCircle(280, 90, 24, YELLOW);
  tft.fillCircle(280, 90, 20, YELLOW);
  tft.setTextColor(BLACK);  tft.setTextSize(1);
  tft.setCursor(268, 87);   tft.println("PLAY");

  //Button 3
  tft.drawCircle(280, 150, 24, MAGENTA);
  tft.fillCircle(280, 150, 20, MAGENTA);
  tft.setTextColor(BLACK);  tft.setTextSize(1);
  tft.setCursor(268, 147);   tft.println("SOLVE");


}

// tft.begin(identifier);




void resetmatrix() { // Initialize the sudoku matrix by setting all locations to zero:
  byte hole = 1;
  for (byte a = 0; a < 4; a++) {
    for (byte b = 0; b < 4; b++) {
      sudoku[hole][0] = 0; // Solve Flag
      sudoku[hole][1] = 0; // Display Value
      sudoku[hole][2] = colcoord[a]; // Matrix Column coordinate
      sudoku[hole][3] = rowcoord[b]; // Matrix Row coordinate
      sudoku[hole][4] = 1; // V1
      sudoku[hole][5] = 2; // V2
      sudoku[hole][6] = 3; // V3
      sudoku[hole][7] = 4; // V4
      sudoku[hole][8] = 5; // V5
      sudoku[hole][9] = 6; // V6
      sudoku[hole][10] = 7; // V7
      sudoku[hole][11] = 8; // V8
      sudoku[hole][12] = 9; // V9
      hole++;
    }
  }
}


void loadpaneldata() {

  // Panel 1
  sudoku[1][13] = 1; sudoku[2][13] = 1;
  sudoku[5][13] = 1; sudoku[6][13] = 1;

  // Panel 2

  sudoku[9][13] = 2; sudoku[10][13] = 2;
  sudoku[13][13] = 2; sudoku[14][13] = 2;

  // Panel 3

  sudoku[3][13] = 3; sudoku[7][13] = 3;
  sudoku[4][13] = 3; sudoku[8][13] = 3;

  // Panel 4

  sudoku[11][13] = 4; sudoku[12][13] = 4;
  sudoku[15][13] = 4; sudoku[16][13] = 4;
}

void loadtestpuzzle1() {
  {
    sudoku[1][1] = 2; sudoku[2][1] = 4; sudoku[3][1] = 3;
    sudoku[4][1] = 1; sudoku[5][1] = 1;
    sudoku[7][1] = 2; sudoku[9][1] = 4;
    sudoku[10][1] = 2;  sudoku[12][1] = 3;
    sudoku[14][1] = 1; sudoku[15][1] = 4;


    sudoku[1][0] = 1; sudoku[2][0] = 1; sudoku[3][0] = 1;
    sudoku[4][0] = 1; sudoku[5][0] = 1; sudoku[6][0] = 1;
    sudoku[7][0] = 1; sudoku[8][0] = 1; sudoku[9][0] = 1;
    sudoku[10][0] = 1; sudoku[11][0] = 1; sudoku[12][0] = 1;
    sudoku[13][0] = 1; sudoku[14][0] = 1; sudoku[15][0] = 1;
    sudoku[16][0] = 1;


  }




}



void solvealigndata() { // Once a location is marked as solved then all data in that location nees to be set to Zero

  for (byte a = 1; a < 17; a++) { // Cycle through all locations

    if (sudoku[a][0] > 0) {

      sudoku[a][4] = 0; // V1
      sudoku[a][5] = 0; // V2
      sudoku[a][6] = 0; // V3
      sudoku[a][7] = 0; // V4
      sudoku[a][8] = 0; // V5
      sudoku[a][9] = 0; // V6
      sudoku[a][10] = 0; // V7
      sudoku[a][11] = 0; // V8
      sudoku[a][12] = 0; // V9

      sudoku[a][(sudoku[a][1] + 3)] = sudoku[a][1];
    }
  }
}


void refreshdisplay() { //Refresh the display once a value has changed

  tft.setTextSize(2);

  for (byte a = 1; a < 17; a++) {

    //Test solve or set condition
    if  (sudoku[a][1] != 0) {

      //First Clear Location
      tft.fillRect(sudoku[a][3], sudoku[a][2], 15, 15, BLACK);

      if (sudoku[a][0] != 0) { // Do not draw 0 just leave blank square
        if (sudoku[a][0] == 1) {
          tft.setTextColor(WHITE);
        } else if (sudoku[a][0] == 2) {
          tft.setTextColor(GREEN); // Set this colour if not a genuine set clue
        }
        tft.setCursor( sudoku[a][3], sudoku[a][2]);
        tft.println(sudoku[a][1]);
      }

    }
  }
}
