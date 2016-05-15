//#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <system_core.h>
#include <core_defs.h>
#include <gameinterface.h>

#include "Game.h"

/*
void CheckCPU(){
	VectorInt2d vPos;
	vPos.x = 5;
	vPos.y = 10;

	printf("x = %d, y = %d\n", vPos.x, vPos.y);

	CCPUInfo cpuInfo;
	cpuInfo.Init();

	unsigned long cpuid = cpuInfo.GetCPUID();
	printf("CPUID: 0x%08x\n", (unsigned int)cpuid );
	printf("Supports MMX: %s\n", (cpuid & CPU_SUPPORT_MMX) ? "Yes" : "No" );
	printf("Supports SSE: %s\n", (cpuid & CPU_SUPPORT_SSE) ? "Yes" : "No" );
	printf("Supports SSE2: %s\n", (cpuid & CPU_SUPPORT_SSE2) ? "Yes" : "No" );
}*/

namespace {
	SympSystem g_System;
	MyGame g_Game;

	void Print( const char * format, ... );
	void Print( const char * format, ... )
	{

	  char buffer[256];
	  va_list args;
	  va_start (args, format);
	  vsnprintf (buffer,256,format, args);
#if defined( __APPLE__ ) && defined( __MACH__ )
        printf( buffer );
#else
	  OutputDebugString( buffer );
#endif
	  va_end (args);
	}

}

int InitializeGameInterface() {
	g_System.RegisterGameInterface(&g_Game);
	char ** c = NULL;
	g_System.Startup( 0, c, 0);

	//TODO: Game should create the main window now.
	//Platform::createWindow("OpenGL Window", 1024, 768, 24, false);

	return 0;
}

int main( int argc, char** argv ) {

//	CheckCPU();
	
	SympSystem m_System;
	MyGame game;
	m_System.RegisterGameInterface(&game);
	
	m_System.Startup(argc, argv, 0);
	

	return 0;
}

int ResizeGameWindow(int width, int height) {
	g_Game.OnResize(width, height);
	return 0;
}

void MouseMove( int x, int y )
{
	g_Game.OnMouseMove( x, y );
}

void MouseClick( int button, int state, int x, int y )
{
	g_Game.OnMouseClick( (EMouseButton)button, state, x, y );
}

void KeyDown( int key )
{
	Print( "Key: %d\n", key );
	g_Game.OnKeyEvent( key, 1, 0, 0 );
}

void KeyUp( int key )
{
	Print( "Key: %d\n", key );
	g_Game.OnKeyEvent( key, 0, 0, 0 );
}

int UpdateGameInterface() {
	g_System.Update();
	return 0;
}