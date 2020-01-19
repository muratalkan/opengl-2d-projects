#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 600

#define FPS 60

#define LOGO 0
#define MENU 1
#define LOAD 2
#define RUN 3
#define CREDITS 4
#define OVER 5
int State = LOGO; // initial Scene 

#define MAXBULLET 31 // 30 bullet in a magazine
#define MAXBOT 5 // MAX 5 OBJECTS

int winWidth, winHeight, we, he;
int resume, logoTime = 0;
int numberofBullet, dotCounter, bulletCounter, accuracy;
int gameScore, gameHs, gameTarget; // HS point will be more
double gunY;
double gameTime, notifCount, loadingTime;
char gunMode[10] = "SINGLE"; 
bool init, spawnBullet, checkTimer, accuracyBool, spawnBot, botMovement, controlSpeed;
int bulletX[MAXBULLET], bulletY[MAXBULLET];

typedef struct { // bot pos.
	double x, y, r, g, b;
	int score, num, visiB, Body, Hs;
}bot_t;
bot_t BOT[MAXBULLET];

void calBotFunc(); //PROTOTYPE

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

	gameTime = 20.00;
	numberofBullet = 30;
	resume = 1;

	bulletX[bulletCounter] = notifCount = gameScore = loadingTime = gameHs = dotCounter = bulletCounter = gunY = accuracy = gameTarget = 0;

	spawnBullet = checkTimer = controlSpeed = accuracyBool = false;
	init = spawnBot = botMovement = true;
	
	for(int i=0; i <= MAXBULLET; i++)
		BOT[i] = {0};

	calBotFunc(); //call the func

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

void gunColor() {

	glColor3f(0, 0, 0);

	glBegin(GL_QUADS);
	glVertex2f(-410, 25 + gunY);
	glVertex2f(-395, -40 + gunY);
	glVertex2f(-415, -45 + gunY);
	glVertex2f(-435, 25 + gunY);
	glEnd();
	glBegin(GL_QUADS);
	glVertex2f(-500, 59 + gunY);
	glVertex2f(-400, 59 + gunY);
	glVertex2f(-400, 25 + gunY);
	glVertex2f(-490, 25 + gunY);
	glEnd();
	glBegin(GL_QUADS);
	glVertex2f(-510, 50 + gunY);
	glVertex2f(-500, 60 + gunY);
	glVertex2f(-490, 25 + gunY);
	glVertex2f(-510, 25 + gunY);
	glEnd();
	glBegin(GL_QUADS);
	glVertex2f(-351, 52 + gunY);
	glVertex2f(-320, 52 + gunY);
	glVertex2f(-298, 25 + gunY);
	glVertex2f(-332, 25 + gunY);
	glEnd();
	glBegin(GL_QUADS);
	glVertex2f(-310, 36 + gunY);
	glVertex2f(-245, 36 + gunY);
	glVertex2f(-245, 24 + gunY);
	glVertex2f(-310, 24 + gunY);
	glEnd();
	glBegin(GL_QUADS);
	glVertex2f(-265, 55 + gunY);
	glVertex2f(-255, 55 + gunY);
	glVertex2f(-255, 24 + gunY);
	glVertex2f(-280, 24 + gunY);
	glEnd();

	///////////other color/////////////
	glColor3ub(139, 69, 19);

	glBegin(GL_QUADS);
	glVertex2f(-475, 25 + gunY);
	glVertex2f(-485, -20 + gunY);
	glVertex2f(-510, -15 + gunY);
	glVertex2f(-500, 25 + gunY);
	glEnd();
	glBegin(GL_QUADS);
	glVertex2f(-595, 50 + gunY);
	glVertex2f(-544, 55 + gunY);
	glVertex2f(-510, 20 + gunY);
	glVertex2f(-595, -10 + gunY);
	glEnd();
	glBegin(GL_QUADS);
	glVertex2f(-540, 47 + gunY);
	glVertex2f(-510, 50 + gunY);
	glVertex2f(-500, 25 + gunY);
	glVertex2f(-535, 12 + gunY);
	glEnd();
	glBegin(GL_QUADS);
	glVertex2f(-400, 59 + gunY);
	glVertex2f(-355, 59 + gunY);
	glVertex2f(-330, 25 + gunY);
	glVertex2f(-400, 25 + gunY);
	glEnd();
}

