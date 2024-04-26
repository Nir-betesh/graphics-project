#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"GL/glut.h"

#define PI 3.14159265359f
#define ANIMATION_DELAY 40
#define SIXTY_FPS 16
#define CAMERA_ROTATION_SPEED 1.5f
#define CAMERA_SPEED 0.25f
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
#define LIGHT_SIGN GL_LIGHT3
#define LIGHT_SOLAR_SYSTEM_SUN GL_LIGHT4
#define LIGHT_STREETLAMP1 GL_LIGHT5
#define LIGHT_STREETLAMP2 GL_LIGHT6
#define LIGHT_STREETLAMP3 GL_LIGHT7
#define SWING_MAX_ANG 40
#define TEXTURE_GROUND (GL_LIGHT7 + 1)
#define TEXTURE_BOUNCING_BALL (GL_LIGHT7 + 2)
#define TEXTURE_SOLAR_SYSTEM (GL_LIGHT7 + 3)
#define TEXTURE_WOOD (GL_LIGHT7 + 4)

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
void menuCB(int value);
void animateBouncingBall(int value);
void animateSolarSystem(int value);
void animateTimeChange(int value);
GLubyte* readBMP(char* imagepath, int* width, int* height);
void TerminationErrorFunc(char* ErrorString);
GLuint load_texture(char* name);
void VerticalCylinder(float radius, float height);
void drawSign(void);
void drawWindSpinner(void);
void DrawSpinner(void);
void drawBlade(void);
//void Update(int value);
void drawStreetLight(GLenum srcLight);
void drawSwings(void);
void drawSwing(void);
void drawFence(vec3 fromPoint, vec3 toPoint, float poleHeight);
void drawFlagPole(void);
void animateFlag(int value);
void drawFountain(void);
void drawDropletsOval(void);
void animateDroplets(int value);
float DrawChains(int length);
float lerp(float a, float b, float t);
float invLerp(float a, float b, float c);
void drawSwingKid(int gender);
void DrawHead(void);
void DrawArm(void);
void Drawlimb(float radius, float height);
void DrawLeg(int gender, int isAnim);
void drawCarousel(void);
void DrawBody(float radius, float height, int gender);
void DrawSquare(float xLen, float yLen, float zLen);
void DrawCoOpSwing(void);
void DrawCoOpSwingKid(int gender);
void Paint(float r, float g, float b);
void PaintSpec(float r, float g, float b);
void GoldColor(void);
void PolishedSilverColor(void);
void RubyColor(void);
void EmeraldColor(void);
void DrawCaruselKid(int gender);
void animateCamera(int value);
void animateSwingKids(int value);
void animateWindSpinner(int value);
void animateCoOpSwing(int value);
void computeCameraVectors(void);

float time = 0;
int isBanchExist = 0, dirSwingA = 1;
float swingAng = SWING_MAX_ANG;

int FOVy = 60;
int camera_mode = CAMERA_FREE;
int lighting = 1, head_light = 1, texture = 1, animation = 1;
int sunEnable = 1, ballEnable = 1, solarEnable = 1, lampEnable = 1, signEnable = 1;
float angleX = 0, angleY = 0, radius = 5;
vec3 cameraPos = { 0, 3, 5 };
vec3 cameraForward = { 0, 0, -1 };
vec3 cameraForwardXZ = { 0, 0, -1 };
vec3 cameraRight = { 1, 0, 0 };

float jointAngLeg = 30;
float jointAng;
float coOpSwingAng;
float ballPosition = 0;
float dxBall = BALL_SPEED;
float earthAngle = 0, earthSelf = 0, sunSelf = 0, moonSelf = 0, moonAngle = 0;
float rotationAngle = 0.0f;
int isUp = 1;

int timeOfDay = TIME_DAY;
int transPhase = PHASE_END;
float phase = 0;
float clearColor[] = { 135.0f / 255, 206.0f / 255, 235.0f / 255 };
GLfloat sunLightColor[] = { 1, 1, 1, 1 };
float sunRotation = 0;

GLuint ground, groundOrig, grass, grassOrig, ball, ballOrig, wall, wallOrig, sun, sunOrig, moon, moonOrig, earth, earthOrig;
GLuint wood, woodOrig, wood2, wood2Orig, wood2front, wood2frontOrig;
GLuint lamp;
GLuint metal;
GLuint flag;
GLuint water;
GLuint face;
GLuint skin;
float droop = 0.0;
int droop_index = 0;
float droplets_offset = -90;


// Sign, Flag, Bounce, WindSpinner, Carusel, ChainSwing, CoopSwing, SolarSys, Fountain
const vec3 positions[9] = { {2.95, 3.25, 19.0},	{12.8, 4.0, 19.54},  {-8.4, 2.75, -3.95}, {6.31, 2.6, -0.10}, {15.34, 4.75, 12.9} , {-0.038, 2.75, -7.86} , {-6.1, 6.0, 10.0}, {13.68, 2.25, -5.65}, {-3.5, 3.25, -4.95} };
const float angles_x[9] = { -15,		-16.5,	-22.5,	6,		-40.5,	-10.5,	-34.5,	-13.5,	-10.5 };
const float angles_y[9] = { 0,	348,	15,		270,	75,		0,		108,	34.5,	216 };
vec3 initialCameraPos;
int selected_model = 0;
float camera_phaze = 0, initialAngleX, initialAngleY;


