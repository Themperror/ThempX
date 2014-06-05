#include "../Headers/SoundHandler.h"

SoundHandler::SoundHandler(HWND handle,DWORD samplesPerSec,WORD bitsPerSample, WORD channels)
{
	initialized = false;
	directSound = NULL;
//	m_primaryBuffer = NULL;
//	samplesSecond = samplesPerSec;
	//bitsSample = bitsPerSample;
	//nChannels = channels;
	initialized = Initialize(handle);
}

bool SoundHandler::Initialize(HWND hwnd)
{
	bool result;
	// Initialize direct sound and the primary sound buffer.
	result = InitializeDirectSound(hwnd);

	if(!result)
	{
		return false;
	}
	return true;
}
void SoundHandler::Stop(std::string tag)
{
	std::string name = LowCaseString(tag);
	for(unsigned int i = 0; i < currentlyPlayingSounds.size();i++)
	{
		if(currentlyPlayingSounds.at(i)->name.compare(name) == 0)
		{
			DWORD status = NULL;
			currentlyPlayingSounds.at(i)->soundBuffer->GetStatus(&status);
			if(status & DSBSTATUS_PLAYING)
			{
				currentlyPlayingSounds.at(i)->soundBuffer->Stop();
			}
		}
	}
	for(unsigned int i = 0; i < sounds.size();i++)
	{
		if(sounds.at(i)->name.compare(name) == 0)
		{
			DWORD status = NULL;
			sounds.at(i)->soundBuffer->GetStatus(&status);
			if(status & DSBSTATUS_PLAYING)
			{
				sounds.at(i)->soundBuffer->Stop();
			}
		}
	}
}
bool SoundHandler::IsPlaying(std::string tag)
{
	std::string name = LowCaseString(tag);
	bool isPlaying = false;
	for(unsigned int i = 0; i < currentlyPlayingSounds.size();i++)
	{
		if(currentlyPlayingSounds.at(i)->name.compare(name) == 0)
		{
			DWORD status = NULL;
			currentlyPlayingSounds.at(i)->soundBuffer->GetStatus(&status);
			if(status & DSBSTATUS_PLAYING)
			{
				isPlaying = true;
			}
		}
	}
	for(unsigned int i = 0; i < sounds.size();i++)
	{
		if(sounds.at(i)->name.compare(name) == 0)
		{
			DWORD status = NULL;
			sounds.at(i)->soundBuffer->GetStatus(&status);
			if(status & DSBSTATUS_PLAYING)
			{
				isPlaying = true;
			}
		}
	}
	return isPlaying;
}
bool SoundHandler::InitializeDirectSound(HWND hwnd)
{
	HRESULT result;
	DSBUFFERDESC bufferDesc;
//	WAVEFORMATEX waveFormat;
 
	result = DirectSoundCreate8(NULL, &directSound, NULL);
	if(result != DS_OK)
	{
		return false;
	}
 
	// Set the cooperative level
	result = directSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if(result != DS_OK)
	{
		return false;
	}

	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;
 
	/*// Get the default sound device.
	result = directSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
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
	*/
	return true;
}
void SoundHandler::Release()
{
	//Release all currently playing sounds
	for(unsigned int i = 0; i < currentlyPlayingSounds.size();i++)
	{
		if(currentlyPlayingSounds.at(i)->soundBuffer != NULL)
		{
			currentlyPlayingSounds.at(i)->soundBuffer->Release();
		}
	}

	//Release all main sounds
	for(unsigned int i = 0; i < sounds.size();i++)
	{
		if(sounds.at(i)->soundBuffer != NULL)
		{
			sounds.at(i)->soundBuffer->Release();
		}
	}
	//shutdown directsound itself
	if(directSound)
	{
		directSound->Release();
		directSound = NULL;
	}
 
	return;
}
bool SoundHandler::LoadWaveFile(std::string filename,std::string tag)
{
	if(initialized)
	{
		std::string name = LowCaseString(tag);
		SoundData* data = new SoundData();
		data->name = name;
		data->soundBuffer = NULL;
		data->tag = "";

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

		error = fopen_s(&filePtr, _strdup(filename.c_str()), "rb");
		if(error != 0 || filePtr == NULL)
		{
			delete data;
			return false;
		}
 
		// Read in the wave file header.
		count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
		//std::cout << "reading file  = " << count << std::endl;
		if(count != 1)
		{
			return false;
		}
	

		
		data->channels = waveFileHeader.numChannels;
		data->samplesPerSec = waveFileHeader.sampleRate;
		data->bitsPerSample = waveFileHeader.bitsPerSample;

		// Check that the chunk ID is the RIFF format.
		if((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') || (waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
		{
			std::cout << "header false" << std::endl;
			return false;
		}
 
		// Check that the file format is the WAVE format.
		if((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') || (waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
		{
			std::cout << "header false" << std::endl;
			return false;
		}
 
		// Check that the sub chunk ID is the fmt format.
		if((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') || (waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
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
 
		// Check for the data chunk header.
		if((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||  (waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
		{
		
			std::cout << "header false" << std::endl;
			return false;
		}

		waveFormat.wFormatTag = WAVE_FORMAT_PCM;
		waveFormat.nSamplesPerSec = data->samplesPerSec;
		waveFormat.wBitsPerSample = data->bitsPerSample;
		waveFormat.nChannels = data->channels;
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

		result = directSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
		if(result != DS_OK)
		{
			std::cout << "result create soundbuffer false" << std::endl;
			return false;
		}
 
		// Test the buffer format against the direct sound 8 interface and create the secondary buffer.
		result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&data->soundBuffer);
		if(result != DS_OK)
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
		result = data->soundBuffer->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
		if(FAILED(result))
		{
			return false;
		}
 
		// Copy the wave data into the buffer.
		memcpy(bufferPtr, waveData, waveFileHeader.dataSize);
 
		// Unlock the secondary buffer after the data has been written to it.
		result = data->soundBuffer->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
		if(result != DS_OK)
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
bool SoundHandler::PlayRandom(std::vector<std::string>* names, DWORD volume)
{
	int r = rand() % names->size();
	return PlayWaveFile(_strdup(names->at(r).c_str()),volume);
}
bool SoundHandler::PlayWaveFile(std::string sName, DWORD volume)
{
	std::string name = LowCaseString(sName);
	if(initialized)
	{
		for(unsigned int i = 0; i < currentlyPlayingSounds.size();i++)
		{
			DWORD status = NULL;
			currentlyPlayingSounds.at(i)->soundBuffer->GetStatus(&status);
			if(status & DSBSTATUS_PLAYING)
			{
				
			}
			else
			{
				currentlyPlayingSounds.at(i)->soundBuffer->Release();
				delete currentlyPlayingSounds.at(i);
				currentlyPlayingSounds.erase(currentlyPlayingSounds.begin()+i);
			}
		}
		for(unsigned int i = 0; i < sounds.size();i++) 
		{
			if(name.compare(sounds.at(i)->name) == 0)
			{
				DWORD status = NULL;
				sounds.at(i)->soundBuffer->GetStatus(&status);
				if(status & DSBSTATUS_PLAYING)
				{
					IDirectSoundBuffer* duplicate;
					directSound->DuplicateSoundBuffer(sounds.at(i)->soundBuffer,&duplicate);

					SoundData* data = new SoundData();

					data->bitsPerSample = sounds.at(i)->bitsPerSample;
					data->bufferDesc = sounds.at(i)->bufferDesc;
					data->channels = sounds.at(i)->channels;
					data->name = sounds.at(i)->name;
					data->samplesPerSec = sounds.at(i)->samplesPerSec;
					data->tag = sounds.at(i)->tag;
					data->soundBuffer = (IDirectSoundBuffer8*)duplicate;

					currentlyPlayingSounds.push_back(data);


					duplicate->SetCurrentPosition(0);
					signed int value  = -((100-volume) * 100);
					duplicate->SetVolume(value);
					duplicate->Play(0, 0, 0);
					return true;
				}
				else
				{
					IDirectSoundBuffer* sBuffer = sounds.at(i)->soundBuffer;
					sBuffer->SetCurrentPosition(0);
					signed int value  = -((100-volume) * 100);
					sBuffer->SetVolume(value);
					sBuffer->Play(0,0,0);
					return true;
				}
			}
		}
	}
	return false;
}