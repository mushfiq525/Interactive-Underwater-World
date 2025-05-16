#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <iostream>

void initialize();

// Global variables to store current window size
int windowWidth = 900;
int windowHeight = 600;
bool isFullScreen = false;

struct Fish {
    float x, y, speed, angle, scale;
    float r, g, b;
    bool facingRight;
};

struct Shark {
    float x, y, speed, angle, scale;
    float r, g, b;
    bool facingRight;
    float hunger;
};

struct Bubble {
    float x, y;
    float radius;
};

struct Food {
    float x, y;
    float life;
};

struct Rock {
    float x, scale;
    float r, g, b;
};

struct Seaweed {
    float x, height;
    float green;
};

struct Ripple {
    float x, y;
    float radius;
    float life;
};

struct Crab {
    float x, y;
    float speed;
    float scale;
    float r, g, b;
    bool facingRight;
};

std::vector<Fish> fishList;
std::vector<Shark> sharkList;
std::vector<Bubble> bubbles;
std::vector<Food> foodList;
std::vector<Rock> rocks;
std::vector<Seaweed> seaweeds;
std::vector<Ripple> ripples;
std::vector<Crab> crabs;
GLuint fishDisplayList = 0, sharkDisplayList = 0, pebbleDisplayList = 0, seaweedDisplayList = 0;
bool usePerspective = false;

void checkGLError(const char* operation) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error after " << operation << ": " << gluErrorString(error) << std::endl;
    }
}

void initGL() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 10.0);
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    GLfloat lightPos[] = {0.0f, 0.0f, 1.0f, 0.0f};
    GLfloat ambient[] = {0.4f, 0.4f, 0.4f, 1.0f};
    GLfloat diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat specular[] = {0.6f, 0.6f, 0.6f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    checkGLError("initGL");
}

// New reshape function to handle window size changes
void reshape(int width, int height) {
    windowWidth = width;
    windowHeight = height > 0 ? height : 1; // Prevent division by zero
    glViewport(0, 0, windowWidth, windowHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (usePerspective) {
        gluPerspective(60.0, (float)windowWidth / windowHeight, 0.1, 10.0);
        glTranslatef(0.0f, 0.0f, -2.0f);
    } else {
        glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 10.0);
    }
    glMatrixMode(GL_MODELVIEW);
    checkGLError("reshape");
}

