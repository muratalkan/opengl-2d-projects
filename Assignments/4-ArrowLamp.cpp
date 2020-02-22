#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "vec.h"

#define WINDOW_WIDTH 1080
#define WINDOW_HEIGHT 720

#define FPS 60
#define PI 3.1415

#define LOGO 0
#define MENU 1
#define LOAD 2
#define RUN 3
#define CREDITS 4
#define OVER 5
int State = LOGO; // initial Scene 

int winWidth, winHeight, we, he;
int resume, logoTime, loadWidth, lampCount;
int mX, mY, k, brTemp = 3;
bool gameState, yellowCol = false;

typedef struct {
	double r, g, b;
}col_t;

typedef struct {
	col_t col[5];
	bool status[5];
}button_t;
button_t buttonS;

typedef struct {
	vec_t pos;
	col_t col[2];
	int angle;
	bool status = false;
}arrow_t;
arrow_t ARROW;

typedef struct {
	vec_t pos;
	col_t col;
	bool status;
	char light[5];
	int brightness[2] = { 0 };// 0 -> levelCoefficient  ||| 1-> level num (1-3) 
} lightSource_t;
lightSource_t LAMP;

typedef struct {
	vec_t pos, N;
} vert_t;

col_t mulColor(float x, col_t coL) {
	return { x * coL.r, x * coL.g, x * coL.b };
}

col_t addColor(col_t coL1, col_t coL2) {
	return { coL1.r + coL2.r, coL1.g + coL2.g, coL1.b + coL2.b };
}

col_t calColor(lightSource_t lamp, vert_t v) {
	vec_t L = subV(lamp.pos, v.pos), uL = unitV(L);;
	float fact = dotP(uL, v.N) * ((-1 / 350.0) * magV(L) + 1);
	return mulColor(fact, lamp.col);
}

