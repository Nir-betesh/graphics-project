#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"GL/glut.h"

#define PI 3.14159265359f
#define ANIMATION_DELAY 40
#define CAMERA_ROTATION_SPEED 1.5f
#define CAMERA_SPEED 0.25f
#define CAMERA_MODEL 0
#define CAMERA_FREE 1
#define BALL_SPEED 0.07f
#define CELESTIAL_SELF_SPIN 1.8f
#define CELESTIAL_SPIN 1.2f
#define TIME_DAY 0
#define TIME_NIGHT 1
#define PHASE_TO_TWILIGHT 0
#define PHASE_TO_END 1
#define PHASE_END 2
#define TIME_RATE 0.05f
#define LIGHT_HEAD GL_LIGHT0
#define LIGHT_BALL GL_LIGHT1
#define LIGHT_SUN GL_LIGHT2
#define LIGHT_STREETLAMP GL_LIGHT3
#define SWING_MAX_ANG 40

typedef struct {
	GLfloat x;
	GLfloat y;
	GLfloat z;
} vec3;

//function declerations
void drawingCB(void);
void reshapeCB(int width, int height);
void keyboardCB(unsigned char key, int x, int y);
void keyboardSpecialCB(int key, int x, int y);
void animateBouncingBall(int value);
void animateSolarSystem(int value);
void animateTimeChange(int value);
GLubyte *readBMP(char *imagepath, int *width, int *height);
void TerminationErrorFunc(char *ErrorString);
GLuint load_texture(char *name);
void VerticalCylinder(float radius, float height);
void DrawSign(void);
void DrawWindSpinner(void);
void DrawSpinner(void);
void drawBlade(void);
void update(int value);
void DrawStreetLight(void);
void DrawSwings(void);
void DrawSwing(void);
void DrawFence(vec3 fromPoint, vec3 toPoint, float poleHeight);
void DrawFlagPole(void);
void update_flag(int value);
void DrawFountain(void);
void DrawDropletsOval(void);
void update_droplets(int value);
float DrawChains(int length);
float lerp(float a, float b, float t);
float invLerp(float a, float b, float c);
void DrawSwingKid(int gender);
void DrawHead(void);
void DrawArm(void);
void Drawlimb(float radius, float height);
void DrawLeg(int gender);
void DrawBody(float radius, float height, int gender);

float time = 0;
int isBanchExist = 0, dirSwingA = 1;
float swingAng = SWING_MAX_ANG;

int FOVy = 60;
int camera_mode = CAMERA_MODEL;
int lighting = 1, head_light = 1;
float angleX = 0, angleY = 0, radius = 5;
vec3 cameraPos = { 0, 3, 5 };
vec3 cameraForward = { 0, 0, -1 };
vec3 cameraForwardXZ = { 0, 0, -1 };
vec3 cameraRight = { 1, 0, 0 };

float jointAngLeg = 30;
float jointAng;
float ballPosition = 0;
float dxBall = BALL_SPEED;
float earthAngle = 0, earthSelf = 0, sunSelf = 0, moonSelf = 0, moonAngle = 0;
float rotationAngle = 0.0f;

int timeOfDay = TIME_DAY;
int transPhase = PHASE_END;
float phase = 0;
float clearColor[] = { 135.0f / 255, 206.0f / 255, 235.0f / 255 };
GLfloat sunLightColor[] = { 1, 1, 1, 1 };
float sunRotation = 0;

GLuint ground, ball, wall, sun, moon, earth;
GLuint wood;
GLuint wood2;
GLuint wood2front;
GLuint lamp;
GLuint metal;
GLuint flag;
GLuint water;
GLuint boyFace;
float droop = 0.0;
int droop_index = 0;
float droplets_offset = -90;