int main(int argc, char** argv)
{
	int texture_menu, light_menu;

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
	glEnable(LIGHT_SIGN);
	glEnable(LIGHT_STREETLAMP1);
	glEnable(LIGHT_STREETLAMP2);
	glEnable(LIGHT_STREETLAMP3);
	glEnable(LIGHT_SOLAR_SYSTEM_SUN);
	glEnable(LIGHT_SUN);

	//registering callbacks
	glutDisplayFunc(drawingCB);
	glutReshapeFunc(reshapeCB);
	glutKeyboardFunc(keyboardCB);
	glutSpecialFunc(keyboardSpecialCB);

	// Start Position

	texture_menu = glutCreateMenu(menuCB);
	glutAddMenuEntry("Toggle ground texture", TEXTURE_GROUND);
	glutAddMenuEntry("Toggle bouncing ball textures", TEXTURE_BOUNCING_BALL);
	glutAddMenuEntry("Toggle solar system textures", TEXTURE_SOLAR_SYSTEM);
	glutAddMenuEntry("Toggle wood textures", TEXTURE_WOOD);

	light_menu = glutCreateMenu(menuCB);
	glutAddMenuEntry("Toggle sun light", LIGHT_SUN);
	glutAddMenuEntry("Toggle sign light", LIGHT_SIGN);
	glutAddMenuEntry("Toggle solar system light", LIGHT_SOLAR_SYSTEM_SUN);
	glutAddMenuEntry("Toggle bouncing ball light", LIGHT_BALL);
	glutAddMenuEntry("Toggle street lamp lights", LIGHT_STREETLAMP1);

	glutCreateMenu(menuCB);
	glutAddSubMenu("Lights", light_menu);
	glutAddSubMenu("Textures", texture_menu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	ground = groundOrig = load_texture("ground.bmp");
	grass = grassOrig = load_texture("grass.bmp");
	ball = ballOrig = load_texture("tex3.bmp");
	wall = wallOrig = load_texture("wall.bmp");
	sun = sunOrig = load_texture("sun.bmp");
	moon = moonOrig = load_texture("moon.bmp");
	earth = earthOrig = load_texture("earth.bmp");
	wood = woodOrig = load_texture("wood.bmp");
	wood2 = wood2Orig = load_texture("wood2.bmp");
	wood2front = wood2frontOrig = load_texture("wood2front.bmp");
	lamp = load_texture("lamp.bmp");
	flag = load_texture("flag.bmp");
	water = load_texture("water.bmp");
	metal = load_texture("metal.bmp");
	face = load_texture("face.bmp");
	skin = load_texture("skin.bmp");

	glBindTexture(GL_TEXTURE_2D, 0);

	glutTimerFunc(0, animateFlag, 0);
	glutTimerFunc(0, animateDroplets, 0);
	glutTimerFunc(0, animateSwingKids, 0);
	glutTimerFunc(0, animateWindSpinner, 0);
	glutTimerFunc(0, animateCoOpSwing, 0);
	glutTimerFunc(ANIMATION_DELAY, animateBouncingBall, 0);
	glutTimerFunc(ANIMATION_DELAY, animateSolarSystem, 0);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//starting main loop
	glutMainLoop();

}

void animateBouncingBall(int value)
{
	if (!animation)
		return;

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

void animateCamera(int value)
{
	const float step = 0.02;
	if (!animation)
		return;

	if (camera_phaze > 1 - step)
		camera_phaze = 1;

	cameraPos.x = lerp(initialCameraPos.x, positions[selected_model].x, camera_phaze);
	cameraPos.y = lerp(initialCameraPos.y, positions[selected_model].y, camera_phaze);
	cameraPos.z = lerp(initialCameraPos.z, positions[selected_model].z, camera_phaze);

	angleX = lerp(initialAngleX, angles_x[selected_model], camera_phaze);
	angleY = lerp(initialAngleY, angles_y[selected_model], camera_phaze);
	computeCameraVectors();
	glutPostRedisplay();

	camera_phaze += step;
	if (camera_phaze >= 1) {
		camera_phaze = 0;
		return;
	}
	glutTimerFunc(ANIMATION_DELAY, animateCamera, 0);
}

void animateCoOpSwing(int value)
{
	if (!animation)
		return;

	float length = 0.8;
	float time_period = 2 * PI * sqrt(length / 9.81);

	if (coOpSwingAng == 18 && isUp) {
		isUp = !isUp;
	}
	else if (coOpSwingAng == -18 && !isUp) {
		isUp = !isUp;
	}
	isUp ? (coOpSwingAng += 0.5) : (coOpSwingAng -= 0.5);

	if (time >= time_period) {
		time = 0;
	}

	glutPostRedisplay();
	glutTimerFunc(16, animateCoOpSwing, 0); // ~60 FPS
}

void animateDroplets(int value)
{
	if (!animation)
		return;

	droplets_offset--;
	if (droplets_offset <= -100) {
		droplets_offset = -90;
	}
	glutTimerFunc(16, animateDroplets, 0);
	glutPostRedisplay();
}

void animateFlag(int value)
{
	if (!animation)
		return;

	float droop_values[16] = { 0.005, 0.01, 0.02, 0.03, 0.05, 0.07, 0.09, 0.11, 0.12,
							0.11, 0.09, 0.07, 0.05, 0.03, 0.02, 0.01 }; // 16
	droop = droop_values[droop_index];
	droop_index++;
	droop_index = droop_index == 16 ? 0 : droop_index;
	glutTimerFunc(64, animateFlag, 0);
	glutPostRedisplay();
}

void animateTimeChange(int value)
{
	float day[] = { 135.0f / 255, 206.0f / 255, 235.0f / 255 };
	float twilight[] = { 218.0f / 255, 91.0f / 255, 20.0f / 255 };
	float night[] = { 19.0f / 255, 24.0f / 255, 98.0f / 255 };
	GLfloat sunlight[] = { 1, 1, 1, 1 };
	GLfloat nightlight[] = { 0, 0, 0, 1 };
	float* from, * to, fromAngle, toAngle;
	GLfloat* fromLight, * toLight;

	if (timeOfDay == TIME_NIGHT) {
		if (transPhase == PHASE_TO_TWILIGHT && animation) {
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
		if (transPhase == PHASE_TO_TWILIGHT && animation) {
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
	if (!animation) {
		phase = 1;
		transPhase = PHASE_TO_END;
	}
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

void animateWindSpinner(int value)
{
	if (!animation)
		return;

	rotationAngle += 2.0f; // Adjust rotation speed as needed
	rotationAngle = wrapAngle(rotationAngle, 360.0);

	glutPostRedisplay();
	glutTimerFunc(SIXTY_FPS, animateWindSpinner, 0); // ~60 FPS
}

void animateSolarSystem(int value)
{
	if (!animation)
		return;

	sunSelf = wrapAngle(sunSelf + CELESTIAL_SELF_SPIN, 360);
	earthSelf = wrapAngle(earthSelf + CELESTIAL_SELF_SPIN, 360);
	earthAngle = wrapAngle(earthAngle + CELESTIAL_SPIN, 360);
	moonAngle = wrapAngle(moonAngle + CELESTIAL_SPIN, 360);
	glutPostRedisplay();
	glutTimerFunc(ANIMATION_DELAY, animateSolarSystem, 0);
}

void animateSwingKids(int value)
{
	if (!animation)
		return;
	float length = 0.8;
	double omega = sqrt(9.81 / length);

	jointAngLeg = abs(90 * cos(omega * time));
	time += 16 * 0.001f;
	swingAng = SWING_MAX_ANG * cos(omega * time);

	glutPostRedisplay();
	glutTimerFunc(SIXTY_FPS, animateSwingKids, 0); // ~60 FPS
}

GLuint load_texture(char* name)
{
	int width, height;
	GLubyte* data = readBMP(name, &width, &height);
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

void resumeModulate(void)
{
	if (lighting)
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	else
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

}

void drawGround(void)
{
	int i, j;
	glBindTexture(GL_TEXTURE_2D, ground);
	glColor3f(0, 1, 0);
	glNormal3f(0, 1, 0);
	texture&& ground ? Paint(1, 1, 1) : Paint(0.3, 0, 0);
	for (i = -200; i < 200; i += 2) {
		for (j = -200; j < 200; j += 2) {
			if (i < -16 || i > 16 || j < -16 || j > 16)
				glBindTexture(GL_TEXTURE_2D, grass);
			else
				glBindTexture(GL_TEXTURE_2D, ground);
			glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex3f(i, 0, j);
			glTexCoord2f(1, 0);
			glVertex3f(i + 2, 0, j);
			glTexCoord2f(1, 1);
			glVertex3f(i + 2, 0, j + 2);
			glTexCoord2f(0, 1);
			glVertex3f(i, 0, j + 2);
			glEnd();
		}
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

void drawBouncingBall(void)
{
	GLfloat ballLight[] = { ballPosition, 0, 0, 1 };
	GLfloat ballColor[] = { 2, 0.7, 0.7, 1 };
	GLfloat ballAmbient[] = { 0.4, 0.4, 0.4, 1 };
	float scale = 1;
	float translatePos = ballPosition > 0 ? -0.4 : 0.4;
	GLUquadric* sphere = gluNewQuadric();
	gluQuadricTexture(sphere, GL_TRUE);

	texture&& ball ? PaintSpec(1, 1, 1) : PaintSpec(0, 0, 0);
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
	PaintSpec(1, 0, 0);
	gluSphere(sphere, 0.4, 30, 30);
	gluDeleteQuadric(sphere);
	glPopMatrix();

	resumeModulate();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void drawSolarSystem(void)
{
	GLUquadric* sphere = gluNewQuadric();
	GLUquadric* disk = gluNewQuadric();

	GLfloat emission1[] = { 0.984, 0.6666, 0.0, 1 };
	GLfloat emission2[] = { 0, 0, 0, 1 };
	GLfloat light_1_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_1_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_1_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	GLfloat light_1_position[] = { 0.0, 0.0, 0.0, 1.0 };


	gluQuadricTexture(sphere, GL_TRUE);
	glColor3f(0.4, 0.4, 0.4);

	glBindTexture(GL_TEXTURE_2D, sun);

	glPushMatrix();
	glRotatef(sunSelf, 0, 1, 0);

	glLightfv(LIGHT_SOLAR_SYSTEM_SUN, GL_DIFFUSE, light_1_diffuse);
	glLightfv(LIGHT_SOLAR_SYSTEM_SUN, GL_SPECULAR, light_1_specular);
	glLightfv(LIGHT_SOLAR_SYSTEM_SUN, GL_AMBIENT, light_1_ambient);
	glLightfv(LIGHT_SOLAR_SYSTEM_SUN, GL_POSITION, light_1_position);
	glLightf(LIGHT_SOLAR_SYSTEM_SUN, GL_QUADRATIC_ATTENUATION, 0.05);
	PaintSpec(0.984, 0.6666, 0.0);
	glMaterialfv(GL_FRONT, GL_EMISSION, emission1);
	gluSphere(sphere, 1, 30, 30);
	glMaterialfv(GL_FRONT, GL_EMISSION, emission2);

	glPopMatrix();

	glPushMatrix();
	glRotatef(earthAngle, 0, 1, 0);
	glTranslatef(3, 0, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	glDisable(GL_LIGHTING);
	glColor3f(1, 1, 1);
	gluDisk(disk, 0.95, 1, 30, 30);
	if (lighting)
		glEnable(GL_LIGHTING);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, earth);
	glPushMatrix();
	glRotatef(earthSelf, -1, 1, 0);

	PaintSpec(0.6, 0.6, 1.0);
	gluSphere(sphere, 0.4, 100, 100);

	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, moon);

	glRotatef(moonAngle, 0, 1, 0);
	glTranslatef(1, 0, 0);
	glRotatef(moonSelf, 0, 1, 0);
	PaintSpec(1.0, 1.0, 1.0);
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
	float boundery = 15;
	GLenum er;
	GLfloat light0Pos[] = { 0, 0, 0, 1 };
	GLfloat light0Color[] = { 1, 1, 1, 1 };
	GLfloat light0Ambient[] = { 0.2, 0.2, 0.2, 1 };
	GLfloat sunPos[] = { 0, 1, 0, 0 };
	vec3 center = vec3_add(cameraPos, cameraForward);
	vec3 up = { 0, 1, 0 };
	vec3 fenceA1 = { -boundery, 0, boundery }, fenceA2 = { -boundery, 0, -boundery };
	vec3 fenceB1 = { -boundery, 0, -boundery }, fenceB2 = { boundery, 0, -boundery };
	vec3 fenceC1 = { boundery, 0, -boundery }, fenceC2 = { boundery, 0, boundery };
	vec3 fenceD1 = { -boundery, 0, boundery }, fenceD2 = { -2, 0, boundery };
	vec3 fenceE1 = { 4.5, 0, boundery }, fenceE2 = { boundery + 0.45, 0, boundery };

	// Set initial position
	//gluLookAt(positions[0].x, positions[0].y - 3.5, positions[0].z - 6, 3.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	gluLookAt( 2.95, -0.6, 13.0 , 3.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	// Clearing the background
	glClearColor(clearColor[0], clearColor[1], clearColor[2], 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Initializing modelview transformation matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glLightfv(LIGHT_HEAD, GL_POSITION, light0Pos);
	glLightfv(LIGHT_HEAD, GL_DIFFUSE, light0Color);
	glLightfv(LIGHT_HEAD, GL_AMBIENT, light0Ambient);

	if (head_light) {
		glEnable(LIGHT_HEAD);
		glLightf(LIGHT_HEAD, GL_QUADRATIC_ATTENUATION, 0.03);
	}
	else {
		glDisable(LIGHT_HEAD);
	}
	if (angleX < -45)
		up = cameraForwardXZ;
	gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z, center.x, center.y, center.z, up.x, up.y, up.z);
	glPushMatrix();
	glRotatef(sunRotation, 0, 0, -1);
	glLightfv(LIGHT_SUN, GL_POSITION, sunPos);
	glLightfv(LIGHT_SUN, GL_DIFFUSE, sunLightColor);
	glPopMatrix();

	drawFence(fenceA1, fenceA2, 2);
	drawFence(fenceB1, fenceB2, 2);
	drawFence(fenceC1, fenceC2, 2);
	drawFence(fenceD1, fenceD2, 2);
	drawFence(fenceE1, fenceE2, 2);

	// Draw Israel Flag
	glPushMatrix();
	glTranslatef(boundery, 0, boundery);
	drawFlagPole();
	glPopMatrix();

	// Draw Israel Flag
	glPushMatrix();
	glTranslatef(-boundery, 0, boundery);
	drawFlagPole();
	glPopMatrix();

	// Draw Sign
	glPushMatrix();
	glTranslatef(3, 0, boundery);
	drawSign();
	glPopMatrix();

	// Draw WindSpinner
	glPushMatrix();
	glTranslatef(boundery - 1, 3, 0);
	glRotatef(180, 0.0, 1.0, 0.0);
	glScalef(2.0, 2.0, 2.0);
	drawWindSpinner();
	glPopMatrix();

	// Draw Fountain
	glPushMatrix();
	drawFountain();
	glPopMatrix();

	// Draw Swings
	glPushMatrix();
	glTranslatef(0.0, 1.0, -boundery + 3.0);
	drawSwings();
	glTranslatef(0.0, -1.0, -1.5);
	drawStreetLight(LIGHT_STREETLAMP3);
	glPopMatrix();

	// Draw Bouncing Ball
	glPushMatrix();
	glTranslatef(-boundery + 3, 1, -boundery + 3);
	glRotatef(45, 0, 1, 0);
	glScalef(2.0, 2.0, 2.0);
	drawBouncingBall();
	glPopMatrix();

	// Draw Solar System
	glPushMatrix();
	glTranslatef(boundery - 4.5, 1, -boundery + 4.5);
	drawSolarSystem();
	glPopMatrix();

	// Draw Carousel
	glPushMatrix();
	glTranslatef(boundery - 4, 0.0, boundery - 4);
	drawCarousel();
	glTranslatef(2, 0, 2.8);
	glRotatef(200, 0.0, 1.0, 0.0);
	drawStreetLight(LIGHT_STREETLAMP1);
	glPopMatrix();

	// Draw Co-Op Swing 
	glPushMatrix();
	glTranslatef(-boundery + 4, 1.0, boundery - 4);
	glRotatef(-45, 0, 1, 0);
	DrawCoOpSwing();
	glRotatef(180, 0, 1, 0);
	glTranslatef(0.0, -1, -2.0);
	drawStreetLight(LIGHT_STREETLAMP2);
	glPopMatrix();

	drawGround();
	glutSwapBuffers();

	// Check for errors
	er = glGetError();  //get errors. 0 for no error, find the error codes in: https://www.opengl.org/wiki/OpenGL_Error
	if (er) printf("error: %d\n", er);
}

void drawCarousel(void)
{
	int i;

	glPushMatrix();
	PaintSpec(187 / 255.0, 162 / 255.0, 60 / 255.0);

	glPushMatrix();
	glRotatef(rotationAngle, 0, 1, 0);
	glTranslatef(0, 0.3, 0); // initial height
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	glutSolidTorus(0.1, 2, 30, 60); // base torus
	glTranslatef(0, 0, 1.4);
	glutSolidTorus(0.1, 2, 30, 60); // heigher torus
	glPopMatrix();
	glPushMatrix();
	glScalef(1, 0.05, 1);
	glutSolidSphere(2.05, 20, 20); // base plate
	glPopMatrix();

	for (i = 0; i < 3; i++) {
		glPushMatrix();
		glRotatef(120 * i, 0, 1, 0);
		glTranslatef(2, 0, 0);
		VerticalCylinder(0.1, 1.4); // support columns
		glPopMatrix();
	}

	glTranslatef(0, 0.8, 0);
	for (i = -25; i < 265; i++) {
		glPushMatrix();
		glRotatef(i, 0, 1, 0);
		glTranslatef(1.7, 0, 0);
		glScalef(1, 0.2, 1);
		glutSolidSphere(0.25, 20, 20); // seat plate
		glPopMatrix();
	}


	glTranslatef(0, 0.3, -1.3);
	glScalef(1.2, 1.2, 1.2);
	DrawCaruselKid(1);
	glPopMatrix();

	glTranslatef(0, 0.3, 0);

	PaintSpec(27 / 255.0, 80 / 255.0, 160 / 255.0);

	VerticalCylinder(0.1, 1.3); // centre cylinder
	glPushMatrix();
	glTranslatef(0, 1.3, 0);
	glRotatef(90, 1, 0, 0);
	glutSolidTorus(0.05, 0.25, 20, 20);
	glRotatef(-90, 1, 0, 0);
	glScalef(1, 0.05, 1);
	glutSolidSphere(0.25, 20, 20); // centre plate ////
	glPopMatrix();


	//drawSwingKid(1);
	glPopMatrix();
}

void drawFountain(void)
{
	int i;
	GLUquadric* sphere = gluNewQuadric();
	gluQuadricTexture(sphere, GL_TRUE);
	glPushMatrix();
	glTranslatef(0, 0.0001, 0);

	Paint(0.7843, 0.7843, 0.7843);
	glBindTexture(GL_TEXTURE_2D, 0);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glPushMatrix();
	glScalef(1, 3.8, 1);
	glTranslatef(0, 0.15, 0);
	glRotatef(90, 1, 0, 0);
	glutSolidTorus(0.2, 2.85, 30, 30);

	glPopMatrix();

	VerticalCylinder(0.2, 2.8);
	glPushMatrix();
	glTranslatef(0, 2.8, 0);
	glRotatef(90, 1, 0, 0);
	glutSolidTorus(0.1, 0.25, 20, 20);
	glPopMatrix();

	resumeModulate();
	glBindTexture(GL_TEXTURE_2D, 0);


	///////// DROPLETS
	texture ? PaintSpec(1, 1, 1) : PaintSpec(0, 0, 1);
	for (i = 0; i < 10; i++) {
		glPushMatrix();
		glRotatef(36 * i, 0, 1, 0);
		glTranslatef(1, 0, 0);
		glScalef(0.5, 1, 1);
		drawDropletsOval();

		glPopMatrix();
	}

	glBindTexture(GL_TEXTURE_2D, water);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glPushMatrix();
	glTranslatef(0, 1, 0);
	glScalef(1, 0.1, 1);
	gluSphere(sphere, 2.85, 30, 30);
	gluDeleteQuadric(sphere);
	glPopMatrix();

	resumeModulate();
	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
}

void drawDropletsOval(void)
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
		if (timeOfDay == TIME_DAY)
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		glScalef(1, 2.5, 1);
		gluSphere(sphere, 0.02, 10, 10);

		resumeModulate();
		glBindTexture(GL_TEXTURE_2D, 0);

		glPopMatrix();

	}

	gluDeleteQuadric(sphere);

	glPopMatrix();
}

void drawFence(vec3 fromPoint, vec3 toPoint, float poleHeight)
{
	float poleDistance = 0.4f;
	float poleRadius = 0.1f;
	fromPoint.y = toPoint.y = 0;
	vec3 direction = vec3_sub(toPoint, fromPoint);
	float distance = vec3_dist(toPoint, fromPoint);
	float n_poles = floor(distance / poleDistance);
	vec3 normal_direction = vec3_scale(direction, 1 / distance);
	vec3 scaled_direction = vec3_scale(normal_direction, poleDistance);
	int i;
	PaintSpec(232.0 / 255.0, 192.0 / 255.0, 155.0 / 255.0);
	glPushMatrix();
	glTranslated(fromPoint.x, fromPoint.y, fromPoint.z);
	glBindTexture(GL_TEXTURE_2D, wood);
	glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	for (i = 0; i < n_poles; i++) {
		VerticalCylinder(poleRadius, poleHeight);
		glTranslatef(0, poleHeight, 0);
		glutSolidSphere(poleRadius, 10, 10);
		glTranslatef(0, -poleHeight, 0);
		glTranslated(scaled_direction.x, scaled_direction.y, scaled_direction.z);
	}
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glPopMatrix();
}

void drawFlagPole(void)
{
	float poleRadius = 0.1f;
	float poleHeight = 4.5;
	float w = 2;
	float h = w * 2 / 3;

	/* 1 in */ glPushMatrix();
	PaintSpec(232.0 / 255.0, 192.0 / 255.0, 155.0 / 255.0);
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

	PaintSpec(1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, flag);
	glBegin(GL_POLYGON);

	glNormal3f(0, 0, 1);
	glTexCoord2f(1, 1); glVertex3f(0, 0, 0);
	glTexCoord2f(0, 1); glVertex3f(-w, -droop, 0);
	glTexCoord2f(0, 0); glVertex3f(-w, -droop - h, 0);
	glTexCoord2f(1, 0); glVertex3f(0, -h, 0);
	glEnd();

	/* 1 out */ glPopMatrix();
}

void drawSign(void)
{
	float sign_width = 2;
	float sign_height = 2;
	float cylinder_height = 1;
	float cylinder_radius = 0.15;

	// TO BE FINISHED
	GLfloat light_1_specular[] = { 1, 1, 1, 1.0 };
	GLfloat light_1_diffuse[] = { 1, 1, 1, 1.0 };
	GLfloat light_1_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat light_1_position[] = { 0.0, 3.28, 0.88, 1.0 };
	GLfloat light_1_spotLight[] = { 0, -1, -0.577, 1 };
	GLfloat light_1_spotCutOff[] = { 25 };
	GLfloat emission1[] = { 1, 1, 1, 1 };
	GLfloat emission2[] = { 0, 0, 0, 1 };

	glLightfv(LIGHT_SIGN, GL_DIFFUSE, light_1_diffuse);
	glLightfv(LIGHT_SIGN, GL_SPECULAR, light_1_specular);
	glLightfv(LIGHT_SIGN, GL_AMBIENT, light_1_ambient);
	glLightfv(LIGHT_SIGN, GL_POSITION, light_1_position);
	glLightfv(LIGHT_SIGN, GL_SPOT_DIRECTION, light_1_spotLight);
	glLightfv(LIGHT_SIGN, GL_SPOT_CUTOFF, light_1_spotCutOff);

	PaintSpec(232.0 / 255.0, 192.0 / 255.0, 155.0 / 255.0);

	glBindTexture(GL_TEXTURE_2D, wood2front);
	glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	resumeModulate();

	glBegin(GL_POLYGON);
	glNormal3f(0.0, 0.0, 1.0);
	glTexCoord2f(0, 0); glVertex3f(-sign_width / 2, cylinder_height, 0.05f);
	glTexCoord2f(1, 0); glVertex3f(sign_width / 2, cylinder_height, 0.05f);
	glTexCoord2f(1, 1); glVertex3f(sign_width / 2, cylinder_height + sign_height, 0.05f);
	glTexCoord2f(0, 1); glVertex3f(-sign_width / 2, cylinder_height + sign_height, 0.05f);
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
	glNormal3f(0.0, 0.0, -1.0);
	glVertex3f(-sign_width / 2, cylinder_height, -0.05f);
	glVertex3f(+sign_width / 2, cylinder_height, -0.05f);
	glVertex3f(+sign_width / 2, cylinder_height + sign_height, -0.05f);
	glVertex3f(-sign_width / 2, cylinder_height + sign_height, -0.05f);
	glEnd();

	// Right
	glBegin(GL_POLYGON);
	glNormal3f(1.0, 0.0, 0.0);
	glVertex3f(sign_width / 2 + 0.001f, cylinder_height, -0.05f);
	glVertex3f(sign_width / 2 + 0.001f, cylinder_height, +0.05f);
	glVertex3f(sign_width / 2 + 0.001f, cylinder_height + sign_height, +0.05f);
	glVertex3f(sign_width / 2 + 0.001f, cylinder_height + sign_height, -0.05f);
	glEnd();

	// Left
	glBegin(GL_POLYGON);
	glNormal3f(-1.0, 0.0, 0.0);
	glVertex3f(-sign_width / 2 + 0.001f, cylinder_height, -0.05f);
	glVertex3f(-sign_width / 2 + 0.001f, cylinder_height, +0.05f);
	glVertex3f(-sign_width / 2 + 0.001f, cylinder_height + sign_height, +0.05f);
	glVertex3f(-sign_width / 2 + 0.001f, cylinder_height + sign_height, -0.05f);
	glEnd();

	// Top
	glBegin(GL_POLYGON);
	glNormal3f(0.0, 1.0, 0.0);
	glVertex3f(+sign_width / 2, cylinder_height + sign_height, +0.05f);
	glVertex3f(-sign_width / 2, cylinder_height + sign_height, +0.05f);
	glVertex3f(-sign_width / 2, cylinder_height + sign_height, -0.05f);
	glVertex3f(+sign_width / 2, cylinder_height + sign_height, -0.05f);
	glEnd();

	//disabling automatic texture coordinates generation
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);


	//wood posts texture
	glBindTexture(GL_TEXTURE_2D, wood);
	glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);

	// left-side post
	/* 1 in */ glPushMatrix();
	glTranslatef(-(sign_width / 2) - cylinder_radius, 0, 0);
	VerticalCylinder(cylinder_radius, cylinder_height + sign_height);
	glTranslatef(0, (cylinder_height + sign_height), 0);
	glutSolidSphere(cylinder_radius, 20, 20);
	glTranslatef(0, -(cylinder_height + sign_height), 0);
	/* 1 out */ glPopMatrix();

	// right-side post
	/* 2 in */ glPushMatrix();
	glTranslatef(sign_width / 2 + cylinder_radius, 0, 0);
	VerticalCylinder(cylinder_radius, cylinder_height + sign_height);
	glTranslatef(0, (cylinder_height + sign_height), 0);
	glutSolidSphere(cylinder_radius, 20, 20);
	glTranslatef(0, -(cylinder_height + sign_height), 0);
	/* 2 out */ glPopMatrix();

	//middle thick post
	/* 3 in */ glPushMatrix();
	glTranslatef(0, 0, -0.05f - cylinder_radius * 2);
	VerticalCylinder(cylinder_radius, cylinder_height + sign_height + 0.7);
	glTranslatef(0, (cylinder_height + sign_height + 0.7), 0);
	glutSolidSphere(cylinder_radius, 20, 20);
	glTranslatef(0, -(cylinder_height + sign_height + 0.7f), 0);
	glTranslatef(0, cylinder_height + sign_height + 0.5, -cylinder_radius);

	//elbow stick
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	VerticalCylinder(cylinder_radius * 0.6, 1.5);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

	//bolb house
	glTranslated(0, 0.05, 1.54);
	glutSolidSphere(cylinder_radius * 0.6, 20, 20);
	glRotatef(210.0f, 1.0f, 0.0f, 0.0f);
	VerticalCylinder(cylinder_radius * 0.6, 0.3);
	glTranslated(0, 0.3, 0);

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glBindTexture(GL_TEXTURE_2D, lamp);
	glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);

	glMaterialfv(GL_FRONT, GL_EMISSION, emission1);
	glutSolidSphere(0.085, 20, 20);
	glMaterialfv(GL_FRONT, GL_EMISSION, emission2);

	/*
	GLfloat pos[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, pos);
	printf("x = %f, y = %f, z = %f\n", pos[12], pos[13] ,pos[14]);
	*/

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glBindTexture(GL_TEXTURE_2D, 0);

	/* 3 out */ glPopMatrix();
}

vec3 getPos() {
	GLfloat pos[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, pos);
	vec3 res = { pos[12], pos[13] ,pos[14] };

	return res;
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

	//Update viewport
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

void computeCameraVectors(void)
{
	float cosa, sina;
	vec3 camFacing = { 0, 0, -1 };
	vec3 camFacingXZ = { 0, 0, -1 };
	vec3 temp;

	if (angleX > 90)
		angleX = 90;
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

void menuCB(int value)
{
	switch (value) {
	case LIGHT_SUN:
		sunEnable = !sunEnable;
		sunEnable ? glEnable(LIGHT_SUN) : glDisable(LIGHT_SUN);
		break;
	case LIGHT_BALL:
		ballEnable = !ballEnable;
		ballEnable ? glEnable(LIGHT_BALL) : glDisable(LIGHT_BALL);
		break;
	case LIGHT_SIGN:
		signEnable = !signEnable;
		signEnable ? glEnable(LIGHT_SIGN) : glDisable(LIGHT_SIGN);
		break;
	case LIGHT_STREETLAMP1:
		lampEnable = !lampEnable;
		if (lampEnable) {
			glEnable(LIGHT_STREETLAMP1);
			glEnable(LIGHT_STREETLAMP2);
			glEnable(LIGHT_STREETLAMP3);
		}
		else {
			glDisable(LIGHT_STREETLAMP1);
			glDisable(LIGHT_STREETLAMP2);
			glDisable(LIGHT_STREETLAMP3);
		}
		break;
	case LIGHT_SOLAR_SYSTEM_SUN:
		solarEnable = !solarEnable;
		solarEnable ? glEnable(LIGHT_SOLAR_SYSTEM_SUN) : glDisable(LIGHT_SOLAR_SYSTEM_SUN);
		break;
	case TEXTURE_GROUND:
		ground = ground ? 0 : groundOrig;
		grass = grass ? 0 : grassOrig;
		break;
	case TEXTURE_BOUNCING_BALL:
		ball = ball ? 0 : ballOrig;
		break;
	case TEXTURE_SOLAR_SYSTEM:
		sun = sun ? 0 : sunOrig;
		moon = moon ? 0 : moonOrig;
		earth = earth ? 0 : earthOrig;
		break;
	case TEXTURE_WOOD:
		wood = wood ? 0 : woodOrig;
		wood2 = wood2 ? 0 : wood2Orig;
		wood2front = wood2front ? 0 : wood2frontOrig;
		break;
	}

	glutPostRedisplay();
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
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
		if (!camera_phaze) {
			initialCameraPos = cameraPos;
			selected_model = key - '0';
			if (abs(angleY - angles_y[selected_model]) > 180) {
				initialAngleY = angleY + ((angleY > angles_y[selected_model]) ? -360 : 360);
			}
			else {
				initialAngleY = angleY;
			}
			initialAngleX = angleX;
			glutTimerFunc(ANIMATION_DELAY, animateCamera, 0);
		}
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
		if (cameraPos.y < 1.25)
			cameraPos.y = 1.25;
		glutPostRedisplay();
		break;
	case 'q':
		if (angleX > -10)
			return;
		angleY = wrapAngle(angleY - CAMERA_ROTATION_SPEED, 360);
		rotateAroundPoint();
		glutPostRedisplay();
		break;
	case 'e':
		if (angleX > -10)
			return;
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
		break;
	case 't':
		texture = !texture;
		texture ? glEnable(GL_TEXTURE_2D) : glDisable(GL_TEXTURE_2D);
		glutPostRedisplay();
		break;
	case 'n':
		animation = !animation;
		if (animation) {
			glutTimerFunc(0, animateSwingKids, 0);
			glutTimerFunc(0, animateWindSpinner, 0);
			glutTimerFunc(0, animateCoOpSwing, 0);
			glutTimerFunc(0, animateFlag, 0);
			glutTimerFunc(0, animateDroplets, 0);
			glutTimerFunc(ANIMATION_DELAY, animateBouncingBall, 0);
			glutTimerFunc(ANIMATION_DELAY, animateSolarSystem, 0);
		}
		break;
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

	computeCameraVectors();
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
GLubyte* readBMP(char* imagepath, int* width, int* height)
{
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int imageSize;   // = width*height*3
	unsigned char* data;
	unsigned char tmp;
	int i;

	// Open the file
	FILE* file = fopen(imagepath, "rb");
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
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	*width = *(int*)&(header[0x12]);
	*height = *(int*)&(header[0x16]);

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

void TerminationErrorFunc(char* ErrorString)
{
	char string[256];
	printf(ErrorString);
	fgets(string, 256, stdin);

	exit(0);
}

void drawWindSpinner(void)
{
	float CylinderHeight = 3.0f;
	float CylinderRadius = 0.1f;

	glPushMatrix();
	glTranslatef(0, -1.5, 0);
	glPushMatrix();

	PolishedSilverColor();
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

	RubyColor();

	glutSolidSphere(0.1, 20, 20);
	DrawSpinner();
	glPopMatrix();

	glPopMatrix();
}

void drawStreetLight(GLenum srcLight)
{
	GLUquadric* quadric;
	quadric = gluNewQuadric();

	// lamp
	GLfloat light_1_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_1_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_1_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	GLfloat light_1_position[] = { 0.0, 4.0, 0.9, 1.0 };
	GLfloat light_1_spotLight[] = { 0, -1, 0, 1 };
	GLfloat light_1_spotCutOff[] = { 55 };
	GLfloat emission1[] = { 1, 1, 1, 1 };
	GLfloat emission2[] = { 0, 0, 0, 1 };

	glLightfv(srcLight, GL_DIFFUSE, light_1_diffuse);
	glLightfv(srcLight, GL_SPECULAR, light_1_specular);
	glLightfv(srcLight, GL_AMBIENT, light_1_ambient);
	glLightfv(srcLight, GL_POSITION, light_1_position);
	glLightfv(srcLight, GL_SPOT_DIRECTION, light_1_spotLight);
	glLightfv(srcLight, GL_SPOT_CUTOFF, light_1_spotCutOff);

	PolishedSilverColor();

	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, metal);
	glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);

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

	glBindTexture(GL_TEXTURE_2D, lamp);
	glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	PaintSpec(232.0 / 255.0, 192.0 / 255.0, 155.0 / 255.0);
	glMaterialfv(GL_FRONT, GL_EMISSION, emission1);
	glutSolidSphere(0.12, 20, 20);
	glMaterialfv(GL_FRONT, GL_EMISSION, emission2);

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	gluDeleteQuadric(quadric);

	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void drawSwings(void)
{
	GLUquadric* quadric;
	glPushMatrix();
	glTranslated(0, -1, 0);

	glBindTexture(GL_TEXTURE_2D, wood);
	PaintSpec(232.0 / 255.0, 192.0 / 255.0, 155.0 / 255.0);

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

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glPushMatrix();

	glTranslated(0, 2.83, 0);

	// Draw Swing Left
	glPushMatrix();
	glTranslated(-1.5, 0, 0);
	glRotatef(swingAng, 1, 0, 0);
	drawSwing();
	glTranslatef(0.5, -1.85, 0);
	glScalef(0.9, 0.9, 0.9);
	drawSwingKid(0); // Draw girl on swing
	glPopMatrix();

	// Draw Swing Right
	glPushMatrix();
	glTranslated(0.5, 0, 0);
	glRotatef(-swingAng, 1, 0, 0);
	drawSwing();
	glTranslatef(0.5, -1.85, 0);
	glScalef(0.9, 0.9, 0.9);
	drawSwingKid(1); // Draw boy on swing
	glPopMatrix();

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
		y += -0.035 * 2;
	}
	glPopMatrix();

	glPopMatrix();

	return y;
}

void drawSwing(void)
{
	float y;

	glPushMatrix();
	PolishedSilverColor();

	// Chain Left
	glBindTexture(GL_TEXTURE_2D, metal);
	glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	y = DrawChains(30);


	glBindTexture(GL_TEXTURE_2D, wood);
	PaintSpec(232.0 / 255.0, 192.0 / 255.0, 155.0 / 255.0);

	// Draw Banch
	glPushMatrix();
	glTranslatef(-0.1, y, -0.27);
	DrawSquare(1.2, 0.1, 0.5);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, metal);
	PolishedSilverColor();
	// Chain Right
	glTranslated(1, 0, 0);
	DrawChains(30);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glPopMatrix();


}

void DrawHead(void)
{
	GLUquadric* quadric;
	glPushMatrix();
	Paint(236.0 / 255, 174.0 / 255, 131.0 / 255);
	glPushMatrix();
	glTranslatef(0.0, 0.65, 0.0);
	glutSolidSphere(0.1, 10, 10);// neck
	glTranslatef(0.0, 0.3, 0.0);
	glBindTexture(GL_TEXTURE_2D, face);
	glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glutSolidSphere(0.23, 10, 10);// head
	glPopMatrix();
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, skin);
	glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTranslatef(0.0, 0.6, 0.0);
	quadric = gluNewQuadric();
	gluQuadricTexture(quadric, GL_TRUE);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadric, 0.25, 0.05, 0.1, 32, 32);
	gluDeleteQuadric(quadric);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glPopMatrix();
	glPopMatrix();
}

void DrawBody(float radius, float height, int gender)
{
	glPushMatrix();
	GLUquadric* quadric;

	if (gender) {
		Paint(0.0, 0.0, 1.0);
		VerticalCylinder(radius, height);
		glutSolidSphere(0.25, 10, 10);
	}
	else {
		Paint(1.0, 0.0, 1.0);
		quadric = gluNewQuadric();
		gluQuadricTexture(quadric, GL_TRUE);
		glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
		glTranslatef(0.0, 0.0, -0.2);
		gluCylinder(quadric, radius + 0.2, radius, height + 0.2, 32, 32);
		gluDeleteQuadric(quadric);
	}
	glPopMatrix();

}

void Drawlimb(float radius, float height)
{
	GLUquadric* quadric;
	//draw pill shaped limb in the pos Z direction
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, skin);
	glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	Paint(236.0 / 255, 174.0 / 255, 131.0 / 255);
	quadric = gluNewQuadric();
	gluCylinder(quadric, radius, radius, height, 32, 32);
	gluDeleteQuadric(quadric);
	glutSolidSphere(radius, 10, 10);
	glPushMatrix();
	glTranslatef(0, 0, height);
	glutSolidSphere(radius, 10, 10);
	glPopMatrix();
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glPopMatrix();
}

void DrawLeg(int gender, int isAnim)
{
	float thighLength = 0.5;
	float shockLength = 0.55;
	float footLength = 0.2;
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, skin);
	glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	Paint(236.0 / 255, 174.0 / 255, 131.0 / 255);
	//theigh
	Drawlimb(0.12, thighLength);
	glTranslatef(0, 0, thighLength);
	//shock
	if (isAnim == 1)
		gender == 1 ? glRotatef(jointAngLeg, 1.0, 0.0, 0.0) : glRotatef(90 - jointAngLeg, 1.0, 0.0, 0.0);
	else
		glRotatef(90, 1.0, 0.0, 0.0);
	Drawlimb(0.1, shockLength);
	glTranslatef(0.0, 0.0, shockLength);
	glRotatef(-90, 1.0, 0.0, 0.0);
	//foot
	Drawlimb(0.12, footLength);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glPopMatrix();
}

void DrawArm(void)
{
	float armLenght = 0.5;
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, skin);
	glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	Paint(236.0 / 255, 174.0 / 255, 131.0 / 255);
	glutSolidSphere(0.13, 10, 10); // Shoulder
	Drawlimb(0.1, armLenght);
	glTranslatef(0.0, 0.0, armLenght);
	glutSolidSphere(0.12, 10, 10); // Palm
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glPopMatrix();
}




