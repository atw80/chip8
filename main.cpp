/*
Copyright (C) 2013, Dmitry Ivanov (ddvanov@gmail.com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>

#include <stdio.h>
#include <unistd.h>

#include "chip8.h"

chip8 chip;


void display(void)
{
	/* clear window */
	glClear(GL_COLOR_BUFFER_BIT); 

	/* draw unit square polygon */

	glPushMatrix();	
	glScalef(1, -1, 1);
	glTranslatef(-32, -16, 0);
	for(int i = 0; i < 32; i++){
		for(int j = 0; j < 64; j++){

			if(chip.gfx[i][j]){
				glColor3f(1.0, 1.0, 1.0);
			}else {
				glColor3f(0.5, 0.5, 0.5);
			}
			
			glRectf(j, i, j + 1, i + 1);			
		}
	}
	
	
	glColor3f(0.0, 0.0, 0.0);	
	for(int i = 0; i < 32; i++){
		for(int j = 0; j < 64; j++){
			glBegin(GL_LINE_LOOP);
			glVertex3f(j, i, 0);
			glVertex3f(j+1, i, 0);
			glVertex3f(j+1, i+1, 0);
			glVertex3f(j, i+1, 0);	
			glVertex3f(j, i, 0);	
			glEnd();
		}
	}

	
	/* flush GL buffers */
	glPopMatrix();
	glFlush(); 
}

void timer(int value){
	chip.emulateCycle();
	
	glutTimerFunc(100, timer, 0);
	display();
}
void keyboardUp(unsigned char key, int x, int y){
	switch(key){
		case '1':
			chip.key[0] = false;
		break;
		case '2':
			chip.key[1] = false;
		break;
		case '3':
			chip.key[2] = false;
		break;
		case '4':
			chip.key[3] = false;
		break;
		case 'q':
			chip.key[4] = false;
		break;
		case 'w':
			chip.key[5] = false;
		break;
		case 'e':
			chip.key[6] = false;
		break;
		case 'r':
			chip.key[7] = false;
		break;
		case 'a':
			chip.key[8] = false;
		break;
		case 's':
			chip.key[9] = false;
		break;
		case 'd':
			chip.key[10] = false;
		break;
		case 'f':
			chip.key[11] = false;
		break;
		case 'z':
			chip.key[12] = false;
		break;
		case 'x':
			chip.key[13] = false;
		break;
		case 'c':
			chip.key[14] = false;
		break;
		case 'v':
			chip.key[15] = false;	
		break;
		default:
		break;
	}
}


void keyboard(unsigned char key, int x, int y){
	switch(key){
		case '1':
			chip.key[0] = true;	
		break;
		case '2':
			chip.key[1] = true;	
		break;
		case '3':
			chip.key[2] = true;	
		break;
		case '4':
			chip.key[3] = true;	
		break;
		case 'q':
			chip.key[4] = true;	
		break;
		case 'w':
			chip.key[5] = true;	
		break;
		case 'e':
			chip.key[6] = true;	
		break;
		case 'r':
			chip.key[7] = true;	
		break;
		case 'a':
			chip.key[8] = true;	
		break;
		case 's':
			chip.key[9] = true;	
		break;
		case 'd':
			chip.key[10] = true;	
		break;
		case 'f':
			chip.key[11] = true;	
		break;
		case 'z':
			chip.key[12] = true;	
		break;
		case 'x':
			chip.key[13] = true;	
		break;
		case 'c':
			chip.key[14] = true;	
		break;
		case 'v':
			chip.key[15] = true;	
		break;
		default:
		break;
	}
}

void init(int argc, char** argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB); 
	glutInitWindowSize(500,500);
	glutInitWindowPosition(0,0);
	glutCreateWindow("simple"); 
	glutDisplayFunc(display);
	glutTimerFunc(1000, timer, 0);
	glutKeyboardFunc(keyboard);	
	glutKeyboardUpFunc(keyboardUp);

	/* set clear color to black */
	glClearColor (0.0, 0.0, 0.0, 0.0);

	/* set fill color to white */
	glColor3f(1.0, 1.0, 1.0); 

	/* set up standard orthogonal view with clipping */
	/* box as cube of side 2 centered at origin */
	/* This is default view and these statement could be removed */
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glOrtho(-50.0, 50.0, -50.0, 50.0, -1.0, 1.0);
	glutMainLoop();
}



int main(int argc, char *argv[]){
	if(argc == 1){
		printf("Error: rom required");
		exit(1);
	}

	chip.initialize();
	chip.loadGame(argv[1]);
	
	
	init(argc, argv); //Initialize the display

	return 0;
}

