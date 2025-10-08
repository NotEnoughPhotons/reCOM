#include "zsnd.h"

#include "gamez/zArchive/zar.h"
#include "gamez/zFTS/zfts.h"
#include "gamez/zNetwork/znet.h"
#include "gamez/zNode/node_world.h"
#include "gamez/zReader/zrdr.h"
#include "gamez/zSeal/zseal.h"
#include "gamez/zSystem/zsys.h"
#include "SDL3/SDL_log.h"

s32 g_iReverbTime = 0;

bool vagArchiveIsOpen = false;
bool bnkArchiveIsOpen = false;
bool snd_system_initialized = false;

zar::CZAR CSnd::m_bnkArchive;
zar::CZAR CSnd::m_vagArchive;

CSnd* CSnd::m_listener = NULL;
bool CSnd::m_hasreverb = false;
bool CSnd::m_listenerIsValid = false;

SDL_AudioStream* CSnd::m_audiostream = NULL;
u8* CSnd::m_snd_data = NULL;
u32 CSnd::m_snd_len = 0;

f32 CSnd::m_fadetime = 0.0f;

s32 CSnd::m_max_num_vags = 0;
bool CSnd::m_isDisabled = false;
bool CSnd::m_bShowSubtitles = false;
bool CSnd::m_vagEnabled = false;
std::vector<CSnd*> CSnd::m_soundlist;
std::unordered_map<const char*, CSnd*> CSnd::m_soundmap;

bool CFileCD::m_streamingSndActive = false;

_zrdr* sound_rdr = NULL;

CSnd::CSnd()
{
	if (!m_isDisabled)
	{
		
	}
}

void CSnd::Init()
{
	if (m_isDisabled)
	{
		return;
	}

	if (!vagArchiveIsOpen)
	{
		vagArchiveIsOpen = true;
		char path_buf[256];
		sprintf_s(path_buf, "%s/SOUNDS/VAGSTORE.ZAR", gamez_GameRunPath);
		m_vagArchive.Open(path_buf, 0, 0x21, 16);
	}
}

void CSnd::Open()
{
	CFileCD::m_streamingSndActive = true;

	if (!sound_rdr)
		sound_rdr = zrdr_read("sounds.rdr", NULL, 0);

	m_max_num_vags = 6;
	// TODO: Write wrapper for the 989snd library
	// snd_InitVAGStreamingEx(6, 0xf000, 1, 1);
}

void CSnd::NetOpen()
{
	if (!sound_rdr)
		sound_rdr = zrdr_read("sounds.rdr", NULL, 0);

	// snd_SetMasterVolume(8, 0x400);

	if (m_vagEnabled)
	{
		CFileCD::m_streamingSndActive = true;
		m_max_num_vags = 4;
		// snd_InitVagStreamingEx(4, 0xf000, 1, 1);
	}
}

bool CSnd::PostTick(f32 dT, void* buf)
{
	if (!m_listenerIsValid)
	{
		m_listenerIsValid = true;
	}

	if (m_listener)
	{
		if (zdb::CWorld::m_world->m_camera->m_IsInside || ftsPlayer->m_inside)
		{
			if (!m_hasreverb)
			{
				// TODO: Handle reverb using some audio engine
				// snd_AutoReverb(3, g_iReverbDepth, 3);
				m_hasreverb = true;
			}
		}
		else if (m_hasreverb)
		{
			// snd_AutoReverb(3, 0, g_iReverbTime, 3);
			m_hasreverb = false;
		}

		// CSnd::m_listener = CSnd::m_listenermatrix;
		// TODO: Do inverse matrix transform on listener
	}

	if (m_fadetime > 0.0f)
	{
		f32 time = m_fadetime - dT;

		if (time <= 0.0f)
		{
			// ResetGroupVolTemp();
		}
	}

	PostTick(dT, buf);
	CSndJukebox::Tick(dT);

	if (!theNetwork.m_bNetwork)
	{
		// Headset::ftsGetHeadset();
		// Headset::Tick(dT);
	}
	
	return false;
}

void CSnd::UIOpen()
{
	if (sound_rdr == NULL)
	{
		sound_rdr = zrdr_read("sounds.rdr");
	}

	if (m_vagEnabled)
	{
		m_max_num_vags = 4;
	}
}

void CSnd::Close()
{
	m_max_num_vags = 0;
	// Headset::ftsDeleteHeadset();
}

bool CSnd::vagReadOffset(const char* name, u32& offset, u32& size)
{
	if (m_isDisabled)
	{
		return false;
	}

	zar::CKey* key = m_vagArchive.FindKey(name);
	
	if (key)
	{
		offset = key->GetOffset();
		size = key->GetSize();
		m_vagArchive.CloseKey(key);
		return true;
	}
	
	return false;
}

