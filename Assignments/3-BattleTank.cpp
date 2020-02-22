#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 600

#define FPS 60
#define D2R 0.0174532

#define LOGO 0
#define MENU 1
#define LOAD 2
#define RUN 3
#define CREDITS 4
#define OVER 5
int State = LOGO; // initial Scene 

int winWidth, winHeight, we, he, mX, mY;
int resume, logoTime, dotCounter, notifCounter;
int plaSco, pcSco, countAll, countCor, countInc, diffLevel = 1;
int op1, op2, randPoint;
double gameTime, initTime, loadingTime;
char diff_Range[10] = "", difficulty[10] = ""; // EASY-NORMAL-HARD (bottomLeft on RunScene)
bool givenAnswer, gameState, resetStatus, checkTimer, correctAnswer, init;

typedef struct {
	double r, g, b;
}col_t;

typedef struct {
	col_t col[5];
	bool status[5];
}button_t;
button_t buttonS;

typedef struct {
	float x, y;
}point_t;
typedef struct {
	point_t pos;
	col_t col;
	int visib, centerX, centerY, angle;
	bool escaped = false;
}plaTank_t;			 plaTank_t plaTank;
typedef struct {
	point_t pos;
	int angle, visib;
	bool hit, align, stop;
} bullet_t;
typedef struct {
	point_t pos;
	bullet_t BULLET;
	int   angle;
} pcMainTank_t;		pcMainTank_t BARREL;
typedef struct {
	int col[3], value, ans, clicked;
}answers_t;			answers_t ANSWERS[3]; // 3 OPTIONS

	void questionsCalculation(); //prototype

