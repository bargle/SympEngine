#include "system_core.h"
#include "performance.h"
#include "platform.h"
#include "VideoInterface3D.h"
#include "InputInterface.h"
#include "Game.h"
#include "PhysicsManager.h"
#include "GameObjectManager.h"
#include "Interfaces.h"
#include "Camera.h"
#include "Player.h"
#include "World.h"

//temp
#include <stdio.h>
#include <time.h>

//TEMP GLOBALS
#define NUM_PRIMS 2
namespace {
	Prim_t prim[NUM_PRIMS];

	Camera	g_Camera;
	Player	g_Player;
	World	g_World;
}

SympSystem *g_pCoreSystem = NULL;

MyGame::MyGame():m_Yaw(0),m_Pitch(0),m_hFont(0),m_Elapsed(0.0f){}
MyGame::~MyGame(){ /*free any memory allocated at runtime...*/ }

int MyGame::OnStartup(SympSystem* pSystem)
{
	srand( time(NULL) );
	
	g_pCoreSystem = pSystem;
	g_pCoreSystem->CreateInterfaces( FLAG_VIDEO_3D | FLAG_INPUT );
	IVideoInterface3D* pVideoInterface3D = g_pCoreSystem->GetVideoInterface3D();
	if(pVideoInterface3D) {
		pVideoInterface3D->Init(NULL, 960, 640, 32, true);
	}

	//////// 2D
	HTEXTURE hTex = pVideoInterface3D->CreateTexture("test");
	for(int i = 0; i < NUM_PRIMS; ++i) {
		prim[i].vPos.Set( (float)i * 100.0f, (float)i * 50.0f, 0.0f);
		prim[i].hTex = hTex;
		prim[i].count = 4;
		prim[i].verts = (Vert3d_t*)malloc(sizeof(Vert3d_t) * 4);

		prim[i].verts[0].vPos.Set(0.0f, 0.0f, 0.0f);
		prim[i].verts[0].vTexCoords.Set(0.0f, 0.0f);
		prim[i].verts[0].vColor.Set(0.0f, 1.0f, 1.0f, 1.0f);

		prim[i].verts[1].vPos.Set(128.0f, 0.0f, 0.0f);
		prim[i].verts[1].vTexCoords.Set(1.0f, 0.0f);
		prim[i].verts[1].vColor.Set(1.0f, 1.0f, 0.0f, 1.0f);

		prim[i].verts[2].vPos.Set(128.0f, 128.0f, 0.0f);
		prim[i].verts[2].vTexCoords.Set(1.0f, 1.0f);
		prim[i].verts[2].vColor.Set(1.0f, 0.0f, 1.0f, 1.0f);

		prim[i].verts[3].vPos.Set(0.0f, 128.0f, 0.0f);
		prim[i].verts[3].vTexCoords.Set(0.0f, 1.0f);
		prim[i].verts[3].vColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
	}

	////////
	if( pVideoInterface3D ) {
		//const char* name, int width, int height, int char_width, int char_height, int rows, int columns
		m_hFont = pVideoInterface3D->CreateHFont( "FreeMono", 128, 128, 12.8f, 22.0f, 5, 10 );
	}

	g_World.Init();

	g_Player.SetPos( Vector3d( 100.0, 65.0f, 300.0f ) );

	return 0;
}

void MyGame::OnShutdown(){
	IVideoInterface3D* pVideoInterface3D = g_pCoreSystem->GetVideoInterface3D();
	if(pVideoInterface3D) {
		for(int i = 0; i < NUM_PRIMS; ++i ){
			pVideoInterface3D->DeleteTexture(prim[i].hTex);
		}
	}

	for(int i = 0; i < NUM_PRIMS; ++i){
		free(prim[i].verts);
	}
}

#define SQUARE( x ) ( x * x )

