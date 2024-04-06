#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"GL/glut.h"

#define PI 3.14159265359f
#define CAMERA_ROTATION_SPEED 1.5f
#define CAMERA_SPEED 0.25f
#define CAMERA_MODEL 0
#define CAMERA_FREE 1
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
void TimerCB(int value);
GLubyte *readBMP(char *imagepath, int *width, int *height, int flip);
void TerminationErrorFunc(char *ErrorString);
GLuint load_texture(char *name, int flip);
void DrawWindSpinner();
void DrawSpinner();
void VerticalCylinder(float radius, float height);
void drawBlade();
void DrawSpinner();
void update(int value);
void DrawStreetLight(void);
void DrawSwings(void);
float DrawChains(int length);
void DrawSwing(void);
void DrawSwingKid(int sex);
void DrawHead(void);
void DrawArm(void);
void DrawLeg(void);


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
float xBP, yBP, zBP;
float xLB, yLB, zLB;
int isLeg = 1;


/// <summary>
/// //////////////
/// </summary>
float jointAngLeg = 30;


// Global variables
float rotationAngle = 0.0f;

GLuint ground;
GLuint metal;
GLuint lamp;
GLuint boyFace;




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

	//enable depth testing
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHT1);

	boyFace = load_texture("boyFace.bmp", 1);




	//registering callbacks
	glutDisplayFunc(drawingCB);
	glutReshapeFunc(reshapeCB);
	glutKeyboardFunc(keyboardCB);
	glutSpecialFunc(keyboardSpecialCB);

	boyFace = load_texture("boyFace.bmp", 1);
	ground = load_texture("ground.bmp", 1);
	//metal = load_texture("metal.bmp", 1);
	lamp = load_texture("lamp.bmp", 1);
	glBindTexture(GL_TEXTURE_2D, 0);

	glutTimerFunc(0, update, 0); // Start the update timer

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//starting main loop
	glutMainLoop();

}

GLuint load_texture(char *name, int flip)
{
	int width, height;
	GLubyte *data = readBMP(name, &width, &height, flip);
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

void drawGround(void)
{
	int i, j;
	glBindTexture(GL_TEXTURE_2D, ground);
	glColor3f(0, 0.3, 0);
	glNormal3f(0, 1, 0);
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

void drawingCB(void)
{
	GLenum er;
	GLfloat light0Pos[] = { 0, 0, 0, 1 };
	GLfloat light0Color[] = { 1, 1, 1, 1 };
	GLfloat light0Ambient[] = { 0.2, 0.2, 0.2, 1 };
	vec3 center = vec3_add(cameraPos, cameraForward);
	vec3 up = { 0, 1, 0 };

	//clearing the background
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//initializing modelview transformation matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Color);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0Ambient);

	if (camera_mode == CAMERA_MODEL) {
		glEnable(GL_LIGHT0);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0);

		glTranslatef(0, 0, -radius);
		glRotatef(angleX, 1, 0, 0);
		glRotatef(angleY, 0, 1, 0);
	}

	if (camera_mode == CAMERA_FREE) {
		if (head_light) {
			glEnable(GL_LIGHT0);
			glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.007);
		}
		else {
			glDisable(GL_LIGHT0);
		}
		if (angleX < -45)
			up = cameraForwardXZ;
		gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z, center.x, center.y, center.z, up.x, up.y, up.z);
		drawGround();
	}

	
	/*
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
	*/

	glPushMatrix();
	//DrawLeg();
	//DrawArm();
	DrawSwings();
	glPopMatrix();
	//swapping buffers and displaying
	glutSwapBuffers();

	//check for errors
	er = glGetError();  //get errors. 0 for no error, find the error codes in: https://www.opengl.org/wiki/OpenGL_Error
	if (er) printf("error: %d\n", er);
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

