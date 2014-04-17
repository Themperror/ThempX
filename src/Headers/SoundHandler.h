#ifndef _SOUNDHANDLER_H_
#define _SOUNDHANDLER_H_

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <MMSystem.h>
#include <dsound.h>
#include <stdio.h>
#include <iostream>
#include <vector>

class SoundHandler
{
public:
	SoundHandler(HWND handle,DWORD samplesPerSec,WORD bitsPerSample, WORD channels);
	
	void ShutdownDirectSound();
	bool Initialize(HWND);
	
	bool LoadWaveFile(char* path, char* name, DWORD samplesPerSec,WORD bitsPerSample, WORD channels);
	bool PlayWaveFile(char* name, DWORD volume = 96);
	bool PlayRandom(std::vector<std::string>* names, DWORD volume = 96);

private:
	struct WaveHeaderType
	{
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[4];
		unsigned long dataSize;
	};
	struct SoundData
	{
		SoundData()
		{
			samplesPerSec = 0;
			bitsPerSample = 0;
			channels = 0;
			soundBuffer= NULL;
			name = "NULL";
			tag = "NULL";
		}
		DWORD samplesPerSec;
		WORD bitsPerSample;
		WORD channels;
		std::vector<IDirectSoundBuffer*> extraSounds;
		IDirectSoundBuffer8* soundBuffer;
		char* name;
		char* tag; //unused atm
		DSBUFFERDESC bufferDesc;
	};

	std::vector<SoundData*> currentlyPlayingSounds;
	bool InitializeDirectSound(HWND);
	//IDirectSoundBuffer8* CreateCopySoundBuffer(char* name);
	//DWORD samplesSecond;
	//WORD bitsSample;
	//WORD nChannels;
	bool initialized;
	std::vector<SoundData*> sounds;
	IDirectSound8* m_DirectSound;
	//IDirectSoundBuffer* m_primaryBuffer;
	//IDirectSoundBuffer8* m_secondaryBuffer1;
};
#endif