void circle(int x, int y, int r)
{
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
	float angle;
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI * i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void vprint(int x, int y, void* font, const char* string, ...)
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

void vprint2(int x, int y, float size, const char* string, ...) {
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

	resume = 1;
	gameState = ARROW.status = false;
	k = lampCount = loadWidth = 0;

	LAMP = { { 0, 0 }, {0,0,0}, false, {0} };
	buttonS = { {{0,0,255},{0,0,255}, {0,0,255}, { 100,100,100 } }, {{false},{false},{false},{false} } };

	strcpy(LAMP.light, "OFF");
	LAMP.brightness[1] = brTemp; // initial value is 3

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
	circle(0, 25, 200);

	glColor3f(1, 1, 1);
	glLineWidth(4);
	vprint2(-125, -50, 1.5, "MA");// LOGO | (M)urat (A)lkan
}

void showLoad() {

	glClearColor(15.0 / 255.0, 15.0 / 255.0, 15.0 / 255.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1, 1, 1);
	glLineWidth(10);
	vprint2(-290, 35, 0.5, "ENJOY THE GAME");

	glLineWidth(5);
	glColor3f(0, 1, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-400, 0);
	glVertex2f(400, 0);
	glVertex2f(400, -100);
	glVertex2f(-400, -100);
	glEnd();

	glColor3f(0, 1, 0);
	glRectf(-400, 0, -400 + loadWidth, -100); //loading box

	if (loadWidth <= 810)
		loadWidth += 10;

}

void checkButtons() {

	if (State == MENU) {
		if (mX > -70 && mX < 70 && mY > 90 && mY < 150) {
			buttonS.col[0] = { 100, 100, 255 };  buttonS.status[0] = true;
		}  // START BUTTON	
		else { buttonS.col[0] = { 0, 0, 255 };	 buttonS.status[0] = false; }

		if (mX > -70 && mX < 70 && mY > 0 && mY < 60) {
			buttonS.col[1] = { 100, 100, 255 };  buttonS.status[1] = true;
		}	// CREDITS BUTTON
		else { buttonS.col[1] = { 0, 0, 255 };   buttonS.status[1] = false; }

		if (mX > -70 && mX < 70 && mY > -95 && mY < -35) {
			buttonS.col[2] = { 100, 100, 255 };	 buttonS.status[2] = true;
		}	// EXIT BUTTON
		else { buttonS.col[2] = { 0, 0, 255 };	 buttonS.status[2] = false; }
	}

	if (State == OVER || State == CREDITS) { //BACK BUTTON

		if (mX > 420 && mX < 500 && mY > -300 && mY < -250) {
			buttonS.col[3] = { 150, 150, 150 };		buttonS.status[3] = true;
		}
		else { buttonS.col[3] = { 100, 100, 100 };	buttonS.status[3] = false; }

		if (State == OVER) { //RETRY BUTTON
			if (mX > -70 && mX < 70 && mY > -130 && mY < -70) {
				buttonS.col[4] = { 50, 255, 50 };	buttonS.status[4] = true;
			}
			else { buttonS.col[4] = { 0, 200, 0 };	buttonS.status[4] = false; }
		}
	}

}

void backButton() { // will be on Credits and Over screen.
	glColor3ub(buttonS.col[3].r, buttonS.col[3].g, buttonS.col[3].b);
	glLineWidth(3);
	glRectf(420, -250, 500, -300);
	glColor3f(0, 0, 0);
	vprint2(433, -282, 0.2, "Back");
}

void showMenu() {
	checkButtons();
	glClearColor(32.0 / 255.0, 32.0 / 255.0, 32.0 / 255.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3ub(50, 150, 250); // nameSurname
	vprint(-(winWidth / 2) + 25, (winHeight / 2) - 20, GLUT_BITMAP_8_BY_13, "Murat Alkan");
	vprint(-(winWidth / 2) + 11.5, (winHeight / 2) - 40, GLUT_BITMAP_8_BY_13, "-Assignment 4-");

	glColor3ub(255, 100, 0);
	glLineWidth(2);
	glBegin(GL_LINE_LOOP); //nameSurname Box
	glVertex2f(-(winWidth / 2) + 10, (winHeight / 2) - 5);
	glVertex2f(-(winWidth / 2) + 125, (winHeight / 2) - 5);
	glVertex2f(-(winWidth / 2) + 125, (winHeight / 2) - 50);
	glVertex2f(-(winWidth / 2) + 10, (winHeight / 2) - 50);
	glEnd();

	glColor3f(1, 1, 1);
	glLineWidth(3);
	vprint2(-100, 220, 0.3, "ArrowLamp");

	glColor3ub(255, 255, 0); // menuCircle
	circle(0, 25, 175);
	glColor3f(0, 0, 0);
	glLineWidth(7);
	circle_wire(0, 25, 175);

	//BUTTONS on Menu
	glColor3ub(buttonS.col[0].r, buttonS.col[0].g, buttonS.col[0].b);  glRectf(-70, 150, 70, 90);
	glColor3ub(buttonS.col[1].r, buttonS.col[1].g, buttonS.col[1].b);  glRectf(-70, 60, 70, 0);
	glColor3ub(buttonS.col[2].r, buttonS.col[2].g, buttonS.col[2].b);  glRectf(-70, -35, 70, -95);

	glLineWidth(3);
	glColor3f(0, 0, 0);
	vprint2(-45, 105, 0.25, "START");
	vprint2(-50, 15, 0.25, "CREDIT");
	vprint2(-27, -75, 0.25, "EXIT");

}

void showCredits() {

	glClearColor(150.0 / 255.0, 50.0 / 255.0, 255.0 / 255.0, 1.0f);
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


	backButton(); //call backButtonFunc

}

void showOver() {

	glClearColor(225.0 / 255.0, 225.0 / 255.0, 175.0 / 255.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0, 0, 0);
	glLineWidth(7);
	vprint2(-170, 115, 1, "OVER");

	glColor3ub(0, 0, 50);
	glRectf(-540, 100, 540, -50);
	glLineWidth(3);
	glColor3f(1, 1, 0);
	vprint2(-125, 30, 0.3, "Broken Lamp");
	glColor3f(1, 1, 1);
	vprint2(-10, -25, 0.3, "%d", lampCount); // it shows how many lamp you broke

	glColor3f(0.75, 0.75, 0.75);
	glLineWidth(10);
	glBegin(GL_LINES);
	glVertex2f(-200, 100);
	glVertex2f(-200, -50);
	glVertex2f(200, 100);
	glVertex2f(200, -50);
	glEnd();

	//////////////// RETRY BUTTON //////////////////////

	glColor3ub(buttonS.col[4].r, buttonS.col[4].g, buttonS.col[4].b); glRectf(-70, -70, 70, -130);

	glLineWidth(3);
	glColor3f(0, 0, 0);
	vprint2(-47, -112, 0.25, "RETRY");

	backButton();  //call backButtonFunc
}

void infoBox() {
	glColor3f(0, 0, 0);
	glRectf(-470, 360, -540, 310);
	glRectf(-520, 310, -490, 270);

	glColor3f(1, 1, 1); //whiteNum
	if (yellowCol)
		glColor3f(1, 1, 0); // yellowNum
	vprint2(-515, 280, 0.225, "%d", LAMP.brightness[1]);

	glColor3f(0.9, 0.4, 0); //ORANGE -OFF
	if (strcmp(LAMP.light, "ON") == 0)
		glColor3f(0, 0.7, 0); // GREEN -ON
	vprint2(-530, 322, 0.225, "%s", LAMP.light);

	glColor3f(1, 1, 1);
	vprint((winWidth / 2) - 185, -(winHeight / 2) + 115, GLUT_BITMAP_HELVETICA_10, "<MOUSE2>	            :  switch on");
	vprint((winWidth / 2) - 185, -(winHeight / 2) + 100, GLUT_BITMAP_HELVETICA_10, "<MOUSE1>		            :  spawn arrow");
	vprint((winWidth / 2) - 185, -(winHeight / 2) + 85, GLUT_BITMAP_HELVETICA_10, "<UP & <DOWN>	    :  adjust brightness");
	vprint((winWidth / 2) - 185, -(winHeight / 2) + 70, GLUT_BITMAP_HELVETICA_10, "<SPACE>	               :  change color");
	vprint((winWidth / 2) - 185, -(winHeight / 2) + 55, GLUT_BITMAP_HELVETICA_10, "<F1>                       :  pause & resume");
	vprint((winWidth / 2) - 185, -(winHeight / 2) + 40, GLUT_BITMAP_HELVETICA_10, "<F5>                       :  retry");
	vprint((winWidth / 2) - 185, -(winHeight / 2) + 25, GLUT_BITMAP_HELVETICA_10, "<ESC>                    :  over");
}
void draw_Road() {

	int x = -540, y = -360, rX = 0, rY = 0;

	while (x <= 540 && y <= 25) //  x value [-500, 500] && y value [-350, 25]
	{
		vert_t ptr = { { x, LAMP.brightness[0]}, { 0, 1 } };
		col_t lightEffect = { 0 };
		lightEffect = addColor(lightEffect, calColor(LAMP, ptr));

		glBegin(GL_LINES);
		glColor3f(lightEffect.r, lightEffect.g, lightEffect.b);
		glVertex2f(x, y + 175); // y incrementation per line is +175
		glColor3f(0, 0, 0);
		glVertex2f(x, y);
		glEnd();

		x += 4;
		y += 1;
	}

	////////////// ROAD LINES /////////////////
	glColor3f(0, 0, 0);
	while (rX <= 750 && rY <= 260) {

		glBegin(GL_QUADS);
		glVertex2f(-425 + rX, -235 + rY);
		glVertex2f(-325 + rX, -210 + rY);
		glVertex2f(-325 + rX, -230 + rY);
		glVertex2f(-425 + rX, -256 + rY);
		glEnd();

		rX += 250;
		rY += 65;
	}

	//GROUND
	glColor3ub(90, 40, 0);
	glBegin(GL_TRIANGLES);
	glVertex2f(540, -350);
	glVertex2f(540, -90);
	glVertex2f(-540, -360);
	glEnd();

	// TOP OF THE RUN SCREEN
	glColor3f(0.1, 0.1, 0.1);
	glRectf(-20, 355, 540, 335);
	glRectf(-20, 355, 20, 315);

	glLineWidth(10);
	glBegin(GL_LINES);
	glVertex2f(540, 230);
	glVertex2f(250, 340);
	glEnd();

}

void draw_Lamp() {

	glColor3f(0.1, 0.1, 0.1);
	glLineWidth(5);
	glBegin(GL_LINES);
	glVertex2f(LAMP.pos.x, LAMP.pos.y + 50);
	glVertex2f(0, 320);
	glEnd();

	glColor3f(LAMP.col.r, LAMP.col.g, LAMP.col.b); // WHITE-YELLOW
	circle(LAMP.pos.x, LAMP.pos.y + 50, 15);

}

void draw_Arrow() {

	glColor3f(ARROW.col[0].r, ARROW.col[0].g, ARROW.col[0].b);// GREEN - ORANGE
	glBegin(GL_QUADS);
	glVertex2f(ARROW.pos.x + 60 * cos((ARROW.angle + 360) * (PI / 180)), ARROW.pos.y + 60 * sin((ARROW.angle + 360) * (PI / 180)));
	glVertex2f(ARROW.pos.x + 8 * cos((ARROW.angle + 260) * (PI / 180)), ARROW.pos.y + 8 * sin((ARROW.angle + 260) * (PI / 180)));
	glVertex2f(ARROW.pos.x + 12 * cos((ARROW.angle + 180) * (PI / 180)), ARROW.pos.y + 12 * sin((ARROW.angle + 180) * (PI / 180)));
	glVertex2f(ARROW.pos.x + 8 * cos((ARROW.angle + 100) * (PI / 180)), ARROW.pos.y + 8 * sin((ARROW.angle + 100) * (PI / 180)));
	glEnd();

	glLineWidth(4);
	glColor3f(ARROW.col[1].r, ARROW.col[1].g, ARROW.col[1].b); // GREEN/BLACK - ORANGE/WHITE
	glBegin(GL_LINE_LOOP);
	glVertex2f(ARROW.pos.x + 60 * cos((ARROW.angle + 360) * (PI / 180)), ARROW.pos.y + 60 * sin((ARROW.angle + 360) * (PI / 180)));
	glVertex2f(ARROW.pos.x + 8 * cos((ARROW.angle + 260) * (PI / 180)), ARROW.pos.y + 8 * sin((ARROW.angle + 260) * (PI / 180)));
	glVertex2f(ARROW.pos.x + 17 * cos((ARROW.angle + 180) * (PI / 180)), ARROW.pos.y + 17 * sin((ARROW.angle + 180) * (PI / 180)));
	glVertex2f(ARROW.pos.x + 8 * cos((ARROW.angle + 100) * (PI / 180)), ARROW.pos.y + 8 * sin((ARROW.angle + 100) * (PI / 180)));
	glEnd();

}

void draw_Stars() {

	glBegin(GL_POINTS);
	glColor3f(1, 1, 1);
	for (int i = 1; i <= 250; i++) {
		int m = rand() % 1120 - 540;
		int n = rand() % 550 - 170;
		glVertex2f(m, n);
	}
	glEnd();

}

void showRun() {

	glClearColor(0 / 255.0, 0 / 255.0, 100.0 / 255.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	draw_Stars();
	draw_Road();
	draw_Lamp();

	if (ARROW.status) // without pressing MOUSE1, arrow will not be shown
		draw_Arrow();

	infoBox(); //bottomRight ( gameFunctions)

}

void display() {

	checkButtons(); // mouse support

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

void onKeyDown(unsigned char key, int x, int y) //optional keyboardSupports 
{
	if (key == 27 && State == RUN && resume && gameState) {
		State = OVER;
		gameState = false; resume = 0;
	}
	if (key == ' ' && State == RUN && resume && gameState && LAMP.status)
		yellowCol = !yellowCol; // changes light color

}

void onSpecialKeyDown(int key, int x, int y)
{
	// pause/resume and retry buttons
	if (key == GLUT_KEY_F5 && State == RUN && resume && gameState)
		State = LOAD;
	if (key == GLUT_KEY_F1 && State == RUN && resume && gameState)
		resume = 0;
	else if (key == GLUT_KEY_F1 && State == RUN && !resume && gameState)
		resume = 1;

	switch (key) { //adjust brightnessLevel
	case GLUT_KEY_UP:
		if (State == RUN && resume && gameState && LAMP.status && LAMP.brightness[1] < 3)
			LAMP.brightness[1]++;
		break;
	case GLUT_KEY_DOWN:
		if (State == RUN && resume && gameState && LAMP.status && LAMP.brightness[1] > 1)
			LAMP.brightness[1]--;
		break;
	}

}

void onClick(int button, int stat, int x, int y)
{
	int mouX = x - winWidth / 2, mouY = winHeight / 2 - y;

	if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN) {
		if (State == MENU) {
			if (buttonS.status[0] == true) // START
			{
				Init_Globals(); State = LOAD;
			}
			else if (buttonS.status[1] == true) // CREDITS
				State = CREDITS;
			else if (buttonS.status[2] == true) // EXIT
				exit(0);
		}

		if (State == CREDITS || State == OVER) {
			if (buttonS.status[3] == true)  // EXIT
			{
				Init_Globals(); State = MENU;
			}
			if (State == OVER && buttonS.status[4] == true)  //RETRY
				State = LOAD;
		}

	}

	if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && State == RUN && !ARROW.status && gameState && resume && LAMP.status) { //spawn Arrow
		ARROW.pos.x = mouX;		ARROW.pos.y = mouY;
		ARROW.status = true;
		ARROW.col[0] = { 0,0.7,0 };	ARROW.col[1] = { 0,0,0 };
	}

	if (button == GLUT_RIGHT_BUTTON && stat == GLUT_DOWN && resume && State == RUN) { //switch on the light
		if (!gameState) { //initial switching
			gameState = true; LAMP.status = true;
			strcpy_s(LAMP.light, "ON");
			if (yellowCol)
				LAMP.col = { 1,1,0 };
			else
				LAMP.col = { 1,1,1 };
		}
		if (!LAMP.status) { // switching after the hit
			gameState = true; LAMP.status = true;
			strcpy_s(LAMP.light, "ON");
			LAMP.brightness[1] = brTemp;
			if (yellowCol)
				LAMP.col = { 1,1,0 };
			else
				LAMP.col = { 1,1,1 };
		}
	}
}

void onMove(int x, int y) { //OPENGL Position

	mX = x - winWidth / 2;
	mY = winHeight / 2 - y;

	glutPostRedisplay();
}

void gameCalculations() {

	if (gameState) {
		ARROW.angle = atan2((LAMP.pos.y + 50 - ARROW.pos.y), (LAMP.pos.x - ARROW.pos.x)) / (PI / 180); // find the correct angle between arrow and lamp

		//Arrow Cal.
		ARROW.pos.x += 5 * cos((ARROW.angle) * (PI / 180));
		ARROW.pos.y += 5 * sin((ARROW.angle) * (PI / 180));

		//Lamp Cal.
		float temp = ((sin((PI / 180) * k) * 52 + 270) * (PI / 180)); //( 270 is default )
		LAMP.pos.x = cos(temp) * 270.0;
		LAMP.pos.y = 270.0 + (sin(temp) * 270.0);

		if (LAMP.pos.x >= 275.75) //prevent generating finite numbers
			k = 91;
		else
			k++;
	}
}

void collision() {
	int hitX = ARROW.pos.x + 60 * cos((360 + ARROW.angle) * (PI / 180)), hitY = ARROW.pos.y + 60 * sin((360 + ARROW.angle) * (PI / 180)); // firstHit

	if (ARROW.status) {
		if (ARROW.pos.x > LAMP.pos.x - 15 && ARROW.pos.x < LAMP.pos.x + 15 && ARROW.pos.y < LAMP.pos.y + 65 && ARROW.pos.y > LAMP.pos.y + 35) { //broken lamp
			LAMP.col = { 0,0,0 };
			LAMP.status = false;	ARROW.status = false;
			lampCount++;
			LAMP.brightness[1] = 0;
			strcpy_s(LAMP.light, "OFF");
		}
		else if (hitX > LAMP.pos.x - 15 && hitX < LAMP.pos.x + 15 && hitY < LAMP.pos.y + 65 && hitY > LAMP.pos.y + 35) { // orangeArrow
			ARROW.col[0] = { 0.9,0.4,0 };		ARROW.col[1] = { 0.85,0.85,0.85 };
		}
	}

}

void brightnessFunc() {

	if (gameState && LAMP.status) { // adjust brightness levels
		switch (LAMP.brightness[1]) {
		case 1:	LAMP.brightness[0] = -250;
			if (!yellowCol)		LAMP.col = { 0.85, 0.85, 0.85 };
			else if (yellowCol)	LAMP.col = { 1,1,0.55 };
			break;
		case 2:	LAMP.brightness[0] = -175;
			if (!yellowCol)		LAMP.col = { 0.95, 0.95, 0.95 };
			else if (yellowCol)	LAMP.col = { 1,1,0.35 };
			break;
		case 3:	LAMP.brightness[0] = -100;
			if (!yellowCol)		LAMP.col = { 1,1,1 };
			else if (yellowCol)	LAMP.col = { 1,1,0 };
			break;
		}
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
		if (loadWidth >= 810) { //LoadingTime
			Init_Globals();	State = RUN;
		}
	}

	if (resume) {
		gameCalculations();
		collision();
		brightnessFunc();
		if (LAMP.status)
			brTemp = LAMP.brightness[1]; //get the last value of brightness before hit.
	}

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
	display();
}

void Init() {

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Init_Globals();
}

void main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("ArrowLamp | MURAT ALKAN");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutMouseFunc(onClick);
	glutPassiveMotionFunc(onMove);

	glutTimerFunc((1000 / FPS), onTimer, 0);

	Init();

	glutMainLoop();
}