void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void vprint(int x, int y, void *font, const char *string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

void vprint2(int x, int y, float size, const char *string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

void Init_Globals() {

	gameTime = 60.00;
	initTime = gameTime - 0.02;
	resume = countAll = 1;
	gameState = givenAnswer = correctAnswer = resetStatus = checkTimer = plaTank.escaped = BARREL.BULLET.hit = BARREL.BULLET.align = BARREL.BULLET.stop = false;
	notifCounter = plaSco = pcSco = countCor = countInc = loadingTime = dotCounter = 0;
	BARREL.pos.x = -5; BARREL.pos.y = 25; BARREL.angle = 270;	 // initial Barrel Pos. and Angle

	buttonS = { {{255,255,255},{255,255,255}, {255,255,255}, { 100,100,100 } }, {{false},{false},{false},{false} } };


	for (int i = 0; i < 3; i++) {
		ANSWERS[i].col[0] = 1; 	ANSWERS[i].col[1] = 1; 	ANSWERS[i].col[2] = 0;
		ANSWERS[i].ans = 0;		ANSWERS[i].clicked = 0;
	}

	plaTank.col.r = rand() % 256;		plaTank.col.g = rand() % 256;		plaTank.col.b = rand() % 256;		plaTank.visib = 255;
	do {
		plaTank.pos.x = rand() % 780 - 390;	plaTank.pos.y = rand() % 155 + 10; // the first random pos. of plaTank
	} while ((plaTank.pos.x > -100 && plaTank.pos.x < 100 && plaTank.pos.y < 95 && plaTank.pos.y > -110)); // to prevent spawning behind the the objects on run scene

	randPoint = rand() % 9 + 1;

		questionsCalculation();

	// reset according to current windows positions
	winWidth = we;
	winHeight = he;
	glViewport(0, 0, we, he);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-we / 2, we / 2, -he / 2, he / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void showLogo() {

	glClearColor(192.0 / 255.0, 192.0 / 255.0, 192.0 / 255.0, 1.0f); //gray
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0, 0, 0);
	circle(0, 5, 175);

	glColor3f(1, 1, 1);
	glLineWidth(4);
	vprint2(-100, -55, 1.25, "MA");// LOGO | (M)urat (A)lkan
}

void showMenu() {
	glClearColor(32.0 / 255.0, 32.0 / 255.0, 32.0 / 255.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3ub(50, 150, 250); // nameSurname
	vprint(-(winWidth / 2) + 25, (winHeight / 2) - 20, GLUT_BITMAP_8_BY_13, "Murat Alkan");
	vprint(-(winWidth / 2) + 11.5, (winHeight / 2) - 40, GLUT_BITMAP_8_BY_13, "-Assignment 3-");

	glColor3ub(255, 100, 0);
	glLineWidth(2);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-(winWidth / 2) + 10, (winHeight / 2) - 5);
	glVertex2f(-(winWidth / 2) + 125, (winHeight / 2) - 5);
	glVertex2f(-(winWidth / 2) + 125, (winHeight / 2) - 50);
	glVertex2f(-(winWidth / 2) + 10, (winHeight / 2) - 50);
	glEnd();

	glColor3ub(220, 200, 0);
	glRectf(-(winWidth / 2), 190, (winWidth / 2), -190); // menuRect	

	glColor3ub(0, 0, 100); // menuCircle
	circle(0, 0, 185);

	glColor3f(1, 1, 0);
	glLineWidth(3);
	vprint2(-90, 100, 0.3, "BattleTank");

	glBegin(GL_LINES);
	glVertex2f(-80, 80);
	glVertex2f(85, 80);
	glEnd();

	glColor3f(0, 0, 0);
	glLineWidth(3);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-110, 50);
	glVertex2f(112, 50);
	glVertex2f(112, -115);
	glVertex2f(-110, -115);
	glEnd();

	//BUTTONS

	glColor3ub(buttonS.col[0].r, buttonS.col[0].g, buttonS.col[0].b); glRectf(-60, 40, 60, 0);
	glColor3ub(buttonS.col[1].r, buttonS.col[1].g, buttonS.col[1].b); glRectf(-60, -15, 60, -55);
	glColor3ub(buttonS.col[2].r, buttonS.col[2].g, buttonS.col[2].b); glRectf(-60, -70, 60, -105);
	glColor3f(0, 0, 0); 
	vprint2(-45, 5, 0.25, "START");
	vprint2(-50, -50, 0.25, "CREDIT");
	vprint2(-27, -100, 0.25, "EXIT");

}

void showLoad() {

	glClearColor(15.0 / 255.0, 15.0 / 255.0, 15.0 / 255.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1, 1, 0);
	vprint2(-150, 0, 0.5, "STARTING");
	glColor3f(1, 1, 1);
	for (int i = 0; i < dotCounter; i++)
		vprint2(-35 + i * 30, -50, 0.3, "| ");
}

void checkButtons() {

	if (State == MENU) {
		if (mX > -60 && mX < 60 && mY > 0 && mY < 40) {
			buttonS.col[0] = { 150, 150, 150 };  buttonS.status[0] = true;
		}  // START BUTTON	
		else { buttonS.col[0] = { 255, 255, 255};	 buttonS.status[0] = false; }

		if (mX > -60 && mX < 60 && mY > -55 && mY < -15) {
			buttonS.col[1] = { 150, 150, 150 };  buttonS.status[1] = true;
		}	// CREDITS BUTTON
		else { buttonS.col[1] = { 255, 255, 255 };   buttonS.status[1] = false; }

		if (mX > -60 && mX < 60 && mY > -102 && mY < -70) {
			buttonS.col[2] = { 150, 150, 150};	 buttonS.status[2] = true;
		}	// EXIT BUTTON
		else { buttonS.col[2] = { 255, 255, 255 };	 buttonS.status[2] = false; }
	}

	if (State == OVER || State == CREDITS) { //MENU BUTTON

		if (mX > 320 && mX < 450 && mY > -275 && mY < -230) {
			buttonS.col[3] = { 150, 150, 150 };		buttonS.status[3] = true;
		}
		else { buttonS.col[3] = { 100, 100, 100 };	buttonS.status[3] = false; }

		if (State == OVER) { //RETRY BUTTON
			if (mX > -450 && mX < -320 && mY > -275 && mY < -230) {
				buttonS.col[4] = { 150, 150, 150 };		buttonS.status[4] = true;
			}
			else { buttonS.col[4] = { 100, 100, 100 };	buttonS.status[4] = false; }
		}
	}

}

void menuButton() { // will be on Credits and Over screen.
	glColor3ub(buttonS.col[3].r, buttonS.col[3].g, buttonS.col[3].b);
	glLineWidth(3);
	glRectf(320, -230, 450, -275);
	glColor3f(0, 0, 0);
	vprint2(350, -262, 0.2, "MENU");
}

void showCredits() {

	glClearColor(220.0 / 255.0, 200.0 / 255.0, 0.0 / 255.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0, 0, 0);
	vprint(-50, 100, GLUT_BITMAP_TIMES_ROMAN_24, "CREDITS");

	glColor3f(0, 0, 0);
	glBegin(GL_LINES);
	glVertex2f(-120, 85);
	glVertex2f(130, 85);
	glEnd();

	glColor3f(1, 1, 1);
	vprint(-71, 50, GLUT_BITMAP_HELVETICA_18, "Bilkent University");
	vprint(-40, 20, GLUT_BITMAP_HELVETICA_18, "CTIS 164");
	vprint(-160, -10, GLUT_BITMAP_HELVETICA_18, "Technical Mathematics with Programming");

	glColor3ub(0, 0, 102);
	vprint(-60, -100, GLUT_BITMAP_HELVETICA_18, "- Murat Alkan - ");


	menuButton();
}

void showOver() {

	glClearColor(255.00 / 255.0, 255.0 / 255.0, 0.0 / 255.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0, 0, 0);
	glLineWidth(4);
	vprint2(-250, 115, 0.7, "TIME'S UP!");

	glColor3ub(32, 32, 32);
	glRectf(-(winWidth / 2), 100, (winWidth / 2), -150);

	glLineWidth(3);
	glColor3f(1, 1, 1);
	vprint2(-180, 50, 0.3, "Player  : %d", plaSco);
	vprint2(-180, -5, 0.3, "PC    : %d", pcSco);
	vprint2(-180, -60, 0.3, "Difficulty : %s", difficulty);
	vprint2(-180, -120, 0.3, "Correct/Incorrect : %d/%d (%d) ", countCor, countInc, countAll); // Correct/Incorrect (number of asked questions)

	glColor3ub(buttonS.col[4].r, buttonS.col[4].g, buttonS.col[4].b); 
	glRectf(-320, -230, -450, -275);
	glLineWidth(3);
	glColor3f(0, 0, 0);
	vprint2(-420, -262, 0.2, "RETRY");

	menuButton();
}

void scoreArea() {

	glColor3ub(153, 76, 0); //SCOREBOARD
	glRectf(-(winWidth / 2), (winHeight / 2), (winWidth / 2), (winHeight / 2) - 70);

	glLineWidth(3);
	glColor3f(1, 1, 1);
	vprint2(-(winWidth / 2) + 5, (winHeight / 2) - 45, 0.25, "TIMER : %4.2f", gameTime);
	glColor3f(1, 1, 1);
	vprint2(-(winWidth / 2) + 370, (winHeight / 2) - 45, 0.25, "Player: %d", plaSco);
	glColor3f(1, 1, 1);
	vprint2(-(winWidth / 2) + 675, (winHeight / 2) - 45, 0.25, "PC: %d", pcSco);

	glColor3f(0, 0, 0);
	glLineWidth(5);
	glBegin(GL_LINES);
	glVertex2f(-(winWidth / 2) + 300, (winHeight / 2));
	glVertex2f(-(winWidth / 2) + 300, (winHeight / 2) - 70);
	glEnd();

}

void questionsCalculation() {

	int ansRand1, ansRand2, randAnswer;

	if (resetStatus == true || init == true) {

		randAnswer = rand() % 3 + 1;// one of the boxes holds correct answer
		ANSWERS[0].ans = ANSWERS[1].ans = ANSWERS[2].ans = 0; // reset 

		switch (diffLevel) {
		case 1:	op1 = rand() % 10 + 0;	op2 = rand() % 10 + 0;
			do { ansRand1 = rand() % 82 + 0;	ansRand2 = rand() % 82 + 0; // possible answer 0-81
			} while (((op1*op2) == ansRand1 || (op1*op2) == ansRand2 || ansRand1 == ansRand2));  // to prevent occuring same numbers
			break;
		case 2:	op1 = rand() % 40 + 10;	op2 = rand() % 40 + 10;
			do { ansRand1 = rand() % 2302 + 100;	ansRand2 = rand() % 2302 + 100; // possible answer 100-2401
			} while (((op1*op2) == ansRand1 || (op1*op2) == ansRand2 || ansRand1 == ansRand2));
			break;
		case 3:	op1 = rand() % 50 + 50;	op2 = rand() % 50 + 50;
			do { ansRand1 = rand() % 7302 + 2500;	ansRand2 = rand() % 7302 + 2500; // possible answer 2500-9801
			} while (((op1*op2) == ansRand1 || (op1*op2) == ansRand2 || ansRand1 == ansRand2));
			break;
		}

		switch (randAnswer) { // random Correct Options
		case 1:	ANSWERS[0].value = (op1 * op2);		ANSWERS[1].value = ansRand1;	ANSWERS[2].value = ansRand2;
			ANSWERS[0].ans = 1; break;
		case 2:	ANSWERS[1].value = (op1 * op2);		ANSWERS[0].value = ansRand1;	ANSWERS[2].value = ansRand2;
			ANSWERS[1].ans = 1; break;
		case 3:	ANSWERS[2].value = (op1 * op2);		ANSWERS[0].value = ansRand1;	ANSWERS[1].value = ansRand2;
			ANSWERS[2].ans = 1; break;
		}
	}
}

void quizBackground() {

	glColor3ub(100, 100, 255); //QUIZ AREA
	glRectf(-(winWidth / 2), -(winHeight / 2), (winWidth / 2), -(winHeight / 2) + 80);

	glLineWidth(2);
	glColor3ub(255, 255, 255);
	vprint2(-(winWidth / 2) + 30, -(winHeight / 2) + 50, 0.15, " %s ", difficulty); // #EASY 0-9 ----- #NORMAL 10-49 -------- #HARD 50-99 
	glColor3ub(255, 255, 255);
	vprint2(-(winWidth / 2) + 30, -(winHeight / 2) + 20, 0.15, " %s ", diff_Range); 

	glColor3f(0, 0, 0);
	glLineWidth(5);
	glBegin(GL_LINES);
	glVertex2f(-(winWidth / 2) + 160, -(winHeight / 2));
	glVertex2f(-(winWidth / 2) + 160, -(winHeight / 2) + 80);
	glEnd();

	glColor3f(0, 0, 0);
	glLineWidth(5);
	glBegin(GL_LINES);
	glVertex2f((winWidth / 2) - 400, -(winHeight / 2));
	glVertex2f((winWidth / 2) - 400, -(winHeight / 2) + 80);
	glEnd();

	glColor3f(1, 1, 1);
	circle(-130, -260, 25);
	glColor3f(0, 0, 0);
	vprint2(-145, -282, 0.5, "*"); // multiplication 
}

void AnswersQuestion() {

		questionsCalculation();

	glColor3f(1, 1, 1);
	vprint2(-250, -284, 0.5, "%-2d", op1);
	vprint2(-90, -284, 0.5, "%2d", op2);

	// answerBoxes
	glColor3f(ANSWERS[0].col[0], ANSWERS[0].col[1], ANSWERS[0].col[2]);
	glRectf(60, -230, 170, -290);
	glColor3f(ANSWERS[1].col[0], ANSWERS[1].col[1], ANSWERS[1].col[2]);
	glRectf(195, -230, 305, -290);
	glColor3f(ANSWERS[2].col[0], ANSWERS[2].col[1], ANSWERS[2].col[2]);
	glRectf(330, -230, 440, -290);

	glColor3f(0, 0, 0);
	vprint2(75, -275, 0.3, "%2d", ANSWERS[0].value);
	vprint2(210, -275, 0.3, "%2d", ANSWERS[1].value);
	vprint2(345, -275, 0.3, "%2d", ANSWERS[2].value);

}

void infoBox() {
	glColor3ub(50, 50, 50);
	vprint((winWidth / 2) - 175, -(winHeight / 2) + 145, GLUT_BITMAP_HELVETICA_10, "<SPACE>               : start");
	vprint((winWidth / 2) - 175, -(winHeight / 2) + 130, GLUT_BITMAP_HELVETICA_10, "<UP> & <DOWN>  : difficulty level");
	vprint((winWidth / 2) - 175, -(winHeight / 2) + 115, GLUT_BITMAP_HELVETICA_10, "<F1>                       : pause & resume");
	vprint((winWidth / 2) - 175, -(winHeight / 2) + 100, GLUT_BITMAP_HELVETICA_10, "<F5>                       : retry");
	vprint((winWidth / 2) - 175, -(winHeight / 2) + 85, GLUT_BITMAP_HELVETICA_10, "<ESC>                    : menu");
}

void drawPCTank() { // STATIC MAIN OBJECT

	glColor3f(0, 0, 0); //tank tracks
	glRectf(-30, 50, -15, -1);
	glRectf(5, 50, 20, -1);

	glColor3f(1, 1, 0); // body
	glRectf(-22, 45, 13, 5);

	glColor3f(0, 0, 0);
	glLineWidth(1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-22, 45);
	glVertex2f(13, 45);
	glVertex2f(13, 5);
	glVertex2f(-22, 5);
	glEnd();

	glColor3f(0, 0, 0);
	glBegin(GL_QUADS); // different BARREL that fires
	glVertex2f(-5 + 50 * cos((360 + BARREL.angle) * D2R), 25 + 50 * sin((360 + BARREL.angle) * D2R));
	glVertex2f(-5 + 14 * cos((260 + BARREL.angle) * D2R), 25 + 14 * sin((260 + BARREL.angle) * D2R));
	glVertex2f(-5 + 16 * cos((180 + BARREL.angle) * D2R), 25 + 16 * sin((180 + BARREL.angle) * D2R));
	glVertex2f(-5 + 14 * cos((100 + BARREL.angle) * D2R), 25 + 14 * sin((100 + BARREL.angle) * D2R));
	glEnd();

	glColor3ub(75, 25, 0);
	circle(-5, 25, 10); //connected to the barrel 
	glColor3f(1, 1, 1);
	vprint(BARREL.pos.x - 8, BARREL.pos.y + -3, GLUT_BITMAP_TIMES_ROMAN_10, "%d", BARREL.angle);

	if (gameState && BARREL.BULLET.hit) {
		glColor4f(0, 0, 0, BARREL.BULLET.visib);
		circle(BARREL.BULLET.pos.x, BARREL.BULLET.pos.y, 6);	//bullet//
	}

	/////////////// MAIN TANK AREA ///////////////
	glColor3f(0, 0, 0);
	glLineWidth(3);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-73, 90);
	glVertex2f(65, 90);
	glVertex2f(65, -50);
	glVertex2f(-73, -50);
	glEnd();
	/////////////// MAIN TANK AREA ///////////////
}