void CSnd::LoadWAV(const char* name)
{
	SDL_PauseAudioStreamDevice(m_audiostream);
	m_snd_data = NULL;
	m_snd_len = 0;
	
	zar::CKey* key = m_vagArchive.OpenKey(name);

	if (!key)
	{
		return;
	}
    
	void* buffer = zmalloc(key->GetSize());
	m_vagArchive.Fetch(key, buffer, key->GetSize());
    
	SDL_IOStream* stream = SDL_IOFromMem(buffer, key->GetSize());
    
	m_snd_data = NULL;
	m_snd_len = NULL;
	SDL_AudioSpec audioSpec { SDL_AUDIO_S16, 1, 11025 };
	m_audiostream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audioSpec, NULL, NULL);
	SDL_LoadWAV_IO(stream, false, &audioSpec, &m_snd_data, &m_snd_len);

	SDL_ResumeAudioStreamDevice(m_audiostream);

	if (SDL_GetAudioStreamAvailable(m_audiostream) < (s32)m_snd_len)
	{
		SDL_PutAudioStreamData(m_audiostream, m_snd_data, m_snd_len);
	}
	
	m_vagArchive.CloseKey(key);
}

void CSnd::LoadVAG(const char* name)
{
	SDL_PauseAudioStreamDevice(m_audiostream);
	m_snd_data = NULL;
	m_snd_len = 0;
	
	zar::CKey* key = m_vagArchive.OpenKey(name);

	if (!key)
	{
		return;
	}

	size_t size = key->GetSize();
	u8* buffer = (u8*)zmalloc(size);
	m_vagArchive.Fetch(key, buffer, size);

	CBufferIO io;
	std::vector<s16> samples;
	
	io.Open(buffer, size);
	
	tag_VAGHeader header;
	vag_read_header(&io, &header);
	
	m_snd_len = (header.size - sizeof(tag_VAGHeader)) / sizeof(tag_VAGChunk);

	for (u32 i = 0; i < m_snd_len; i++)
	{
		vag_decode(&io, samples);
	}
	
	io.Close();
	
	SDL_AudioSpec audioSpec
	{
		SDL_AUDIO_S16,
		1,
		(s32)header.rate
	};
	
	m_audiostream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audioSpec, NULL, NULL);

	SDL_ResumeAudioStreamDevice(m_audiostream);

	if (SDL_GetAudioStreamAvailable(m_audiostream) < (s32)m_snd_len)
	{
		SDL_PutAudioStreamData(m_audiostream, samples.data(), samples.size() * sizeof(s16));
	}
	
	m_vagArchive.CloseKey(key);
}

void CSnd::LoadVPK(const char* name)
{
	SDL_PauseAudioStreamDevice(m_audiostream);
	m_snd_data = NULL;
	m_snd_len = 0;
	
	zar::CKey* key = m_vagArchive.OpenKey(name);

	if (!key)
	{
		return;
	}

	size_t size = key->GetSize();
	u8* buffer = (u8*)zmalloc(size);
	m_vagArchive.Fetch(key, buffer, size);

	CBufferIO io;
	std::vector<s16> samples;
	
	io.Open(buffer, size);
	
	tag_VPKHeader header;
	vpk_read_header(&io, &header);

	io.fseek(header.start_offset, SEEK_SET);
	
	m_snd_len = (header.channel_size / 2 - sizeof(tag_VPKHeader)) / sizeof(tag_VAGChunk);

	SDL_Log("%ui", m_snd_len);
	
	for (u32 i = 0; i < m_snd_len; i++)
	{
		vag_decode(&io, samples);
	}
	
	io.Close();
	
	SDL_AudioSpec audioSpec
	{
		SDL_AUDIO_S16,
		1,
		header.sample_rate
	};
	
	m_audiostream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audioSpec, NULL, NULL);

	SDL_ResumeAudioStreamDevice(m_audiostream);

	if (SDL_GetAudioStreamAvailable(m_audiostream) < (s32)m_snd_len)
	{
		SDL_PutAudioStreamData(m_audiostream, samples.data(), samples.size() * sizeof(s16));
	}
	
	m_vagArchive.CloseKey(key);
}

void CSnd::AddNewCSnd(CSnd* sound)
{
	if (!m_isDisabled)
	{
		m_soundmap[sound->m_name] = sound;
		m_soundlist.insert(m_soundlist.begin(), sound);
	}
}

CSnd* CSnd::GetSoundByName(const char* name)
{
	if (m_isDisabled)
	{
		return NULL;
	}

	auto sound = m_soundmap.find(name);

	if (sound == m_soundmap.end())
	{
		return NULL;
	}
	else
	{
		return sound->second;
	}

	return NULL;
}

bool CSnd::GetSubtitles()
{
	return m_bShowSubtitles;
}

float CSnd::CalcVol(f32 volume, f32 masterVolume)
{
	return 0.0f;
}

void CSnd::LoadCSnd(_zrdr* reader)
{

}