#include <stdio.h>
#include "objloader.h"
#include <GL/glut.h>

static obj_model *model;
static double spin = 0;

static void init(void) {
	glClearColor(0,0,0,0);

}

static void display(void) {
	if (!model) return;
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	glRotated(spin, 0, 1, 0);
	glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_BACK, GL_LINE);
	for (int i = 0; i < obj_n_faces(model); ++i) {
		glBegin(GL_POLYGON);
		for (int j = 0; j < obj_face(model, i).len; ++j) {
			geometric_vertex vert = obj_geometric_vertex(model,
					obj_face(model, i).points[j].iv);
			glVertex4d(vert.x, vert.y, vert.z, vert.w);
		}
		glEnd();
	}
	glPopMatrix();
	glutSwapBuffers();
}

static void reshape(int w, int h) {
	glViewport(0, 0, w, h);
	double wdiff = w/500.0;
	double hdiff = h/500.0;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.5*wdiff,1.5*wdiff,-1.5*hdiff,1.5*hdiff,-10,10);
}

static void spinDisplay(void) {
	spin += 0.5;
	if (spin > 360)
		spin -= 360;
	glutPostRedisplay();
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Exactly one parameter expected\n");
		return 1;
	}

	model = load_obj(argv[1]);
	if (!model) {
		fprintf(stderr, "Can't load model %s\n", argv[1]);
		return 2;
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(spinDisplay);
	glutMainLoop();
}
