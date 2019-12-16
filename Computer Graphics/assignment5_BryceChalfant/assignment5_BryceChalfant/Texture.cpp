// This example demonstrates how to do texture mapping in opengl with the freeimage lib
// you can download the texture image files used in this program from http://borg.csueastbay.edu/~bhecker/Previous%20Terms/CS-455-Fall18 / Textbook / source / glut / textures /
#include <GL/glut.h>
#include <stdio.h>
#include <iostream>
#include <FI/freeimage.h>
/* Draws a simple scene to show how to use texture objects. Three texture images
* are loaded into three texture objects. The scene contains three copies of the
* GLUT teapot, using the three different textures.
* The program uses the FreeImage image processing library to read the texture
* images from files.
*/
// --------------------------------- global variables --------------------------------
GLuint texID[3]; // Texture ID's for the three textures.
char* textureFileNames[3] = { // file names for the files from which texture images are loaded
(char*)"textures/Earth-1024x512.jpg",
(char*)"textures/brick001.jpg",
(char*)"textures/marble.jpg"
};
#define PI 3.141592


GLint leftMouseButton, rightMouseButton; //status of the mouse buttons
int mouseX = 0, mouseY = 0; //last known X and Y of the mouse
float cameraTheta, cameraPhi, cameraRadius; //camera position in spherical coordinates
float x, y, z;
// Note that you need to make a folder named “textures” and then copy the image files to
// the folder.
float rotateX = 0; // rotations for a simple viewing transform, applied to
float rotateY = 0; // each teapot, controlled by the arrow and HOME keys.
// ------------------------ OpenGL initialization and rendering -----------------------
void initGL() {
	// called by main() to initialize the OpenGL drawing context
	glClearColor(0.0, 0.0, 0.0, 1.0); // background color
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	float white[4] = { 1, 1, 1, 1 }; // A white material, suitable for texturing.
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-3.75, 3.75, -1.5, 1.5, -2, 2);
	glMatrixMode(GL_TEXTURE); // Matrix mode for manipulating the texture transform matrix.
	glLoadIdentity();
	glScalef(1, -1, 1); // It turns out the texture coordinates for the GLUT teapotput an upside
		// down texture on the sides of the teapot. To fix that, I apply a texture
		// transform that flips the texture vertically.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
} // end initGL()
void recomputeOrientation()
{
	x = cameraRadius * sinf(cameraTheta)*sinf(cameraPhi);
	z = cameraRadius * cosf(cameraTheta)*sinf(cameraPhi);
	y = cameraRadius * cosf(cameraPhi);
	glutPostRedisplay();
}
void mouseCallback(int button, int state, int thisX, int thisY)
{
	//update the left and right mouse button states, if applicable
	if (button == GLUT_LEFT_BUTTON)
		leftMouseButton = state;
	else if (button == GLUT_RIGHT_BUTTON)
		rightMouseButton = state;
	//and update the last seen X and Y coordinates of the mouse
	mouseX = thisX;
	mouseY = thisY;
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
	else if (rightMouseButton == GLUT_DOWN) {
		double totalChangeSq = (x - mouseX) + (y - mouseY);
		cameraRadius += totalChangeSq * 0.01;
		//limit the camera radius to some reasonable values so the user can't get lost
			if (cameraRadius < 2.0)
				cameraRadius = 2.0;
		if (cameraRadius > 10.0)
			cameraRadius = 10.0;
		recomputeOrientation(); //update camera (x,y,z) based on (radius,theta,phi)
	}
	mouseX = x;
	mouseY = y;
}
void display() {
	// Draws the scene, consisting of three teapots with different textures.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glEnable(GL_TEXTURE_GEN_S);

	glEnable(GL_TEXTURE_GEN_T);
	/* Draw a teapot with the second texture at (0,-0.5,0) */
	glBindTexture(GL_TEXTURE_2D, texID[0]); // Bind texture #1 for use on the secondteapot.
	glPushMatrix();
	gluLookAt(x, y, z, //camera is located at (x,y,z)
	0, 0, 0, //camera is looking at (0,0,0)
		0.0f, 1.0f, 0.0f);
	glutSolidCube(1);
	glPopMatrix();
	// Draw a teapot with the third texture at (2.3,0.3,0)
	// enable automatic texture coordinate generation
	// suppose that the texture has been loaded to texImage
	glBindTexture(GL_TEXTURE_2D, texID[1]);
	glPushMatrix();
	glTranslatef(-2.5, 0, 0);
	glutSolidTeapot(1); // draw a teapot with this sphere map
	glBindTexture(GL_TEXTURE_2D, texID[1]);
	glTranslatef(5, 0, 0.0);
	glutSolidSphere(1.0, 20, 20); // draw a sphere with this sphere map
	glPopMatrix();
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glutSwapBuffers(); // (required for double-buffered drawing)
}

