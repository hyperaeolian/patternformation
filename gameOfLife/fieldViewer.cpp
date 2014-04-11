#include <cmath>
#include "FIELD_2D.h"
#include "VEC3F.h"
#include <iostream>
#include "QUICKTIME_MOVIE.h"
#include "MERSENNE_TWISTER.h"
#include "TimeStamper.h"

#if _WIN32
#include <gl/glut.h>
#elif __APPLE__
#include <GLUT/glut.h>
#endif

using namespace std;


// resolution of the field
int xRes = 100;
int yRes = 100;

// the field being drawn and manipulated
FIELD_2D field(xRes, yRes);
FIELD_2D next(xRes, yRes);
// the resolution of the OpenGL window -- independent of the field resolution
int xScreenRes = 850;
int yScreenRes = 850;

// Text for the title bar of the window
string windowLabel("Field Viewer");

// mouse tracking variables
int xMouse         = -1;
int yMouse         = -1;
int mouseButton    = -1;
int mouseState     = -1;
int mouseModifiers = -1;

// current grid cell the mouse is pointing at
int xField = -1;
int yField = -1;

// animate the current runEverytime()?
bool animate = false;

// draw the grid over the field?
bool drawingGrid = false;

// print out what the mouse is pointing at?
bool drawingValues = false;

// currently capturing frames for a movie?
bool captureMovie = false;

// the current viewer eye position
VEC3F eyeCenter(0.5, 0.5, 1);

// current zoom level into the field
float zoom = 1.0;

// Quicktime movie to capture to
QUICKTIME_MOVIE movie;

// a random number generator
MERSENNE_TWISTER twister(123456);

// forward declare the caching function here so that we can
// put it at the bottom of the file
void runOnce();

// forward declare the timestepping function here so that we can
// put it at the bottom of the file
void runEverytime();
int frameCounter = 0;
struct Algo {
    
    void glider(int x, int y){
        field(x,y) = 1;
        field(x+1,y) = 1;
        field(x+2,y) = 1;
        field(x+2,y+1) = 1;
        field(x+1,y+2) = 1;
    }
    /*
     field(25,9) = field(23,8) = field(25,8) = field(13,7) = field(14,7) =
        field(21,7) = field(22,7) = field(35,7) = field(36,7) = field(12,6) =
        field(16,6) = field(21,6) = field(22,6) = field(35,6) = field(36,6) = 
        field(1,5)  = field(2,5)  = field(11,5) = field(17,5) = field(21,5) = 
        field(22,5) = field(1,4)  = field(2,4)  = field(11,4) = field(15,4) = 
        field(17,4) = field(18,4) = field(23,4) = field(25,4) = field(11,3) = 
        field(17,3) = field(25,3) = field(12,2) = field(16,2) = field(13,1) =
        field(14,1) = 1;
*/
    void glosperGliderGun(int x){
        //y = x - 16
        int y = x - 16;
        field(x,y)     = field(x-2,y-1) = field(x,y-1)   = field(x-12,y-2) = field(x-11,y-2) =
        field(x-4,y-2) = field(x-3,y-2) = field(x+10,y-2)= field(x+11,y-2) = field(x-13,y-3) =
        field(x-9,y-3) = field(x-4,y-3) = field(x-3,y-3) = field(x+10,y-3) = field(x+11,y-3) = 
        field(x-24,y-4)= field(x-23,y-4)= field(x-14,y-4)= field(x-8,y-4)  = field(x-4,y-4) = 
        field(x-3,y-4) = field(x-24,y-5)= field(x-23,y-5)= field(x-14,y-5) = field(x-10,y-5) = 
        field(x-8,y-5) = field(x-7,y-5) = field(x-2,y-5) = field(x,y-5)   = field(x-14,y-6) = 
        field(x-8,y-6) = field(x,y-6)   = field(x-13,y-7)= field(x-9,y-7)  = field(x-12,y-8) =
        field(x-11,y-8) = field(x+4,y+10) = 1;
    }