void DrawSquare(float xLen, float yLen, float zLen)
{
	glPushMatrix();

	// Back
	glBegin(GL_POLYGON);
	glNormal3f(0, 0, -1);
	glVertex3f(0, 0, 0);
	glVertex3f(xLen, 0, 0);
	glVertex3f(xLen, yLen, 0);
	glVertex3f(0, yLen, 0);
	glEnd();

	// Front
	glBegin(GL_POLYGON);
	glNormal3f(0, 0, 1);
	glVertex3f(0.0, 0.0, zLen);
	glVertex3f(xLen, 0.0, zLen);
	glVertex3f(xLen, yLen, zLen);
	glVertex3f(0.0, yLen, zLen);
	glEnd();

	// Right
	glBegin(GL_POLYGON);
	glNormal3f(1, 0, 0);
	glVertex3f(xLen, 0.0, 0.0);
	glVertex3f(xLen, yLen, 0.0);
	glVertex3f(xLen, yLen, zLen);
	glVertex3f(xLen, 0.0, zLen);
	glEnd();

	// Left
	glBegin(GL_POLYGON);
	glNormal3f(-1, 0, 0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, zLen);
	glVertex3f(0.0, yLen, zLen);
	glVertex3f(0.0, yLen, 0.0);
	glEnd();

	// Top
	glBegin(GL_POLYGON);
	glNormal3f(0, 1, 0);
	glVertex3f(0.0, yLen, 0.0);
	glVertex3f(0.0, yLen, zLen);
	glVertex3f(xLen, yLen, zLen);
	glVertex3f(xLen, yLen, 0.0);
	glEnd();

	// Down
	glBegin(GL_POLYGON);
	glNormal3f(0, -1, 0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, zLen);
	glVertex3f(xLen, 0.0, zLen);
	glVertex3f(xLen, 0.0, 0.0);
	glEnd();

	glPopMatrix();

}

