#include <stdio.h>
#include<GL/glut.h>
#include <math.h>
#include "vector.h"
/*Fja koja crta brod tako sto nacrta sferu pa koacku skaliranu da izgleda kao "sipka"
pa dve kocke skalirane kao ploce na kraju sipke da bi dobili svemirski brod po uzoru na Tie Fighter iz Star Warsa*/
void ShipDraw(vector pos, vector forward){

	// Koeficijenti ambijentalne refleksije materijala.
    GLfloat ambient_coeffs[] = { 0.5, 0.5, 0.5, 1 };

    // Koeficijenti difuzne refleksije materijala.
    GLfloat diffuse_coeffs[] = { 0.8, 0.8, 0.8, 1 };

    // Koeficijenti spekularne refleksije materijala.
    GLfloat specular_coeffs[] = { 1, 1, 1, 1 };

    // Koeficijent glatkosti materijala.
    GLfloat shininess = 40;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);

		float angleX = atan2(forward.y, forward.z)/M_PI*180;
    	glRotatef(angleX, -1, 0, 0);
    	float angleY = atan2(forward.x, forward.z)/M_PI*180;
    	glRotatef(angleY, 0, -1, 0);

		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    	glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    	glColor3f(0,1,0);

    	glPushMatrix();
    		glutSolidSphere(1.2,20,20);
		glPopMatrix();
		
		glPushMatrix();
			glTranslatef(3.6, 0, 0);
			glScalef(0.11,4,4);
			glutSolidCube(2);
		glPopMatrix();

		glPushMatrix();
			glTranslatef(-3.6, 0, 0);
			glScalef(0.11,4,4);
			glutSolidCube(2);
		glPopMatrix();

		glPushMatrix();
			glScalef(3.7,0.15,0.15);
			glutSolidCube(2);
		glPopMatrix();
	glPopMatrix();
}
//Fja koja vrsi pomeranje broda tako sto centar translira za vektor pravca pomnozen brzinom
void moveShip(vector* shipPos, vector forward, double speed){
	shipPos->x += forward.x * speed;
	shipPos->y += forward.y * speed;
	shipPos->z += forward.z * speed;
}