void initDisplayLists() {
    fishDisplayList = glGenLists(1);
    glNewList(fishDisplayList, GL_COMPILE);
    glBegin(GL_QUADS);
    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 20; ++j) {
            float theta = i * 3.1416f / 20;
            float phi = j * 2 * 3.1416f / 20;
            float thetaNext = (i + 1) * 3.1416f / 20;
            float phiNext = (j + 1) * 2 * 3.1416f / 20;
            float x0 = cosf(phi) * sinf(theta);
            float y0 = cosf(theta);
            float z0 = sinf(phi) * sinf(theta);
            float x1 = cosf(phiNext) * sinf(theta);
            float y1 = cosf(theta);
            float z1 = sinf(phiNext) * sinf(theta);
            float x2 = cosf(phiNext) * sinf(thetaNext);
            float y2 = cosf(thetaNext);
            float z2 = sinf(phiNext) * sinf(thetaNext);
            float x3 = cosf(phi) * sinf(thetaNext);
            float y3 = cosf(thetaNext);
            float z3 = sinf(phi) * sinf(thetaNext);
            glNormal3f(x0, y0, z0);
            glVertex3f(x0, y0, z0);
            glNormal3f(x1, y1, z1);
            glVertex3f(x1, y1, z1);
            glNormal3f(x2, y2, z2);
            glVertex3f(x2, y2, z2);
            glNormal3f(x3, y3, z3);
            glVertex3f(x3, y3, z3);
        }
    }
    glEnd();
    glEndList();
    checkGLError("fishDisplayList");

    sharkDisplayList = glGenLists(1);
    glNewList(sharkDisplayList, GL_COMPILE);
    glBegin(GL_QUADS);
    for (int i = 0; i < 30; ++i) {
        float t = i / 30.0f;
        float tNext = (i + 1) / 30.0f;
        float yScale = (t < 0.2f) ? (0.1f + 0.5f * t) : (0.2f * (1.0f - 0.5f * (t - 0.5f) * (t - 0.5f)));
        float zScale = (t < 0.2f) ? (0.08f + 0.35f * t) : (0.15f * (1.0f - 0.5f * (t - 0.5f) * (t - 0.5f)));
        for (int j = 0; j < 20; ++j) {
            float phi = j * 2 * 3.1416f / 20;
            float phiNext = (j + 1) * 2 * 3.1416f / 20;
            float x0 = -0.5f + t * 0.9f;
            float y0 = yScale * cosf(phi);
            float z0 = zScale * sinf(phi);
            float x1 = -0.5f + t * 0.9f;
            float y1 = yScale * cosf(phiNext);
            float z1 = zScale * sinf(phiNext);
            float x2 = -0.5f + tNext * 0.9f;
            float y2 = yScale * cosf(phiNext);
            float z2 = zScale * sinf(phiNext);
            float x3 = -0.5f + tNext * 0.9f;
            float y3 = yScale * cosf(phi);
            float z3 = zScale * sinf(phi);
            float nx = cosf(phi) / yScale;
            float ny = sinf(phi) / yScale;
            float nz = sinf(phi) / zScale;
            glNormal3f(nx, ny, nz);
            glVertex3f(x0, y0, z0);
            glVertex3f(x1, y1, z1);
            glVertex3f(x2, y2, z2);
            glVertex3f(x3, y3, z3);
        }
    }
    glEnd();
    glBegin(GL_QUADS);
    for (int i = 0; i < 3; ++i) {
        float x = -0.25f + i * 0.03f;
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(x, 0.05f, 0.08f);
        glVertex3f(x + 0.01f, 0.05f, 0.08f);
        glVertex3f(x + 0.01f, -0.05f, 0.08f);
        glVertex3f(x, -0.05f, 0.08f);
        glVertex3f(x, 0.05f, -0.08f);
        glVertex3f(x + 0.01f, 0.05f, -0.08f);
        glVertex3f(x + 0.01f, -0.05f, -0.08f);
        glVertex3f(x, -0.05f, -0.08f);
    }
    glEnd();
    glEndList();
    checkGLError("sharkDisplayList");

    pebbleDisplayList = glGenLists(1);
    glNewList(pebbleDisplayList, GL_COMPILE);
    glutSolidSphere(1.0f, 8, 8);
    glEndList();
    checkGLError("pebbleDisplayList");

    seaweedDisplayList = glGenLists(1);
    glNewList(seaweedDisplayList, GL_COMPILE);
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= 5; ++i) {
        float t = i / 5.0f;
        float width = 0.02f * (1.0f - t);
        glVertex3f(-width, t, 0.0f);
        glVertex3f(width, t, 0.0f);
    }
    glEnd();
    glEndList();
    checkGLError("seaweedDisplayList");
}

void drawFish(float x, float y, float r, float g, float b, float scale, bool facingRight) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(facingRight ? 1 : -1, 1, 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLfloat matDiffuse[] = {r, g, b, 1.0f};
    GLfloat matSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat matShininess = 50.0f;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, matShininess);

    glPushMatrix();
    glScalef(scale * 0.07f, scale * 0.035f, scale * 0.035f);
    glColor3f(r + 0.1f, g + 0.1f, b + 0.1f);
    glCallList(fishDisplayList);
    glPopMatrix();

    float tailSway = sinf(glutGet(GLUT_ELAPSED_TIME) * 0.005f) * 0.02f;
    GLfloat tailDiffuse[] = {std::max(0.0f, r - 0.25f), std::max(0.0f, g - 0.25f), std::max(0.0f, b - 0.25f), 0.7f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, tailDiffuse);
    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-scale * 0.07f, 0.0f, 0.0f);
    glVertex3f(-scale * 0.11f, scale * 0.04f + tailSway, 0.0f);
    glVertex3f(-scale * 0.11f, -scale * 0.04f - tailSway, 0.0f);
    glVertex3f(-scale * 0.07f, 0.0f, 0.0f);
    glEnd();

    GLfloat finDiffuse[] = {std::max(0.0f, r - 0.15f), std::max(0.0f, g - 0.15f), std::max(0.0f, b - 0.15f), 0.7f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, finDiffuse);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-scale * 0.02f, scale * 0.035f, 0.0f);
    glVertex3f(scale * 0.01f, scale * 0.06f + tailSway * 0.5f, 0.0f);
    glVertex3f(scale * 0.04f, scale * 0.035f, 0.0f);
    glVertex3f(-scale * 0.02f, scale * 0.035f, 0.0f);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(scale * 0.02f, 0.0f, 0.0f);
    glVertex3f(scale * 0.05f, scale * 0.015f + tailSway * 0.5f, 0.0f);
    glVertex3f(scale * 0.05f, -scale * 0.015f - tailSway * 0.5f, 0.0f);
    glVertex3f(scale * 0.02f, 0.0f, 0.0f);
    glEnd();

    GLfloat eyeDiffuse[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, eyeDiffuse);
    glPushMatrix();
    glTranslatef(scale * 0.055f, scale * 0.01f, scale * 0.01f);
    glutSolidSphere(scale * 0.007f, 10, 10);
    glPopMatrix();

    glDisable(GL_BLEND);
    glPopMatrix();
    checkGLError("drawFish");
}