void DrawCoOpSwing(void)
{
	GLUquadric* quadric;
	PaintSpec(1, 1, 0);
	glPushMatrix();
	glRotatef(coOpSwingAng, 0, 0, 1);
	DrawSquare(3.5, 0.1, 0.6); // half swing
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glScalef(1, 1, -1);
	DrawSquare(3.5, 0.1, 0.6);// half swing 2
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0, -0.05, -0.4);
	quadric = gluNewQuadric();
	gluQuadricTexture(quadric, GL_TRUE);
	gluCylinder(quadric, 0.1, 0.1, 1.4, 32, 32);
	gluDeleteQuadric(quadric);
	glutSolidSphere(0.15, 10, 10);
	glTranslatef(0.0, 0.0, 1.4);
	glutSolidSphere(0.15, 10, 10);
	glPopMatrix();

	//bench 1
	glPushMatrix();
	PaintSpec(200.0 / 255.0, 160.0 / 255.0, 120.0 / 255.0);
	glTranslatef(-3.5, 0, 0);
	DrawSquare(0.1, 0.7, 0.6);
	glTranslatef(0.9, 0.0, 0.05);
	VerticalCylinder(0.05, 0.6);
	glTranslatef(0.0, 0.0, 0.5);
	VerticalCylinder(0.05, 0.6);
	glTranslatef(0.0, 0.6, -0.55);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	VerticalCylinder(0.07, 0.6);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glutSolidSphere(0.08, 10, 10);
	glTranslatef(0.0, 0.0, 0.6);
	glutSolidSphere(0.08, 10, 10);
	glPopMatrix();
	//bench 2
	glPushMatrix();
	glTranslatef(-3.5, 0, 0);
	glRotatef(180.0, 0.0, 1.0, 0.0);
	glTranslatef(-7.0, 0.0, -0.6);
	DrawSquare(0.1, 0.7, 0.6);
	glTranslatef(0.9, 0.0, 0.05);
	VerticalCylinder(0.05, 0.6);
	glTranslatef(0.0, 0.0, 0.5);
	VerticalCylinder(0.05, 0.6);
	glTranslatef(0.0, 0.6, -0.55);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	VerticalCylinder(0.07, 0.6);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glutSolidSphere(0.08, 10, 10);
	glTranslatef(0.0, 0.0, 0.6);
	glutSolidSphere(0.08, 10, 10);
	glPopMatrix();
	glPushMatrix();
	glRotatef(90.0, 0.0, 1.0, 0.0);
	glTranslatef(-0.3, 0.35, -2.85);
	DrawCoOpSwingKid(0);
	glRotatef(180, 0.0, 1.0, 0.0);
	glTranslatef(0.0, 0.0, -5.8);
	DrawCoOpSwingKid(1);
	glPopMatrix();
	glPopMatrix();
	// Left Stand
	PaintSpec(0.7, 0.7, 0.7);
	glPushMatrix();
	glTranslatef(-0.25, -1, -0.2);
	DrawSquare(0.5, 1.5, 0.1);
	// Right Stand
	glTranslatef(0, 0, 0.9);
	DrawSquare(0.5, 1.5, 0.1);
	glPopMatrix();

}