int main(int argc, char **argv)
{
	//initizlizing GLUT
	glutInit(&argc, argv);

	//initializing window
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("Graphics project - Children's park");
	glClearColor(1.0, 1.0, 1.0, 0.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	glEnable(LIGHT_BALL);
	glEnable(LIGHT_STREETLAMP);

	//registering callbacks
	glutDisplayFunc(drawingCB);
	glutReshapeFunc(reshapeCB);
	glutKeyboardFunc(keyboardCB);
	glutSpecialFunc(keyboardSpecialCB);
	glutTimerFunc(ANIMATION_DELAY, animateBouncingBall, 0);
	glutTimerFunc(ANIMATION_DELAY, animateSolarSystem, 0);

	ground = load_texture("ground.bmp");
	ball = load_texture("tex3.bmp");
	wall = load_texture("wall.bmp");
	sun = load_texture("sun.bmp");
	moon = load_texture("moon.bmp");
	earth = load_texture("earth.bmp");
	wood = load_texture("wood.bmp");
	wood2 = load_texture("wood2.bmp");
	wood2front = load_texture("wood2front.bmp");
	lamp = load_texture("lamp.bmp");
	flag = load_texture("flag.bmp");
	water = load_texture("water.bmp");
	metal = load_texture("metal.bmp");
	boyFace = load_texture("boyFace.bmp");

	glBindTexture(GL_TEXTURE_2D, 0);

	glutTimerFunc(0, update, 0); // Start the update timer
	glutTimerFunc(0, update_flag, 0);
	glutTimerFunc(0, update_droplets, 0);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//starting main loop
	glutMainLoop();

}

GLuint load_texture(char *name)
{
	int width, height;
	GLubyte *data = readBMP(name, &width, &height);
	GLuint id;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	free(data);
	return id;
}

vec3 vec3_add(vec3 a, vec3 b)
{
	vec3 res;
	res.x = a.x + b.x;
	res.y = a.y + b.y;
	res.z = a.z + b.z;
	return res;
}

vec3 vec3_scale(vec3 a, float s)
{
	vec3 res;
	res.x = s * a.x;
	res.y = s * a.y;
	res.z = s * a.z;
	return res;
}

vec3 vec3_neg(vec3 a)
{
	return vec3_scale(a, -1);
}

vec3 vec3_sub(vec3 a, vec3 b)
{
	return vec3_add(a, vec3_neg(b));
}

float vec3_dist(vec3 a, vec3 b)
{
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	float dz = a.z - b.z;
	return sqrt((double)dx * dx + (double)dy * dy + (double)dz * dz);
}

void drawGround(void)
{
	int i, j;
	GLfloat diffuse[] = { 1, 1, 1, 1 };
	glBindTexture(GL_TEXTURE_2D, ground);
	glColor3f(0, 0.3, 0);
	glNormal3f(0, 1, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glBegin(GL_QUADS);
	for (i = -200; i < 200; i += 2) {
		for (j = -200; j < 200; j += 2) {
			glTexCoord2f(0, 0);
			glVertex3f(i, 0, j);
			glTexCoord2f(1, 0);
			glVertex3f(i + 2, 0, j);
			glTexCoord2f(1, 1);
			glVertex3f(i + 2, 0, j + 2);
			glTexCoord2f(0, 1);
			glVertex3f(i, 0, j + 2);
		}
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void drawBouncingBall(void)
{
	GLfloat ballLight[] = { ballPosition, 0, 0, 1 };
	GLfloat ballColor[] = { 2, 0.7, 0.7, 1 };
	GLfloat ballAmbient[] = { 0.4, 0.4, 0.4, 1 };
	float scale = 1;
	float translatePos = ballPosition > 0 ? -0.4 : 0.4;
	GLUquadric *sphere = gluNewQuadric();
	gluQuadricTexture(sphere, GL_TRUE);

	glBindTexture(GL_TEXTURE_2D, wall);
	glBegin(GL_QUADS);
	//glNormal3f(ballPosition + 1, -1, -1);
	glNormal3f(1, 0, 0);
	glTexCoord2f(0, 1);
	glVertex3f(-1, 1, 1);
	//glNormal3f(ballPosition + 1, 1, -1);
	glTexCoord2f(0, 0);
	glVertex3f(-1, -1, 1);
	//glNormal3f(ballPosition + 1, 1, 1);
	glTexCoord2f(1, 0);
	glVertex3f(-1, -1, -1);
	//glNormal3f(ballPosition + 1, -1, 1);
	glTexCoord2f(1, 1);
	glVertex3f(-1, 1, -1);
	//glNormal3f(ballPosition - 1, -1, -1);
	glNormal3f(-1, 0, 0);
	glTexCoord2f(1, 1);
	glVertex3f(1, 1, 1);
	//glNormal3f(ballPosition - 1, 1, -1);
	glTexCoord2f(1, 0);
	glVertex3f(1, -1, 1);
	//glNormal3f(ballPosition - 1, 1, 1);
	glTexCoord2f(0, 0);
	glVertex3f(1, -1, -1);
	//glNormal3f(ballPosition - 1, -1, 1);
	glTexCoord2f(0, 1);
	glVertex3f(1, 1, -1);
	glEnd();

	if (ballPosition < -0.6)
		scale = invLerp(-1, -0.6, ballPosition);
	else if (ballPosition > 0.6)
		scale = invLerp(1, 0.6, ballPosition);

	glBindTexture(GL_TEXTURE_2D, ball);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glPushMatrix();
	glTranslatef(ballPosition, 0, 0);
	glTranslatef(-translatePos * scale, 0, 0);
	glScalef(scale, 2 - scale, 2 - scale);
	glTranslatef(translatePos, 0, 0);

	glLightfv(LIGHT_BALL, GL_POSITION, ballLight);
	glLightfv(LIGHT_BALL, GL_DIFFUSE, ballColor);
	glLightfv(LIGHT_BALL, GL_AMBIENT, ballAmbient);
	glLightf(LIGHT_BALL, GL_QUADRATIC_ATTENUATION, 0.2);

	gluSphere(sphere, 0.4, 30, 30);
	gluDeleteQuadric(sphere);
	glPopMatrix();

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void drawSolarSystem(void)
{
	GLUquadric *sphere = gluNewQuadric();
	GLUquadric *disk = gluNewQuadric();
	gluQuadricTexture(sphere, GL_TRUE);
	glColor3f(0.4, 0.4, 0.4);

	glBindTexture(GL_TEXTURE_2D, sun);

	glPushMatrix();
	glRotatef(sunSelf, 0, 1, 0);
	gluSphere(sphere, 1, 30, 30);
	glPopMatrix();

	glPushMatrix();
	glRotatef(earthAngle, 0, 1, 0);
	glTranslatef(3, 0, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	glDisable(GL_LIGHTING);
	gluDisk(disk, 0.95, 1, 30, 30);
	if (lighting)
		glEnable(GL_LIGHTING);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, earth);
	glPushMatrix();
	glRotatef(earthSelf, -1, 1, 0);
	gluSphere(sphere, 0.4, 100, 100);

	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, moon);

	glRotatef(moonAngle, 0, 1, 0);
	glTranslatef(1, 0, 0);
	glRotatef(moonSelf, 0, 1, 0);
	gluSphere(sphere, 0.25, 30, 30);

	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, 0);
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	glDisable(GL_LIGHTING);
	gluDisk(disk, 2.95, 3, 100, 100);
	if (lighting)
		glEnable(GL_LIGHTING);
	glPopMatrix();

	gluDeleteQuadric(sphere);
	gluDeleteQuadric(disk);
}

void drawingCB(void)
{
	GLenum er;
	GLfloat light0Pos[] = { 0, 0, 0, 1 };
	GLfloat light0Color[] = { 1, 1, 1, 1 };
	GLfloat light0Ambient[] = { 0.2, 0.2, 0.2, 1 };
	GLfloat sunPos[] = { 0, 1, 0, 0 };
	vec3 center = vec3_add(cameraPos, cameraForward);
	vec3 up = { 0, 1, 0 };

	//clearing the background
	if (camera_mode == CAMERA_MODEL)
		glClearColor(0, 0, 0, 0);
	else
		glClearColor(clearColor[0], clearColor[1], clearColor[2], 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//initializing modelview transformation matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glLightfv(LIGHT_HEAD, GL_POSITION, light0Pos);
	glLightfv(LIGHT_HEAD, GL_DIFFUSE, light0Color);
	glLightfv(LIGHT_HEAD, GL_AMBIENT, light0Ambient);

	if (camera_mode == CAMERA_MODEL) {
		glEnable(LIGHT_HEAD);
		glLightf(LIGHT_HEAD, GL_QUADRATIC_ATTENUATION, 0);
		glDisable(LIGHT_SUN);

		glTranslatef(0, 0, -radius);
		glRotatef(angleX, 1, 0, 0);
		glRotatef(angleY, 0, 1, 0);
	}

	if (camera_mode == CAMERA_FREE) {
		if (head_light) {
			glEnable(LIGHT_HEAD);
			glLightf(LIGHT_HEAD, GL_QUADRATIC_ATTENUATION, 0.007);
		}
		else {
			glDisable(LIGHT_HEAD);
		}
		if (angleX < -45)
			up = cameraForwardXZ;
		gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z, center.x, center.y, center.z, up.x, up.y, up.z);
		glEnable(LIGHT_SUN);
		glPushMatrix();
		glRotatef(sunRotation, 0, 0, -1);
		glLightfv(LIGHT_SUN, GL_POSITION, sunPos);
		glLightfv(LIGHT_SUN, GL_DIFFUSE, sunLightColor);
		glPopMatrix();
		drawGround();
	}
	//glTranslatef(0, 2, 0);
	//drawSolarSystem();
	//drawBouncingBall();
	/*
	float boundery = 10;

	DrawFence((vec3) { -boundery, 0, boundery }, (vec3) { -boundery, 0, -boundery }, 2);
	DrawFence((vec3) { -boundery, 0, -boundery },  (vec3) { boundery, 0, -boundery }, 2);
	DrawFence((vec3) { boundery, 0, -boundery }, (vec3) { boundery, 0, boundery }, 2);

	glPushMatrix();
	glTranslated(0, 1, 0);

	glPushMatrix();
	glRotated(-90, 0, 1, 0);
	DrawWindSpinner();
	glPopMatrix();

	glTranslated(2, 0, 0);
	DrawStreetLight();
	glPopMatrix();

	glPushMatrix();
		DrawSwings();
	glPopMatrix();

	//DrawFountain();
	DrawFountain();
	*/

	//drawSolarSystem();
	//DrawFountain();
	//swapping buffers and displaying
	//DrawStreetLight();
	DrawSwings();
	glutSwapBuffers();
	//DrawWindSpinner();

	//check for errors
	er = glGetError();  //get errors. 0 for no error, find the error codes in: https://www.opengl.org/wiki/OpenGL_Error
	if (er) printf("error: %d\n", er);
}

void DrawFountain(void)
{
	int i;
	GLUquadric* sphere = gluNewQuadric();
	gluQuadricTexture(sphere, GL_TRUE);
	glPushMatrix();
	glTranslatef(0, 0.0001, 0);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	
	glPushMatrix();
	for (i = 0; i < 360; i++) {
		glPushMatrix();
		glTranslatef(3, 0, 0);
		VerticalCylinder(0.3, 1.1);
		glPopMatrix();
		glRotatef(1, 0, 1, 0);
	}
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 1.1, 0);
	glRotatef(90, 1, 0, 0);
	glutSolidTorus(0.3, 3, 50, 100);
	glPopMatrix();

	VerticalCylinder(0.2, 2.8);
	glPushMatrix();
	glTranslatef(0, 2.8, 0);
	glRotatef(90, 1, 0 ,0);
	glutSolidTorus(0.1, 0.25, 20, 20);
	glPopMatrix();

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	
	///////// DROPLETS

	for (i = 0; i < 10; i++) {
		glPushMatrix();
		glRotatef(36 * i, 0, 1, 0);
		glTranslatef(1, 0, 0);
		glScalef(0.5, 1, 1);
		DrawDropletsOval();

		glPopMatrix();
	}
	
	glBindTexture(GL_TEXTURE_2D, water);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glPushMatrix();
	glTranslatef(0, 1, 0);
	glScalef(1, 0.1, 1);
	gluSphere(sphere, 2.85, 30, 30);
	gluDeleteQuadric(sphere);
	glPopMatrix();

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glPopMatrix();
}

void DrawDropletsOval(void)
{
	float raius = 1;
	float scale = 0.3;
	int i;
	GLUquadric* sphere = gluNewQuadric();
	gluQuadricTexture(sphere, GL_TRUE);

	glPushMatrix();
	glTranslatef(0, 2, 0);
	for (i = droplets_offset; i < 180; i += 10) {
		glPushMatrix();
		glRotatef(i, 0, 0, 1);
		glTranslatef(2, 0, 0);

		glBindTexture(GL_TEXTURE_2D, water);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		glScalef(1, 2.5, 1);
		gluSphere(sphere, 0.02, 10, 10);

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		glPopMatrix();
		
	}

	gluDeleteQuadric(sphere);

	glPopMatrix();
}

void DrawFence(vec3 fromPoint, vec3 toPoint, float poleHeight)
{
	float poleDistance = 0.4f;
	float poleRadius = 0.1f;
	fromPoint.y = toPoint.y = 0;
	vec3 direction = vec3_sub(toPoint, fromPoint);
	float distance = vec3_dist(toPoint, fromPoint);
	float n_poles = floor(distance / poleDistance);
	vec3 normal_direction = vec3_scale(direction, 1/distance);
	vec3 scaled_direction = vec3_scale(normal_direction, poleDistance);
	int i;
	glPushMatrix();
	glTranslated(fromPoint.x, fromPoint.y, fromPoint.z);
	glBindTexture(GL_TEXTURE_2D, wood);
	glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	for (i = 0; i < n_poles; i++) {
		VerticalCylinder(poleRadius, poleHeight);
		glTranslated(scaled_direction.x, scaled_direction.y, scaled_direction.z);
	}
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glPopMatrix();
}

void DrawFlagPole(void)
{
	float poleRadius = 0.1f;
	float poleHeight = 4.5;
	float w = 2;
	float h = w * 2 / 3;

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, wood);
	glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	VerticalCylinder(poleRadius, poleHeight);
	glTranslatef(0, poleHeight, 0);
	glutSolidSphere(poleRadius * 1.5, 10, 10);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glTranslatef(poleRadius, 0, 0);

	glBindTexture(GL_TEXTURE_2D, flag);
	glBegin(GL_POLYGON);

	glNormal3f(0, 0, 1);
	glTexCoord2f(1, 1); glVertex3f(0, 0, 0);
	glTexCoord2f(0, 1); glVertex3f(-w, -droop, 0);
	glTexCoord2f(0, 0); glVertex3f(-w, -droop-h, 0);
	glTexCoord2f(1, 0); glVertex3f(0, -h, 0);

	glEnd();

	glPopMatrix();
}

void DrawSign(void)
{
	float sign_width = 2;
	float sign_height = 2;
	float cylinder_height = 1;
	float cylinder_radius = 0.15;
	
	glBindTexture(GL_TEXTURE_2D, wood2front);
	glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	// Front TBD MAY BE OTHER TEXTURE
	glBegin(GL_POLYGON);
	
	glTexCoord2f(0, 0); glVertex3f(-sign_width / 2,	cylinder_height,				0.05f);
	glTexCoord2f(1, 0); glVertex3f(	sign_width / 2,	cylinder_height,				0.05f);
	glTexCoord2f(1, 1); glVertex3f(	sign_width / 2,	cylinder_height + sign_height,	0.05f);
	glTexCoord2f(0, 1); glVertex3f(-sign_width / 2,	cylinder_height + sign_height,	0.05f);
	glEnd();
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glBindTexture(GL_TEXTURE_2D, wood2);
	glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);

	// Back
	glBegin(GL_POLYGON);
	glVertex3f(-sign_width / 2, cylinder_height,				-0.05f);
	glVertex3f(+sign_width / 2, cylinder_height,				-0.05f);
	glVertex3f(+sign_width / 2, cylinder_height + sign_height,	-0.05f);
	glVertex3f(-sign_width / 2, cylinder_height + sign_height,	-0.05f);
	glEnd();

	// Right
	glBegin(GL_POLYGON);
	glVertex3f(sign_width / 2 + 0.001f, cylinder_height,				-0.05f);
	glVertex3f(sign_width / 2 + 0.001f, cylinder_height,				+0.05f);
	glVertex3f(sign_width / 2 + 0.001f, cylinder_height + sign_height,	+0.05f);
	glVertex3f(sign_width / 2 + 0.001f, cylinder_height + sign_height,	-0.05f);
	glEnd();

	// Left
	glBegin(GL_POLYGON);
	glVertex3f(-sign_width / 2 + 0.001f, cylinder_height,				-0.05f);
	glVertex3f(-sign_width / 2 + 0.001f, cylinder_height,				+0.05f);
	glVertex3f(-sign_width / 2 + 0.001f, cylinder_height + sign_height,	+0.05f);
	glVertex3f(-sign_width / 2 + 0.001f, cylinder_height + sign_height,	-0.05f);
	glEnd();

	// Top
	glBegin(GL_POLYGON);
	glVertex3f(+sign_width / 2, cylinder_height + sign_height,	+0.05f);
	glVertex3f(-sign_width / 2, cylinder_height + sign_height,	+0.05f);
	glVertex3f(-sign_width / 2, cylinder_height + sign_height,	-0.05f);
	glVertex3f(+sign_width / 2, cylinder_height + sign_height,	-0.05f);
	glEnd();

	//disabling automatic texture coordinates generation
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);


	glBindTexture(GL_TEXTURE_2D, wood);

	glPushMatrix();
	glTranslatef(- (sign_width/2) - cylinder_radius, 0, 0);
	VerticalCylinder(cylinder_radius, cylinder_height + sign_height);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(sign_width/2 + cylinder_radius, 0, 0);
	VerticalCylinder(cylinder_radius, cylinder_height + sign_height);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, -0.05f - cylinder_radius*2);
	VerticalCylinder(cylinder_radius, cylinder_height + sign_height + 0.7);
	glTranslatef(0, cylinder_height + sign_height + 0.5, -cylinder_radius);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	VerticalCylinder(cylinder_radius*0.6, 1.5);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

	glTranslated(0, 0.05, 1.54);
	glRotatef(210.0f, 1.0f, 0.0f, 0.0f);
	VerticalCylinder(cylinder_radius * 0.6, 0.3);
	glTranslated(0, 0.3, 0);

	glBindTexture(GL_TEXTURE_2D, lamp);
	glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glutSolidSphere(0.085,20,20);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glPopMatrix();

}