// --------------- keyboard event function ---------------------------------------
void specialKeyFunction(int key, int x, int y) {
	// Change rotation amounts in response to arrow and home keys.
	if (key == GLUT_KEY_LEFT)
		rotateY -= 15;
	else if (key == GLUT_KEY_RIGHT)
		rotateY += 15;
	else if (key == GLUT_KEY_DOWN)
		rotateX += 15;
	else if (key == GLUT_KEY_UP)
		rotateX -= 15;
	else if (key == GLUT_KEY_HOME)
		rotateX = rotateY = 0;
	glutPostRedisplay();

}
// --------------------- texture loading -----------------------------------------
/* This function loads three textures from texture files, which must be available
* to the program when it is run. Paths to the files are stored in the global
* array textureFileNames. The function generates three texture object identifiers
* and stores them in the global array textID so that they can be used when
* binding textures in display(). It then loads the three texture images into
* the texture objects. It calles glTexParameteri for each texture to change
* the minification filter to GL_LINEAR (without this, the texture won't work
* because there are no mipmaps for the textures).
*/
void loadTextures() {
	int i;
	glGenTextures(3, texID); // Get the texture object IDs.
	for (i = 0; i < 3; i++) {
		void* imgData; // Pointer to image color data read from the file.
		int imgWidth; // The width of the image that was read.
		int imgHeight; // The height.
		FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(textureFileNames[i]);
		if (format == FIF_UNKNOWN) {
			printf("Unknown file type for texture image file %s\n",
				textureFileNames[i]);
			continue;
		}
		FIBITMAP* bitmap = FreeImage_Load(format, textureFileNames[i], 0); // Read image from file.
		if (!bitmap) {
			printf("Failed to load image %s\n", textureFileNames[i]);
			continue;
		}
		FIBITMAP* bitmap2 = FreeImage_ConvertTo24Bits(bitmap); // Convert to RGB or BGR format
		FreeImage_Unload(bitmap);
		imgData = FreeImage_GetBits(bitmap2); // Grab the data we need from thebitmap.
		imgWidth = FreeImage_GetWidth(bitmap2);
		imgHeight = FreeImage_GetHeight(bitmap2);
		if (imgData) {
			printf("Texture image loaded from file %s, size %dx%d\n",
				textureFileNames[i], imgWidth, imgHeight);
			glBindTexture(GL_TEXTURE_2D, texID[i]); // Will load image data into texture object #i
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0,
				GL_BGR_EXT,
				GL_UNSIGNED_BYTE, imgData);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //Required since there are no mipmaps.
		}
		else {
			printf("Failed to get texture data from %s\n", textureFileNames[i]);
		}
	}
}

// ----------------- main routine -------------------------------------------------
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(750, 300);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Bryce Chalfant Assignment 5");
	initGL();
	loadTextures();
	cameraRadius = 2.0f;
	cameraTheta = 1;
	cameraPhi = 1;
	recomputeOrientation();
	glutDisplayFunc(display);
	glutSpecialFunc(specialKeyFunction);
	glutMouseFunc(mouseCallback);
	glutMotionFunc(mouseMotion);
	glutMainLoop();
	return 0;
}
