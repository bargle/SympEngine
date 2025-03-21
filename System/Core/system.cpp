#include "core_defs.h"
#include "system.h"
#include "gameinterface.h"

/*
#define GL_GLEXT_PROTOTYPES
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
*/
//Interfaces
#include "../Input/InputInterfaceBase.h"
#include "../Video/VideoInterface3D.h"
#include "../Audio/AudioInterface.h"
#include "../Platform/platform.h"

//FIXME: move this to a platform implementation class
#if defined( __APPLE__ ) && defined( __MACH__ )

#include <GLFW/glfw3.h>

extern "C" {
void cb_keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    
    switch( action )
    {
        case GLFW_RELEASE:
        {
            g_pSystem->KeyEvent(key, 0, 0, 0);
        }break;
        case GLFW_PRESS:
        {
            g_pSystem->KeyEvent(key, 1, 0, 0);
        }break;
        default:
            break;
    }
}
}

#elif defined( __GNUC__ )
    
    //FIXME: move this to a platform specific implementation class

    void cb_idle( void ) 
    {
    	if(g_pSystem){
    		g_pSystem->Update();
    	}
    }
    
    void cb_display( void ) 
    {
    	if(g_pSystem){
    		g_pSystem->Render();
    	}
    }

    void cb_resize( int Width, int Height )
    {
    	if(g_pSystem){
    		g_pSystem->Resize(Width, Height);
    	}
    }

    void cb_mouseclick( int button, int state, int x, int y)
    {
    	if(g_pSystem){
    		g_pSystem->MouseClick(button, state, x, y);
    	}
    }

    void cb_mousemove(int x, int y) {
    	if(g_pSystem){
    		g_pSystem->MouseMove(x, y);
    	}
    }
    
    void cb_keyboard_dn( unsigned char key, int x, int y) {
    	if(g_pSystem){
    		g_pSystem->KeyEvent(key, 1, x, y);
    	}
    }
    
    void cb_keyboard_up( unsigned char key, int x, int y) {
    	if(g_pSystem){
    		g_pSystem->KeyEvent(key, 0, x, y);
    	}
    }

#endif

SympSystem *g_pSystem = NULL;

SympSystem::SympSystem():
m_pVideoInterface(NULL),
m_pVideoInterface3D(NULL),
m_pAudioInterface(NULL),
m_pInputInterface(NULL),
m_pNetInterface(NULL),
m_pResourceInterface(NULL),
m_pGameInterface(NULL),
m_fFrameTime(0.0f)
{
	g_pSystem = this;
}

SympSystem::~SympSystem()
{
	g_pSystem = NULL;
}

GEN_RESULT SympSystem::Init()
{
	m_Time.Start();
	return RESULT_OK;
}

GEN_RESULT SympSystem::Term()
{
	if(m_pGameInterface){
		m_pGameInterface->OnShutdown();
	}

	m_pGameInterface = NULL;

	return RESULT_OK;
}

GEN_RESULT SympSystem::Shutdown()
{
	Platform::setActive( false );
	return RESULT_OK;
}

GEN_RESULT SympSystem::Update(){
	assert(m_pGameInterface);
	
	Platform::pump();
	
	if(m_pGameInterface){
		m_Time.Stop();
		#ifdef __GNUC__
		m_fFrameTime = m_Time.TimeInMilliseconds();
		#else
		m_fFrameTime = m_Time.TimeInSeconds();
		#endif
		m_Time.Start();
		m_pGameInterface->OnUpdate(m_fFrameTime);
	
		if ( Platform::getActive() == false ) {
			return RESULT_FINISHED;
		}
	}
	return RESULT_OK;
}

GEN_RESULT SympSystem::Render(){
	assert(m_pGameInterface);
	if(m_pGameInterface){
		m_pGameInterface->OnRender(m_fFrameTime);
	}
	return RESULT_OK;
}