void VerticalCylinder(float radius, float height)
{
	glPushMatrix();
	GLUquadric* quadric;
	quadric = gluNewQuadric();
	gluQuadricTexture(quadric, GL_TRUE);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadric, radius, radius, height, 32, 32);
	glPopMatrix();
	gluDeleteQuadric(quadric);
}

void reshapeCB(int width, int height)
{
	float zNear, zFar;

	zNear = 1; zFar = 1000;

	//update viewport
	glViewport(0, 0, width, height);

	//clear the transformation matrices (load identity)
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//projection
	gluPerspective(FOVy, (GLdouble)width / height, zNear, zFar);
}

float wrapAngle(float angle, float maxAngle)
{
	if (angle < 0)
		return angle + maxAngle;
	if (angle >= maxAngle)
		return angle - maxAngle;
	return angle;
}

void resetCamera(void)
{
	angleX = angleY = 0;
	FOVy = 60;
	if (camera_mode == CAMERA_MODEL) {
		radius = 5;
	}
	else if (camera_mode == CAMERA_FREE) {
		cameraPos.x = 0;
		cameraPos.y = 2;
		cameraPos.z = 5;
		cameraForward.x = 0;
		cameraForward.y = 0;
		cameraForward.z = -1;
		cameraForwardXZ = cameraForward;
		cameraRight.x = 1;
		cameraRight.y = 0;
		cameraRight.z = 0;
	}
}