void drawPlayerTank() { // specified random position, color, number

	int BX = 1, BX2 = 0;

	glColor4ub(0, 0, 0, plaTank.visib);
	glRectf(-30 + plaTank.pos.x, 50 + plaTank.pos.y, 19 + plaTank.pos.x, 35 + plaTank.pos.y);
	glRectf(-30 + plaTank.pos.x, 20 + plaTank.pos.y, 19 + plaTank.pos.x, 5 + plaTank.pos.y);

	glColor4ub(plaTank.col.r, plaTank.col.g, plaTank.col.b, plaTank.visib); // body
	glRectf(-23 + plaTank.pos.x, 46 + plaTank.pos.y, 13 + plaTank.pos.x, 9 + plaTank.pos.y);

	glColor4ub(0, 0, 0, plaTank.visib);
	glLineWidth(1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-23 + plaTank.pos.x, 46 + plaTank.pos.y);
	glVertex2f(13 + plaTank.pos.x, 46 + plaTank.pos.y);
	glVertex2f(13 + plaTank.pos.x, 9 + plaTank.pos.y);
	glVertex2f(-23 + plaTank.pos.x, 9 + plaTank.pos.y);
	glEnd();

	circle(-5 + plaTank.pos.x, 28 + plaTank.pos.y, 12);

	if (plaTank.pos.x < 0) { // then, the barrel will be rotated to left side
		BX = -1; BX2 = 8;
	}

	glColor4ub(0, 0, 0, plaTank.visib);
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2f((-4 + BX2) * BX + plaTank.pos.x, 28 + plaTank.pos.y);
	glVertex2f((36 + BX2) * BX + plaTank.pos.x, 28 + plaTank.pos.y);
	glEnd();
	glRectf((35 + BX2) * BX + plaTank.pos.x, 35 + plaTank.pos.y, (46 + BX2) * BX + plaTank.pos.x, 21 + plaTank.pos.y);

	glColor4ub(255, 255, 255, plaTank.visib);
	vprint(-10 + plaTank.pos.x, 21 + plaTank.pos.y, GLUT_BITMAP_HELVETICA_18, "%d", randPoint); // the random number on the tank will also show the points that player/pc can get

	if (gameState && checkTimer) {
		if (BARREL.BULLET.align == true && plaTank.escaped == false) {
			glColor3f(1, 0, 0);
			circle_wire(plaTank.centerX, plaTank.centerY, 35); // aim indicator
			circle_wire(plaTank.centerX, plaTank.centerY, 17);
		}
		 if ( notifCounter > 0 && notifCounter < 32) {
			glColor3ub(150, 0, 0);		circle(plaTank.centerX, plaTank.centerY, 30);
			glColor3ub(200, 0, 0);			circle(plaTank.centerX, plaTank.centerY, 20);// explosion effect
			glColor3ub(255, 0, 0);			circle(plaTank.centerX, plaTank.centerY, 10);
			BARREL.BULLET.visib = 0;	givenAnswer = true; // after the explosion, no way to answer.
		}
	}
}

