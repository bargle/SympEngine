#ifndef _CORE_SYSTEM_H_
#define _CORE_SYSTEM_H_

#include "performance.h"

extern void cb_resize( int Width, int Height );
extern void cb_mousemove(int x, int y);
extern void cb_keyboard_dn( unsigned char key, int x, int y);
extern void cb_keyboard_up( unsigned char key, int x, int y);
extern void cb_mouseclick( int button, int state, int x, int y);

#define CPU_SUPPORT_MMX (1<<23)
#define CPU_SUPPORT_SSE (1<<25)
#define CPU_SUPPORT_SSE2 (1<<26)

class CCPUInfo
{
public:
	CCPUInfo():m_CPUID(0){}
	~CCPUInfo(){}

	bool Init()
	{
		try
		{
			unsigned long nTemp = 0;
			
			#ifdef MSC_VER
			__asm mov eax, 01H;
			__asm cpuid;
			__asm mov nTemp, edx; //grab the EDX register
            #elif MACOSX
            nTemp = 0; //TODO: implement this for OSX
			#elif __GNUC__
			asm ("cpuid"
			: "=d" (nTemp)
			: "a" (1) );
			#endif
			m_CPUID = nTemp;

			return true;
		}
		catch(...)
		{
			return false;
		}
	}

	unsigned long GetCPUID(){return m_CPUID;}

protected:
	unsigned long m_CPUID;
};

class IVideoInterface;
class IVideoInterface3D;
class IAudioInterface;
class IInputInterface;
class INetInterface;
class IResourceInterface;
//Userland interface.  Not created here.
class IGameInterface;

#define FLAG_VIDEO		(1<<0) //This one is probably going away...
#define FLAG_VIDEO_3D	(1<<1)
#define FLAG_AUDIO		(1<<2)
#define FLAG_INPUT		(1<<3)
#define FLAG_NET		(1<<4)
#define FLAG_RESOURCE	(1<<5)

#define FLAG_NOGLUT		(1<<6)

class SympSystem
{
public:
	SympSystem();
	~SympSystem();

	GEN_RESULT Init();
	GEN_RESULT Term();
	
	GEN_RESULT Shutdown();

	//FIXME: this is a hack to allow glut callbacks to update the game...
	GEN_RESULT Update();
	GEN_RESULT Render();
	GEN_RESULT Resize(int nWidth, int nHeight);
	GEN_RESULT KeyEvent( unsigned char key, int state, int x, int y);
	GEN_RESULT MouseClick(int button, int state, int x, int y);
	GEN_RESULT MouseMove(int x, int y);

	// Interface Accessors
	inline IVideoInterface*		GetVideoInterface(){return m_pVideoInterface;}
	inline IVideoInterface3D*	GetVideoInterface3D(){return m_pVideoInterface3D;}
	inline IAudioInterface*		GetAudioInterface(){return m_pAudioInterface;}
	inline IInputInterface*		GetInputInterface(){return m_pInputInterface;}
	inline INetInterface*		GetNetInterface(){return m_pNetInterface;}
	inline IResourceInterface*	GetResourceInterface(){return m_pResourceInterface;}

	inline void RegisterGameInterface(IGameInterface* pGameInterface) { m_pGameInterface = pGameInterface; }
	void	Startup(int argc, char** argv, unsigned int flags);

	GEN_RESULT CreateInterfaces(uint32 nFlags);
	GEN_RESULT TermInterfaces(uint32 nFlags);

protected:

	//Interfaces
	IVideoInterface		*m_pVideoInterface;
	IVideoInterface3D	*m_pVideoInterface3D;
	IAudioInterface		*m_pAudioInterface;
	IInputInterface		*m_pInputInterface;
	INetInterface		*m_pNetInterface;
	IResourceInterface	*m_pResourceInterface;

	IGameInterface		*m_pGameInterface;

	CPerformanceCounter	m_Time;
	float				m_fFrameTime;

	CCPUInfo m_CPUID;
};

extern SympSystem *g_pSystem;

#endif
