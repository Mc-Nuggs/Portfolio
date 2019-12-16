#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <FI/FreeImage.h>
#define FILENAME "4by3.jpg"
char outFileName[] = "outimg.tif";  // file name to be saved
//the pixel structure
typedef struct {
	GLubyte r, g, b;
} pixel;
//the global structure
typedef struct {
	pixel *data;
	int w, h;
} glob;
glob global;
FIBITMAP *gimage;
enum { MENU_FILTER, MENU_SAVE, MENU_GRAY, MENU_COLOR, MENU_ROTATE, MENU_QUIT,MENU_BOX, MENU_MEDIAN, MENU_EDGE, MENU_SOBEL};
//read image
pixel *read_img(char *name, int *width, int *height) {
	FIBITMAP *image;
	int i, j, pnum;
	RGBQUAD aPixel;
	pixel *data;
	if ((image = FreeImage_Load(FIF_JPEG, name, 0)) == NULL) {
		return NULL;
	}
	*width = FreeImage_GetWidth(image);
	*height = FreeImage_GetHeight(image);
	gimage = image;// share the pointer 
	data = (pixel *)malloc((*height)*(*width) * sizeof(pixel *));
	pnum = 0;
	for (i = 0; i < (*height); i++) {
		for (j = 0; j < (*width); j++) {
			FreeImage_GetPixelColor(image, j, i, &aPixel);
			data[pnum].r = (aPixel.rgbRed);
			data[pnum].g = (aPixel.rgbGreen);
			data[pnum++].b = (aPixel.rgbBlue);
		}
	}
	FreeImage_Unload(image);
	return data;
}//read_img
//write_img
void write_img(char *name, pixel *data, int width, int height) {
	FIBITMAP *image;
	RGBQUAD aPixel;
	int i, j;
	image = FreeImage_Allocate(width, height, 24, 0, 0, 0);
	if (!image) {
		perror("FreeImage_Allocate");
		return;
	}
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			aPixel.rgbRed = data[i*width + j].r;
			aPixel.rgbGreen = data[i*width + j].g;
			aPixel.rgbBlue = data[i*width + j].b;
			FreeImage_SetPixelColor(image, j, i, &aPixel);
		}
	}
	if (!FreeImage_Save(FIF_TIFF, image, name, 0)) {
		perror("FreeImage_Save");
	}
	FreeImage_Unload(image);
}//write_img
/*draw the image ‐ it is already in the format openGL requires for
glDrawPixels*/
void display_image(void)
{
	// draw the image 
	glDrawPixels(global.w, global.h, GL_RGB, GL_UNSIGNED_BYTE,
		(GLubyte*)global.data);
	glFlush();
}//display_image()
void draw_triangle(void) // draw a triangle at a random position
{
	glDrawPixels(global.w, global.h, GL_RGB,
		GL_UNSIGNED_BYTE, (GLubyte*)global.data);
	glBegin(GL_TRIANGLES);
	glColor3f(1.0, 0, 0);
	glVertex2i(rand() % global.w, rand() % global.h);
	glColor3f(0, 1.0, 0);
	glVertex2i(rand() % global.w, rand() % global.h);
	glColor3f(0, 0, 1.0);
	glVertex2i(rand() % global.w, rand() % global.h);
	glEnd();
	glFlush();
}
//A simple thresholding filter.
void toBinary(pixel* Im, int myIm_Width, int
	myIm_Height) {
	int x, y;
	for (x = 0; x < myIm_Width; x++)
		for (y = 0; y < myIm_Height; y++) {
			if (Im[x + y * myIm_Width].g > 128) // thresholding
			{
			Im[x + y * myIm_Width].r = 255;
			Im[x + y * myIm_Width].g = 255;
			Im[x + y * myIm_Width].b = 255;
			}
			else
			{
				Im[x + y * myIm_Width].r = 0;
				Im[x + y * myIm_Width].g = 0;
				Im[x + y * myIm_Width].b = 0;
			}
		}
	glutPostRedisplay();  // Tell glut that the image has been updated and needs to be redrawn
}//binary
// gray level conversion
void toGray(pixel* Im, int myIm_Width, int
	myIm_Height) {
	int x, y;
	int grayval = 0;
	
	for (x = 0; x < myIm_Width; x++)
		for (y = 0; y < myIm_Height; y++) {
			grayval = (Im[x + y * myIm_Width].r +
				Im[x + y * myIm_Width].g + Im[x + y *
				myIm_Width].b) / 3;
			Im[x + y * myIm_Width].r = grayval;
			Im[x + y * myIm_Width].g = grayval;
			Im[x + y * myIm_Width].b = grayval;
		}
	glutPostRedisplay();
}//gray
void median(pixel* Im, int myIm_Width, int myIm_Height) {
	for (int x = 1; x < myIm_Width; x++) {

		for (int y = 1; y < myIm_Height; ++y) {
			
			//red
			Im[x + y * myIm_Width].r = (Im[(x - 1) + (y + 1) * myIm_Width].r + Im[(x)+(y + 1) * myIm_Width].r + Im[(x + 1) + (y + 1) * myIm_Width].r +
				Im[(x - 1) + (y)* myIm_Width].r + Im[(x)+(y)* myIm_Width].r + Im[(x + 1) + (y)* myIm_Width].r + Im[(x - 1) + (y - 1) * myIm_Width].r +
				Im[(x)+(y - 1) * myIm_Width].r + Im[(x + 1) + (y - 1) * myIm_Width].r) / 9;
			//blue
			Im[x + y * myIm_Width].b = (Im[(x - 1) + (y + 1) * myIm_Width].b + Im[(x)+(y + 1) * myIm_Width].b + Im[(x + 1) + (y + 1) * myIm_Width].b +
				Im[(x - 1) + (y)* myIm_Width].b + Im[(x)+(y)* myIm_Width].b + Im[(x + 1) + (y)* myIm_Width].b + Im[(x - 1) + (y - 1) * myIm_Width].b +
				Im[(x)+(y - 1) * myIm_Width].b + Im[(x + 1) + (y - 1) * myIm_Width].b) / 9;
			//green
			Im[x + y * myIm_Width].g = (Im[(x - 1) + (y + 1) * myIm_Width].g + Im[(x)+(y + 1) * myIm_Width].g + Im[(x + 1) + (y + 1) * myIm_Width].g +
				Im[(x - 1) + (y)* myIm_Width].g + Im[(x)+(y)* myIm_Width].g + Im[(x + 1) + (y)* myIm_Width].g + Im[(x - 1) + (y - 1) * myIm_Width].g + 
				Im[(x)+(y - 1) * myIm_Width].g + Im[(x + 1) + (y - 1) * myIm_Width].g) / 9;
			
		}
	}
	glutPostRedisplay();
}