void keyboardCB(unsigned char key, int x, int y) {
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

void TimerCB(int value)
{
	
}

// Function to load bmp file
// buffer for the image is allocated in this function, you should free this buffer
GLubyte *readBMP(char *imagepath, int *width, int *height, int flip)
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

	if (flip) {
		//swap the r and b values to get RGB (bitmap is BGR)
		for (i = 0; i < *width * *height; i++)
		{
			tmp = data[i * 3];
			data[i * 3] = data[i * 3 + 2];
			data[i * 3 + 2] = tmp;
		}
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
	glPushMatrix();
	glTranslatef(0, -1.5, 0);
	float CylinderHeight = 3.0f;
	float CylinderRadius = 0.1f;

	glPushMatrix();
	// Polished Silver Color
	GLfloat mat_ambient[] = { 0.23125, 0.23125, 0.23125, 1.0 };
	GLfloat mat_diffuse[] = { 0.2775, 0.2775, 0.2775, 1.0 };
	GLfloat mat_specular[] = { 0.773911, 0.773911, 0.773911, 1.0 };
	GLfloat mat_shininess[] = { 51.22 };

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
	// Ruby Color
	GLfloat mat_ambient2[] = { 0.1745, 0.01175, 0.01175, 0.55 };
	GLfloat mat_diffuse2[] = { 0.61424, 0.04136, 0.04136, 0.55 };
	GLfloat mat_specular2[] = { 0.727811, 0.626959, 0.626959, 0.55 };
	GLfloat mat_shininess2[] = { 76.8 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient2);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse2);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular2);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess2);

	glutSolidSphere(0.1, 20, 20);
	DrawSpinner();
	glPopMatrix();

	glPopMatrix();
}

void VerticalCylinder(float radius, float height) {
	glPushMatrix();
	GLUquadricObj* quadratic;
	quadratic = gluNewQuadric();
	gluQuadricTexture(quadratic, GL_TRUE);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadratic, radius, radius, height, 32, 32);
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
	glPushMatrix();
	glRotatef(rotationAngle, 1.0, 0.0, 0.0);

	// Draw the wind spinner blades
	for (int i = 0; i < 6; ++i) {
		glRotatef(60.0, 1.0, 0.0, 0.0);
		drawBlade();
	}

	glPopMatrix();
}

void DrawStreetLight(void)
{
	glPushMatrix();
	glTranslatef(0, -1.7, 0);
	GLUquadricObj* quadratic;
	quadratic = gluNewQuadric(); 

	// Polished Silver Color
	GLfloat mat_ambient[] = { 0.23125, 0.23125, 0.23125, 1.0 };
	GLfloat mat_diffuse[] = { 0.2775, 0.2775, 0.2775, 1.0 };
	GLfloat mat_specular[] = { 0.773911, 0.773911, 0.773911, 1.0 };
	GLfloat mat_shininess[] = { 51.22 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	VerticalCylinder(0.1, 4.0);
	glTranslated(0, 4, 0);
	glutSolidSphere(0.1, 20, 20);
	gluCylinder(quadratic, 0.1, 0.1, 1, 32, 32);
	glTranslatef(0, 0, 1);
	glutSolidSphere(0.1, 20, 20);
	glTranslatef(0, -0.32, -0.1);

	glPushMatrix();
	glRotated(-90.0, 1, 0, 0);
	glutSolidCone(0.2, 0.3, 10, 10);
	glPopMatrix();
	
	// lamp
	GLfloat light_1_position[] = { 0, 3.68, 0.9, 1 };
	GLfloat light_1_specular[] = { 0.1, 0.1, 0.1, 1 };
	GLfloat light_1_diffuse[] = { 0.7, 0.7, 0.7, 1 };
	GLfloat light_1_ambient[] = { 0.3, 0.3, 0.3, 1 };
	GLfloat light_1_spotLight[] = { 0, -1, 0, 1 };
	GLfloat light_1_spotCutOff[] = { 25 };

	glLightfv(GL_LIGHT1, GL_POSITION, light_1_position);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light_1_spotLight);
	glLightfv(GL_LIGHT1, GL_SPOT_CUTOFF, light_1_spotCutOff);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_1_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_1_ambient);

	glBindTexture(GL_TEXTURE_2D, lamp);
	glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glutSolidSphere(0.12, 20, 20);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glPopMatrix(); 
}

void DrawSwings(void) 
{
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
			GLUquadricObj* quadratic;
			quadratic = gluNewQuadric();
			gluQuadricTexture(quadratic, GL_TRUE);
			glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
			gluCylinder(quadratic, 0.1, 0.1, 4.0, 32, 32);	
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
				DrawSwingKid(0); // 
			glPopMatrix();

			// Draw Swing Right
			glPushMatrix();
				glTranslated(0.5, 0, 0);
				glRotatef(-swingAng, 1, 0, 0);
				DrawSwing();
				glTranslatef(0.5, -1.85, 0);
				glScalef(0.9, 0.9, 0.9);
				DrawSwingKid(1);//
			glPopMatrix();

		glPopMatrix();

	glPopMatrix();
}