void computeCameraVectors(void)
{
	float cosa, sina;
	vec3 camFacing = { 0, 0, -1 };
	vec3 camFacingXZ = { 0, 0, -1 };
	vec3 temp;

	if (angleX > -2)
		angleX = -2;
	else if (angleX < -90)
		angleX = -90;

	cosa = cos(angleX * PI / 180);
	sina = sin(angleX * PI / 180);
	temp.x = camFacing.x;
	temp.y = cosa * camFacing.y - sina * camFacing.z;
	temp.z = sina * camFacing.y + cosa * camFacing.z;

	cosa = cos(angleY * PI / 180);
	sina = sin(angleY * PI / 180);
	cameraForward.x = cosa * temp.x + sina * temp.z;
	cameraForward.y = temp.y;
	cameraForward.z = -sina * temp.x + cosa * temp.z;

	cameraForwardXZ.x = cosa * camFacingXZ.x + sina * camFacingXZ.z;
	cameraForwardXZ.y = camFacingXZ.y;
	cameraForwardXZ.z = -sina * camFacingXZ.x + cosa * camFacingXZ.z;

	cosa = cos(-PI / 2);
	sina = sin(-PI / 2);
	cameraRight.x = cosa * cameraForwardXZ.x + sina * cameraForwardXZ.z;
	cameraRight.y = cameraForwardXZ.y;
	cameraRight.z = -sina * cameraForwardXZ.x + cosa * cameraForwardXZ.z;
}