void prewitt(pixel* Im, int myIm_Width, int myIm_Height) {
	GLbyte sum;

	for (int x = 1; x < myIm_Width; x++) {
		for (int y = 1; y < myIm_Height; ++y) {
			
			//red
			GLbyte red = abs(((Im[(x - 1) + y * myIm_Width].r - Im[(x + 1) + y * myIm_Width].r)/2 + (Im[(x - 1) + (y+1) * myIm_Width].r - Im[(x + 1) + (y+1) * myIm_Width].r) / 2 +
				(Im[(x - 1) +( y-1) * myIm_Width].r - Im[(x + 1) + (y-1) * myIm_Width].r) / 2)/3) + 
				abs(((Im[x + (y+1) * myIm_Width].r - Im[x + (y-1) * myIm_Width].r)/2 + (Im[(x-1) + (y + 1) * myIm_Width].r - Im[(x-1) + (y - 1) * myIm_Width].r) / 2 + 
				(Im[(x+1) + (y + 1) * myIm_Width].r - Im[(x+1) + (y - 1) * myIm_Width].r) / 2)/3);
			//blue
			GLbyte blue = abs(((Im[(x - 1) + y * myIm_Width].b - Im[(x + 1) + y * myIm_Width].b) / 2 + (Im[(x - 1) + (y + 1) * myIm_Width].b - Im[(x + 1) + (y + 1) * myIm_Width].b) / 2 +
				(Im[(x - 1) + (y - 1) * myIm_Width].b - Im[(x + 1) + (y - 1) * myIm_Width].b) / 2) / 3) +
				abs(((Im[x + (y + 1) * myIm_Width].b - Im[x + (y - 1) * myIm_Width].b) / 2 + (Im[(x - 1) + (y + 1) * myIm_Width].b - Im[(x - 1) + (y - 1) * myIm_Width].b) / 2 +
				(Im[(x + 1) + (y + 1) * myIm_Width].b - Im[(x + 1) + (y - 1) * myIm_Width].b) / 2) / 3);
			//green
			GLbyte green = abs(((Im[(x - 1) + y * myIm_Width].g - Im[(x + 1) + y * myIm_Width].g) / 2 + (Im[(x - 1) + (y + 1) * myIm_Width].g - Im[(x + 1) + (y + 1) * myIm_Width].g) / 2 +
				(Im[(x - 1) + (y - 1) * myIm_Width].g - Im[(x + 1) + (y - 1) * myIm_Width].g) / 2) / 3) +
				abs(((Im[x + (y + 1) * myIm_Width].g - Im[x + (y - 1) * myIm_Width].g) / 2 + (Im[(x - 1) + (y + 1) * myIm_Width].g - Im[(x - 1) + (y - 1) * myIm_Width].g) / 2 +
				(Im[(x + 1) + (y + 1) * myIm_Width].g - Im[(x + 1) + (y - 1) * myIm_Width].g) / 2) / 3);
			/*
			GLbyte gradientx = abs((
				((Im[(x - 1) + (y+1) * myIm_Width].r + Im[(x - 1) + (y+1) * myIm_Width].g + Im[(x - 1) + (y+1) * myIm_Width].b) - (Im[(x + 1) + (y+1) * myIm_Width].r + Im[(x + 1) + (y+1) * myIm_Width].g + Im[(x + 1) + (y+1) * myIm_Width].b)) + //row1
				((Im[(x-1) + y * myIm_Width].r +Im[(x-1) + y * myIm_Width].g + Im[(x-1) + y * myIm_Width].b) - (Im[(x+1) + y * myIm_Width].r + Im[(x+1) + y * myIm_Width].g + Im[(x+1) + y * myIm_Width].b)) + //row2
				((Im[(x - 1) + (y - 1) * myIm_Width].r + Im[(x - 1) + (y - 1) * myIm_Width].g + Im[(x - 1) + (y - 1) * myIm_Width].b) - (Im[(x + 1) + (y - 1) * myIm_Width].r + Im[(x + 1) + (y - 1) * myIm_Width].g + Im[(x + 1) + (y - 1) * myIm_Width].b))));//row3
			GLbyte gradienty = abs((
				((Im[(x - 1) + (y + 1) * myIm_Width].r + Im[(x - 1) + (y + 1) * myIm_Width].g + Im[(x - 1) + (y + 1) * myIm_Width].b) - (Im[(x - 1) + (y - 1) * myIm_Width].r + Im[(x - 1) + (y - 1) * myIm_Width].g + Im[(x - 1) + (y - 1) * myIm_Width].b))  +//collumn 1
				((Im[(x) + (y + 1) * myIm_Width].r + Im[(x) + (y + 1) * myIm_Width].g + Im[(x) + (y + 1) * myIm_Width].b) - (Im[(x) + (y - 1) * myIm_Width].r + Im[(x) + (y - 1) * myIm_Width].g + Im[(x) + (y - 1) * myIm_Width].b)) / 2 + // collumn 2
				((Im[(x - 1) + (y + 1) * myIm_Width].r + Im[(x - 1) + (y + 1) * myIm_Width].g + Im[(x - 1) + (y + 1) * myIm_Width].b) - (Im[(x + 1) + (y - 1) * myIm_Width].r + Im[(x + 1) + (y - 1) * myIm_Width].g + Im[(x + 1) + (y - 1) * myIm_Width].b))));//column 3
			sum = gradientx + gradienty;
			*/
			sum = (red + green + blue);
				
			sum = sum > 255 ? 255 : sum;
			sum = sum < 0 ? 0 : sum;
			Im[x + y * myIm_Width].r = sum;
			Im[x + y * myIm_Width].b = sum;
			Im[x + y * myIm_Width].g = sum;
		}
	}
	glutPostRedisplay();
}
void sobel(pixel* Im, int myIm_Width, int myIm_Height) {
	GLbyte sum;

	for (int x = 1; x < myIm_Width; x++) {
		for (int y = 1; y < myIm_Height; ++y) {
			GLbyte gradientx = abs((
				(-(Im[(x - 1) + (y + 1) * myIm_Width].r + Im[(x - 1) + (y + 1) * myIm_Width].g + Im[(x - 1) + (y + 1) * myIm_Width].b) - 2 * (Im[(x) + (y + 1) * myIm_Width].r + Im[(x) + (y + 1) * myIm_Width].g + Im[(x) + (y + 1) * myIm_Width].b) -(Im[(x + 1) + (y + 1) * myIm_Width].r + Im[(x + 1) + (y + 1) * myIm_Width].g + Im[(x + 1) + (y + 1) * myIm_Width].b)) + //row1
				(-(Im[(x - 1) + y * myIm_Width].r + Im[(x - 1) + y * myIm_Width].g + Im[(x - 1) + y * myIm_Width].b) - 2* (Im[(x + 1) + y * myIm_Width].r + Im[(x) + y * myIm_Width].g + Im[(x) + y * myIm_Width].b) - (Im[(x+1) + y * myIm_Width].r + Im[(x + 1) + y * myIm_Width].g + Im[(x + 1) + y * myIm_Width].b)) + //row2
				(-(Im[(x - 1) + (y - 1) * myIm_Width].r + Im[(x - 1) + (y - 1) * myIm_Width].g + Im[(x - 1) + (y - 1) * myIm_Width].b) - 2* (Im[(x) + (y - 1) * myIm_Width].r + Im[(x) + (y - 1) * myIm_Width].g + Im[(x) + (y - 1) * myIm_Width].b) - (Im[(x + 1) + (y - 1) * myIm_Width].r + Im[(x + 1) + (y - 1) * myIm_Width].g + Im[(x + 1) + (y - 1) * myIm_Width].b))));//row3
			GLbyte gradienty = abs((
				(-(Im[(x - 1) + (y + 1) * myIm_Width].r + Im[(x - 1) + (y + 1) * myIm_Width].g + Im[(x - 1) + (y + 1) * myIm_Width].b) - 2* (Im[(x - 1) + (y) * myIm_Width].r + Im[(x - 1) + (y) * myIm_Width].g + Im[(x - 1) + (y) * myIm_Width].b) - (Im[(x - 1) + (y - 1) * myIm_Width].r + Im[(x - 1) + (y - 1) * myIm_Width].g + Im[(x - 1) + (y - 1) * myIm_Width].b)) +//collumn 1
				(-(Im[(x)+(y + 1) * myIm_Width].r + Im[(x)+(y + 1) * myIm_Width].g + Im[(x)+(y + 1) * myIm_Width].b) -2 * (Im[(x)+(y) * myIm_Width].r + Im[(x)+(y) * myIm_Width].g + Im[(x)+(y) * myIm_Width].b)  - (Im[(x)+(y-1) * myIm_Width].r + Im[(x)+(y - 1) * myIm_Width].g + Im[(x)+(y - 1) * myIm_Width].b)) / 2 + // collumn 2
				(-(Im[(x - 1) + (y + 1) * myIm_Width].r + Im[(x - 1) + (y + 1) * myIm_Width].g + Im[(x - 1) + (y + 1) * myIm_Width].b) - 2 * (Im[(x + 1) + (y) * myIm_Width].r + Im[(x + 1) + (y) * myIm_Width].g + Im[(x + 1) + (y) * myIm_Width].b) - (Im[(x + 1) + (y - 1) * myIm_Width].r + Im[(x + 1) + (y - 1) * myIm_Width].g + Im[(x + 1) + (y - 1) * myIm_Width].b))));//column 3
			sum = gradientx + gradienty;
			
				sum = gradientx + gradienty;

			sum = sum > 255 ? 255 : sum;
			sum = sum < 0 ? 0 : sum;
			Im[x + y * myIm_Width].r = sum;
			Im[x + y * myIm_Width].b = sum;
			Im[x + y * myIm_Width].g = sum;
		}
	}
}
void drawBox() {
	glDrawPixels(global.w, global.h, GL_RGB,
		GL_UNSIGNED_BYTE, (GLubyte*)global.data);


	glBegin(GL_LINE_LOOP);
	glColor3f(1, 0, 0); glVertex2i(175, 100); glVertex2i(200, 100);//down x horizontal
	glColor3f(1, 0, 0); glVertex2i(200, 100); glVertex2i(200, 75);//down y vertical
	glColor3f(1, 0, 0); glVertex2i(200, 75); glVertex2i(175, 75);//back to x
	glColor3f(1, 0, 0); glVertex2i(175, 75); glVertex2i(175, 100);//up y
	glEnd();
	glFlush();
}
/*glut keyboard function*/
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 0x1B:   case'q':  case 'Q':
		exit(0);  break;
	case's': case'S':
		printf("SAVING IMAGE: outimg.tif\n");
		write_img(outFileName, global.data, global.w,
			global.h);
		break;
	case 'g': case 'G':
		toGray(global.data, global.w, global.h);
		break;
	case'b': case'B':
		toBinary(global.data, global.w, global.h);
		break;
	case'c':   case'C':
		global.data = read_img((char*)FILENAME,
			&global.w, &global.h);  display_image();
		break;
	case't': case'T':
		draw_triangle();
		break;

	case'd': case'D':
		drawBox();
		break;
	case'm': case 'M':
		median(global.data, global.w,global.h);
		break;
	case'e': case 'E':
		prewitt(global.data, global.w, global.h);
		break;
	case 'f': case 'F':
		sobel(global.data, global.w, global.h);
		break;
	} 
}

