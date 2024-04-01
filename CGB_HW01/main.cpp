#include <GL/freeglut.h>
#include <cmath>
#include <vector>
#include <iostream>

// Global variables for camera position and angle
float cameraAngleX = 0.0f;
float cameraAngleY = 0.0f;
float cameraPosX = 0.0f;
float cameraPosY = 0.0f; // This will remain constant for level movement
float cameraPosZ = 5.0f;

// Initialize mouse position
int mouseX = 0, mouseY = 0;

struct Wall {
    float minX, maxX, minZ, maxZ; // Bounding box of the wall
};

std::vector<Wall> walls;

// Initialize your maze structure, including walls
void initMaze() {
    // Example: Add a cube wall centered at (0, 0) with a side length of 1
    Wall wall = { -0.5f, 0.5f, -0.5f, 0.5f }; // This assumes Y is up, and we're looking down on XZ plane
    walls.push_back(wall);
    // Add other walls as needed
}

bool checkCollisionWithWalls(float nextX, float nextZ) {
    for (const Wall& wall : walls) {
        if (nextX >= wall.minX && nextX <= wall.maxX &&
            nextZ >= wall.minZ && nextZ <= wall.maxZ) {
            return true; // Collision detected
        }
    }
    return false; // No collision
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Calculate camera direction based on mouse movement
    float lx = sin(cameraAngleX);
    float lz = -cos(cameraAngleX);
    float ly = sin(cameraAngleY);

    // The camera's Y position is constant; it doesn't change with mouse movement
    gluLookAt(cameraPosX, cameraPosY, cameraPosZ,
        cameraPosX + lx, cameraPosY + ly, cameraPosZ + lz,
        0.0f, 1.0f, 0.0f);

    for (const Wall& wall : walls) {
        glPushMatrix();
        float centerX = (wall.minX + wall.maxX) / 2.0f;
        float centerZ = (wall.minZ + wall.maxZ) / 2.0f;
        float sizeX = wall.maxX - wall.minX;
        float sizeZ = wall.maxZ - wall.minZ;

        glTranslatef(centerX, 0.0f, centerZ); // Translate to the center of the cube
        glScalef(sizeX, 1.0f, sizeZ); // Scale cube to correct size (assuming uniform height for simplicity)
        glutWireCube(1.0f); // Draw cube with unit size, scaling will adjust
        glPopMatrix();
    }


    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    float ratio = 1.0 * w / h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glViewport(0, 0, w, h);
    gluPerspective(45.0, ratio, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
}

void processNormalKeys(unsigned char key, int x, int y) {
    float fraction = 0.1f;

    float deltaX = 0, deltaZ = 0;

    // Calculate camera direction components based on the current camera angle
    float lx = sin(cameraAngleX);
    float lz = -cos(cameraAngleX);

    switch (key) {
    case 'w':
        deltaZ = fraction * lz;
        deltaX = fraction * lx;
        break;
    case 's':
        deltaZ = -fraction * lz;
        deltaX = -fraction * lx;
        break;
    case 'a':
        // Move left
        deltaX = -fraction * cos(cameraAngleX);
        deltaZ = -fraction * sin(cameraAngleX);
        break;
    case 'd':
        // Move right
        deltaX = fraction * cos(cameraAngleX);
        deltaZ = fraction * sin(cameraAngleX);
        break;
    }

    float nextX = cameraPosX + deltaX;
    float nextZ = cameraPosZ + deltaZ;

    if (!checkCollisionWithWalls(nextX, nextZ)) {
        // If no collision, proceed with movement
        cameraPosX = nextX;
        cameraPosZ = nextZ;
    }
    else {
        std::cout << "Collision" << "\n";
    }
}

void mouseMove(int x, int y) {
    // Initialize mouse position to the center of the window
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

    // Calculate the mouse movement from the center of the window
    int deltaX = x - windowWidth / 2;
    int deltaY = y - windowHeight / 2;

    // Only update camera angles if there was any movement
    if (deltaX != 0 || deltaY != 0) {
        cameraAngleX += deltaX * 0.001f;
        cameraAngleY -= deltaY * 0.001f;

        // Limit the vertical look angle to prevent flipping
        if (cameraAngleY < -1.5f)
            cameraAngleY = -1.5f;
        else if (cameraAngleY > 1.5f)
            cameraAngleY = 1.5f;
    }

    // Always re-center the mouse pointer to prevent it from leaving the window
    glutWarpPointer(windowWidth / 2, windowHeight / 2);
}


int main(int argc, char** argv) {
    // Initialize GLUT and create a window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(1600, 1200);
    glutCreateWindow("3D Navigation Example");
    glutSetCursor(GLUT_CURSOR_NONE);

    initMaze();

    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(display);
    glutKeyboardFunc(processNormalKeys);
    glutPassiveMotionFunc(mouseMove); // Mouse movement without buttons pressed

    // OpenGL initialization
    glEnable(GL_DEPTH_TEST);

    // Set the mouse pointer to the center of the window
    glutWarpPointer(400, 300);

    // Enter the GLUT event processing cycle
    glutMainLoop();

    return 1;
}