void MyGame::OnUpdate(float elapsed){

	m_Elapsed = elapsed;
	g_Camera.m_ViewPort.yaw = g_Player.GetYaw();
	g_Camera.m_ViewPort.pitch = g_Player.GetPitch();
	
	Matrix4x4 rotYaw;
	rotYaw.Identity();
	rotYaw.RotateYaw( DEG_TO_RAD( g_Camera.m_ViewPort.yaw ) );
	
	Matrix4x4 rotPitch;
	rotPitch.Identity();
	rotPitch.RotatePitch( DEG_TO_RAD(g_Camera.m_ViewPort.pitch) );
	
	g_Camera.m_ViewPort.rRotation.Set( rotYaw );
	g_Camera.m_ViewPort.rRotation.Multiply( rotPitch );
	g_Camera.m_ViewPort.rRotation.NormalizeRotation();
	//g_Camera.m_ViewPort.rRotation.Identity();
	//g_Camera.m_ViewPort.rRotation.RotateYaw( DEG_TO_RAD(g_Camera.m_ViewPort.yaw) );
	//g_Camera.m_ViewPort.rRotation.NormalizeRotation();
	//FIXME: figure out how to apply both of these without denormalizing the 3rd (pitch) row.
	//g_Camera.m_ViewPort.rRotation.RotatePitch( DEG_TO_RAD(g_Camera.m_ViewPort.pitch) );
	//g_Camera.m_ViewPort.rRotation.NormalizeRotation();
	/* //debug
	float res0 = SQUARE( g_Camera.m_ViewPort.rRotation.m[0] ) + SQUARE( g_Camera.m_ViewPort.rRotation.m[1] ) + SQUARE( g_Camera.m_ViewPort.rRotation.m[2] );
	float res1 = SQUARE( g_Camera.m_ViewPort.rRotation.m[4] ) + SQUARE( g_Camera.m_ViewPort.rRotation.m[5] ) + SQUARE( g_Camera.m_ViewPort.rRotation.m[6] );
	float res2 = SQUARE( g_Camera.m_ViewPort.rRotation.m[8] ) + SQUARE( g_Camera.m_ViewPort.rRotation.m[9] ) + SQUARE( g_Camera.m_ViewPort.rRotation.m[10] );
	printf("0 (%f,%f,%f) = %f\n", g_Camera.m_ViewPort.rRotation.m[0], g_Camera.m_ViewPort.rRotation.m[1], g_Camera.m_ViewPort.rRotation.m[2], res0);
	printf("1 (%f,%f,%f) = %f\n", g_Camera.m_ViewPort.rRotation.m[4], g_Camera.m_ViewPort.rRotation.m[5], g_Camera.m_ViewPort.rRotation.m[6], res1);
	printf("2 (%f,%f,%f) = %f\n", g_Camera.m_ViewPort.rRotation.m[8], g_Camera.m_ViewPort.rRotation.m[9], g_Camera.m_ViewPort.rRotation.m[10], res2);
	*/

	//let the player move...
	g_Player.Update( elapsed );

	Vector3d camPos;
	camPos.Set( g_Player.m_Pos );
	
	Vector3d playerHalfwidths;
	g_Player.GetHalfWidths( playerHalfwidths );
	camPos.y += (playerHalfwidths.y * 0.75f);

	g_Camera.m_Pos.Set( camPos );
	g_Camera.m_ViewPort.vPos.Set( camPos.x * -1.0f, camPos.y * -1.0f, camPos.z * -1.0f );

	g_Camera.m_ViewPort.rRotation.SetTranslation(g_Camera.m_ViewPort.vPos);

	//Render the scene
	OnRender(elapsed);

	//
	/*
	float fTimeRemaining = ((1.0f/65.0f)*1000.0f) - elapsed;
	if(fTimeRemaining > 0.0f)
	{
		long nSleep = (long)(fTimeRemaining);
		if(nSleep > 17)
		{
			nSleep = 16;
		}
		//Sleep(dwSleep);
		//Platform::sleep(0, nSleep);
		//printf("sleep: %d\n", nSleep);
	}
	*/
	Platform::sleep(0, 10);
}

