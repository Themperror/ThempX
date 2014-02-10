#include "../Headers/SoundHandler.h"

SoundHandler::SoundHandler(HWND handle,DWORD samplesPerSec,WORD bitsPerSample, WORD channels)
{
	initialized = false;
	m_DirectSound = NULL;
	m_primaryBuffer = NULL;
	samplesSecond = samplesPerSec;
	bitsSample = bitsPerSample;
	nChannels = channels;
	initialized = Initialize(handle);
}
SoundHandler::~SoundHandler()
{
	Shutdown();
}

bool SoundHandler::Initialize(HWND hwnd)
{
	bool result;
	// Initialize direct sound and the primary sound buffer.
	result = InitializeDirectSound(hwnd);
	std::cout << " init sound = " << result << std::endl;
	if(!result)
	{
		return false;
	}
	return true;
}
void SoundHandler::Shutdown()
{
	// Release the secondary buffer.
	for(unsigned int i =0;i< sounds.size();i++)
	{
		for(unsigned int j=0;j<sounds.at(i).extraSounds.size();j++)
		{
			sounds.at(i).extraSounds.at(j)->Release();
			delete sounds.at(i).extraSounds.at(i);
		}
		sounds.at(i).soundBuffer->Release();
	}
	sounds.clear();

	// Shutdown the Direct Sound API.
	ShutdownDirectSound();
 
	return;
}
bool SoundHandler::InitializeDirectSound(HWND hwnd)
{
	HRESULT result;
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;
 
	result = DirectSoundCreate8(NULL, &m_DirectSound, NULL);
	if(FAILED(result))
	{
		return false;
	}
 
	// Set the cooperative level
	result = m_DirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if(FAILED(result))
	{
		return false;
	}

	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;
 
	// Get the default sound device.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
	if(FAILED(result))
	{
		return false;
	}

	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = samplesSecond;
	waveFormat.wBitsPerSample = bitsSample;
	waveFormat.nChannels = nChannels;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;
 
	// Set the primary buffer to be the wave format specified.
	result = m_primaryBuffer->SetFormat(&waveFormat);
	if(FAILED(result))
	{
		return false;
	}
 
	return true;
}

void SoundHandler::ShutdownDirectSound()
{
	// Release the primary sound buffer pointer.
	if(m_primaryBuffer)
	{
		m_primaryBuffer->Release();
		m_primaryBuffer = 0;
	}
 
	// Release the direct sound pointer.
	if(m_DirectSound)
	{
		m_DirectSound->Release();
		m_DirectSound = 0;
	}
 
	return;
}

