#include "camera.h"
#include "model.h"
#include "draw.h"
#include <GL/glut.h>
#include <stdio.h>
#include <SOIL/SOIL.h>
#include <math.h>
#define VIEWPORT_RATIO (4.0 / 3.0)
#define VIEWPORT_ASPECT 50.0

#define CAMERA_SPEED 1.0

int mouse_x, mouse_y;
int needhelp = 0;
float _angle=0.0;
GLfloat light_color[] = {0.0f, 0.0f, 1.0f, 1.0f};
GLfloat light_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat light_diffuse[] = { 0.0f, 0.0f, 0.7f, 1.0f };
GLfloat light_specular[] = { 0.0f, 1.0f, 1.0f, 1.0f };
GLfloat mat_emission[] = {-1.0f,-1.0f,-1.0f,1.0f};
GLfloat sky_emission[] = {0.0f,0.0f,0.4f,1.0f};
GLfloat ground_emission[] = {-0.7f,-0.7f,-0.7f,1.0f};
GLfloat light_position[] = {4.0f, 8.0f, 10000.0f, 0.0f};
GLuint texture_id;
GLuint skytexture_id;
GLuint help_texture_id;
struct Camera camera;
struct Model tree1;
struct Model tree2;
struct Model tree3;
struct Model tree4;
struct Model etriangle;
struct Model ground;

typedef GLubyte Pixel[3];
struct Action
{
    int move_forward;
    int move_backward;
    int step_left;
    int step_right;
    int move_up;
    int move_down;
};
void display();
void update(int value);
void key_handler(int key, int x, int y);
void key_up_handler(int key, int x, int y);
void mouse_handler(int button, int state, int x, int y);
void motion_handler(int x, int y);
void key_handler_help(int key, int x, int y);
void idle();
struct Action action;
int time;


GLuint load_texture(char* filename)
{
    GLuint texture_name;
    glGenTextures(1, &texture_name);

    int width;
    int height;

    Pixel* image = (Pixel*)SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);

    glBindTexture(GL_TEXTURE_2D, texture_name);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (Pixel*)image);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

    return texture_name;
}

void update(int value)
{
    _angle+=2.0f;
    if(_angle>360.f)
    {
    _angle-=360;
    }
    glutPostRedisplay();
    glutTimerFunc(25,update,0);
}

void update_camera_position(struct Camera* camera, double elapsed_time)
{
    double distance;

    distance = elapsed_time * CAMERA_SPEED;
    if (action.move_forward == TRUE) {
		move_camera_forward(camera, distance);
    }

    if (action.move_backward == TRUE) {
		move_camera_backward(camera, distance);
    }

    if (action.step_left == TRUE) {
	    step_camera_left(camera, distance);
    }

    if (action.step_right == TRUE) {
		step_camera_right(camera, distance);
    }
     if (action.move_up == TRUE) {
		move_camera_up(camera, distance);
    }
     if (action.move_down == TRUE) {
		move_camera_down(camera, distance);
    }
}

double calc_elapsed_time()
{
    int current_time;
    double elapsed_time;

    current_time = glutGet(GLUT_ELAPSED_TIME);
    elapsed_time = (double)(current_time - time) / 1000.0;
    time = current_time;

    return elapsed_time;
}

