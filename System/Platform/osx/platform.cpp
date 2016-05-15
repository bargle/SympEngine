#include "core_defs.h"
#include "system.h"
#include "gameinterface.h"
#include "../platform.h"


#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

extern "C" void cb_keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    MouseMove( (int)xpos, (int)ypos );
}

namespace Platform {
    
    GLFWwindow* window = nullptr;

	void sleep(long seconds, long milliseconds) {} //stub
	int createWindow(char *title, int width, int height, int bpp, bool fullscreen)
    {
        if (!glfwInit()) {
            return 1;
        }
        
        window = glfwCreateWindow(width, height, title, NULL, nullptr);
        
        if ( window == nullptr ) {
            printf( "Failed to create GLFW window.\n" );
            
            glfwTerminate();
            
            return 1;
        }
        
        glfwMakeContextCurrent(window);
        
        glewExperimental = GL_TRUE;
        
        glewInit();
        
        glfwSetKeyCallback(window, cb_keyCallback);
        
        glfwSetCursorPosCallback(window, cursor_position_callback );
        
        const GLubyte* openGLRenderer = glGetString(GL_RENDERER);
        
        const GLubyte* openGLVersion = glGetString(GL_VERSION);
        
        printf ( "Renderer: %s\n", openGLRenderer );
        printf ( "OpenGL version supported: %s\n", openGLVersion );
        
        return 0;
    }
	int closeWindow()
    {
        glfwTerminate();
        return 0;
    }
	void swapBuffers()
    {
        if ( window != nullptr )
        {
            glfwSwapBuffers(window);
        }
    }
	void pump() { //do our work here...
        glfwPollEvents();
	}
	void grabMouse()
    {
        printf("Grab mouse");
    }
	void ungrabMouse()
    {
        printf("Ungrab mouse");
    }
	void toggleGrabMouse() {}
	bool getActive() { return true; }
	void setActive( bool active ) {}

	void* getWindowHandle() { return (void*)0; }
}