void rotateAroundPoint(void)
{
	float cosa = cos((90 - fabs(angleX)) * PI / 180);
	float len = cameraPos.y / cosa;

	cameraPos = vec3_add(cameraPos, vec3_scale(cameraForward, len));
	computeCameraVectors();
	cameraPos = vec3_sub(cameraPos, vec3_scale(cameraForward, len));
}

void keyboardCB(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	case '+':
		if (camera_mode == CAMERA_FREE) {
			FOVy = FOVy - 1;
			reshapeCB(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		}
		else {
			radius -= 0.05;
		}
		glutPostRedisplay();
		break;
	case '-':
		if (camera_mode == CAMERA_FREE) {
			FOVy = FOVy + 1;
			reshapeCB(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		}
		else {
			radius += 0.05;
		}
		glutPostRedisplay();
		break;
	case '1':
		camera_mode = CAMERA_MODEL;
		resetCamera();
		glutPostRedisplay();
		break;
	case '2':
		camera_mode = CAMERA_FREE;
		resetCamera();
		glutPostRedisplay();
		break;
	case 'w':
		cameraPos = vec3_add(cameraPos, vec3_scale(cameraForwardXZ, CAMERA_SPEED));
		glutPostRedisplay();
		break;
	case 's':
		cameraPos = vec3_sub(cameraPos, vec3_scale(cameraForwardXZ, CAMERA_SPEED));
		glutPostRedisplay();
		break;
	case 'd':
		cameraPos = vec3_add(cameraPos, vec3_scale(cameraRight, CAMERA_SPEED));
		glutPostRedisplay();
		break;
	case 'a':
		cameraPos = vec3_sub(cameraPos, vec3_scale(cameraRight, CAMERA_SPEED));
		glutPostRedisplay();
		break;
	case 'r':
		cameraPos.y += CAMERA_SPEED;
		glutPostRedisplay();
		break;
	case 'f':
		cameraPos.y -= CAMERA_SPEED;
		if (cameraPos.y < 2)
			cameraPos.y = 2;
		glutPostRedisplay();
		break;
	case 'q':
		angleY = wrapAngle(angleY - CAMERA_ROTATION_SPEED, 360);
		rotateAroundPoint();
		glutPostRedisplay();
		break;
	case 'e':
		angleY = wrapAngle(angleY + CAMERA_ROTATION_SPEED, 360);
		rotateAroundPoint();
		glutPostRedisplay();
		break;
	case 'l':
		lighting = !lighting;
		lighting ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);
		glutPostRedisplay();
		break;
	case 'h':
		head_light = !head_light;
		glutPostRedisplay();
		break;
	case 'c':
		if (transPhase != PHASE_END)
			return;
		timeOfDay = timeOfDay == TIME_DAY ? TIME_NIGHT : TIME_DAY;
		transPhase = PHASE_TO_TWILIGHT;
		phase = 0;
		glutTimerFunc(ANIMATION_DELAY, animateTimeChange, 0);
	}
}

void keyboardSpecialCB(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_LEFT:
		angleY += CAMERA_ROTATION_SPEED;
		break;
	case GLUT_KEY_RIGHT:
		angleY -= CAMERA_ROTATION_SPEED;
		break;
	case GLUT_KEY_UP:
		angleX += CAMERA_ROTATION_SPEED;
		break;
	case GLUT_KEY_DOWN:
		angleX -= CAMERA_ROTATION_SPEED;
		break;
	}

	angleY = wrapAngle(angleY, 360);
	if (camera_mode == CAMERA_MODEL)
		angleX = wrapAngle(angleX, 360);

	if (camera_mode == CAMERA_FREE) {
		computeCameraVectors();
	}
	glutPostRedisplay();
}

void animateBouncingBall(int value)
{
	ballPosition += dxBall;

	if (ballPosition >= 0.9) {
		ballPosition = 0.9;
		dxBall = -dxBall;
	}
	else if (ballPosition <= -0.9) {
		ballPosition = -0.9;
		dxBall = -dxBall;
	}

	glutPostRedisplay();
	glutTimerFunc(ANIMATION_DELAY, animateBouncingBall, 0);
}

void animateSolarSystem(int value)
{
	sunSelf = wrapAngle(sunSelf + CELESTIAL_SELF_SPIN, 360);
	earthSelf = wrapAngle(earthSelf + CELESTIAL_SELF_SPIN, 360);
	moonSelf = wrapAngle(moonSelf + CELESTIAL_SELF_SPIN, 360);
	earthAngle = wrapAngle(earthAngle + CELESTIAL_SPIN, 360);
	moonAngle = wrapAngle(moonAngle + CELESTIAL_SPIN, 360);
	glutPostRedisplay();
	glutTimerFunc(ANIMATION_DELAY, animateSolarSystem, 0);
}

void animateTimeChange(int value)
{
	float day[] = { 135.0f / 255, 206.0f / 255, 235.0f / 255 };
	float twilight[] = { 218.0f / 255, 91.0f / 255, 20.0f / 255 };
	float night[] = { 19.0f / 255, 24.0f / 255, 98.0f / 255 };
	GLfloat sunlight[] = { 1, 1, 1, 1 };
	GLfloat nightlight[] = { 0, 0, 0, 1 };
	float *from, *to, fromAngle, toAngle;
	GLfloat *fromLight, *toLight;

	if (timeOfDay == TIME_NIGHT) {
		if (transPhase == PHASE_TO_TWILIGHT) {
			from = day;
			fromLight = sunlight;
			fromAngle = 0;
			to = twilight;
			toLight = twilight;
			toAngle = 45;
		}
		else {
			from = twilight;
			fromLight = twilight;
			fromAngle = 45;
			to = night;
			toLight = nightlight;
			toAngle = 90;
		}
	}
	else {
		if (transPhase == PHASE_TO_TWILIGHT) {
			from = night;
			fromLight = nightlight;
			fromAngle = -90;
			to = twilight;
			toLight = twilight;
			toAngle = -45;
		}
		else {
			from = twilight;
			fromLight = twilight;
			fromAngle = -45;
			to = day;
			toLight = sunlight;
			toAngle = 0;
		}
	}

	if (phase > 1 - TIME_RATE)
		phase = 1;
	clearColor[0] = lerp(from[0], to[0], phase);
	clearColor[1] = lerp(from[1], to[1], phase);
	clearColor[2] = lerp(from[2], to[2], phase);
	sunLightColor[0] = lerp(fromLight[0], toLight[0], phase);
	sunLightColor[1] = lerp(fromLight[1], toLight[1], phase);
	sunLightColor[2] = lerp(fromLight[2], toLight[2], phase);
	sunRotation = lerp(fromAngle, toAngle, phase);
	glutPostRedisplay();
	
	phase += TIME_RATE;
	if (phase >= 1) {
		phase = 0;
		if (transPhase == PHASE_TO_END) {
			transPhase = PHASE_END;
			return;
		}
		else {
			transPhase = PHASE_TO_END;
		}
	}
	glutTimerFunc(ANIMATION_DELAY, animateTimeChange, value);
}