void showRun() {

	glClearColor(200.0 / 255.0, 200.0 / 255.0, 200.0 / 255.0, 1.0f); //gray
	glClear(GL_COLOR_BUFFER_BIT);

	drawPCTank(); //main-static object
		quizBackground();
			scoreArea();

	if (gameState == true) { // will be displayed after the game starting
		drawPlayerTank();
			AnswersQuestion();
	}
		infoBox();
}

void display() {

	checkButtons();

	switch (State) {
	case LOGO:    showLogo(); break;
	case MENU:    showMenu(); break;
	case LOAD:    showLoad(); break;
	case RUN:     showRun(); break;
	case CREDITS: showCredits(); break;
	case OVER:    showOver(); break;
	}
	glutSwapBuffers();
}

void onKeyUp(unsigned char key, int x, int y)
{
	if (resume == 1) {
		if (key == ' ' && State == RUN && gameState == false) { // space
			gameState = true;	checkTimer = true;	init = true;
		}
	}

	if (key == 27 && State == RUN)
		State = MENU;
}

void onSpecialKeyDown(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP:
		if (State == RUN && gameState == false && diffLevel < 3) // must be done before game starting
			diffLevel++;	break;
	case GLUT_KEY_DOWN:
		if (State == RUN && gameState == false && diffLevel > 1)
			diffLevel--;	break;
	}
}