        void acorn(int x){
        int y = x;
        field(x,y)     = field(x+1,y) = field(x+1,y+2)   = field(x+3,y+1) = field(x+4,y) =
        field(x+5,y) = field(x+6,y) = 1;
    }

    void pulsar(int x, int y){
        field(x-4,y-6) = field(x-3,y-6) = field(x-2,y-6) = field(x+2,y-6) = field(x+3,y-6) = 
        field(x+4,y-6) = field(x-6,y-4) = field(x-1,y-4) = field(x+1,y-4) = field(x+6,y-4) =
        field(x-6,y-3) = field(x-1,y-3) = field(x+1,y-3) = field(x+6,y-3) = field(x-6,y-2) = 
        field(x-1,y-2) = field(x+1,y-2) = field(x+6,y-2) = field(x-4,y-1) = field(x-3,y-1) = 
        field(x-2,y-1) = field(x+2,y-1) = field(x+3,y-1) = field(x+4,y-1) = field(x-4,y+1) = 
        field(x-3,y+1) = field(x-2,y+1) = field(x+2,y+1) = field(x+3,y+1) = field(x+4,y+1) = 
        field(x-6,y+2) = field(x-1,y+2) = field(x+1,y+2) = field(x+6,y+2) = field(x-6,y+3) = 
        field(x-1,y+3) = field(x+1,y+3) = field(x+6,y+3) = field(x-6,y+4) = field(x-1,y+4) = 
        field(x+1,y+4) = field(x+6,y+4) = field(x-4,y+6) = field(x-3,y+6) = field(x-2,y+6) = 
        field(x+2,y+6) = field(x+3,y+6) = field(x+4,y+6) = 1;
    }
};

typedef Algo* gol_algo;
gol_algo automaton = new Algo();
///////////////////////////////////////////////////////////////////////
// Figure out which field element is being pointed at, set xField and
// yField to them
///////////////////////////////////////////////////////////////////////
void refreshMouseFieldIndex(int x, int y)
{
    // make the lower left the origin
    y = yScreenRes - y;
    
    float xNorm = (float)x / xScreenRes;
    float yNorm = (float)y / yScreenRes;
    
    float halfZoom = 0.5 * zoom;
    float xWorldMin = eyeCenter[0] - halfZoom;
    float xWorldMax = eyeCenter[0] + halfZoom;
    
    // get the bounds of the field in screen coordinates
    //
    // if non-square textures are ever supported, change the 0.0 and 1.0 below
    float xMin = (0.0 - xWorldMin) / (xWorldMax - xWorldMin);
    float xMax = (1.0 - xWorldMin) / (xWorldMax - xWorldMin);
    
    float yWorldMin = eyeCenter[1] - halfZoom;
    float yWorldMax = eyeCenter[1] + halfZoom;
    
    float yMin = (0.0 - yWorldMin) / (yWorldMax - yWorldMin);
    float yMax = (1.0 - yWorldMin) / (yWorldMax - yWorldMin);
    
    float xScale = 1.0;
    float yScale = 1.0;
    
    if (xRes < yRes)
        xScale = (float)yRes / xRes;
    if (xRes > yRes)
        yScale = (float)xRes / yRes;
    
    // index into the field after normalizing according to screen
    // coordinates
    xField = xScale * xRes * ((xNorm - xMin) / (xMax - xMin));
    yField = yScale * yRes * ((yNorm - yMin) / (yMax - yMin));
    
    // clamp to something inside the field
    xField = (xField < 0) ? 0 : xField;
    xField = (xField >= xRes) ? xRes - 1 : xField;
    yField = (yField < 0) ? 0 : yField;
    yField = (yField >= yRes) ? yRes - 1 : yField;
}

///////////////////////////////////////////////////////////////////////
// Print a string to the GL window
///////////////////////////////////////////////////////////////////////
void printGlString(string output)
{
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    for (unsigned int x = 0; x < output.size(); x++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, output[x]);
}