void drawGun() {

	//GUN
	glLineWidth(3);
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_STRIP);
	glVertex2f(-595, 50 + gunY);
	glVertex2f(-545, 55 + gunY);
	glVertex2f(-535, 47 + gunY);
	glVertex2f(-510, 50 + gunY);
	glVertex2f(-500, 60 + gunY);
	glVertex2f(-355, 60 + gunY);
	glVertex2f(-350, 53 + gunY);
	glVertex2f(-320, 53 + gunY);
	glVertex2f(-305, 35 + gunY);
	glVertex2f(-275, 35 + gunY);
	glVertex2f(-265, 55 + gunY);
	glVertex2f(-255, 55 + gunY);
	glVertex2f(-255, 35 + gunY);
	glVertex2f(-243, 35 + gunY);
	glVertex2f(-243, 25 + gunY);
	glVertex2f(-410, 25 + gunY);
	glVertex2f(-435, 25 + gunY);
	glVertex2f(-410, 25 + gunY);
	glVertex2f(-395, -40 + gunY);
	glVertex2f(-415, -45 + gunY);
	glVertex2f(-435, 25 + gunY);
	glVertex2f(-475, 25 + gunY);
	glVertex2f(-485, -20 + gunY);
	glVertex2f(-510, -15 + gunY);
	glVertex2f(-500, 25 + gunY);
	glVertex2f(-595, -10 + gunY);
	glVertex2f(-595, 50 + gunY);
	glEnd();

	//trigger
	glBegin(GL_LINE_STRIP);
	glVertex2f(-472, 25 + gunY);
	glVertex2f(-472, 8 + gunY);
	glVertex2f(-445, 8 + gunY);
	glVertex2f(-445, 25 + gunY);
	glEnd();

	//trigger-2
	glLineWidth(2);
	glBegin(GL_LINE_STRIP);
	glVertex2f(-472, 25 + gunY);
	glVertex2f(-462, 15 + gunY);
	glVertex2f(-462, 25 + gunY);
	glEnd();

	//notch
	glBegin(GL_LINE_STRIP);
	glVertex2f(-401, 60 + gunY);
	glVertex2f(-401, 64 + gunY);
	glVertex2f(-394, 64 + gunY);
	glVertex2f(-394, 60 + gunY);
	glEnd();

	glRectf(-410, 63 + gunY, -385, 60 + gunY);

	gunColor();

}

void drawBullet(double x, double y) {

	glColor3ub(75, 75, 75);
	glRectf(-260 + x, 33 + y, -245 + x, 27 + y);
	glColor3ub(255, 165, 0);
	glBegin(GL_TRIANGLES);
	glVertex2f(-245 + x, 35 + y);
	glVertex2f(-245 + x, 25 + y);
	glVertex2f(-239 + x, 30 + y);
	glEnd();
}