void drawShark(float x, float y, float scale, float r, float g, float b, bool facingRight) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, scale);
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    if (!facingRight) {
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLfloat matDiffuse[] = {r * 0.5f, g * 0.5f, b * 0.5f, 1.0f};
    GLfloat matSpecular[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat matShininess = 20.0f;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, matShininess);

    float bodySway = sinf(glutGet(GLUT_ELAPSED_TIME) * 0.003f) * 0.05f;
    glPushMatrix();
    glTranslatef(0.0f, bodySway, 0.0f);
    glColor3f(r * 0.5f + 0.15f, g * 0.5f + 0.15f, b * 0.5f + 0.15f);
    glCallList(sharkDisplayList);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, bodySway, 0.0f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f);
    GLfloat ventralDiffuse[] = {r * 0.8f, g * 0.8f, b * 0.8f, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, ventralDiffuse);
    for (int i = 0; i < 10; ++i) {
        float t = i / 10.0f;
        float tNext = (i + 1) / 10.0f;
        float yScale = (t < 0.2f) ? (0.1f + 0.5f * t) : (0.2f * (1.0f - 0.5f * (t - 0.5f) * (t - 0.5f)));
        float x0 = -0.5f + t * 0.9f;
        float x1 = -0.5f + tNext * 0.9f;
        glVertex3f(x0, -yScale, -0.05f);
        glVertex3f(x1, -yScale, -0.05f);
        glVertex3f(x1, -yScale, 0.05f);
        glVertex3f(x0, -yScale, 0.05f);
    }
    glEnd();
    glPopMatrix();

    float finSway = sinf(glutGet(GLUT_ELAPSED_TIME) * 0.005f) * 0.05f;
    GLfloat finDiffuse[] = {r, g, b, 0.6f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, finDiffuse);
    glPushMatrix();
    glTranslatef(0.0f, bodySway, 0.0f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.2f, 0.0f);
    glVertex3f(0.2f, 0.5f + finSway, 0.05f);
    glVertex3f(0.3f, 0.2f, 0.05f);
    glVertex3f(0.0f, 0.2f, 0.0f);
    glVertex3f(0.1f, 0.0f, 0.0f);
    glVertex3f(0.3f, -0.2f - finSway, 0.1f);
    glVertex3f(0.05f, -0.1f, 0.05f);
    glVertex3f(0.1f, 0.0f, 0.0f);
    glVertex3f(0.1f, 0.0f, 0.0f);
    glVertex3f(0.3f, 0.2f + finSway, -0.1f);
    glVertex3f(0.05f, 0.1f, -0.05f);
    glVertex3f(0.1f, 0.0f, 0.0f);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.4f, bodySway, 0.0f);
    glRotatef(finSway * 20.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(-0.4f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(0.4f, 0.0f, 0.0f);
    glVertex3f(0.6f, 0.25f, 0.05f);
    glVertex3f(0.6f, -0.25f, 0.05f);
    glVertex3f(0.4f, 0.0f, 0.0f);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.3f, 0.08f, 0.08f);
    GLfloat eyeDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, eyeDiffuse);
    glutSolidSphere(0.03f, 10, 10);
    glTranslatef(0.0f, 0.0f, 0.01f);
    GLfloat pupilDiffuse[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, pupilDiffuse);
    glScalef(0.01f, 0.02f, 0.01f);
    glutSolidSphere(1.0f, 8, 8);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-0.3f, 0.08f, -0.08f);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, eyeDiffuse);
    glutSolidSphere(0.03f, 10, 10);
    glTranslatef(0.0f, 0.0f, -0.01f);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, pupilDiffuse);
    glScalef(0.01f, 0.02f, 0.01f);
    glutSolidSphere(1.0f, 8, 8);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-0.3f, 0.08f, 0.08f);
    GLfloat membraneDiffuse[] = {1.0f, 1.0f, 1.0f, 0.3f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, membraneDiffuse);
    glScalef(0.02f, 0.02f, 0.01f);
    glutSolidSphere(1.0f, 8, 8);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-0.3f, 0.08f, -0.08f);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, membraneDiffuse);
    glScalef(0.02f, 0.02f, 0.01f);
    glutSolidSphere(1.0f, 8, 8);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.35f, 0.0f, 0.0f);
    glDisable(GL_LIGHTING);
    glColor3f(0.8f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex3f(0.0f, 0.05f, 0.05f);
    glVertex3f(0.05f, 0.05f, 0.05f);
    glVertex3f(0.05f, -0.05f, 0.05f);
    glVertex3f(0.0f, -0.05f, 0.05f);
    glVertex3f(0.0f, 0.05f, -0.05f);
    glVertex3f(0.05f, 0.05f, -0.05f);
    glVertex3f(0.05f, -0.05f, -0.05f);
    glVertex3f(0.0f, -0.05f, -0.05f);
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.05f, 0.04f, 0.05f);
    glVertex3f(0.05f, 0.02f, 0.05f);
    glVertex3f(0.03f, 0.03f, 0.05f);
    glVertex3f(0.05f, -0.02f, 0.05f);
    glVertex3f(0.05f, -0.04f, 0.05f);
    glVertex3f(0.03f, -0.03f, 0.05f);
    glVertex3f(0.05f, 0.04f, -0.05f);
    glVertex3f(0.05f, 0.02f, -0.05f);
    glVertex3f(0.03f, 0.03f, -0.05f);
    glVertex3f(0.05f, -0.02f, -0.05f);
    glVertex3f(0.05f, -0.04f, -0.05f);
    glVertex3f(0.03f, -0.03f, -0.05f);
    glEnd();
    glEnable(GL_LIGHTING);
    glPopMatrix();

    glDisable(GL_BLEND);
    glPopMatrix();
    checkGLError("drawShark");
}

