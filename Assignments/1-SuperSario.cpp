#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define WINDOW_WIDTH 750
#define WINDOW_HEIGHT 500

#define FPS 60

#define LOGO 0
#define MENU 1
#define LOAD 2
#define RUN 3
#define CREDITS 4
#define OVER 5
int State = LOGO; // initial Scene

#define MAXCHAR 11 //MAX CHARACTER

int winWidth, winHeight, we, he; //current Window width and height
int dotCounter, characterNum ;
double loadingTime, logoTime = 0;
double mX, mY; // mouseX - mouseY
double sunSpX, clSpX[MAXCHAR]; //Sun speed / Clouds speed
bool characterStatus, control2Xspeed;

typedef struct {
	double x1, x2, y1, y2;
} hitbox_t;

typedef struct {
	float x, y; // characterX - characterY
	int speed;
	hitbox_t hit;
} character_t;
character_t CHARACTER[MAXCHAR];

void circle(int x, int y, int r) {
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++) {
		angle = 2 * PI*i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void vprint(int x, int y, void *font, const char *string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++) {
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
	for (i = 0; i < len; i++) {
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}

void Init_Globals() {

	loadingTime = characterNum = dotCounter = sunSpX = 0;
	for (int i = 0; i <= MAXCHAR; i++) {
		CHARACTER[i] = { 0 };
		clSpX[i] = { 0 };
	}

	characterStatus = control2Xspeed = false;

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

void characterModel(double chX, double chY, int i) {

	CHARACTER[i].hit = { chX - 150, chX + 125, chY + 60, chY - 60 };

	//head
	glColor3f(0, 0, 0);
	circle(chX, chY, 25);
	glColor3f(1, 1, 1);
	vprint(chX - 7, chY - 3, GLUT_BITMAP_8_BY_13, "%2d", i); // print characterNum on head

	//body
	glColor3ub(75, 25, 0);
	glRectf(chX - 75, chY + 17, chX - 25, chY - 20);

	//legs
	glColor3f(0, 0, 0);
	glRectf(chX - 120, chY + 10, chX - 75, chY + 5);
	glRectf(chX - 120, chY - 10, chX - 75, chY - 15);

	//hat
	glColor3f(0, 0, 0);
	glLineWidth(2);
	glBegin(GL_LINES);
	glVertex2f(chX + 20, chY + 10);
	glVertex2f(chX + 33, chY + 10);
	glVertex2f(chX + 20, chY - 10);
	glVertex2f(chX + 33, chY - 10);
	glEnd();

	glPointSize(5);
	glBegin(GL_POINTS);
	glVertex2f(chX + 35, chY + 10);
	glVertex2f(chX + 35, chY - 10);
	glEnd();

	//shoes
	glLineWidth(2);
	glBegin(GL_LINE_LOOP);
	glVertex2f(chX - 130, chY + 17);
	glVertex2f(chX - 120, chY + 17);
	glVertex2f(chX - 120, chY + 6);
	glVertex2f(chX - 130, chY + 6);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex2f(chX - 130, chY - 11);
	glVertex2f(chX - 120, chY - 11);
	glVertex2f(chX - 120, chY - 22);
	glVertex2f(chX - 130, chY - 22);
	glEnd();

	//hand
	glLineWidth(2);
	glBegin(GL_LINE_STRIP); //left
	glVertex2f(chX - 32, chY + 15);
	glVertex2f(chX - 32, chY + 30);
	glVertex2f(chX - 90, chY + 30);
	glEnd();

	glBegin(GL_LINES); //right
	glVertex2f(chX - 32, chY - 20);
	glVertex2f(chX + 50, chY - 30);
	glVertex2f(chX - 50, chY - 20);
	glVertex2f(chX + 50, chY - 35);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex2f(chX + 50, chY - 27);
	glVertex2f(chX + 60, chY - 27);
	glVertex2f(chX + 60, chY - 38);
	glVertex2f(chX + 50, chY - 38);
	glEnd();

	//cape
	glColor3f(0.8, 0, 0);
	glBegin(GL_TRIANGLES);
	glVertex2f(chX - 20, chY + 0);
	glVertex2f(chX - 90, chY + 35);
	glVertex2f(chX - 90, chY - 35);
	glEnd();

	// letter S
	glLineWidth(2);
	glColor3f(1, 1, 1);
	glBegin(GL_LINE_STRIP);
	glVertex2f(chX - 55, chY - 7);
	glVertex2f(chX - 55, chY + 7);
	glVertex2f(chX - 70, chY - 7);
	glVertex2f(chX - 75, chY - 7);
	glVertex2f(chX - 75, chY + 7);
	glEnd();

}

void sunClouds() {

	//ANIMATION
	if (-365 + sunSpX > 375)
		sunSpX = -100;
	if (-215 + clSpX[1] > 375)
		clSpX[1] = -280;
	if (20 + clSpX[2] > 375)
		clSpX[2] = -515;
	if (240 + clSpX[3] > 375)
		clSpX[3] = -735;

	//ANIMATION
	glColor3ub(255, 255, 0); //sun
	circle(-330 + sunSpX, 205, 35);

	//sun eyes
	glColor3f(1, 1, 1); //white eye
	circle(-342 + sunSpX, 215, 5);
	circle(-317 + sunSpX, 215, 5);

	glColor3f(0, 0, 0); //black pupil
	circle(-342 + sunSpX, 215, 2);
	circle(-317 + sunSpX, 215, 2);

	glColor3f(1, 1, 1); // white mouth
	glBegin(GL_POLYGON);
	glVertex2f(-345 + sunSpX, 192);
	glVertex2f(-330 + sunSpX, 185);
	glVertex2f(-315 + sunSpX, 192);
	glEnd();

	//CLOUD 1
	glColor3f(1, 1, 1);
	circle(-125 + clSpX[1], 150, 30);
	circle(-180 + clSpX[1], 150, 30);
	circle(-150 + clSpX[1], 155, 33);
	circle(-150 + clSpX[1], 145, 33);
	//CLOUD 2
	circle(110 + clSpX[2], 110, 30);
	circle(55 + clSpX[2], 110, 30);
	circle(85 + clSpX[2], 115, 33);
	circle(85 + clSpX[2], 105, 33);
	//CLOUD 3
	circle(330 + clSpX[3], 200, 30);
	circle(275 + clSpX[3], 200, 30);
	circle(305 + clSpX[3], 205, 33);
	circle(305 + clSpX[3], 195, 33);
}

void greenPipes() {

	glColor3ub(0, 255, 0); // green pipes
	glRectf(-(winWidth / 2), 0, -280, -60); //pipeLeft
	glRectf(-300, 10, -250, -70); //headerLeft

	glColor3f(0, 0, 0); //pipeLeftShadow
	glBegin(GL_LINES);
	glVertex2f(-(winWidth / 2), 0);
	glVertex2f(-300, 0);
	glVertex2f(-(winWidth / 2), -60);
	glVertex2f(-300, -60);
	glEnd();

	glBegin(GL_LINE_LOOP);//headerLeftShadow
	glVertex2f(-300, 10);
	glVertex2f(-250, 10);
	glVertex2f(-250, -70);
	glVertex2f(-300, -70);
	glEnd();

	glColor3ub(0, 255, 0); // green pipes
	glRectf(280, 0, (winWidth / 2), -60); //pipeRight
	glRectf(300, 10, 250, -70);//headerRight

	glColor3f(0, 0, 0); //pipeRightShadow
	glBegin(GL_LINES);
	glVertex2f((winWidth / 2), 0);
	glVertex2f(300, 0);
	glVertex2f((winWidth / 2), -60);
	glVertex2f(300, -60);
	glEnd();

	glBegin(GL_LINE_LOOP);//headerRightShadow
	glVertex2f(300, 10);
	glVertex2f(250, 10);
	glVertex2f(250, -70);
	glVertex2f(300, -70);
	glEnd();

}

void display_logo() {

	glClearColor(192.0 / 255.0, 192.0 / 255.0, 192.0 / 255.0, 1.0f); //gray
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0, 0, 0);
	circle(0, 0, 125);

	glColor3f(1, 1, 1);
	glLineWidth(4);
	vprint2(-85, -45, 1, "MA");// (M)urat (A)lkan
}

void display_load() {  //LOAD

	glClearColor(32.0 / 255.0, 32.0 / 255.0, 32.0 / 255.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3ub(0, 255, 0);

	vprint2(-100, -25, 0.4, "LOADING");
	for (int i = 0; i < dotCounter; i++)
		vprint2(-40 + i * 30, -70, 0.3, "-");

}

void display_menu() { //MENU

	glClearColor(32.0 / 255.0, 32.0 / 255.0, 32.0 / 255.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3ub(50, 150, 250); // nameSurname
	vprint(-(winWidth / 2) + 25, (winHeight / 2) - 20, GLUT_BITMAP_8_BY_13, "Murat Alkan");
	vprint(-(winWidth / 2) + 11.5, (winHeight / 2) - 40, GLUT_BITMAP_8_BY_13, "-Assignment 1-");

	glColor3ub(255, 100, 0);
	glLineWidth(2);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-(winWidth / 2) + 10, (winHeight / 2) - 5);
	glVertex2f(-(winWidth / 2) + 125, (winHeight / 2) - 5);
	glVertex2f(-(winWidth / 2) + 125, (winHeight / 2) - 50);
	glVertex2f(-(winWidth / 2) + 10, (winHeight / 2) - 50);
	glEnd();

	glColor3ub(50, 0, 100); // menuCircle
	circle(0, 25, 175);

	glColor3f(1, 1, 0);
	glLineWidth(4);
	vprint(-50, 160, GLUT_BITMAP_TIMES_ROMAN_24, "SuperSario");

	glBegin(GL_LINES);
	glVertex2f(-60, 150);
	glVertex2f(65, 150);
	glEnd();

	vprint2(-85, 80, 0.5, "MENU");

	glColor3f(0, 0, 0);
	glLineWidth(3);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-110, 60);
	glVertex2f(110, 60);
	glVertex2f(110, -75);
	glVertex2f(-110, -75);
	glEnd();

	glColor3f(1, 1, 1); // Buttons
	vprint2(-90, 20, 0.25, "CHALLENGE");
	vprint2(-55, -20, 0.25, "CREDIT");
	vprint2(-35, -60, 0.25, "EXIT");

	glColor3ub(0, 255, 0);//Hints
	vprint((winWidth / 2) - 170, -(winHeight / 2) + 50, GLUT_BITMAP_8_BY_13, "<Space>  : challenge");
	vprint((winWidth / 2) - 170, -(winHeight / 2) + 35, GLUT_BITMAP_8_BY_13, "<C>      : credit");
	vprint((winWidth / 2) - 170, -(winHeight / 2) + 20, GLUT_BITMAP_8_BY_13, "<ESC>    : exit");

}

void displayChallengeScene() { //RUN-CHALLENGE

	glClearColor(50.0 / 255.0, 150.0 / 255.0, 250.0 / 255.0, 1.0f); //blue background 
	glClear(GL_COLOR_BUFFER_BIT);

	sunClouds();

	if (characterStatus)
		for (int i = 1; i <= characterNum; i++)
			characterModel(CHARACTER[i].x, CHARACTER[i].y, i);

	greenPipes();

	glColor3ub(100, 50, 0);
	glRectf(-(winWidth / 2), -(winHeight / 2) + 80, (winWidth / 2), -(winHeight / 2)); // brown road - characters will not be spawned behind this object.

								  ///////////////////// INFORMATIONS ////////////////////////////
	glColor3f(1, 1, 0); // white
	vprint(-(winWidth / 2) + 20, -(winHeight / 2) + 55, GLUT_BITMAP_8_BY_13, "Number of Characters: %d/10", characterNum);

	if (characterNum > 9)
		vprint(-(winWidth / 2) + 204, -(winHeight / 2) + 68, GLUT_BITMAP_TIMES_ROMAN_10, "MAX");

	glColor3f(1, 1, 1); // yellow
	vprint(-(winWidth / 2) + 5, -(winHeight / 2) + 13, GLUT_BITMAP_HELVETICA_12, "Mouse Position: "); ////OPENGL coordinates
	vprint(-(winWidth / 2) + 105, -(winHeight / 2) + 22, GLUT_BITMAP_HELVETICA_12, "x = %.f", mX);
	vprint(-(winWidth / 2) + 105, -(winHeight / 2) + 5, GLUT_BITMAP_HELVETICA_12, "y = %.f", mY);

	glColor3ub(150, 150, 150); //gray
	vprint((winWidth / 2) - 220, -(winHeight / 2) + 65, GLUT_BITMAP_8_BY_13, "<MOUSE1>      : create");
	vprint((winWidth / 2) - 220, -(winHeight / 2) + 49, GLUT_BITMAP_8_BY_13, "<MOUSE2>      : move & stop");
	vprint((winWidth / 2) - 220, -(winHeight / 2) + 34, GLUT_BITMAP_8_BY_13, "<UP> & <DOWN> : speed");
	vprint((winWidth / 2) - 220, -(winHeight / 2) + 19, GLUT_BITMAP_8_BY_13, "<F5>          : retry");
	vprint((winWidth / 2) - 220, -(winHeight / 2) + 3, GLUT_BITMAP_8_BY_13, "<ESC>         : over");
}

void displayCreditsScene() { //CREDITS

	glClearColor(175.0 / 255.0, 175.0 / 255.0, 100.0 / 255.0, 1.0f);
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

void displayOver() { //OVER

	glClearColor(32.0 / 255.0, 32.0 / 255.0, 32.0 / 255.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//circle
	glColor3ub(255, 0, 0);
	circle(0, 25, 175);

	glColor3f(1, 1, 1);
	glLineWidth(4);
	vprint2(-100, -7, 0.6, "OVER");

	glColor3f(1, 1, 1);
	vprint((winWidth / 2) - 135, -(winHeight / 2) + 30, GLUT_BITMAP_8_BY_13, "<F5>    : retry");
	vprint((winWidth / 2) - 135, -(winHeight / 2) + 15, GLUT_BITMAP_8_BY_13, "<ESC>   : menu");
}

void display() {

	switch (State) {
	case LOGO: display_logo(); break;
	case MENU: display_menu(); break;
	case LOAD: display_load(); break;
	case RUN: displayChallengeScene(); break;
	case CREDITS: displayCreditsScene(); break;
	case OVER: displayOver(); break;
	}

	glutSwapBuffers();
}

void onKeyDown(unsigned char key, int x, int y)
{

	if (key == 27 && State == MENU)
		exit(0);
	if (key == 27 && State == RUN) 
		State = OVER;
	else if (key == 27 && State == OVER)
		State = MENU;
	if (key == ' ' && State == MENU)
		State = LOAD;
	if (State == MENU)
		if (key == 'C' || key == 'c')
			State = CREDITS;
	if (key == 27 && State == CREDITS)
		State = MENU;

}

void onSpecialKeyDown(int key, int x, int y)
{
	if (key == GLUT_KEY_F5 && State == RUN || State == OVER) 
		State = LOAD;

	for (int i = 1; i <= characterNum; i++) {
		if (State == RUN && characterStatus && CHARACTER[i].speed == 1) {
			if (key == GLUT_KEY_UP)
				control2Xspeed = true;
			else if (key == GLUT_KEY_DOWN)
				control2Xspeed = false;
		}
	}
}

void onClick(int button, int stat, int x, int y) {

	double mouX = x - winWidth / 2, mouY = winHeight / 2 - y;
	bool positionStatus = true;

	if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN & State == RUN && characterNum < MAXCHAR - 1) {

		for (int i = 1; i < MAXCHAR - 1; i++)
			if ((mouX > CHARACTER[i].hit.x1 && mouX < CHARACTER[i].hit.x2 && mouY < CHARACTER[i].hit.y1 && mouY > CHARACTER[i].hit.y2))  // inside
				positionStatus = false;

		if (positionStatus && !(mouY > -250 && mouY < -130)) { // do not spawn behind the brown road
			characterNum++;
			CHARACTER[characterNum].x = mouX;
			CHARACTER[characterNum].y = mouY;
			characterStatus = true;
		}
	}

	if (button == GLUT_RIGHT_BUTTON && stat == GLUT_DOWN && State == RUN && characterStatus) {

		for (int i = 1; i <= characterNum; i++) {

			if (CHARACTER[i].speed == 0) {
				if ((mouX > CHARACTER[i].hit.x1 + 60 && mouX < CHARACTER[i].hit.x2 - 100 && mouY < CHARACTER[i].hit.y1 - 40 && mouY > CHARACTER[i].hit.y2 + 40))
					CHARACTER[i].speed = 1;
			}

			else if (CHARACTER[i].speed == 1) {
				if ((mouX > CHARACTER[i].hit.x1 + 60 && mouX < CHARACTER[i].hit.x2 - 100 && mouY < CHARACTER[i].hit.y1 - 40 && mouY > CHARACTER[i].hit.y2 + 40))
					CHARACTER[i].speed = 0;
			}
		}
	}

	glutPostRedisplay();
}

void onMove(int x, int y) { //OPENGL Position

	mX = x - winWidth / 2;
	mY = winHeight / 2 - y;

	glutPostRedisplay();
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
		if (loadingTime >= 30) {
			Init_Globals();		State = RUN;
		}
		
	}

	if (State == RUN) {
		sunSpX += 0.15;
		clSpX[1] += 0.25; clSpX[2] += 0.25; clSpX[3] += 0.25;
	}

	if (State == RUN && characterStatus)
	{
		float speedX = 0.0;

			if (control2Xspeed)
				speedX += 4.0;
			else
				speedX += 2.0;

			for (int i = 1; i <= characterNum; i++)
			{
				if (CHARACTER[i].speed == 1) //assign speeds
					(*(CHARACTER + i)).x += speedX;
				if (-200 + (*(CHARACTER + i)).x > 375)
					(*(CHARACTER + i)).x = -425;
			}
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
	display(); // refresh window.
}

void init() {

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Init_Globals();
}

void main(int argc, char *argv[]) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("SuperSario | MURAT ALKAN");

	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutMouseFunc(onClick);
	glutPassiveMotionFunc(onMove);

	init();

	glutTimerFunc((1000/FPS), onTimer, 0);

	glutMainLoop();
}