void update_flag(int value)
{
	float droop_values[16] = {0.005, 0.01, 0.02, 0.03, 0.05, 0.07, 0.09, 0.11, 0.12, 
							0.11, 0.09, 0.07, 0.05, 0.03, 0.02, 0.01 }; // 16
	droop = droop_values[droop_index];
	droop_index++;
	droop_index = droop_index == 16 ? 0 : droop_index;
	glutTimerFunc(64, update_flag, 0);
	glutPostRedisplay();
}

void update_droplets(int value)
{
	droplets_offset--;
	if (droplets_offset <= -100) {
		droplets_offset = -90;
	}
	glutTimerFunc(16, update_droplets, 0);
	glutPostRedisplay();
}

float lerp(float a, float b, float t)
{
	return a * (1 - t) + b * t;
}

float invLerp(float a, float b, float c)
{
	return (c - a) / (b - a);
}

// Function to load bmp file
// buffer for the image is allocated in this function, you should free this buffer
GLubyte *readBMP(char *imagepath, int *width, int *height)
{
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int imageSize;   // = width*height*3
	unsigned char *data;
	unsigned char tmp;
	int i;

	// Open the file
	FILE *file = fopen(imagepath, "rb");
	if (!file) {
		TerminationErrorFunc("Image could not be opened\n");
	}

	if (fread(header, 1, 54, file) != 54) { // If not 54 bytes read : problem
		TerminationErrorFunc("Not a correct BMP file\n");
	}

	if (header[0] != 'B' || header[1] != 'M') {
		TerminationErrorFunc("Not a correct BMP file\n");
	}

	// Read ints from the byte array
	dataPos = *(int *)&(header[0x0A]);
	imageSize = *(int *)&(header[0x22]);
	*width = *(int *)&(header[0x12]);
	*height = *(int *)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)
		imageSize = *width * *height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)
		dataPos = 54; // The BMP header is done that way

	// Create a buffer
	data = malloc(imageSize * sizeof(GLubyte));
	if (data == NULL) {
		TerminationErrorFunc("Failed to allocate memory\n");
	}

	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);

	//swap the r and b values to get RGB (bitmap is BGR)
	for (i = 0; i < *width * *height; i++)
	{
		tmp = data[i * 3];
		data[i * 3] = data[i * 3 + 2];
		data[i * 3 + 2] = tmp;
	}

	//Everything is in memory now, the file can be closed
	fclose(file);

	return data;
}

void TerminationErrorFunc(char *ErrorString)
{
	char string[256];
	printf(ErrorString);
	fgets(string, 256, stdin);

	exit(0);
}

void DrawWindSpinner(void)
{
	float CylinderHeight = 3.0f;
	float CylinderRadius = 0.1f;
	// Ruby Color
	GLfloat mat_ambient2[] = { 0.1745, 0.01175, 0.01175, 0.55 };
	GLfloat mat_diffuse2[] = { 0.61424, 0.04136, 0.04136, 0.55 };
	GLfloat mat_specular2[] = { 0.727811, 0.626959, 0.626959, 0.55 };
	GLfloat mat_shininess2[] = { 76.8 };

	// Polished Silver Color
	GLfloat mat_ambient[] = { 0.23125, 0.23125, 0.23125, 1.0 };
	GLfloat mat_diffuse[] = { 0.2775, 0.2775, 0.2775, 1.0 };
	GLfloat mat_specular[] = { 0.773911, 0.773911, 0.773911, 1.0 };
	GLfloat mat_shininess[] = { 51.22 };
	glPushMatrix();
	glTranslatef(0, -1.5, 0);
	glPushMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	VerticalCylinder(0.1, 3.0);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 3, 0);
	glutSolidSphere(0.1, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 3.0f, 0);

	glutSolidSphere(0.1, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.2, 3.0f, 0);

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient2);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse2);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular2);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess2);

	glutSolidSphere(0.1, 20, 20);
	DrawSpinner();
	glPopMatrix();

	glPopMatrix();
}

void DrawStreetLight(void)
{
	// Polished Silver Color
	GLfloat mat_ambient[] = { 0.23125, 0.23125, 0.23125, 1.0 };
	GLfloat mat_diffuse[] = { 0.2775, 0.2775, 0.2775, 1.0 };
	GLfloat mat_specular[] = { 0.773911, 0.773911, 0.773911, 1.0 };
	GLfloat mat_shininess[] = { 51.22 };
	GLUquadric* quadric;
	// lamp
	GLfloat light_1_position[] = { 0, 3.68, 0.9, 1 };
	GLfloat light_1_specular[] = { 0.1, 0.1, 0.1, 1 };
	GLfloat light_1_diffuse[] = { 0.7, 0.7, 0.7, 1 };
	GLfloat light_1_ambient[] = { 0.3, 0.3, 0.3, 1 };
	GLfloat light_1_spotLight[] = { 0, -1, 0, 1 };
	GLfloat light_1_spotCutOff[] = { 25 };

	glPushMatrix();
	glTranslatef(0, -1.7, 0);
	quadric = gluNewQuadric(); 

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	VerticalCylinder(0.1, 4.0);
	glTranslated(0, 4, 0);
	glutSolidSphere(0.1, 20, 20);
	gluCylinder(quadric, 0.1, 0.1, 1, 32, 32);
	glTranslatef(0, 0, 1);
	glutSolidSphere(0.1, 20, 20);
	glTranslatef(0, -0.32, -0.1);

	glPushMatrix();
	glRotated(-90.0, 1, 0, 0);
	glutSolidCone(0.2, 0.3, 10, 10);
	glPopMatrix();
	

	glLightfv(LIGHT_STREETLAMP, GL_POSITION, light_1_position);
	glLightfv(LIGHT_STREETLAMP, GL_SPOT_DIRECTION, light_1_spotLight);
	glLightfv(LIGHT_STREETLAMP, GL_SPOT_CUTOFF, light_1_spotCutOff);
	glLightfv(LIGHT_STREETLAMP, GL_DIFFUSE, light_1_diffuse);
	glLightfv(LIGHT_STREETLAMP, GL_SPECULAR, light_1_specular);
	glLightfv(LIGHT_STREETLAMP, GL_AMBIENT, light_1_ambient);

	glBindTexture(GL_TEXTURE_2D, lamp);
	glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glutSolidSphere(0.12, 20, 20);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	gluDeleteQuadric(quadric);

	glPopMatrix(); 
}