void drawBubble(float x, float y, float radius) {
    glDisable(GL_LIGHTING);
    glColor4f(0.6f, 0.9f, 1.0f, 0.5f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 12; i++) {
        float angle = i * 3.1416f / 6;
        glVertex2f(x + radius * cosf(angle), y + radius * sinf(angle));
    }
    glEnd();
    glEnable(GL_LIGHTING);
    checkGLError("drawBubble");
}

void drawRock(float x, float scale, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, -0.85f, 0.0f);
    glScalef(scale, scale * 0.6f, scale);
    GLfloat matDiffuse[] = {r, g, b, 1.0f};
    GLfloat matSpecular[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, 10.0f);
    glutSolidSphere(1.0f, 15, 15);
    glPopMatrix();
    checkGLError("drawRock");
}

void drawGrass() {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (float x = -1.0f; x < 1.0f; x += 0.02f) {
        float height = 0.08f + (float(rand()) / RAND_MAX) * 0.08f;
        float sway = sinf(glutGet(GLUT_ELAPSED_TIME) * 0.002f + x * 5.0f) * 0.02f;
        float green = 0.3f + (float(rand()) / RAND_MAX) * 0.3f;
        glColor4f(0.0f, green, 0.0f, 0.8f);
        glBegin(GL_QUADS);
        glVertex2f(x, -0.8f);
        glVertex2f(x + 0.005f, -0.8f);
        glVertex2f(x + 0.005f + sway, -0.8f + height);
        glVertex2f(x + sway, -0.8f + height);
        glEnd();
    }
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    checkGLError("drawGrass");
}

void drawPebbles() {
    glEnable(GL_LIGHTING);
    srand(12345);
    for (int i = 0; i < 50; ++i) {
        float x = (float(rand()) / RAND_MAX) * 2.0f - 1.0f;
        float y = -0.85f + (float(rand()) / RAND_MAX) * 0.05f;
        float scale = 0.01f + (float(rand()) / RAND_MAX) * 0.02f;
        float r = 0.4f + (float(rand()) / RAND_MAX) * 0.3f;
        float g = 0.3f + (float(rand()) / RAND_MAX) * 0.3f;
        float b = 0.2f + (float(rand()) / RAND_MAX) * 0.3f;
        glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glScalef(scale, scale, scale);
        GLfloat matDiffuse[] = {r, g, b, 1.0f};
        GLfloat matSpecular[] = {0.1f, 0.1f, 0.1f, 1.0f};
        glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
        glMaterialf(GL_FRONT, GL_SHININESS, 5.0f);
        glCallList(pebbleDisplayList);
        glPopMatrix();
    }
    srand((unsigned)time(nullptr));
    checkGLError("drawPebbles");
}