///////////////////////////////////////////////////////////////////////
// dump the field contents to a GL texture for drawing
///////////////////////////////////////////////////////////////////////
void updateTexture(FIELD_2D& texture)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, 
                 texture.xRes(), 
                 texture.yRes(), 0, 
                 GL_LUMINANCE, GL_FLOAT, 
                 texture.data());
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glEnable(GL_TEXTURE_2D);
}

///////////////////////////////////////////////////////////////////////
// draw a grid over everything
///////////////////////////////////////////////////////////////////////
void drawGrid()
{
    glColor4f(0.1, 0.1, 0.1, 1.0);
    
    float dx = 1.0 / xRes;
    float dy = 1.0 / yRes;
    
    if (xRes < yRes)
        dx *= (float)xRes / yRes;
    if (xRes > yRes)
        dy *= (float)yRes / xRes;
    
    glBegin(GL_LINES);
    for (int x = 0; x < field.xRes() + 1; x++)
    {
        glVertex3f(x * dx, 0, 1);
        glVertex3f(x * dx, 1, 1);
    }
    for (int y = 0; y < field.yRes() + 1; y++)
    {
        glVertex3f(0, y * dy, 1);
        glVertex3f(1, y * dy, 1);
    }
    glEnd();
}

///////////////////////////////////////////////////////////////////////
// GL and GLUT callbacks
///////////////////////////////////////////////////////////////////////
void glutDisplay()
{
    // Make ensuing transforms affect the projection matrix
    glMatrixMode(GL_PROJECTION);
    
    // set the projection matrix to an orthographic view
    glLoadIdentity();
    float halfZoom = zoom * 0.5;
    
    glOrtho(-halfZoom, halfZoom, -halfZoom, halfZoom, -10, 10);
    
    // set the matrix mode back to modelview
    glMatrixMode(GL_MODELVIEW);
    
    // set the lookat transform
    glLoadIdentity();
    gluLookAt(eyeCenter[0], eyeCenter[1], 1,  // eye
              eyeCenter[0], eyeCenter[1], 0,  // center 
              0, 1, 0);   // up
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    float xLength = 1.0;
    float yLength = 1.0;
    
    if (xRes < yRes)
        xLength = (float)xRes / yRes;
    if (yRes < xRes)
        yLength = (float)yRes / xRes;
    
    glEnable(GL_TEXTURE_2D); 
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(0.0, 0.0, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(0.0, yLength, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(xLength, yLength, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(xLength, 0.0, 0.0);
    glEnd();
    glDisable(GL_TEXTURE_2D); 
    
    // draw the grid, but only if the user wants
    if (drawingGrid)
        drawGrid();
    
    // if there's a valid field index, print it
    if (xField >= 0 && yField >= 0 &&
        xField < field.xRes() && yField < field.yRes())
    {
        glLoadIdentity();
        
        // must set color before setting raster position, otherwise it won't take
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        
        // normalized screen coordinates (-0.5, 0.5), due to the glLoadIdentity
        float halfZoom = 0.5 * zoom;
        glRasterPos3f(-halfZoom* 0.95, -halfZoom* 0.95, 0);
        
        // build the field value string
        char buffer[256];
        string fieldValue("(");
        sprintf(buffer, "%i", xField);
        fieldValue = fieldValue + string(buffer);
        sprintf(buffer, "%i", yField);
        fieldValue = fieldValue + string(", ") + string(buffer) + string(") = ");
        sprintf(buffer, "%f", field(xField, yField));
        fieldValue = fieldValue + string(buffer);
        
        // draw the grid, but only if the user wants
        if (drawingValues)
            printGlString(fieldValue);
    }
    
    // if we're recording a movie, capture a frame
    if (captureMovie)
        movie.addFrameGL();
    
    glutSwapBuffers();
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void printCommands()
{
    cout << "=============================================================== " << endl;
    cout << " Field viewer code for MAT 200C " << endl;
    cout << "=============================================================== " << endl;
    cout << " q           - quit" << endl;
    cout << " v           - type the value of the cell under the mouse" << endl;
    cout << " g           - throw a grid over everything" << endl;
    cout << " m           - start/stop capturing a movie" << endl;
    cout << " r           - read in a PNG file " << endl;
    cout << " w           - write out a PNG file " << endl;
    cout << " left mouse  - pan around" << endl;
    cout << " right mouse - zoom in and out " << endl;
    cout << " shift left mouse - draw on the grid " << endl;
}

///////////////////////////////////////////////////////////////////////
// Map the arrow keys to something here
///////////////////////////////////////////////////////////////////////
void glutSpecial(int key, int x, int y)
{
    switch (key)
    {
        case GLUT_KEY_LEFT:
            break;
        case GLUT_KEY_RIGHT:
            break;
        case GLUT_KEY_UP:
            break;
        case GLUT_KEY_DOWN:
            break;
        default:
            break;
    }
}

///////////////////////////////////////////////////////////////////////
// Map the keyboard keys to something here
///////////////////////////////////////////////////////////////////////
void glutKeyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'a':
            animate = !animate;
            break;
        case 'g':
            drawingGrid = !drawingGrid;
            break;
        case '?':
            printCommands();
            break;
        case 'v':
            drawingValues = !drawingValues;
            break;
        case 'm':
            // if we were already capturing a movie
            if (captureMovie)
            {
                // write out the movie
                movie.writeMovie("movie.mov");
                
                // reset the movie object
                movie = QUICKTIME_MOVIE();
                
                // stop capturing frames
                captureMovie = false;
            }
            else
            {
                cout << " Starting to capture movie. " << endl;
                captureMovie = true;
            }
            break;
        case 'r':
            field.readPNG("lena.png");
            xRes = field.xRes();
            yRes = field.yRes();
            break;
        case 'w':
        {
            TimeStamper ts;
            field.writePNG(ts.timestampedFilename("output",".png"));
        }
            break;
        case 'q':
            exit(0);
            break;
        default:
            break;
    }
}

///////////////////////////////////////////////////////////////////////
// Do something if the mouse is clicked
///////////////////////////////////////////////////////////////////////
void glutMouseClick(int button, int state, int x, int y)
{
    int modifiers = glutGetModifiers();
    mouseButton = button;
    mouseState = state;
    mouseModifiers = modifiers;
    
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && modifiers & GLUT_ACTIVE_SHIFT)
    {
        // figure out which cell we're pointing at
        refreshMouseFieldIndex(x,y);
        
        // set the cell
        field(xField, yField) = 1;
        
        // make sure nothing else is called
        return;
    }
    
    xMouse = x;  
    yMouse = y;
}

///////////////////////////////////////////////////////////////////////
// Do something if the mouse is clicked and moving
///////////////////////////////////////////////////////////////////////
void glutMouseMotion(int x, int y)
{
    if (mouseButton == GLUT_LEFT_BUTTON && 
        mouseState == GLUT_DOWN && 
        mouseModifiers & GLUT_ACTIVE_SHIFT)
    {
        // figure out which cell we're pointing at
        refreshMouseFieldIndex(x,y);
        
        // set the cell
        field(xField, yField) = 1;
        
        // make sure nothing else is called
        return;
    }
    
    float xDiff = x - xMouse;
    float yDiff = y - yMouse;
    float speed = 0.001;
    
    if (mouseButton == GLUT_LEFT_BUTTON) 
    {
        eyeCenter[0] -= xDiff * speed;
        eyeCenter[1] += yDiff * speed;
    }
    if (mouseButton == GLUT_RIGHT_BUTTON)
        zoom -= yDiff * speed;
    
    xMouse = x;
    yMouse = y;
}

///////////////////////////////////////////////////////////////////////
// Do something if the mouse is not clicked and moving
///////////////////////////////////////////////////////////////////////
void glutPassiveMouseMotion(int x, int y)
{
    refreshMouseFieldIndex(x,y);
}

///////////////////////////////////////////////////////////////////////
// animate and display new result
///////////////////////////////////////////////////////////////////////
void glutIdle()
{
    if (animate)
    {
        runEverytime();
    }
    updateTexture(field);
    glutPostRedisplay();
}

//////////////////////////////////////////////////////////////////////////////
// open the GLVU window
//////////////////////////////////////////////////////////////////////////////
int glvuWindow()
{
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE| GLUT_RGBA);
    glutInitWindowSize(xScreenRes, yScreenRes); 
    glutInitWindowPosition(10, 10);
    glutCreateWindow(windowLabel.c_str());
    
    // set the viewport resolution (w x h)
    glViewport(0, 0, (GLsizei) xScreenRes, (GLsizei) yScreenRes);
    
    // set the background color to gray
    glClearColor(0.1, 0.1, 0.1, 0);
    
    // register all the callbacks
    glutDisplayFunc(&glutDisplay);
    glutIdleFunc(&glutIdle);
    glutKeyboardFunc(&glutKeyboard);
    glutSpecialFunc(&glutSpecial);
    glutMouseFunc(&glutMouseClick);
    glutMotionFunc(&glutMouseMotion);
    glutPassiveMotionFunc(&glutPassiveMouseMotion);
    
    // enter the infinite GL loop
    glutMainLoop();
    
    // Control flow will never reach here
    return EXIT_SUCCESS;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
    // In case the field is rectangular, make sure to center the eye
    if (xRes < yRes)
    {
        float xLength = (float)xRes / yRes;
        eyeCenter[0] = xLength * 0.5;
    }
    if (yRes < xRes)
    {
        float yLength = (float)yRes / xRes;
        eyeCenter[1] = yLength * 0.5;
    }
    runOnce();
    
    // initialize GLUT and GL
    glutInit(&argc, argv);
    
    // open the GL window
    glvuWindow();
    delete automaton;
    return 1;
}