void MyGame::OnRender(float elapsed){
	IVideoInterface3D* pVideoInterface3D = g_pCoreSystem->GetVideoInterface3D();
	if(pVideoInterface3D) {
		pVideoInterface3D->BeginScene();

		pVideoInterface3D->SetViewport(&g_Camera.m_ViewPort);
		pVideoInterface3D->Start3D();
		
		//pVideoInterface3D->RenderPrim3DVBO(g_pBox);
		g_World.OnRender( elapsed );

		pVideoInterface3D->SetViewport(NULL);
		
		// Draw UI
		pVideoInterface3D->Start2D();
		
		/*
		//add draw commands...
		for(int i = 0 ; i < NUM_PRIMS; ++i){ 
			pVideoInterface3D->RenderPrim2D(&prim[i]);
		}
		*/

		{
		char buf[64];
		sprintf(buf, "(FPS: %3d) (FACES: %d) PITCH: %.2f YAW: %.2f", (int)(1.0f/elapsed), g_World.GetNumFaces(), g_Player.GetPitch(), g_Player.GetYaw() );
		
		pVideoInterface3D->RenderText(0, buf, 0.0f, 25.0f);
            
        char buf2[64];
        sprintf(buf2, "Pos: %0.2f, %0.2f, %0.2f", g_Player.m_Pos.x, g_Player.m_Pos.y, g_Player.m_Pos.z);
        pVideoInterface3D->RenderText(0, buf2, 0.0f, 0.0f);
		
        char buf3[64];
		sprintf(buf3, "Vel: %0.3f, %0.3f, %0.3f -> sp: %0.3f", g_Player.GetVelocity().x, g_Player.GetVelocity().y, g_Player.GetVelocity().z, g_Player.GetSpeed());
        pVideoInterface3D->RenderText(0, buf3, 0.0f, 50.0f);

		/*
		Matrix4x4& mat = g_Camera.m_ViewPort.rRotation;
		
		float offset = 20.0f;
		sprintf(buf, "%f %f %f %f", mat.ma[0][0], mat.ma[0][1], mat.ma[0][2], mat.ma[0][3]);
		pVideoInterface3D->RenderText(0, buf, 0.0f, offset + 0.0f);
		sprintf(buf, "%f %f %f %f", mat.ma[1][0], mat.ma[1][1], mat.ma[1][2], mat.ma[1][3]);
		pVideoInterface3D->RenderText(0, buf, 0.0f, offset + 20.0f);
		sprintf(buf, "%f %f %f %f", mat.ma[2][0], mat.ma[2][1], mat.ma[2][2], mat.ma[2][3]);
		pVideoInterface3D->RenderText(0, buf, 0.0f, offset + 40.0f);
		sprintf(buf, "%f %f %f %f", mat.ma[3][0], mat.ma[3][1], mat.ma[3][2], mat.ma[3][3]);
		pVideoInterface3D->RenderText(0, buf, 0.0f, offset + 60.0f);
		bool isOrtho = mat.IsOrthogonal();
		pVideoInterface3D->RenderText(0, isOrtho ? "Ortho" : "NonOrtho", 0.0f, offset + 80.0f);
        */
		}
		

		pVideoInterface3D->EndScene();
	}
}

void MyGame::OnResize(int nWidth, int nHeight){
	IVideoInterface3D* pVideoInterface3D = g_pCoreSystem->GetVideoInterface3D();
	if(pVideoInterface3D) {
		pVideoInterface3D->Resize(nWidth, nHeight);
	}
}

void MyGame::OnInputEvent(){
}