void drawSeaweed() {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (const auto& seaweed : seaweeds) {
        glPushMatrix();
        glTranslatef(seaweed.x, -0.8f, 0.0f);
        glScalef(1.0f, seaweed.height, 1.0f);
        float sway = sinf(glutGet(GLUT_ELAPSED_TIME) * 0.0015f + seaweed.x * 3.0f) * 0.05f;
        glTranslatef(sway, 0.0f, 0.0f);
        glColor4f(0.0f, seaweed.green, 0.0f, 0.7f);
        glCallList(seaweedDisplayList);
        glPopMatrix();
    }
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    checkGLError("drawSeaweed");
}

void drawRipples() {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (const auto& ripple : ripples) {
        glColor4f(0.7f, 0.9f, 1.0f, ripple.life * 0.5f);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 24; ++i) {
            float angle = i * 2.0f * 3.1416f / 24;
            glVertex2f(ripple.x + ripple.radius * cosf(angle), ripple.y + ripple.radius * sinf(angle));
        }
        glEnd();
    }
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    checkGLError("drawRipples");
}

void drawCrab(float x, float y, float scale, float r, float g, float b, bool facingRight) {
    std::cout << "Drawing crab at x=" << x << ", y=" << y << ", scale=" << scale << std::endl;
    glPushMatrix();
    glTranslatef(x, y, 0.02f);
    glScalef(facingRight ? scale : -scale, scale, scale);
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);

    GLfloat matDiffuse[] = {r, g, b, 1.0f};
    GLfloat matSpecular[] = {0.4f, 0.4f, 0.4f, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, 30.0f);
    glPushMatrix();
    glScalef(0.1f, 0.04f, 0.05f);
    glutSolidSphere(1.0f, 12, 12);
    glPopMatrix();

    float legSway = sinf(glutGet(GLUT_ELAPSED_TIME) * 0.01f) * 0.015f;
    GLfloat legDiffuse[] = {r * 0.8f, g * 0.8f, b * 0.8f, 1.0f};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, legDiffuse);

    glPushMatrix();
    glTranslatef(0.04f, 0.0f, 0.025f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.04f, 0.02f + legSway, 0.0f);
    glVertex3f(0.04f, 0.02f + legSway, -0.008f);
    glVertex3f(0.0f, 0.0f, -0.008f);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.02f, 0.0f, 0.025f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.04f, -0.02f - legSway, 0.0f);
    glVertex3f(0.04f, -0.02f - legSway, -0.008f);
    glVertex3f(0.0f, 0.0f, -0.008f);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.04f, 0.0f, 0.025f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-0.04f, 0.02f + legSway, 0.0f);
    glVertex3f(-0.04f, 0.02f + legSway, -0.008f);
    glVertex3f(0.0f, 0.0f, -0.008f);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.02f, 0.0f, 0.025f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-0.04f, -0.02f - legSway, 0.0f);
    glVertex3f(-0.04f, -0.02f - legSway, -0.008f);
    glVertex3f(0.0f, 0.0f, -0.008f);
    glEnd();
    glPopMatrix();

    glPopMatrix();
    checkGLError("drawCrab");
}