///////////////////////////////////////////////////////////////////////
// This function is called every frame -- do something interesting
// here.
///////////////////////////////////////////////////////////////////////
void runEverytime(){
   
    for (int x = 0; x < xRes; x++){
        for (int y = 0; y < yRes; y++){ 
            int xBound1 = x % xRes;
            int xBound2 = xRes - x;
            int yBound1 = y % yRes;
            int yBound2 = yRes - y;
    
            x = x < 0    ? xBound2 : x;
            x = x > xRes ? xBound1 : x;
            y = y < 0    ? yBound2 : y;
            y = y > yRes ? yBound1 : y;

            int neighbors = field(x-1,y+1) + field(x,y+1) + field(x+1,y+1) +
                            field(x-1,y)   + field(x+1,y) + field(x-1,y-1) +
                            field(x,y-1)   + field(x+1,y-1);
                            
            if (field(x,y) == 1){
                next(x,y) = neighbors == 2 || neighbors == 3 ? 1 : 0;
            }   
            if (field(x,y) == 0){
                next(x,y) = neighbors == 3 ? 1 : 0;
            }

        }
    }
    field = next;
    usleep(1000000);
}

///////////////////////////////////////////////////////////////////////
// This is called once at the beginning so you can precache
// something here
///////////////////////////////////////////////////////////////////////
void runOnce(){
     for (int x = 0; x < xRes; x++){
        for (int y = 0; y < yRes; y++){
          field(x,y) = 0;
        }
     }
     //automaton->glosperGliderGun(60); 
    //automaton->glosperGliderGun(80);
     //automaton->glosperGliderGun(100);
     //automaton->glosperGliderGun(120);
     //automaton->glosperGliderGun(140);
     //automaton->glosperGliderGun(160);
   //  automaton->acorn(160);
     automaton->pulsar(25,25);
     automaton->pulsar(35,35);
     automaton->pulsar(45,45);
     automaton->pulsar(55,55);
     automaton->pulsar(65,65);
     automaton->pulsar(75,75);

     automaton->pulsar(25,75);
     automaton->pulsar(35,65);
     automaton->pulsar(45,55);

     automaton->pulsar(75,25);
     automaton->pulsar(65,35);
     automaton->pulsar(55,45);

     
}