void drawBots(double x, double y, int i) { //random pos, random color, random score(hs-body)

	glColor4f(0, 0, 0, BOT[i].visiB);
	circle(75 + x, -100 + y, 35); //FACE

	glColor4f(1, 1, 1, BOT[i].visiB);
	circle(90 + x, -90 + y, 5);	//LEFT EYE
	circle(60 + x, -90 + y, 5);	//RIGHT EYE
	glColor4f(0, 0, 0, BOT[i].visiB);
	circle(90 + x, -90 + y, 2.5);	//LEFT EYE-2
	circle(60 + x, -90 + y, 2.5);	//RIGHT EYE-2

	//NOSE
	glColor4f(0, 1, 0, BOT[i].visiB);
	glBegin(GL_TRIANGLES);
	glVertex2f(70 + x, -110 + y);
	glVertex2f(80 + x, -110 + y);
	glVertex2f(75 + x, -90 + y);
	glEnd();
	//NOSE

	glColor4ub(204, 0, 0, BOT[i].visiB);
	glRectf(60 + x, -115 + y, 90 + x, -125 + y);//MOUTH

	glColor4ub(BOT[i].r, BOT[i].g, BOT[i].b, BOT[i].visiB);
	glRectf(35 + x, -135 + y, 115 + x, -225 + y);//BODY - Only bots body will be changed by random RGB values.

	//checkHitBox // (will be displayed)
	glColor4f(0, 0, 0, BOT[i].visiB);
	glLineWidth(1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(28 + x, -53 + y);
	glVertex2f(28 + x, -237 + y);
	glVertex2f(122 + x, -237 + y);
	glVertex2f(122 + x, -53 + y);
	glEnd();

	//faceHit
	glColor4f(0, 0, 0, BOT[i].visiB);
	glLineWidth(1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(40 + x, -65 + y);
	glVertex2f(40 + x, -135 + y);
	glVertex2f(110 + x, -135 + y);
	glVertex2f(110 + x, -65 + y);
	glEnd();
	//checkHitBox

		/////////////////HS AND TARGET HIT TEXTS////////////////////
	glColor4f(1, 1, 1, BOT[i].visiB);
	vprint2(70 + x, -180 + y, 0.11, "%d", BOT[i].num);

	glLineWidth(3);
	if (BOT[i].Body == 1 ) {
		BOT[i].Hs = 0;
		if (notifCount == 0)
			BOT[i].Body = 0;
		glColor3ub(255, 128, 0);
		vprint2(55 + x, -50 + y, 0.2, "%d", BOT[i].score);
	}
	else if (BOT[i].Hs == 1) {
		BOT[i].Body = 0;
		if (notifCount == 0)
			BOT[i].Hs = 0;
		glColor3ub(204, 0, 102);
		vprint2(20 + x, -50 + y, 0.25, "%d HS", BOT[i].score);
	}
	/////////////////HS AND TARGET HIT TEXTS////////////////////
}

void showMenu() {

	glClearColor(32.0 / 255.0, 32.0 / 255.0, 32.0 / 255.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3ub(50, 150, 250); // nameSurname
	vprint(-(winWidth / 2) + 25, (winHeight / 2) - 20, GLUT_BITMAP_8_BY_13, "Murat Alkan");
	vprint(-(winWidth / 2) + 11.5, (winHeight / 2) - 40, GLUT_BITMAP_8_BY_13, "-Assignment 2-");

	glColor3ub(255, 100, 0);
	glLineWidth(2);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-(winWidth / 2) + 10, (winHeight / 2) - 5);
	glVertex2f(-(winWidth / 2) + 125, (winHeight / 2) - 5);
	glVertex2f(-(winWidth / 2) + 125, (winHeight / 2) - 50);
	glVertex2f(-(winWidth / 2) + 10, (winHeight / 2) - 50);
	glEnd();

	glColor3ub(0, 0, 75); // menuRect
	glRectf(-200, 200, 200, -150);

	glColor3f(1, 1, 0);
	glLineWidth(3);
	vprint2(-143, 100, 0.3, "Shooting Range");

	glBegin(GL_LINES);
	glVertex2f(-80, 80);
	glVertex2f(85, 80);
	glEnd();

	glColor3f(0, 0, 0);
	glLineWidth(3);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-110, 50);
	glVertex2f(112, 50);
	glVertex2f(112, -85);
	glVertex2f(-110, -85);
	glEnd();

	glColor3f(1, 1, 1); // Buttons
	vprint2(-45, 10, 0.25, "START");
	vprint2(-50, -30, 0.25, "CREDIT");
	vprint2(-27, -70, 0.25, "EXIT");

	glColor3ub(0, 255, 0);//Hints
	vprint((winWidth / 2) - 170, -(winHeight / 2) + 50, GLUT_BITMAP_8_BY_13, "<Enter>  : start");
	vprint((winWidth / 2) - 170, -(winHeight / 2) + 35, GLUT_BITMAP_8_BY_13, "<C>      : credit");
	vprint((winWidth / 2) - 170, -(winHeight / 2) + 20, GLUT_BITMAP_8_BY_13, "<ESC>    : exit");

}

void infoBoxes() {

	glColor3f(1, 1, 1);
	glRectf(-(winWidth / 2) + 180, -(winHeight / 2) + 50, -(winWidth / 2) + 325, -(winHeight / 2) + 25); //background for timer

	glColor3f(0, 0, 0);
	vprint2(-(winWidth / 2) + 185, -(winHeight / 2) + 30, 0.15, "Timer : %4.2f", gameTime);

	glColor3f(0, 0, 0);
	glRectf(-(winWidth / 2), -(winHeight / 2) + 80, -(winWidth / 2) + 180, -(winHeight / 2)); //background score, etc.

	glColor3ub(50, 150, 255);
	vprint2(-(winWidth / 2) + 10, -(winHeight / 2) + 55, 0.15, "Target   : %d", gameTarget);
	glColor3f(0, 1, 0);
	vprint2(-(winWidth / 2) + 10, -(winHeight / 2) + 30, 0.15, "Score   : %d", gameScore);
	glColor3f(1, 0, 0);
	vprint2(-(winWidth / 2) + 10, -(winHeight / 2) + 5, 0.15, "Headshot : %d", gameHs);

	glColor3f(0, 0, 0);
	glLineWidth(2);
	glBegin(GL_LINE_STRIP);
	glVertex2f(-(winWidth / 2), (winHeight / 2) + -50);
	glVertex2f(-(winWidth / 2) + 140, (winHeight / 2) + -50);
	glVertex2f(-(winWidth / 2) + 140, (winHeight / 2));
	glEnd();

	glColor3f(1, 1, 0);
	vprint2(-(winWidth / 2) + 5, (winHeight / 2) + -20, 0.115, "Gun : AK-47");
	glColor3f(1, 1, 1);
	vprint2(-(winWidth / 2) + 5, (winHeight / 2) + -40, 0.115, "Mode : %s", gunMode);

	if (numberofBullet == 0) {
		glColor3f(1, 0, 0);
		vprint2(-(winWidth / 2) + 155, (winHeight / 2) + -30, 0.13, "%2d/0", numberofBullet);
	}
	else {
		glColor3f(0, 0, 1);
		vprint2(-(winWidth / 2) + 150, (winHeight / 2) + -30, 0.13, "%2d/%2d", numberofBullet, MAXBULLET - 1);
	}

	glColor3ub(50, 50, 50);
	vprint((winWidth / 2) - 275, -(winHeight / 2) + 90, GLUT_BITMAP_8_BY_13, "<SPACE>          : start & shoot");
	vprint((winWidth / 2) - 275, -(winHeight / 2) + 72, GLUT_BITMAP_8_BY_13, "<UP> & <DOWN>    : move the gun");
	vprint((winWidth / 2) - 275, -(winHeight / 2) + 55, GLUT_BITMAP_8_BY_13, "<LEFT> & <RIGHT> : bot speed");
	vprint((winWidth / 2) - 275, -(winHeight / 2) + 39, GLUT_BITMAP_8_BY_13, "<F1>             : pause & resume");
	vprint((winWidth / 2) - 275, -(winHeight / 2) + 22, GLUT_BITMAP_8_BY_13, "<F5>             : retry");
	vprint((winWidth / 2) - 275, -(winHeight / 2) + 5, GLUT_BITMAP_8_BY_13, "<ESC>            : menu");

}

void showRun() {

	glClearColor(205.0 / 255.0, 185.0 / 255.0, 156.0 / 255.0, 1.0f); //dust
	glClear(GL_COLOR_BUFFER_BIT);

	if (strcmp(gunMode, "SINGLE") == 0 && spawnBullet)
		drawBullet(bulletX[bulletCounter], bulletY[bulletCounter]); // if press spacebar

	if (spawnBot == true)
		for (int i = 0; i < MAXBOT; i++)
			drawBots(BOT[i].x, BOT[i].y, i);

	drawGun();
	infoBoxes();
}

void showLoad() {

	glClearColor(32.0 / 255.0, 32.0 / 255.0, 32.0 / 255.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1, 1, 1);
	vprint2(-180, 0, 0.5, "INITIALIZING");
	glColor3f(1, 1, 0);
	for (int i = 0; i < dotCounter; i++)
		vprint2(-57 + i * 30, -50, 0.3, "*");

}

void showCredits() {

	glClearColor(150.0 / 255.0, 150.0 / 255.0, 255.0 / 255.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1, 1, 0);
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


	glColor3ub(0, 0, 0);
	vprint((winWidth / 2) - 140, -(winHeight / 2) + 20, GLUT_BITMAP_8_BY_13, "<ESC>   : menu");

}

void showOver() {

	glClearColor(32.0 / 255.0, 32.0 / 255.0, 32.0 / 255.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1, 1, 1);
	glLineWidth(4);
	vprint2(-250, 115, 0.7, "TIME'S UP!");

	glColor3f(1, 1, 0);
	glRectf(-200, 90, 175, -150);

	glLineWidth(3);
	glColor3f(0, 0, 0);
	vprint2(-180, 50, 0.3, "Score   : %d", gameScore);
	vprint2(-180, -5, 0.3, "Target   : %d", gameTarget);
	vprint2(-180, -60, 0.3, "Headshot : %d", gameHs);
	vprint2(-180, -120, 0.3, "Accuracy : %d%%", (accuracy * 100) / (MAXBULLET - 1 - numberofBullet));

	glColor3f(1, 1, 1);
	vprint((winWidth / 2) - 170, -(winHeight / 2) + 30, GLUT_BITMAP_8_BY_13, "<F5>   : retry");
	vprint((winWidth / 2) - 170, -(winHeight / 2) + 15, GLUT_BITMAP_8_BY_13, "<ESC>  : menu");
}

void display() {

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

void onKeyDown(unsigned char key, int x, int y)
{

	if (key == 27 && State == MENU)// ESC-Exit (only in menu)
		exit(0);
	if (key == 27 && State == RUN) 
		State = MENU;	
	if (key == 13 && State == MENU)
		State = LOAD;
	if (State == MENU)
		if (key == 'C' || key == 'c')
			State = CREDITS;
	if (key == 27 && State == CREDITS)
		State = MENU;
	if (key == 27 && State == OVER) 
		State = MENU;
	
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	if (resume == 1) {

		if (key == ' ' && State == RUN && numberofBullet > 0) // space
		{
			spawnBullet = true;
			checkTimer = true;

			if (bulletX[bulletCounter] == 0)
				bulletY[bulletCounter] = gunY;

			if (spawnBullet) {
				if (bulletX[bulletCounter] > -50 && bulletX[bulletCounter] < 50) {
					numberofBullet--;
					if (numberofBullet == -1)
						spawnBullet = false;
				}
			}
		}
	}

	glutPostRedisplay();
}

void onSpecialKeyDown(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP:
		if (State == RUN && gunY < 180 && resume) // will not moved behind the information boxes.
				gunY += 10;	
		break;
	case GLUT_KEY_DOWN:
		if (State == RUN && gunY > -180 && resume) 
				gunY -= 10;
		break;
	case GLUT_KEY_LEFT:
		controlSpeed = false;
		break;

	case GLUT_KEY_RIGHT:
		controlSpeed = true;
		break;
	}

}

void onSpecialKeyUp(int key, int x, int y)
{

	if (key == GLUT_KEY_F5 && State == OVER) 
		State = LOAD;
	if (key == GLUT_KEY_F5 && State == RUN && checkTimer) 
		State = LOAD;
	
	if (key == GLUT_KEY_F1 && State == RUN && checkTimer && resume == 1) {
		resume = 0;
		checkTimer = false;		botMovement = false;
	}
	else if (key == GLUT_KEY_F1 && State == RUN && !checkTimer && resume == 0) {
		resume = 1;
		checkTimer = true;		botMovement = true;
	}

	glutPostRedisplay();
}

void calBulletFunc() {

	if (State == RUN && checkTimer) {

		if (bulletX[bulletCounter] >= 840) {
			if (accuracyBool )
				accuracy++;
			spawnBullet = false;
			accuracyBool = false;
			bulletX[bulletCounter] = 0;
		}
		if (strcmp(gunMode, "SINGLE") == 0 && spawnBullet)
			bulletX[bulletCounter] += 15;
	}
}

void calGunFunc() {

	if (State == RUN && spawnBullet && checkTimer) {

		if (strcmp(gunMode, "SINGLE") == 0) {

			for (int i = 0; i < MAXBOT; i++) {
				if (bulletX[bulletCounter] >= 274 + BOT[i].x &&  bulletX[bulletCounter] <= 289 + BOT[i].x)
				{
					if (bulletY[bulletCounter] <=  -160 + BOT[i].y && bulletY[bulletCounter] >= -250 + BOT[i].y) { // BODY PART
						BOT[i].visiB = 0;
						BOT[i].score = rand() % 19 + 1;//normal points / between 1-20
						BOT[i].Body = 1;
						gameScore += BOT[i].score;
						notifCount = 25;
						gameTarget++;
						accuracyBool = true;
					}
					else if (bulletY[bulletCounter] <=  -90 + BOT[i].y && bulletY[bulletCounter] >= -160 + BOT[i].y) { // HS PART
						BOT[i].visiB = 0;
						BOT[i].score = rand() % 79 + 21; // extra hs points / 20 - 100 
						BOT[i].Hs = 1;
						gameScore += BOT[i].score;
						notifCount = 25;
						gameTarget++;
						gameHs++;
						accuracyBool = true;
					}
				}
			}
		}
	}
}

void calBotFunc() {

	if (State == RUN && resume == 1) {

		for (int m = 0; m < MAXBOT; m++) {

			if(controlSpeed)
				BOT[m].y += 8.0;
			else
				BOT[m].y += 5.0;

			if (BOT[m].y >= 552.0 || init) {
				BOT[m] = { {rand() % 15 + (115.0 * m)}, {(rand() % 225 + 350.0) * (-1)}, //Positions
				{rand() % 256*1.0}, {rand() % 256*1.0}, {rand() % 256*1.0}, //colors
				0, {rand() % 4 + 1}, 255, 0, 0 }; 
				notifCount = -5; 
			}
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
		dotCounter = (dotCounter + 1) % 4;
		loadingTime += 0.5;
		if (loadingTime >= 25) {
			Init_Globals(); State = RUN;
		}
	}

	if (State == RUN && checkTimer && gameTime != 0) {
		gameTime -= 0.0165;
		if (gameTime < 0) {
			gameTime = 0;
			State = OVER;
			checkTimer = false;
		}
	}

	////////////////////////////////////////////////////// call the functions //////////////////////////////////////////////////////
		calGunFunc();	
			calBulletFunc();
				calBotFunc();	init = false;
	////////////////////////////////////////////////////// call the functions //////////////////////////////////////////////////////
			
				if (notifCount > -5 && resume) 
					notifCount -= 1;
				
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

void main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("ShootingRange | MURAT ALKAN");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);
	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	srand(time(NULL));
	glutTimerFunc((1000 / FPS), onTimer, 0);

	Init();
	glutMainLoop();
}