bool SoundHandler::LoadWaveFile(char* filename,char* name, DWORD samplesPerSec,WORD bitsPerSample, WORD channels)
{
	if(initialized)
	{
		SoundData data;
		data.bitsPerSample = bitsPerSample;
		data.samplesPerSec = samplesPerSec;
		data.channels = channels;
		data.name = name;
		data.soundBuffer = NULL;
		data.tag = NULL;

		int error;
		FILE* filePtr;
		unsigned int count;
		WaveHeaderType waveFileHeader;
		WAVEFORMATEX waveFormat;
		DSBUFFERDESC bufferDesc;
		HRESULT result;
		IDirectSoundBuffer* tempBuffer;
		unsigned char* waveData;
		unsigned char *bufferPtr;
		unsigned long bufferSize;

		error = fopen_s(&filePtr, filename, "rb");
		if(error != 0)
		{
			std::cout << "Opening file  = " << 0 << std::endl;
			return false;
		}
		else
		{
			std::cout << "Opening file  = " << 1 << std::endl;
		}
 
		// Read in the wave file header.
		count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
		std::cout << "reading file  = " << count << std::endl;
		if(count != 1)
		{
			return false;
		}
 
		// Check that the chunk ID is the RIFF format.
		if((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') || 
		   (waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
		{
		
			std::cout << "header false" << std::endl;
			return false;
		}
 
		// Check that the file format is the WAVE format.
		if((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
		   (waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
		{
		
			std::cout << "header false" << std::endl;
			return false;
		}
 
		// Check that the sub chunk ID is the fmt format.
		if((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
		   (waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
		{
			std::cout << "header false" << std::endl;
			return false;
		}
 
		// Check that the audio format is WAVE_FORMAT_PCM.
		if(waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
		{
			std::cout << "format false" << std::endl;
			return false;
		}
 
		// Check that the wave file was recorded in stereo format.
		if(waveFileHeader.numChannels != data.channels)
		{
			std::cout << "channels false" << std::endl;
			return false;
		}
 
		// Check that the wave file was recorded at a sample rate of 44.1 KHz.
		if(waveFileHeader.sampleRate != data.samplesPerSec)
		{
			std::cout << "sample rate  false" << std::endl;
			return false;
		}
 
		// Ensure that the wave file was recorded in 16 bit format.
		if(waveFileHeader.bitsPerSample != data.bitsPerSample)
		{
		
			std::cout << "bits per sample false" << std::endl;
			return false;
		}
 
		// Check for the data chunk header.
		if((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
		   (waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
		{
		
			std::cout << "header false" << std::endl;
			return false;
		}

		waveFormat.wFormatTag = WAVE_FORMAT_PCM;
		waveFormat.nSamplesPerSec = data.samplesPerSec;
		waveFormat.wBitsPerSample = data.bitsPerSample;
		waveFormat.nChannels = data.channels;
		waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
		waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
		waveFormat.cbSize = 0;
 
		// Set the buffer description of the secondary sound buffer that the wave file will be loaded onto.
		bufferDesc.dwSize = sizeof(DSBUFFERDESC);
		bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
		bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
		bufferDesc.dwReserved = 0;
		bufferDesc.lpwfxFormat = &waveFormat;
		bufferDesc.guid3DAlgorithm = GUID_NULL;

		result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
		if(FAILED(result))
		{
			std::cout << "result create soundbuffer false" << std::endl;
			return false;
		}
 
		// Test the buffer format against the direct sound 8 interface and create the secondary buffer.
		result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&data.soundBuffer);
		if(FAILED(result))
		{
			std::cout << "result query interface false" << std::endl;
			return false;
		}
 
		// Release the temporary buffer.
		tempBuffer->Release();
		tempBuffer = 0;

		fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);
 
		// Create a temporary buffer to hold the wave file data.
		waveData = new unsigned char[waveFileHeader.dataSize];
		if(!waveData)
		{
			return false;
		}
 
		// Read in the wave file data into the newly created buffer.
		count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);
		if(count != waveFileHeader.dataSize)
		{
			return false;
		}
 
		// Close the file once done reading.
		error = fclose(filePtr);
		if(error != 0)
		{
			return false;
		}
 
		// Lock the secondary buffer to write wave data into it.
		result = data.soundBuffer->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
		if(FAILED(result))
		{
			return false;
		}
 
		// Copy the wave data into the buffer.
		memcpy(bufferPtr, waveData, waveFileHeader.dataSize);
 
		// Unlock the secondary buffer after the data has been written to it.
		result = data.soundBuffer->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
		if(FAILED(result))
		{
			return false;
		}

		sounds.push_back(data);
		// Release the wave data since it was copied into the secondary buffer.
		delete [] waveData;
		waveData = 0;
		
		return true;
	}		   
	return false;
}

IDirectSoundBuffer8* SoundHandler::CreateCopySoundBuffer(char* name)
{
	IDirectSoundBuffer8* finalBuffer;
	for(unsigned int i = 0; i<sounds.size();i++)
	{
		if(sounds.at(i).name == name)
		{
			std::memcpy(&finalBuffer,sounds.at(i).soundBuffer,sizeof(IDirectSoundBuffer8));
			return finalBuffer;
		}
	}
	std::cout << "no sound found with that name" << std::endl;
	return NULL;
}

void SoundHandler::ShutdownWaveFile(IDirectSoundBuffer8** secondaryBuffer)
{
	// Release the secondary sound buffer.
	for(unsigned int i = 0;i < sounds.size();i++)
	{
		if(sounds.at(i).soundBuffer)
		{
			sounds.at(i).soundBuffer->Release();
			sounds.at(i).soundBuffer = 0;
		}
	}
	return;
}

bool SoundHandler::PlayWaveFile(char* name)
{
	if(initialized)
	{
		for(unsigned int i =0; i<sounds.size();i++) //yo je was bezig met meerdere geluidjes, er gaat iets mis in deze nested forloop, het gekloonde geluidje geeft errors naar iets vaags, je wist nog niet wat want dat moest je nog uitzoeken.
		{
			if(name == sounds.at(i).name)
			{
				DWORD status = NULL;
				sounds.at(i).soundBuffer->GetStatus(&status);
				if(status & DSBSTATUS_PLAYING)
				{
					IDirectSoundBuffer* duplicate;
					m_DirectSound->DuplicateSoundBuffer(sounds.at(i).soundBuffer,&duplicate);
					sounds.at(i).extraSounds.push_back(duplicate);
					HRESULT result;
					result = duplicate->SetCurrentPosition(0);
					result = duplicate->SetVolume(DSBVOLUME_MAX);
					result = duplicate->Play(0, 0, 0);
					return true;
				}
				else
				{
					HRESULT result;
					// Set volume of the buffer to 100%.
					result = sounds.at(i).soundBuffer->SetVolume(DSBVOLUME_MAX);
					if(FAILED(result))
					{
						return false;
					}
 
					// Play the contents of the secondary sound buffer.
					result = sounds.at(i).soundBuffer->Play(0, 0, 0);
					if(FAILED(result))
					{
						return false;
					}
					return true;
				}
			}
			DWORD status = NULL;
			sounds.at(i).soundBuffer->GetStatus(&status);
			for(unsigned int j = 0; j<sounds.size();j++)
			{
				if(status & DSBSTATUS_PLAYING)
				{

				}
				else
				{
					sounds.at(i).extraSounds.at(j)->Release();
					sounds.at(i).extraSounds.erase(sounds.at(i).extraSounds.begin()+j);
					j = 0;
				}
			}
		}
	}
	return false;
}