//keyboard
//Glut menu callback function
void menuFunc(int value)
{
	switch (value) {
	case MENU_QUIT: { exit(0);  }break;
	case MENU_SAVE:
	{
		printf("SAVING IMAGE: outimg.tif\n");
		write_img(outFileName, global.data, global.w, global.h);
	}
		break;
	case MENU_GRAY:
	{
		toGray(global.data, global.w, global.h);
	}
		break;
	case MENU_COLOR: {
		global.data = read_img((char*)FILENAME, &global.w, &global.h);
		display_image();
	}
		break;
	case MENU_FILTER:
	{
		toBinary(global.data, global.w, global.h);
	}
		break;
	case MENU_ROTATE:
	{
		gimage = FreeImage_Rotate(gimage, 30);
		RGBQUAD aPixel;
		int pnum = 0;
		for (int i = 0; i < global.h; i++) {
			for (int j = 0; j < global.w; j++) {
				FreeImage_GetPixelColor(gimage, j, i, &aPixel);
				global.data[pnum].r = (aPixel.rgbRed);
				global.data[pnum].g = (aPixel.rgbGreen);
				global.data[pnum++].b = (aPixel.rgbBlue);
			}
		}
		display_image();
	}
		break;
	case MENU_BOX:
	{
		drawBox();
		display_image();
	}
		break;
	case MENU_MEDIAN:
	{
		median(global.data, global.w, global.h);
	}
		break;
	case MENU_EDGE:
	{
		prewitt(global.data, global.w, global.h);
	}
	case MENU_SOBEL: 
	{
		sobel(global.data, global.w, global.h);
	}
	break;
	}
}//menuFunc
void show_keys()
{
	printf("Q:quit\nB:Binary\nC:Color\nG:Gray\nT:Draw a triangle\nD:Draw a Box\nM:Median\nE:Edge\nF:Sobel\nS:save\n");
}
//Glut menu set up
void init_menu()
{
	int sub_menu = glutCreateMenu(&menuFunc);
	glutAddMenuEntry("Gray", MENU_GRAY);
	glutAddMenuEntry("Color", MENU_COLOR);
	glutAddMenuEntry("Binary", MENU_FILTER);
	glutAddMenuEntry("Rotate", MENU_ROTATE);
	glutAddMenuEntry("Median", MENU_MEDIAN);
	glutAddMenuEntry("Edge", MENU_EDGE);
	glutAddMenuEntry("Sobel", MENU_SOBEL);
	int main_menu = glutCreateMenu(&menuFunc);
	glutAddSubMenu("Modify", sub_menu);
	glutAddMenuEntry("Box",MENU_BOX);
	glutAddMenuEntry("Save", MENU_SAVE);
	glutAddMenuEntry("Quit", MENU_QUIT);
	
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
int main(int argc, char** argv)
{
	global.data = read_img((char*)FILENAME, &global.w,
		&global.h);
	if (global.data == NULL)
	{
		printf("Error loading image file %s\n",
			FILENAME);
		return 1;
	}
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(global.w, global.h);
	glutCreateWindow("Image Display and Simple Filtering");
	glutDisplayFunc(display_image);
	glutKeyboardFunc(keyboard);
	glMatrixMode(GL_PROJECTION);
	glOrtho(0, global.w, 0, global.h, 0, 1);
	init_menu();
	show_keys();
	glutMainLoop();
	return 0;
}