void DrawSwings(void) 
{
	GLUquadric* quadric;
	glPushMatrix();
		glTranslated(0, -1, 0);
		// Right pillar
		glPushMatrix();
			glTranslated(2, 0, 0);
			VerticalCylinder(0.1, 3.0);
			glTranslated(0, 3, 0);
			glutSolidSphere(0.1, 10, 10);
		glPopMatrix();

		// Left pillar
		glPushMatrix();
			glTranslated(-2, 0, 0);
			VerticalCylinder(0.1, 3.0);
			glTranslated(0, 3, 0);
			glutSolidSphere(0.1, 10, 10);
		glPopMatrix();

		// Top Pillar
		glPushMatrix();
			glTranslated(2, 3, 0);
			quadric = gluNewQuadric();
			gluQuadricTexture(quadric, GL_TRUE);
			glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
			gluCylinder(quadric, 0.1, 0.1, 4.0, 32, 32);	
			gluDeleteQuadric(quadric);
		glPopMatrix();

		glPushMatrix();

			glTranslated(0, 2.83, 0);
				
			// Draw Swing Left
			glPushMatrix();
				glTranslated(-1.5, 0, 0);
				glRotatef(swingAng, 1, 0, 0);
				DrawSwing();
				glTranslatef(0.5, -1.85, 0);
				glScalef(0.9, 0.9, 0.9);
				DrawSwingKid(0); 
			glPopMatrix();

			// Draw Swing Right
			glPushMatrix();
				glTranslated(0.5, 0, 0);
				glRotatef(-swingAng, 1, 0, 0);
				DrawSwing();
				glTranslatef(0.5, -1.85, 0);
				glScalef(0.9, 0.9, 0.9);
				DrawSwingKid(1);
			glPopMatrix();

		glPopMatrix();

	glPopMatrix();
}

void DrawSwingKid(int gender)
{
	vec3 legPos = {0.14, -0.1, 0.0};
	vec3 armPos = { 0.23, 0.5, 0.0 };

	glPushMatrix();
		// Body
		glTranslatef(0.0, -0.05, -0.18);
		DrawBody(0.25, 0.6, gender);
		// Head
		DrawHead();

		glPushMatrix();
			// Left-leg
			glTranslatef(legPos.x, legPos.y, legPos.z);
			DrawLeg(gender);
		glPopMatrix();

		glPushMatrix();
			// Right-leg
			glTranslatef(-legPos.x, legPos.y, legPos.z);
			DrawLeg(gender);
		glPopMatrix();

		// Left-arm
		glPushMatrix();
			glTranslatef(armPos.x, armPos.y, armPos.z);
			glRotatef(80.0, 1.0, 0.85, 0.0);
			DrawArm();
		glPopMatrix();	

		// Right-arm
		glPushMatrix();
			glTranslatef(-armPos.x, armPos.y, armPos.z);
			glRotatef(80.0, 1.0, -0.85, 0.0);
			DrawArm();
		glPopMatrix();
	glPopMatrix();

}

void drawBlade(void)
{
	glPushMatrix();
	glTranslated(0, 0, -1);
	glutSolidCone(0.1, 1.0, 20, 20);
	glPopMatrix();
}

void DrawSpinner(void)
{
	int i;
	glPushMatrix();
	glRotatef(rotationAngle, 1.0, 0.0, 0.0);

	// Draw the wind spinner blades
	for (i = 0; i < 6; ++i) {
		glRotatef(60.0, 1.0, 0.0, 0.0);
		drawBlade();
	}

	glPopMatrix();
}

float DrawChains(int length) 
{
	float y = -0.035;
	int i;

	glPushMatrix();
		glutSolidTorus(0.02, 0.06, 10, 10);
		glScaled(1, 2, 1);

		glTranslatef(0, -0.035, 0);
		glRotatef(90.0, 0.0, 1.0, 0.0);

		glPushMatrix();
			for (i = 0; i < length; i++) {
				glutSolidTorus(0.01, 0.02, 10, 10);
				glRotatef(90.0, 0.0, 1.0, 0.0);
				glTranslatef(0, -0.035, 0);
				y += -0.035*2;
			}
		glPopMatrix();

	glPopMatrix();

	return y;
}

void DrawSwing(void)
{
	float y;

	glPushMatrix();
		// Chain Left
		y = DrawChains(30);

		glBindTexture(GL_TEXTURE_2D, wood);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		// Draw Banch
		glPushMatrix();
		glTranslatef(0, y, 0);
		glScalef(12.0, 1.0, 5.0);
		glTranslatef(0.042, 0.0, -0.005);
		glutSolidCube(0.1);
		glPopMatrix();
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Chain Right
		glTranslated(1, 0, 0);
		DrawChains(30);
	glPopMatrix();


}