void helpdisplay(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, help_texture_id);

    glBegin(GL_QUADS);

    glTexCoord2f(0.0,0.0);
    glVertex3d(-7.0,7.0,0.0);


    glTexCoord2f(0.0, 1.0);
    glVertex3d(-7.0,-7.0,0.0);


    glTexCoord2f(1.0, 1.0);
    glVertex3d(7.0,-7.0,0.0);


    glTexCoord2f(1.0,0.0);
    glVertex3d(7.0,7.0,0.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
    glutSwapBuffers();
    if(needhelp == 0)
    {
        glutDisplayFunc(display);
        glutKeyboardFunc(key_handler);
        glutKeyboardUpFunc(key_up_handler);
        glutMouseFunc(mouse_handler);
        glutMotionFunc(motion_handler);
        glMatrixMode (GL_PROJECTION);
        glLoadIdentity();
        int width = glutGet(GLUT_WINDOW_WIDTH);
        int height = glutGet(GLUT_WINDOW_HEIGHT);
        gluPerspective(50.0, (GLdouble)width / (GLdouble)height, 0.01, 10000.0);
        glEnable(GL_LIGHTING);
    }
}
void display()
{
    double elapsed_time;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    elapsed_time = calc_elapsed_time();
    update_camera_position(&camera, elapsed_time);
	setViewPoint(&camera);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D,texture_id);
    draw_model(&tree1);

    glBindTexture(GL_TEXTURE_2D,texture_id);
    draw_model(&tree2);

    glBindTexture(GL_TEXTURE_2D,texture_id);
    draw_model(&tree3);

    glBindTexture(GL_TEXTURE_2D,texture_id);
    draw_model(&tree4);

    glBindTexture(GL_TEXTURE_2D,texture_id);
    glPushMatrix();
    glRotatef(_angle,0.0f,0.0f,1.0f);
    draw_model(&etriangle);
    glPopMatrix();

    glBindTexture(GL_TEXTURE_2D,texture_id);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,ground_emission);
    draw_model(&ground);

    glBindTexture(GL_TEXTURE_2D,skytexture_id);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,sky_emission);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,light_color);
    draw_skybox();
	glutSwapBuffers();
	if(needhelp == 1){
        glutKeyboardFunc(idle);
        glutKeyboardUpFunc(idle);
        glutMouseFunc(idle);
        glutMotionFunc(idle);
        action.move_forward = FALSE;
        action.move_backward = FALSE;
        action.step_left = FALSE;
        action.step_right = FALSE;
        glMatrixMode (GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-9.0, 9.0, -9.0, 9.0, 0.0, 30.0);
        glDisable(GL_LIGHTING);
        glutDisplayFunc(helpdisplay);
	}

}


