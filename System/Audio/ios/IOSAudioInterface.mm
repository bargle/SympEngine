#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#import <AudioToolbox/AudioToolbox.h>

//Obj-c
typedef ALvoid	AL_APIENTRY	(*alBufferDataStaticProcPtr) (const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq);
ALvoid  alBufferDataStaticProc(const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq)
{
	static	alBufferDataStaticProcPtr	proc = NULL;
    
    if (proc == NULL) {
        proc = (alBufferDataStaticProcPtr) alcGetProcAddress(NULL, (const ALCchar*) "alBufferDataStatic");
    }
    
    if (proc)
        proc(bid, format, data, size, freq);
	
    return;
}

void CloseAudioFile( AudioFileID afid ) {
    if ( afid ) { 
        AudioFileClose(afid);
    }
}

void* GetOpenALAudioData(CFURLRef inFileURL, ALsizei *outDataSize, ALenum *outDataFormat, ALsizei*	outSampleRate)
{
	OSStatus						err = noErr;	
	UInt64							fileDataSize = 0;
	AudioStreamBasicDescription		theFileFormat;
	UInt32							thePropertySize = sizeof(theFileFormat);
	AudioFileID						afid = 0;
	void*							theData = NULL;
	
	// Open a file with ExtAudioFileOpen()
	err = AudioFileOpenURL(inFileURL, kAudioFileReadPermission, 0, &afid);
	if(err) { 
        printf("MyGetOpenALAudioData: AudioFileOpenURL FAILED, Error = %ld\n", err); 
        CloseAudioFile( afid ); 
        return NULL;
    }
	
	// Get the audio data format
	err = AudioFileGetProperty(afid, kAudioFilePropertyDataFormat, &thePropertySize, &theFileFormat);
	if(err) { 
        printf("MyGetOpenALAudioData: AudioFileGetProperty(kAudioFileProperty_DataFormat) FAILED, Error = %ld\n", err); 
        CloseAudioFile( afid ); 
        return NULL; 
    }
	
	if (theFileFormat.mChannelsPerFrame > 2)  { 
		printf("MyGetOpenALAudioData - Unsupported Format, channel count is greater than stereo\n"); 
        CloseAudioFile( afid ); 
        return NULL;
	}
	
	if ((theFileFormat.mFormatID != kAudioFormatLinearPCM) || (!TestAudioFormatNativeEndian(theFileFormat))) { 
		printf("MyGetOpenALAudioData - Unsupported Format, must be little-endian PCM\n"); 
        CloseAudioFile( afid ); 
        return NULL;
	}
	
	if ((theFileFormat.mBitsPerChannel != 8) && (theFileFormat.mBitsPerChannel != 16)) { 
		printf("MyGetOpenALAudioData - Unsupported Format, must be 8 or 16 bit PCM\n"); 
        CloseAudioFile( afid ); 
        return NULL;
	}
	
	
	thePropertySize = sizeof(fileDataSize);
	err = AudioFileGetProperty(afid, kAudioFilePropertyAudioDataByteCount, &thePropertySize, &fileDataSize);
	if(err) { 
        printf("MyGetOpenALAudioData: AudioFileGetProperty(kAudioFilePropertyAudioDataByteCount) FAILED, Error = %ld\n", err); 
        CloseAudioFile( afid ); 
        return NULL;
    }
	
	// Read all the data into memory
	UInt32		dataSize = fileDataSize;
	theData = malloc(dataSize);
	if (theData)
	{
		AudioFileReadBytes(afid, false, 0, &dataSize, theData);
		if(err == noErr)
		{
			// success
			*outDataSize = (ALsizei)dataSize;
			*outDataFormat = (theFileFormat.mChannelsPerFrame > 1) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
			*outSampleRate = (ALsizei)theFileFormat.mSampleRate;
		}
		else 
		{ 
			// failure
			free (theData);
			theData = NULL; // make sure to return NULL
			printf("MyGetOpenALAudioData: ExtAudioFileRead FAILED, Error = %ld\n", err); 
            CloseAudioFile( afid ); 
            return NULL;
		}	
	}
	
	// Dispose the ExtAudioFileRef, it is no longer needed
	CloseAudioFile( afid );
	return theData;
}

void InitOpenAL() {
	ALCcontext		*newContext = NULL;
	ALCdevice		*newDevice = NULL;
	
	// Create a new OpenAL Device
	// Pass NULL to specify the systemÕs default output device
	newDevice = alcOpenDevice(NULL);
	if (newDevice != NULL)
	{
		// Create a new OpenAL Context
		// The new context will render to the OpenAL Device just created 
		newContext = alcCreateContext(newDevice, 0);
		if (newContext != NULL){
			//good...
            // Make the new context the Current OpenAL Context
			alcMakeContextCurrent(newContext);
		}
	}
	
	// clear any errors
	alGetError();
}
void TermOpenAL() {
    ALCcontext	*context = NULL;
    ALCdevice	*device = NULL;
	
	//TODO: delete any sources and buffers
	
	//Get active context (there can only be one)
    context = alcGetCurrentContext();
    //Get device for active context
    device = alcGetContextsDevice(context);
    //Release context
    if ( context != NULL ) {
        alcDestroyContext(context);
    }
    //Close device
    if ( device != NULL ) {
        alcCloseDevice(device);
    }
}



void interruptionListener(	void *	inClientData,
                          UInt32	inInterruptionState)
{
	if (inInterruptionState == kAudioSessionBeginInterruption)
	{
        TermOpenAL();
        
        //FLAG SOUNDS THAT WERE PLAYING AS SUCH
        
		// do nothing
        /*
		[THIS teardownOpenAL];
		if ([THIS isPlaying]) {
			THIS->_wasInterrupted = YES;
			THIS->_isPlaying = NO;
		}
        */
	}
	else if (inInterruptionState == kAudioSessionEndInterruption)
	{
		OSStatus result = AudioSessionSetActive(true);
		if (result) printf("Error setting audio session active! %d\n", (int)result);
        
        InitOpenAL();
        
        //resume sounds that were playing....
        
        /*
		[THIS initOpenAL];
		if (THIS->_wasInterrupted)
		{
			[THIS startSound];			
			THIS->_wasInterrupted = NO;
		}
         */
	}
}

//C
void InitAudioInterface() {

    // setup our audio session
    OSStatus result = AudioSessionInitialize(NULL, NULL, interruptionListener, NULL);
    if (result) printf("Error initializing audio session! %d\n", (int)result);
    else {
        UInt32 category = kAudioSessionCategory_AmbientSound;
        result = AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(category), &category);
        if (result) printf("Error setting audio session category! %d\n", (int)result);
        else {
            result = AudioSessionSetActive(true);
            if (result) printf("Error setting audio session active! %d\n", (int)result);
        }
    }
    
    //TODO: return result code...

}

void TermAudioInterface() {
}