void drawSwingKid(int gender)
{
	vec3 legPos = { 0.14, -0.1, 0.0 };
	vec3 armPos = { 0.23, 0.5, 0.0 };

	glPushMatrix();
	// Body
	glTranslatef(0.0, -0.05, -0.18);
	DrawBody(0.25, 0.6, gender);
	// Head
	DrawHead();
	// Left-leg
	glPushMatrix();
	glTranslatef(legPos.x, legPos.y, legPos.z);
	DrawLeg(gender, 1);
	glPopMatrix();
	// Right-leg
	glPushMatrix();
	glTranslatef(-legPos.x, legPos.y, legPos.z);
	DrawLeg(gender, 1);
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

void DrawCoOpSwingKid(int gender) {
	vec3 legPos = { 0.14, -0.1, 0.0 };
	vec3 armPos = { 0.23, 0.5, 0.0 };
	glPushMatrix();
	// Body
	glTranslatef(0.0, -0.05, -0.18);
	DrawBody(0.25, 0.6, gender);
	// Head
	DrawHead();
	// Left-leg
	glPushMatrix();
	glTranslatef(legPos.x, legPos.y, legPos.z);
	glRotatef(30, 0.0, 1.0, 0.0);
	DrawLeg(gender, 1);
	glPopMatrix();
	// Right-leg
	glPushMatrix();
	glTranslatef(-legPos.x, legPos.y, legPos.z);
	glRotatef(-30, 0.0, 1.0, 0.0);
	DrawLeg(gender, 1);
	glPopMatrix();
	// Left-arm
	glPushMatrix();
	glTranslatef(armPos.x, armPos.y, armPos.z);
	glRotatef(10.0, 1.0, 0.0, 0.0);
	DrawArm();
	glPopMatrix();
	// Right-arm
	glPushMatrix();
	glTranslatef(-armPos.x, armPos.y, armPos.z);
	glRotatef(10.0, 1.0, 0.0, 0.0);
	DrawArm();
	glPopMatrix();
	glPopMatrix();
}

void DrawCaruselKid(int gender) {
	vec3 legPos = { 0.14, -0.1, 0.0 };
	vec3 armPos = { 0.23, 0.5, 0.0 };

	glPushMatrix();
	// Body
	glTranslatef(0.0, -0.05, -0.18);
	DrawBody(0.25, 0.6, gender);
	// Head
	DrawHead();
	// Left-leg
	glPushMatrix();
	glTranslatef(legPos.x, legPos.y, legPos.z);
	glRotatef(30, 0.0, 1.0, 0.0);
	DrawLeg(gender, 0);
	glPopMatrix();
	// Right-leg
	glPushMatrix();
	glTranslatef(-legPos.x, legPos.y, legPos.z);
	glRotatef(-30, 0.0, 1.0, 0.0);
	DrawLeg(gender, 0);
	glPopMatrix();
	// Left-arm
	glPushMatrix();
	glTranslatef(armPos.x, armPos.y, armPos.z);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glRotatef(30.0, 0.0, 1.0, 0.0);
	DrawArm();
	glPopMatrix();
	// Right-arm
	glPushMatrix();
	glTranslatef(-armPos.x, armPos.y, armPos.z);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glRotatef(-30.0, 0.0, 1.0, 0.0);
	DrawArm();
	glPopMatrix();
	glPopMatrix();
}

void GoldColor(void) {
	// Gold Color
	GLfloat mat_ambient1[] = { 0.24725, 0.2245, 0.0645, 1.0 };
	GLfloat mat_diffuse1[] = { 0.34615, 0.3143, 0.0903, 1.0 };
	GLfloat mat_specular1[] = { 0.628281, 0.555802, 0.366065, 1.0 };
	GLfloat mat_shininess1[] = { 51.2 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient1);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse1);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular1);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess1);
}

