// this program demonstrates an interactive program that uses glColorMaterial()
//to change material parameters. Pressing each of the three mouse buttons changes the color of the diffuse reflection.
// the cube demonstrates a rotating light whenever the mouse left button clicked
#include <iostream>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <FI/FreeImage.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define PI 3.141592
GLint leftMouseButton, rightMouseButton; //status of the mouse buttons
int mouseX = 0, mouseY = 0; //last known X and Y of the mouse
float cameraTheta, cameraPhi, cameraRadius; //camera position in spherical coordinates
float x, y, z;
GLuint texID[2]; // Texture ID's for the three textures.
char* textureFileNames[2] = { // file names for the files from which texture images areloaded
(char*)"textures/The Sun.jpg",
(char*)"textures/The Earth.jpg",
};
static bool active = false;
static int spin = 0;
int speed = 1000 / 60;
void recomputeOrientation()
{
	x = cameraRadius * sinf(cameraTheta)*sinf(cameraPhi);
	z = cameraRadius * cosf(cameraTheta)*sinf(cameraPhi);
	y = cameraRadius * cosf(cameraPhi);
	glutPostRedisplay();
}
void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
}
/* Here is where the light position is reset after the modeling
* transformation (glRotated) is called. This places the
* light at a new position in world coordinates. The cube

* represents the position of the light.
*/
void loadTextures() {
	int i;
	glGenTextures(2, texID); // Get the texture object IDs.
	for (i = 0; i < 2; i++) {
		void* imgData; // Pointer to image color data read from the file.
		int imgWidth; // The width of the image that was read.
		int imgHeight; // The height.
		FREE_IMAGE_FORMAT format =
			FreeImage_GetFIFFromFilename(textureFileNames[i]);
		if (format == FIF_UNKNOWN) {
			printf("Unknown file type for texture image file %s\n",
				textureFileNames[i]);
			continue;
		}
		FIBITMAP* bitmap = FreeImage_Load(format, textureFileNames[i], 0); // Readimage from file.
		if (!bitmap) {
			printf("Failed to load image %s\n", textureFileNames[i]);
			continue;
		}
		FIBITMAP* bitmap2 = FreeImage_ConvertTo24Bits(bitmap); // Convert to RGBor BGR format
		FreeImage_Unload(bitmap);
		imgData = FreeImage_GetBits(bitmap2); // Grab the data we need from thebitmap.
		imgWidth = FreeImage_GetWidth(bitmap2);
		imgHeight = FreeImage_GetHeight(bitmap2);
		if (imgData) {
			printf("Texture image loaded from file %s, size %dx%d\n",
				textureFileNames[i], imgWidth, imgHeight);
			glBindTexture(GL_TEXTURE_2D, texID[i]); // Will load image dat	into texture object #i
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0,
				GL_RGB,
				GL_UNSIGNED_BYTE, imgData);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //Required since there are no mipmaps.
		}
		else {
			printf("Failed to get texture data from %s\n", textureFileNames[i]);
		}
	}
}
void mouseWheel(int wheel, int direction, int x, int y) {
	if (direction > 0) {

		z -= (z * .2);
		std::cout << z;
		glutPostRedisplay();
	}
	else {
		z += (z * .2);
		glutPostRedisplay();
	}
	//attempting to move the camera to and away from the model on the z axis

}
void display(void)
{

	GLfloat position[] = { 0.0, 0.0, 1.5, 2.0};
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glShadeModel(GL_SMOOTH);
	
	glBindTexture(GL_TEXTURE_2D, texID[0]);
	glPushMatrix();
	glTranslatef(0.0, 0.0, -5.0);
	gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0);
	glPushMatrix();
	glRotated((GLdouble)spin, 0.0, 1.0, 0.0);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glTranslated(0.0, 0.0, 1.5);
	glDisable(GL_LIGHTING);
	glutSolidSphere(.1, 20, 20);
	glEnable(GL_LIGHTING);
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, texID[1]);
	glutSolidSphere(.5, 20, 20);

	glPopMatrix();
	glutSwapBuffers();
}
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void mouse(int button, int state, int x, int y)
{
	switch (button) {
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN) {
			spin = (spin + 30) % 360;
			glutPostRedisplay();
		}
		break;
	case GLUT_LEFT_BUTTON:
		leftMouseButton = state;
		mouseX = x;
		mouseY = y;
	default:
		break;
	}
	
}
void mouseMotion(int x, int y)
{
	if (leftMouseButton == GLUT_DOWN)
	{
		cameraTheta += (mouseX - x)*0.005;
		cameraPhi += (mouseY - y)*0.005;
		// make sure that phi stays within the range (0, PI)
		if (cameraPhi <= 0)
			cameraPhi = 0 + 0.001;
		if (cameraPhi >= PI)
			cameraPhi = PI - 0.001;
		recomputeOrientation(); //update camera (x,y,z)
	}
		// camera zoom in/out
	mouseX = x;
	mouseY = y;
}
void keyboard(unsigned char key, int x, int y) {
	if (key == 'a' || key == 'A') {
		active = !active;
	}	
}
void specialInputs(int key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_UP:
		speed -= 1;
		if (speed == 0) speed = 1;

		break;
	case GLUT_KEY_DOWN:
		speed += 1;
		
		break;
	default:
		break;
	}
}
void timer(int value) {
	if(active)
	{
		spin++;
		glutPostRedisplay();
	}
	glutTimerFunc(speed, timer, 0);
}
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	init();
	loadTextures();
	cameraRadius = 3.0f;
	cameraTheta = 1;
	cameraPhi = 1;
	recomputeOrientation();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialInputs);
	glutMouseWheelFunc(mouseWheel);
	glutTimerFunc(1000/60, timer, 0);
	
	glutMainLoop();
	return 0;
}