float DrawChains(int length) 
{
	float y = -0.035;

	glPushMatrix();
		glutSolidTorus(0.02, 0.06, 10, 10);
		glScaled(1, 2, 1);

		glTranslatef(0, -0.035, 0);
		glRotatef(90.0, 0.0, 1.0, 0.0);

		glPushMatrix();
			for (int i = 0; i < length; i++) {
				glutSolidTorus(0.01, 0.02, 50, 50);
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

		// Draw Banch
		glPushMatrix();
		glTranslatef(0, y, 0);
		glScalef(12.0, 1.0, 5.0);
		glTranslatef(0.042, 0.0, -0.005);
		glutSolidCube(0.1);
		glPopMatrix();

		// Chain Right
		glTranslated(1, 0, 0);
		DrawChains(30);
	glPopMatrix();


}

void DrawHead()
{		
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
			GLUquadricObj* quadratic;
			quadratic = gluNewQuadric();
			gluQuadricTexture(quadratic, GL_TRUE);
			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
			gluCylinder(quadratic, 0.25, 0.05, 0.1, 32, 32);
		glPopMatrix();
	glPopMatrix();
}

void DrawBody(float radius, float height,int sex)
{

	if (sex) {
		VerticalCylinder(radius, height);
		glutSolidSphere(0.25, 10, 10);
	}
	else {
		glPushMatrix();
			GLUquadricObj* quadratic;
			quadratic = gluNewQuadric();
			gluQuadricTexture(quadratic, GL_TRUE);
			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
			glTranslatef(0.0, 0.0, -0.2);
			gluCylinder(quadratic, radius + 0.2, radius , height + 0.2  , 32, 32);
		glPopMatrix();
	}

}

void Drawlimb(float radius, float height)
{
	//draw pill shaped limb in the pos Z direction
	glPushMatrix();
		GLUquadricObj* quadratic;
		quadratic = gluNewQuadric();
		gluCylinder(quadratic, radius, radius, height, 32, 32);
		glutSolidSphere(radius, 10, 10);
		glPushMatrix();
			glTranslatef(0, 0, height);
			glutSolidSphere(radius, 10, 10);
		glPopMatrix();
	glPopMatrix();
}

void DrawSwingKid(int sex)
{
	vec3 legPos = {0.14, -0.1, 0.0};
	vec3 armPos = { 0.23, 0.5, 0.0 };

	glPushMatrix();
		// Body
		glTranslatef(0.0, -0.05, -0.18);
		DrawBody(0.25, 0.6, sex);
		// Head
		DrawHead();

		glPushMatrix();
			// Left-leg
			glTranslatef(legPos.x, legPos.y, legPos.z);
			DrawLeg(sex);
		glPopMatrix();

		glPushMatrix();
			// Right-leg
			glTranslatef(-legPos.x, legPos.y, legPos.z);
			DrawLeg(sex);
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

void DrawLeg(int sex)
{
	float thighLength = 0.5;
	float shockLength = 0.55;
	float footLength = 0.2;
	glPushMatrix();
		//theigh
		Drawlimb(0.12, thighLength);
		glTranslatef(0 ,0, thighLength);
		//shock
		sex == 1 ? glRotatef(jointAngLeg, 1.0, 0.0, 0.0) : glRotatef(90 - jointAngLeg, 1.0, 0.0, 0.0);
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
	rotationAngle += 2.0f; // Adjust rotation speed as needed
	rotationAngle = wrapAngle(rotationAngle, 360.0);

	float length = 0.8;
	float omega = sqrt(9.81 / length);
	float time_period = 2 * PI * sqrt(length / 9.81);
	int isForward = 1;

	time += 16 * 0.001f;
	swingAng = SWING_MAX_ANG * cos(omega * time);

	//jointAngLeg = cos(omega * time) < 0 ? 0 : 90;

	//------------------------
		if (isForward) {
			jointAngLeg++;
		}
		else {
			jointAngLeg--;
		}

		isForward = jointAngLeg > 0.9 ? 0 : 1;
		isForward = jointAngLeg < -0.9 ? 1 : 0;

	//------------------------
	printf("angle = %f \n", cos(omega * time));


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