void onSpecialKeyUp(int key, int x, int y)
{
	if (key == GLUT_KEY_F5 && State == RUN && notifCounter < -5) 
		 State = LOAD;
	if (key == GLUT_KEY_F1 && State == RUN && resume == 1 && notifCounter < -5 && gameState) {
		resume = 0; checkTimer = false;	 givenAnswer = true;
	}
	else if (key == GLUT_KEY_F1 && State == RUN && resume == 0) {
		resume = 1; checkTimer = true;	 givenAnswer = false;
	}
}

void onClick(int button, int stat, int x, int y)
{
	int mouX = x - winWidth / 2, mouY = winHeight / 2 - y;

	if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN) {
		if (State == MENU) {
			if (buttonS.status[0] == true) // START
				{Init_Globals(); State = LOAD;}
			else if (buttonS.status[1] == true) // CREDITS
				State = CREDITS;
			else if (buttonS.status[2] == true) // EXIT
				exit(0);
		}

		if (State == CREDITS || State == OVER) {
			if (buttonS.status[3] == true)  // EXIT
			{Init_Globals(); State = MENU;}
			if (State == OVER && buttonS.status[4] == true)  //RETRY
				State = LOAD;
		}

	}

	if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && State == RUN && givenAnswer == false) {

		if (mouX > 60 && mouX < 170 && mouY > -290 && mouY < -230) {//ansBox-1
			ANSWERS[0].clicked = 1;		givenAnswer = true;
			if (ANSWERS[0].ans == 1) { correctAnswer = true; countCor++; }
			else { correctAnswer = false; countInc++; }
		} 

		if (mouX > 195 && mouX < 305 && mouY > -290 && mouY < -230) {//ansBox-2
			ANSWERS[1].clicked = 1;		givenAnswer = true;
			if (ANSWERS[1].ans == 1) { correctAnswer = true; countCor++; }
			else { correctAnswer = false; countInc++; }
		} 

		if (mouX > 330 && mouX < 440 && mouY > -290 && mouY < -230) {//ansBox-3
			ANSWERS[2].clicked = 1;		givenAnswer = true;
			if (ANSWERS[2].ans == 1) { correctAnswer = true; countCor++; }
			else { correctAnswer = false; countInc++; }
		} 
	}
}