void drawBackground() {
    glClearColor(0.0f, 0.4f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    checkGLError("clear");

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(0.76f, 0.7f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -0.8f);
    glVertex2f(1.0f, -0.8f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(-1.0f, -1.0f);
    glEnd();
    glBegin(GL_QUADS);
    for (float x = -1.0f; x < 1.0f; x += 0.03f) {
        for (float y = -1.0f; y < -0.8f; y += 0.03f) {
            float t = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
            float caustic1 = 0.5f + 0.5f * sinf(x * 10 + y * 10 + t);
            float caustic2 = 0.5f + 0.5f * sinf(x * 15 - y * 12 + t * 0.7f);
            float caustic = (caustic1 + caustic2) * 0.5f;
            glColor4f(0.8f, 0.9f, 1.0f, caustic * 0.4f);
            glVertex2f(x, y);
            glVertex2f(x + 0.03f, y);
            glVertex2f(x + 0.03f, y + 0.03f);
            glVertex2f(x, y + 0.03f);
        }
    }
    glEnd();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    checkGLError("drawBackground");
}

void updateFish() {
    std::vector<Fish> newFishList;
    std::vector<Food> newFoodList = foodList;
    for (auto& f : fishList) {
        bool eaten = false;
        float avgAngle = f.angle;
        int nearbyCount = 1;
        float minFoodDist = 0.5f;
        float foodAngle = f.angle;
        bool foodNearby = false;

        for (auto& s : sharkList) {
            float dx = f.x - s.x;
            float dy = f.y - s.y;
            float dist = sqrtf(dx * dx + dy * dy);
            if (dist < 0.12f) {
                eaten = true;
                s.hunger = std::min(1.0f, s.hunger + 0.2f);
                break;
            }
            if (dist < 0.3f) {
                float fleeAngle = atan2f(dy, dx);
                f.angle = fleeAngle;
                f.speed = 0.01f;
                f.facingRight = dx > 0;
                break;
            }
        }

        if (!eaten) {
            for (auto it = newFoodList.begin(); it != newFoodList.end();) {
                float dx = it->x - f.x;
                float dy = it->y - f.y;
                float dist = sqrtf(dx * dx + dy * dy);
                if (dist < 0.05f) {
                    it = newFoodList.erase(it);
                    continue;
                }
                if (dist < minFoodDist) {
                    minFoodDist = dist;
                    foodAngle = atan2f(dy, dx);
                    foodNearby = true;
                }
                ++it;
            }

            if (foodNearby) {
                f.angle = f.angle * 0.4f + foodAngle * 0.6f;
                f.speed = 0.008f;
                f.facingRight = cosf(foodAngle) > 0;
            } else {
                for (const auto& rock : rocks) {
                    float dx = f.x - rock.x;
                    float dy = f.y - (-0.85f);
                    float dist = sqrtf(dx * dx + dy * dy);
                    if (dist < 0.1f) {
                        float repelAngle = atan2f(dy, dx);
                        avgAngle += repelAngle;
                        nearbyCount++;
                    }
                }
                for (const auto& other : fishList) {
                    if (&other != &f) {
                        float dx = f.x - other.x;
                        float dy = f.y - other.y;
                        float dist = sqrtf(dx * dx + dy * dy);
                        if (dist < 0.2f) {
                            avgAngle += other.angle;
                            nearbyCount++;
                        }
                    }
                }
                f.angle = f.angle * 0.8f + (avgAngle / nearbyCount) * 0.2f;
                if (rand() % 100 < 2) {
                    f.angle += (float(rand()) / RAND_MAX - 0.5f) * 0.5f;
                }
                f.speed = 0.002f + (float(rand()) / RAND_MAX) * 0.005f;
                f.facingRight = cosf(f.angle) > 0;
            }

            f.x += f.speed * cosf(f.angle);
            f.y += f.speed * 0.1f * sinf(f.angle);
            if (f.x > 1.2f) {
                f.x = -1.2f;
                f.facingRight = true;
                f.angle = 0.0f;
            } else if (f.x < -1.2f) {
                f.x = 1.2f;
                f.facingRight = false;
                f.angle = 3.1416f;
            }
            if (f.y > 0.7f) f.y = 0.7f;
            if (f.y < -0.6f) f.y = -0.6f;
            newFishList.push_back(f);
        }
    }
    fishList = newFishList;
    foodList = newFoodList;
    checkGLError("updateFish");
}

void updateSharks() {
    for (auto& s : sharkList) {
        s.hunger -= 0.001f;
        if (s.hunger < 0.0f) s.hunger = 0.0f;
        float minDist = 1e9;
        float targetAngle = s.angle;
        if (s.hunger > 0.3f) {
            for (const auto& f : fishList) {
                float dx = f.x - s.x;
                float dy = f.y - s.y;
                float dist = sqrtf(dx * dx + dy * dy);
                if (dist < minDist) {
                    minDist = dist;
                    targetAngle = atan2f(dy, dx);
                }
            }
        } else {
            if (rand() % 100 < 5) {
                targetAngle += (float(rand()) / RAND_MAX - 0.5f) * 0.5f;
            }
        }
        for (const auto& rock : rocks) {
            float dx = s.x - rock.x;
            float dy = s.y - (-0.85f);
            float dist = sqrtf(dx * dx + dy * dy);
            if (dist < 0.1f) {
                float repelAngle = atan2f(dy, dx);
                targetAngle = s.angle * 0.5f + repelAngle * 0.5f;
            }
        }
        if (minDist < 0.5f && s.hunger > 0.3f) {
            s.angle = s.angle * 0.6f + targetAngle * 0.4f;
            s.speed = 0.012f * (0.5f + s.hunger);
        } else {
            s.speed = 0.004f;
            s.angle = s.angle * 0.8f + targetAngle * 0.2f;
        }
        s.facingRight = cosf(s.angle) > 0;
        s.x += s.speed * cosf(s.angle);
        s.y += s.speed * 0.1f * sinf(s.angle);
        if (s.x > 1.3f) {
            s.x = -1.3f;
            s.facingRight = true;
            s.angle = 0.0f;
        } else if (s.x < -1.3f) {
            s.x = 1.3f;
            s.facingRight = false;
            s.angle = 3.1416f;
        }
        if (s.y > 0.6f) s.y = 0.6f;
        if (s.y < -0.5f) s.y = -0.5f;
    }
    checkGLError("updateSharks");
}

void updateBubbles() {
    for (auto& b : bubbles) {
        b.y += 0.006f;
        b.x += sinf(b.y * 10.0f) * 0.002f;
        if (b.y > 1.1f) {
            b.x = (float(rand()) / RAND_MAX) * 2.0f - 1.0f;
            b.y = -1.0f;
            b.radius = 0.01f + (float(rand()) / RAND_MAX) * 0.02f;
        }
    }
    checkGLError("updateBubbles");
}

void updateRipples() {
    for (auto& r : ripples) {
        r.radius += 0.005f;
        r.life -= 0.02f;
    }
    ripples.erase(std::remove_if(ripples.begin(), ripples.end(),
        [](const Ripple& r) { return r.life <= 0.0f; }), ripples.end());
    checkGLError("updateRipples");
}

void updateCrabs() {
    for (auto& c : crabs) {
        c.x += c.speed;
        bool reverse = false;
        if (c.x > 1.0f || c.x < -1.0f) {
            reverse = true;
        }
        for (const auto& rock : rocks) {
            float dx = c.x - rock.x;
            float dy = c.y - (-0.85f);
            float dist = sqrtf(dx * dx + dy * dy);
            if (dist < 0.1f) {
                reverse = true;
                break;
            }
        }
        if (reverse) {
            c.speed = -c.speed;
            c.facingRight = c.speed > 0;
        }
        std::cout << "Updated crab at x=" << c.x << ", y=" << c.y << ", speed=" << c.speed << std::endl;
    }
    checkGLError("updateCrabs");
}

void display() {
    float t = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
    GLfloat lightPos[] = {0.5f * cosf(t), 0.5f * sinf(t), 1.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    checkGLError("updateLight");

    drawBackground();
    drawPebbles();
    std::cout << "Drawing " << crabs.size() << " crabs" << std::endl;
    for (const auto& c : crabs) {
        drawCrab(c.x, c.y, c.scale, c.r, c.g, c.b, c.facingRight);
    }
    for (const auto& rock : rocks) {
        drawRock(rock.x, rock.scale, rock.r, rock.g, rock.b);
    }
    drawGrass();
    drawSeaweed();
    for (const auto& b : bubbles) {
        drawBubble(b.x, b.y, b.radius);
    }
    drawRipples();
    for (const auto& f : fishList) {
        drawFish(f.x, f.y, f.r, f.g, f.b, f.scale, f.facingRight);
    }
    for (const auto& s : sharkList) {
        drawShark(s.x, s.y, s.scale, s.r, s.g, s.b, s.facingRight);
    }
    glDisable(GL_LIGHTING);
    glPointSize(3.0f);
    glColor3f(1.0f, 0.5f, 0.0f);
    glBegin(GL_POINTS);
    for (const auto& food : foodList) {
        glVertex2f(food.x, food.y);
    }
    glEnd();
    glEnable(GL_LIGHTING);
    glutSwapBuffers();
    checkGLError("display");
}

void timer(int value) {
    updateFish();
    updateSharks();
    updateBubbles();
    updateRipples();
    updateCrabs();
    for (auto& food : foodList) {
        food.life -= 0.016f;
    }
    foodList.erase(std::remove_if(foodList.begin(), foodList.end(),
        [](const Food& f) { return f.life <= 0; }), foodList.end());
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 'r') {
        initialize();
    }
    if (key == 'p') {
        usePerspective = !usePerspective;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        if (usePerspective) {
            gluPerspective(60.0, (float)windowWidth / windowHeight, 0.1, 10.0);
            glTranslatef(0.0f, 0.0f, -2.0f);
        } else {
            glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 10.0);
        }
        glMatrixMode(GL_MODELVIEW);
        checkGLError("keyboard projection");
    }
    if (key == 'f') { // Toggle full-screen mode
        isFullScreen = !isFullScreen;
        if (isFullScreen) {
            glutFullScreen();
        } else {
            glutReshapeWindow(900, 600);
            glutPositionWindow(300, 100);
        }
        checkGLError("full screen toggle");
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Use current window size for coordinate transformation
        float wx = (2.0f * x / (float)windowWidth) - 1.0f;
        float wy = 1.0f - (2.0f * y / (float)windowHeight);
        foodList.push_back({ wx, wy, 10.0f });
        ripples.push_back({ wx, 0.8f, 0.01f, 1.0f });
        std::cout << "Added food at wx=" << wx << ", wy=" << wy << std::endl; // Debug output
    }
}

void initialize() {
    srand((unsigned)time(nullptr));
    fishList.clear();
    sharkList.clear();
    bubbles.clear();
    foodList.clear();
    rocks.clear();
    seaweeds.clear();
    ripples.clear();
    crabs.clear();

    for (int i = 0; i < 35; ++i) {
        bool right = rand() % 2 == 0;
        fishList.push_back({
            (float(rand()) / RAND_MAX) * 2.0f - 1.0f,
            (float(rand()) / RAND_MAX) * 1.3f - 0.6f,
            0.002f + (float(rand()) / RAND_MAX) * 0.005f,
            right ? 0.0f : 3.1416f,
            (0.05f + (float(rand()) / RAND_MAX) * 0.07f) * 4.5f,
            (float(rand()) / RAND_MAX),
            (float(rand()) / RAND_MAX),
            (float(rand()) / RAND_MAX),
            right
            });
    }

    for (int i = 0; i < 3; ++i) {
        bool right = rand() % 2 == 0;
        sharkList.push_back({
            (float(rand()) / RAND_MAX) * 2.0f - 1.0f,
            (float(rand()) / RAND_MAX) * 1.0f - 0.5f,
            (0.002f + (float(rand()) / RAND_MAX) * 0.004f) * 0.5f,
            right ? 0.0f : 3.1416f,
            0.2f + (float(rand()) / RAND_MAX) * 0.15f,
            0.4f + (float(rand()) / RAND_MAX) * 0.1f,
            0.4f + (float(rand()) / RAND_MAX) * 0.1f,
            0.4f + (float(rand()) / RAND_MAX) * 0.1f,
            right,
            (float(rand()) / RAND_MAX)
            });
    }

    for (int i = 0; i < 15; ++i) {
        bubbles.push_back({
            (float(rand()) / RAND_MAX) * 2.0f - 1.0f,
            (float(rand()) / RAND_MAX) * 2.0f - 1.0f,
            0.01f + (float(rand()) / RAND_MAX) * 0.02f
            });
    }

    for (int i = 0; i < 5; ++i) {
        rocks.push_back({
            (float(rand()) / RAND_MAX) * 2.0f - 1.0f,
            0.05f + (float(rand()) / RAND_MAX) * 0.1f,
            0.4f + (float(rand()) / RAND_MAX) * 0.2f,
            0.3f + (float(rand()) / RAND_MAX) * 0.2f,
            0.2f + (float(rand()) / RAND_MAX) * 0.2f
            });
    }

    for (int i = 0; i < 12; ++i) {
        seaweeds.push_back({
            (float(rand()) / RAND_MAX) * 2.0f - 1.0f,
            0.2f + (float(rand()) / RAND_MAX) * 0.2f,
            0.4f + (float(rand()) / RAND_MAX) * 0.3f
            });
    }

    for (int i = 0; i < 8; ++i) {
        bool right = rand() % 2 == 0;
        float speed = right ? (0.002f + (float(rand()) / RAND_MAX) * 0.002f) : -(0.002f + (float(rand()) / RAND_MAX) * 0.002f);
        crabs.push_back({
            (float(rand()) / RAND_MAX) * 2.0f - 1.0f,
            -0.83f,
            speed,
            0.06f + (float(rand()) / RAND_MAX) * 0.02f,
            0.9f + (float(rand()) / RAND_MAX) * 0.1f,
            0.2f + (float(rand()) / RAND_MAX) * 0.1f,
            0.1f + (float(rand()) / RAND_MAX) * 0.1f,
            right
            });
        std::cout << "Initialized crab " << i << " at x=" << crabs.back().x << ", y=" << crabs.back().y << ", speed=" << speed << std::endl;
    }

    initDisplayLists();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(300, 100);
    int window = glutCreateWindow("OpenGL Aquarium with Fish, Sharks, Bubbles, and Grass");
    if (window == 0) {
        std::cerr << "Failed to create GLUT window" << std::endl;
        return -1;
    }

    initGL();
    initialize();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape); // Register reshape function
    glutTimerFunc(0, timer, 0);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);

    glutMainLoop();
    return 0;
}
