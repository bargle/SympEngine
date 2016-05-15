#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void DrawTriangles()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
    glLoadIdentity();									// Reset The Current Modelview Matrix
    glTranslatef(-1.5f,0.0f,-6.0f);						// Move Left 1.5 Units And Into The Screen 6.0
    glBegin(GL_TRIANGLES);								// Drawing Using Triangles
    glColor3f(1.0f,0.0f,0.0f);						// Set The Color To Red
    glVertex3f( 0.0f, 1.0f, 0.0f);					// Top
    glColor3f(0.0f,1.0f,0.0f);						// Set The Color To Green
    glVertex3f(-1.0f,-1.0f, 0.0f);					// Bottom Left
    glColor3f(0.0f,0.0f,1.0f);						// Set The Color To Blue
    glVertex3f( 1.0f,-1.0f, 0.0f);					// Bottom Right
    glEnd();											// Finished Drawing The Triangle
    glTranslatef(3.0f,0.0f,0.0f);						// Move Right 3 Units
    glColor3f(0.5f,0.5f,1.0f);							// Set The Color To Blue One Time Only
    glBegin(GL_QUADS);									// Draw A Quad
    glVertex3f(-1.0f, 1.0f, 0.0f);					// Top Left
    glVertex3f( 1.0f, 1.0f, 0.0f);					// Top Right
    glVertex3f( 1.0f,-1.0f, 0.0f);					// Bottom Right
    glVertex3f(-1.0f,-1.0f, 0.0f);					// Bottom Left
    glEnd();
}

int main(int argc, const char* argv[]) {
    if (!glfwInit()) {
        return 1;
    }
    
    
    int monitorCount;
    
    GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
    
    GLFWmonitor* largestMonitor = monitors[0];
    
    const GLFWvidmode* largestVidmode = glfwGetVideoMode(largestMonitor);
    
    for (int i = 1; i < monitorCount; i += 1) {
        const GLFWvidmode* vidmode = glfwGetVideoMode(monitors[i]);
        
        if (vidmode->width * vidmode->height > largestVidmode->width * largestVidmode->height) {
            largestVidmode = vidmode;
            
            largestMonitor = monitors[i];
        }
    }
    
    
    /*
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
    
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    */
    //GLFWwindow* window = glfwCreateWindow(largestVidmode->width, largestVidmode->height, "Project-L", NULL, nullptr);
    
    GLFWwindow* window = glfwCreateWindow(640, 480, "Project-L", NULL, nullptr);
    
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window." << std::endl;
        
        glfwTerminate();
        
        return 1;
    }
    
    glfwMakeContextCurrent(window);
    
    glewExperimental = GL_TRUE;
    
    glewInit();
    
    const GLubyte* openGLRenderer = glGetString(GL_RENDERER);
    
    const GLubyte* openGLVersion = glGetString(GL_VERSION);
    
    std::cout << "Renderer: " << openGLRenderer << std::endl;
    
    std::cout << "OpenGL version supported: " << openGLVersion << std::endl;
    
    glViewport(0,0,640,480);						// Reset The Current Viewport
    
    glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
    glLoadIdentity();									// Reset The Projection Matrix
    
    // Calculate The Aspect Ratio Of The Window
    gluPerspective(45.0f,(GLfloat)640/(GLfloat)480,0.1f,100.0f);
    
    glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
    glLoadIdentity();
    
    glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
    glClearDepth(1.0f);									// Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
    glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    
    glfwSetKeyCallback(window, keyCallback);
    
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        DrawTriangles();
        
        glfwSwapBuffers(window);
        
        glfwPollEvents();
    }
    
    glfwTerminate();
    
    return 0;
};