void onMove(int x, int y) {
	mX = x - winWidth / 2; mY = winHeight / 2 - y;

	if (gameState == true && givenAnswer == false) {

		if (mX > 60 && mX < 170 && mY > -290 && mY < -230) { ANSWERS[0].col[0] = 0;  ANSWERS[0].col[1] = 0;  ANSWERS[0].col[2] = 1; } //ansBox-1
		else { ANSWERS[0].col[0] = 1;  ANSWERS[0].col[1] = 1;  ANSWERS[0].col[2] = 0; }
		if (mX > 195 && mX < 305 && mY > -290 && mY < -230) { ANSWERS[1].col[0] = 0;  ANSWERS[1].col[1] = 0;  ANSWERS[1].col[2] = 1; } //ansBox-2
		else { ANSWERS[1].col[0] = 1;  ANSWERS[1].col[1] = 1;  ANSWERS[1].col[2] = 0; }
		if (mX > 330 && mX < 440 && mY > -290 && mY < -230) { ANSWERS[2].col[0] = 0;  ANSWERS[2].col[1] = 0;  ANSWERS[2].col[2] = 1; } //ansBox-3
		else { ANSWERS[2].col[0] = 1;  ANSWERS[2].col[1] = 1;  ANSWERS[2].col[2] = 0; }
	}
}