void draw_skybox()
{
    double theta, phi1, phi2;
    double x1, y1, z1;
    double x2, y2, z2;
    double u, v1, v2;

    int n_slices, n_stacks;
    double radius;
    int i, k;

    n_slices = 100;
    n_stacks = 50;
    radius = 50;

    glPushMatrix();

    glScaled(radius, radius, radius);

    glColor3f(1, 1, 1);

	glBegin(GL_TRIANGLE_STRIP);

    for (i = 0; i < n_stacks; ++i) {
        v1 = (double)i / n_stacks;
        v2 = (double)(i + 1) / n_stacks;
        phi1 = v1 * M_PI / 2.0;
        phi2 = v2 * M_PI / 2.0;
        for (k = 0; k <= n_slices; ++k) {
            u = (double)k / n_slices;
            theta = u * 2.0 * M_PI;
            x1 = cos(theta) * cos(phi1);
            y1 = sin(theta) * cos(phi1);
            z1 = sin(phi1);
            x2 = cos(theta) * cos(phi2);
            y2 = sin(theta) * cos(phi2);
            z2 = sin(phi2);
            glTexCoord2d(u, 1.0 - v1);
            glVertex3d(x1, y1, z1);
            glTexCoord2d(u, 1.0 - v2);
            glVertex3d(x2, y2, z2);
        }
    }

    glEnd();

    glPopMatrix();
}
void reshape(GLsizei width, GLsizei height)
{
    int x, y, w, h;
    double ratio;

    ratio = (double)width / height;
    if (ratio > VIEWPORT_RATIO) {
        w = (int)((double)height * VIEWPORT_RATIO);
        h = height;
        x = (width - w) / 2;
        y = 0;
    }
    else {
        w = width;
        h = (int)((double)width / VIEWPORT_RATIO);
        x = 0;
        y = (height - h) / 2;
    }

    glViewport (x, y, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if(needhelp == 1)
        {
            glOrtho(-9.0, 9.0, -9.0, 9.0, 0.0, 30.0);
        }
    else
        {
            gluPerspective(50.0, (GLdouble)width / (GLdouble)height, 0.01, 10000.0);
        }
}
void mouse_handler(int button, int state, int x, int y)
{
	mouse_x = x;
	mouse_y = y;
}
void motion_handler(int x, int y)
{
	double horizontal, vertical;

	horizontal = mouse_x - x;
	vertical = mouse_y - y;
	rotate_camera(&camera, horizontal, vertical);

	mouse_x = x;
	mouse_y = y;

    glutPostRedisplay();
}
void key_handler(int key, int x, int y)
{

	switch (key) {
	case 'w':
        action.move_forward = TRUE;
		break;
	case 's':
        action.move_backward = TRUE;
		break;
	case 'a':
        action.step_left = TRUE;
		break;
	case 'd':
        action.step_right = TRUE;
		break;
    case ' ':
        action.move_up = TRUE;
        break;
    case 'c':
        action.move_down = TRUE;
        break;
    case '+':
        if(mat_emission[0] < 1.0){
        mat_emission[0] += 0.01;
        mat_emission[1] += 0.01;
        mat_emission[2] += 0.01;
        sky_emission[0] += 0.01;
        sky_emission[1] += 0.01;
        sky_emission[2] += 0.01;
        ground_emission[0] += 0.01;
        ground_emission[1] += 0.01;
        ground_emission[2] += 0.01;
        }
        break;
    case '-':
        if(mat_emission[0] > -1.0){
        mat_emission[0] -= 0.01;
        mat_emission[1] -= 0.01;
        mat_emission[2] -= 0.01;
        sky_emission[0] -= 0.01;
        sky_emission[1] -= 0.01;
        sky_emission[2] -= 0.01;
        ground_emission[0] -= 0.01;
        ground_emission[1] -= 0.01;
        ground_emission[2] -= 0.01;
        }
        break;
	glutPostRedisplay();
}
}
void speckeys(int key, int x, int y)
{
    switch(key)
    {
    case GLUT_KEY_F1:
        if(needhelp == 0){
            needhelp = 1;
        }
        else{
            needhelp = 0;
        }
    }

}
void key_up_handler(int key, int x, int y)
{
	switch (key) {
	case 'w':
        action.move_forward = FALSE;
		break;
	case 's':
        action.move_backward = FALSE;
		break;
	case 'a':
        action.step_left = FALSE;
		break;
	case 'd':
        action.step_right = FALSE;
		break;
    case ' ':
        action.move_up = FALSE;
        break;
    case 'c':
        action.move_down = FALSE;
        break;
	}
	glutPostRedisplay();
}

void idle()
{
    glutPostRedisplay();
}

void initialize()
{


    glEnable(GL_NORMALIZE);
    glEnable(GL_AUTO_NORMAL);

    glClearColor(0.7, 0.9, 1.0, 1.0);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
	glMatrixMode(GL_MODELVIEW);

    glShadeModel(GL_SMOOTH);
    texture_id = load_texture("colors.png");
    skytexture_id = load_texture("Skydome.png");
    help_texture_id = load_texture("help_texture.jpg");

    int i;

    for(i=0;i<ground.n_vertices;i++){
        ground.vertices[i].z += 5;
    }
    for(i=0;i<tree1.n_vertices;i++){
        tree1.vertices[i].x += 9;
        tree1.vertices[i].z += 4.9;
    }
    for(i=0;i<tree2.n_vertices;i++){
        tree2.vertices[i].x += -9;
        tree2.vertices[i].z += 4.9;
    }
    for(i=0;i<tree3.n_vertices;i++){
        tree3.vertices[i].y += 9;
        tree3.vertices[i].z += 4.9;
    }
    for(i=0;i<tree4.n_vertices;i++){
        tree4.vertices[i].y += -9;
        tree4.vertices[i].z += 5;
    }
    for(i=0;i<etriangle.n_vertices;i++){
        etriangle.vertices[i].x *= 0.2;
        etriangle.vertices[i].y *= 0.2;
        etriangle.vertices[i].z *= 0.2;
        etriangle.vertices[i].z += 6;
    }

    glClearDepth(1.0);


}


int main(int argc, char* argv[])
{
    load_model("ground.obj", &ground);
    load_model("tree.obj", &tree1);
    load_model("tree.obj", &tree2);
    load_model("tree.obj", &tree3);
    load_model("tree.obj", &tree4);
    load_model("cucc.obj", &etriangle);
    glutInit(&argc, argv);

	glutInitWindowSize(1920, 1080);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	int window = glutCreateWindow("GLUT Window");
	glutSetWindow(window);

    initialize();


    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(key_handler);
    glutKeyboardUpFunc(key_up_handler);
    glutMouseFunc(mouse_handler);
    glutMotionFunc(motion_handler);
    glutIdleFunc(idle);
    glutSpecialFunc(speckeys);

    init_camera(&camera);

    action.move_forward = FALSE;
    action.move_backward = FALSE;
    action.step_left = FALSE;
    action.step_right = FALSE;

    glutTimerFunc(25,update,0);

    glutMainLoop();

    return 0;
}