void DrawHead(void)
{		
	GLUquadric* quadric;

	glPushMatrix();
		glPushMatrix();
			glTranslatef(0.0, 0.65, 0.0);
			glutSolidSphere(0.1, 10, 10);// neck
			glTranslatef(0.0, 0.3, 0.0);
			
			/// texture of smaily
			//glBindTexture(GL_TEXTURE_2D, boyFace);
			//glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			//glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			//glEnable(GL_TEXTURE_GEN_S);
			//glEnable(GL_TEXTURE_GEN_T);
			glutSolidSphere(0.23, 10, 10);// head
			//glDisable(GL_TEXTURE_GEN_S);
			//glDisable(GL_TEXTURE_GEN_T);
		glPopMatrix();

		/*
		glBindTexture(GL_TEXTURE_2D, metal);
		glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_LINEAR);
		glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_LINEAR);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glutSolidSphere(0.23, 10, 10);// head
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		*/

		glPushMatrix();

			glTranslatef(0.0, 0.6, 0.0);
			quadric = gluNewQuadric();
			gluQuadricTexture(quadric, GL_TRUE);
			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
			gluCylinder(quadric, 0.25, 0.05, 0.1, 32, 32);
			gluDeleteQuadric(quadric);
		glPopMatrix();
	glPopMatrix();
}

void DrawBody(float radius, float height, int gender)
{
	GLUquadric* quadric;

	if (gender) {
		VerticalCylinder(radius, height);
		glutSolidSphere(0.25, 10, 10);
	}
	else {
		glPushMatrix();
			quadric = gluNewQuadric();
			gluQuadricTexture(quadric, GL_TRUE);
			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
			glTranslatef(0.0, 0.0, -0.2);
			gluCylinder(quadric, radius + 0.2, radius , height + 0.2  , 32, 32);
			gluDeleteQuadric(quadric);
		glPopMatrix();
	}

}

void Drawlimb(float radius, float height)
{
	GLUquadric* quadric;
	//draw pill shaped limb in the pos Z direction
	glPushMatrix();
		quadric = gluNewQuadric();
		gluCylinder(quadric, radius, radius, height, 32, 32);
		gluDeleteQuadric(quadric);
		glutSolidSphere(radius, 10, 10);
		glPushMatrix();
			glTranslatef(0, 0, height);
			glutSolidSphere(radius, 10, 10);
		glPopMatrix();
	glPopMatrix();
}

void DrawLeg(int gender)
{
	float thighLength = 0.5;
	float shockLength = 0.55;
	float footLength = 0.2;
	glPushMatrix();
		//theigh
		Drawlimb(0.12, thighLength);
		glTranslatef(0 ,0, thighLength);
		//shock
		gender == 1 ? glRotatef(jointAngLeg, 1.0, 0.0, 0.0) : glRotatef(90 - jointAngLeg, 1.0, 0.0, 0.0);
		Drawlimb(0.1, shockLength);
		glTranslatef(0.0, 0.0,  shockLength);
		glRotatef(-90, 1.0, 0.0, 0.0);
		//foot
		Drawlimb(0.12, footLength);

	
	glPopMatrix();
}

void DrawArm(void)
{
	float armLenght = 0.5;

	glPushMatrix();
		glutSolidSphere(0.13, 10, 10); // Shoulder
		Drawlimb(0.1, armLenght);
		glTranslatef(0.0, 0.0, armLenght);
		glutSolidSphere(0.12, 10, 10); // Palm
	glPopMatrix();
}

void update(int value) 
{
	float length = 0.8;
	double omega = sqrt(9.81 / length);
	float time_period = 2 * PI * sqrt(length / 9.81);
	int isForward = 1;

	rotationAngle += 2.0f; // Adjust rotation speed as needed
	rotationAngle = wrapAngle(rotationAngle, 360.0);


	time += 16 * 0.001f;
	swingAng = SWING_MAX_ANG * cos(omega * time);

	//jointAngLeg = cos(omega * time) < 0 ? 0 : 90;

	//------------------------
	jointAngLeg = abs(90 * cos(omega * time));

	/*
	if (isForward && jointAngLeg < 90) {
		jointAngLeg++;
	}
	else if (!isForward && jointAngLeg >= 90) {
		jointAngLeg--;
	}
	isForward = jointAng > 0.9 ? 0 : 1;
	isForward = jointAng < -0.9 ? 1 : 0;
	*/
		//printf("is forward? = %d, angle is: %f ,other ang:%f\n", isForward, jointAngLeg, jointAng);

	//------------------------
	//printf("angle = %f \n", cos(omega * time));


	if (time >= time_period) {
		time = 0;
	}

	glutPostRedisplay();
	glutTimerFunc(16, update, 0); // ~60 FPS
}

/*
	Colors:

		// Emerald Color
		GLfloat mat_ambient1[] = { 0.0215, 0.1745, 0.0215, 0.55 };
		GLfloat mat_diffuse1[] = { 0.07568, 0.61424, 0.07568, 0.55 };
		GLfloat mat_specular1[] = { 0.633, 0.727811, 0.633, 0.55 };
		GLfloat mat_shininess1[] = { 76.8 };
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient1);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse1);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular1);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess1);
	

		// Ruby Color
		GLfloat mat_ambient2[] = { 0.1745, 0.01175, 0.01175, 0.55 };
		GLfloat mat_diffuse2[] = { 0.61424, 0.04136, 0.04136, 0.55 };
		GLfloat mat_specular2[] = { 0.727811, 0.626959, 0.626959, 0.55 };
		GLfloat mat_shininess2[] = { 76.8 };
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient2);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse2);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular2);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess2);


		// Gold Color
		GLfloat mat_ambient1[] = { 0.24725, 0.2245, 0.0645, 1.0 };
		GLfloat mat_diffuse1[] = { 0.34615, 0.3143, 0.0903, 1.0 };
		GLfloat mat_specular1[] = { 0.628281, 0.555802, 0.366065, 1.0 };
		GLfloat mat_shininess1[] = { 51.2 };
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient1);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse1);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular1);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess1);

		
		// Polished Silver Color
		GLfloat mat_ambient[] = { 0.23125, 0.23125, 0.23125, 1.0 };
		GLfloat mat_diffuse[] = { 0.2775, 0.2775, 0.2775, 1.0 };
		GLfloat mat_specular[] = { 0.773911, 0.773911, 0.773911, 1.0 };
		GLfloat mat_shininess[] = { 51.22 };
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	
*/

/*
	// Light Settings

	GLfloat light_1_specular[] = { 1, 1, 1, 1 };
	GLfloat light_1_diffuse[] = { 1, 1, 1, 1 };
	GLfloat light_1_ambient[] = { 1, 1, 1, 1 };
	GLfloat light_1_position[] = { 5, 5, 0, 0 };
	glLightfv(GL_LIGHT1, GL_POSITION, light_1_position);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_1_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_1_ambient);

*/