GEN_RESULT SympSystem::Resize(int nWidth, int nHeight) {
	assert(m_pGameInterface);
	if(m_pGameInterface){
		m_pGameInterface->OnResize(nWidth, nHeight);
	}
	return RESULT_OK;
}

GEN_RESULT SympSystem::KeyEvent( unsigned char key, int state, int x, int y) {
	assert(m_pGameInterface);
    
    printf ("System: key = %c (%d)\n", key, key);
    
#if MACOSX
    const char escape_key = 0;
#else
    const char escape_key = 27;
#endif
    

	if ( ( key == escape_key ) && ( state == 1 ) ) { //ESC
		Platform::toggleGrabMouse();
		return RESULT_OK;
	}
    
	if ( m_pGameInterface ) {
		m_pGameInterface->OnKeyEvent(key, state, x, y);
	}

    if ( key == 81 && state == 1 ) //quit
    {
        Platform::setActive(false); //Tell the system to shutdown
    }
    
	return RESULT_OK;
}

GEN_RESULT SympSystem::MouseClick(int button, int state, int x, int y) {
	
	assert(m_pGameInterface);
	if(m_pGameInterface){
		EMouseButton eButton = MOUSE_BUTTON_LEFT;
		switch(button)
		{
		case 1:
			eButton = MOUSE_BUTTON_LEFT;
			break;
		case 3:
			eButton = MOUSE_BUTTON_RIGHT;
			break;
		case 2:
			eButton = MOUSE_BUTTON_MIDDLE;
			break;
		default:
			break;
		}
		m_pGameInterface->OnMouseClick(eButton, state, x, y);
	}
	
	return RESULT_OK;
}

GEN_RESULT SympSystem::MouseMove(int x, int y) {
	assert(m_pGameInterface);
	if(m_pGameInterface){
		m_pGameInterface->OnMouseMove(x, y);
	}
	return RESULT_OK;
}

void  SympSystem::Startup(int argc, char** argv, unsigned int flags) {
    #ifdef __IOS
        m_pGameInterface->OnStartup(this);
	#elif __GNUC__
		assert(m_pGameInterface);
		if(!m_pGameInterface) 
			return;

	    //
	    m_pGameInterface->OnStartup(this);

	    while ( g_pSystem ) {
    		if ( g_pSystem->Update() == RESULT_FINISHED ) {
    			break;
    		}
    	}
	    
    	g_pSystem->Term();
    	
	#else
	    m_pGameInterface->OnStartup(this);
	#endif
}

GEN_RESULT SympSystem::CreateInterfaces(uint32 nFlags)
{
	if ( nFlags & FLAG_VIDEO_3D ) {
		m_pVideoInterface3D = new IVideoInterface3D;
	}

	//TODO: refactor this a bit.
	if ( nFlags & FLAG_AUDIO ) {
#if !defined(__IOS)
#if defined( __linux__ ) || defined( __linux )
		m_pAudioInterface = new IAudioInterface_fmod;
#else
		m_pAudioInterface = new IAudioInterface;	
#endif
#endif
	} 

	if ( nFlags & FLAG_INPUT ) {
		m_pInputInterface = new IInputInterface;
	}

	if ( nFlags & FLAG_NET ) {
	}

	if ( nFlags & FLAG_RESOURCE ) {
	}

	return RESULT_OK;
}

GEN_RESULT SympSystem::TermInterfaces(uint32 nFlags)
{
	if ( nFlags & FLAG_VIDEO_3D ) {
		if ( m_pVideoInterface3D) {
			delete m_pVideoInterface3D;
			m_pVideoInterface3D = NULL;
		}
	}

	if ( nFlags & FLAG_AUDIO ) {
		delete m_pAudioInterface;
		m_pAudioInterface = NULL;

	}

	if ( nFlags & FLAG_INPUT ) {
		if ( m_pInputInterface ) {
			delete m_pInputInterface;
			m_pInputInterface = NULL;
		}
	}

	if ( nFlags & FLAG_NET ) {
	}

	if( nFlags & FLAG_RESOURCE ) {
	}

	return RESULT_OK;
}