void gameCalculations() {

	for (int i = 0; i < 3; i++) {
		if (ANSWERS[i].clicked == 1) {
			if (correctAnswer == true) { ANSWERS[i].col[0] = 0; 	ANSWERS[i].col[1] = 1; 	ANSWERS[i].col[2] = 0;}
			else { ANSWERS[i].col[0] = 1; 	ANSWERS[i].col[1] = 0; 	ANSWERS[i].col[2] = 0; }
		}
	}

	if (gameState && checkTimer) {

		if (correctAnswer == true && notifCounter < -5) {
			if (plaTank.pos.x > 0)	plaTank.pos.x += 5;
			if (plaTank.pos.x < 0)	plaTank.pos.x -= 5; ;
			plaTank.escaped = true;
		}

		if (plaTank.pos.x > 520 || plaTank.pos.x < -520) {  // player wins
			plaSco += randPoint;	resetStatus = true;
		}
	}

	switch (diffLevel) {
	case 1: strcpy_s(difficulty, "EASY"); strcpy_s(diff_Range, "[0-9]"); break;
	case 2: strcpy_s(difficulty, "NORMAL"); strcpy_s(diff_Range, "[10-49]"); break;
	case 3: strcpy_s(difficulty, "HARD"); strcpy_s(diff_Range, "[50-99]"); break;
	}
}