void MyGame::OnKeyEvent(unsigned char key, int state, int x, int y) {
	//printf("Key %c, state: %d\n", key, state);

	if( key == 'w' || key == 'W' || key == 87 ) {
		if ( state ) {
			g_Player.TurnOnMoveFlag( MOVE_FORWARD );
		} else {
			g_Player.TurnOffMoveFlag( MOVE_FORWARD );
		}
	}

	if( key == 's' || key == 'S' || key == 83 ) {
		if ( state ) {
			g_Player.TurnOnMoveFlag( MOVE_BACKWARD );
		} else {
			g_Player.TurnOffMoveFlag( MOVE_BACKWARD );
		}
	}

	if( /*key == 'd' || key == 'D' ||*/ key == 65 ) {
		if ( state ) {
			g_Player.TurnOnMoveFlag( MOVE_RIGHT );
		} else {
			g_Player.TurnOffMoveFlag( MOVE_RIGHT );
		}
	}

	if( /*key == 'a' || key == 'A' ||*/ key == 68 ) {
		if ( state ) {
			g_Player.TurnOnMoveFlag( MOVE_LEFT );
		} else {
			g_Player.TurnOffMoveFlag( MOVE_LEFT );
		}
	}

	if( key == 'r' || key == 'R' ) {
		if ( state ) {
			g_Player.m_Pos.Set( 100.0, 65.0f, 300.0f );
		}
	}

	if( key == 32 ) {
		if ( state ) {
			g_Player.TurnOnMoveFlag( MOVE_JUMP );
		} else {
			g_Player.TurnOffMoveFlag( MOVE_JUMP );
		}
	}

	float keySpeed = 250.0f;
	if ( key == 37 ) //left
	{
		g_Player.RotateOnYaw( keySpeed * m_Elapsed  );

	} else if ( key == 39 ) //right
	{
		g_Player.RotateOnYaw( -keySpeed * m_Elapsed  );
	}  else if ( key == 38 ) //up
	{
			g_Player.RotateOnPitch( keySpeed * m_Elapsed  );
	}  else if ( key == 40 ) //down
	{
			g_Player.RotateOnPitch( -keySpeed * m_Elapsed  );
	} 

	if( key == 'p' || key == 'P' ) {
		if ( state ) {
			printf("Pos: %f %f %f\n", g_Player.m_Pos.x, g_Player.m_Pos.y, g_Player.m_Pos.z);
		}
	}
	
	if ( key == '1' && state ) {
		g_Camera.m_ViewPort.flags ^= (1<<1);
	}

	//esc...
	if ( key == 27 && state )
	{
		//toggle capture mouse...
		IInputInterface* input = g_pCoreSystem->GetInputInterface();
		input->GrabMouse( true );
		
	}
}

void MyGame::OnMouseClick(EMouseButton button, int state, int x, int y) {
	//printf("CLICK: %d %d\n", button, state);
	if( button == MOUSE_BUTTON_RIGHT ) {
		if ( state ) {
			g_Player.TurnOnMoveFlag( MOVE_JET );
		} else {
			g_Player.TurnOffMoveFlag( MOVE_JET );
		}
	}

#ifdef __IOS
    if( state == 0 ){
        OnMouseMove(x, y);
    }
#endif
    
	m_Yaw = x;
	m_Pitch = y;
}

void MyGame::OnMouseMove(int x, int y) {

#ifdef __IOS
	int nDeltaYaw = x - m_Yaw;
	int nDeltaPitch = m_Pitch - y;
	m_Yaw = x;
	m_Pitch = y;
#elif WIN32
	int nDeltaYaw = -x;
	int nDeltaPitch = -y;
	m_Yaw -= x;
	m_Pitch -= y;
#else
	int nDeltaYaw = m_Yaw - x;
	int nDeltaPitch = m_Pitch - y;
	m_Yaw = x;
	m_Pitch = y;
#endif

	//printf("pitch: %d\n", m_Pitch);

	g_Player.RotateOnYaw( (float)nDeltaYaw * 0.15f );
	g_Player.RotateOnPitch( (float)nDeltaPitch * 0.125f );
}