void PolishedSilverColor(void) {
	// Polished Silver Color
	GLfloat mat_ambient[] = { 0.23125, 0.23125, 0.23125, 1.0 };
	GLfloat mat_diffuse[] = { 0.2775, 0.2775, 0.2775, 1.0 };
	GLfloat mat_specular[] = { 0.773911, 0.773911, 0.773911, 1.0 };
	GLfloat mat_shininess[] = { 51.22 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glColor3f(0.2775, 0.2775, 0.2775);
}

void RubyColor(void)
{
	// Ruby Color
	GLfloat mat_ambient2[] = { 0.1745, 0.01175, 0.01175, 0.55 };
	GLfloat mat_diffuse2[] = { 0.61424, 0.04136, 0.04136, 0.55 };
	GLfloat mat_specular2[] = { 0.727811, 0.626959, 0.626959, 0.55 };
	GLfloat mat_shininess2[] = { 76.8 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient2);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse2);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular2);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess2);
	glColor3f(0.61424, 0.04136, 0.04136);
}

void EmeraldColor(void)
{
	// Emerald Color
	GLfloat mat_ambient1[] = { 0.0215, 0.1745, 0.0215, 0.55 };
	GLfloat mat_diffuse1[] = { 0.07568, 0.61424, 0.07568, 0.55 };
	GLfloat mat_specular1[] = { 0.633, 0.727811, 0.633, 0.55 };
	GLfloat mat_shininess1[] = { 76.8 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient1);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse1);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular1);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess1);
}

void Paint(float r, float g, float b)
{
	GLfloat mat[] = { r, g, b, 1.0 };
	GLfloat matSpec[] = { 0, 0, 0, 1.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
	glMaterialf(GL_FRONT, GL_SHININESS, 0);
	glColor3f(r, g, b);

}

void PaintSpec(float r, float g, float b)
{
	GLfloat mat[] = { r, g, b, 1.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat);
	glColor3f(r, g, b);

}