void Barrel_Bullet_Calculations() {

	int dX, dY;

	if (gameState && checkTimer) {

		if (plaTank.escaped == false) { // tank escape

			plaTank.centerX = (-30 + plaTank.pos.x + 20 + plaTank.pos.x) / 2;		plaTank.centerY = (5 + plaTank.pos.y + 50 + plaTank.pos.y) / 2; // center Point
			dY = plaTank.centerY - BARREL.pos.y;		dX = plaTank.centerX - BARREL.pos.x;
			if (dY < 0) {
				plaTank.angle = 360 + (atan2(dY, dX) / D2R); // the angle  (for negative coordinates)
			}
			else
				plaTank.angle = atan2((plaTank.centerY - BARREL.pos.y), (plaTank.centerX - BARREL.pos.x)) / D2R;
		}

		if (BARREL.angle < 0)
			BARREL.angle += 360;
		else if (BARREL.angle > 360)
			BARREL.angle -= 360;

		if (BARREL.BULLET.hit == false) { // calculations of the nearest pos.

			if (BARREL.angle >= 0 && BARREL.angle <= 180) {

				if (BARREL.angle < plaTank.angle && (BARREL.angle + 180) > plaTank.angle) {
					BARREL.angle += 1;
				}
				else
					BARREL.angle -= 1;
			}
			else if (BARREL.angle > 180 && BARREL.angle <= 360) {

				if (BARREL.angle > plaTank.angle && (BARREL.angle - 180) < plaTank.angle) {
					BARREL.angle -= 1;
				}
				else
					BARREL.angle += 1;
			}
		} // end

		if (BARREL.angle == plaTank.angle && BARREL.BULLET.stop == false) { // calculations of finding object 
			BARREL.BULLET.pos = BARREL.pos;			BARREL.BULLET.angle = BARREL.angle;			BARREL.BULLET.visib = 255;
			BARREL.BULLET.align = true;		BARREL.BULLET.hit = true;		BARREL.BULLET.stop = true;
		}
		
		if (BARREL.BULLET.align) { // if it aligns
			BARREL.BULLET.pos.x += 7 * cos((BARREL.BULLET.angle) * D2R);		BARREL.BULLET.pos.y += 7 * sin((BARREL.BULLET.angle) * D2R);
		}

		if (BARREL.BULLET.pos.x > -30 + plaTank.pos.x && BARREL.BULLET.pos.x < 20 + plaTank.pos.x && BARREL.BULLET.pos.y > 0 + plaTank.pos.y && BARREL.BULLET.pos.y < 50 + plaTank.pos.y)
		{ // hitBox
			if (gameState && checkTimer && BARREL.BULLET.align == true) {
				pcSco += randPoint;
				plaTank.visib = 0;	 notifCounter = 35;	  BARREL.BULLET.pos.x = 0;		BARREL.BULLET.visib = 0; // invisible
				BARREL.BULLET.align = false;
			}
		}
		else
			plaTank.escaped = false;
	}
}

void ResetFunc() {

	if (gameState && checkTimer && resetStatus) {

		for (int i = 0; i < 3; i++) {
			ANSWERS[i].col[0] = 1; 	ANSWERS[i].col[1] = 1; 	ANSWERS[i].col[2] = 0;
			ANSWERS[i].clicked = 0;
		}

		randPoint = rand() % 9 + 1;
		correctAnswer = givenAnswer = resetStatus = false;
		plaTank.visib = 255;	BARREL.BULLET.visib = 0;
		BARREL.BULLET.align = false;	BARREL.BULLET.hit = false;			BARREL.BULLET.stop = false;		plaTank.escaped = false;
		countAll++;

		plaTank.col.r = rand() % 256;	 plaTank.col.g = rand() % 256;		plaTank.col.b = rand() % 256;
		do {
			plaTank.pos.x = rand() % 780 - 390;	plaTank.pos.y = rand() % 375 - 210;
		} while ((plaTank.pos.x > -100 && plaTank.pos.x < 100 && plaTank.pos.y < 95 && plaTank.pos.y > -110));
	}
}

void onTimer(int v) {

	glutTimerFunc((1000 / FPS), onTimer, 0);

	if (State == LOGO) {
		logoTime++;
		if (logoTime >= 75)
			State = MENU;
	}

	if (State == LOAD) {
		dotCounter = (dotCounter + 1) % 4;
		loadingTime += 0.5;
		if (loadingTime >= 25) {
			Init_Globals();	State = RUN;
		}
		
	}

	if (State == RUN && checkTimer && gameState && gameTime != 0) {
		gameTime -= 0.0165;
		if (gameTime <= initTime)
			init = false;
		if (gameTime < 0) {
			gameTime = 0;
			State = OVER;
			gameState = false;		checkTimer = false;
		}
	}	
	///////////////////// call the functions ///////////////////////////////////
		ResetFunc();
			gameCalculations();
				Barrel_Bullet_Calculations();
	///////////////////// call the functions ///////////////////////////////////

			if (gameState && notifCounter == 0) // explosionTime
				resetStatus = true;
			if (!(notifCounter < -10)) // terminationPoint 
				notifCounter -= 1;

	glutPostRedisplay();
}

void onResize(int w, int h)
{
	we = w; he = h;
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void Init() {

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Init_Globals();

}

void main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("BattleTank | MURAT ALKAN");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	glutSpecialFunc(onSpecialKeyDown);
	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	glutMouseFunc(onClick);
	glutPassiveMotionFunc(onMove);

	srand(time(NULL));
	glutTimerFunc((1000 / FPS), onTimer, 0);

	Init();

	glutMainLoop();
}
