/*
 * $Id: fsound.c 27 2004-07-27 08:06:17Z sam $
 *
 *    (c) 2004 Sam Hocevar <sam@zoy.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License as published by Banlu Kemiyatorn. See
 *  http://sam.zoy.org/projects/COPYING.WTFPL for more details.
 */

#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstdint>

#include "flessd/types.h"
#include "flessd/music.h"
#include "flessd/sound.h"

#include "defs.h"

#include <vector>
#include <memory>

/* Sample variables */

using Stream = FSOUND_STREAM;
using Sample = FSOUND_SAMPLE;

struct Track {
    MIX_Track * handle = nullptr;
    bool is_used = false;
    size_t channel = 0;
};

struct FSound
{
    std::vector<std::unique_ptr<Stream>> streams;
    std::vector<std::unique_ptr<Sample>> samples;
    std::vector<std::unique_ptr<Track>> tracks;
    MIX_Mixer* mixer = nullptr;
};

static FSound global_instance;

DLL_API signed char F_API FSOUND_SetOutput(int outputtype)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_SetDriver(int driver)
{
    if(driver < 0)
        return TRUE; /* FIXME: disable everything */

    if(driver != 0)
        return FALSE;

    return TRUE;
}

DLL_API signed char F_API FSOUND_SetMixer(int mixer)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_SetBufferSize(int len_ms)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_SetHWND(void *hwnd)
{
    /* FIXME: we ignore the HWND value */
    return TRUE;
}

DLL_API signed char F_API FSOUND_SetMinHardwareChannels(int min)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_SetMaxHardwareChannels(int max)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_SetMemorySystem(void *pool, int poollen, FSOUND_ALLOCCALLBACK useralloc, FSOUND_REALLOCCALLBACK userrealloc, FSOUND_FREECALLBACK userfree)
{
    STUB();
    return TRUE;
}

// see SDL_mixer/docs/README-migration.md
#define MIX_DEFAULT_FREQUENCY 44100
#define MIX_DEFAULT_FORMAT SDL_AUDIO_S16
#define MIX_DEFAULT_CHANNELS 2

DLL_API signed char F_API FSOUND_Init(int mixrate, int maxsoftwarechannels, unsigned int flags)
{
    int i;

    /* Initialise SDL_mixer */
    if(!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        return false;
    }

    if (!MIX_Init()) {
        return false;
    }

    SDL_AudioSpec spec;
    spec.format = MIX_DEFAULT_FORMAT;
    spec.channels = MIX_DEFAULT_CHANNELS;
    spec.freq = MIX_DEFAULT_FREQUENCY;
    global_instance.mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
    if(!global_instance.mixer) {
        return false;
    }

    int tracks_count = maxsoftwarechannels;

    for (i = 0; i < tracks_count; ++i) {
        auto track = std::make_unique<Track>();

        track->channel = i;

        track->handle = MIX_CreateTrack(global_instance.mixer);
        if (!track->handle) {
            return false;
        }
        track->is_used = false;

        if (i == 0) {
            // never return channel zero, keep it for internal use
            // channels should be positive integers
            track->is_used = true;
        }

		global_instance.tracks.emplace_back(std::move(track));
    }

    return true;
}

DLL_API void F_API FSOUND_Close()
{
    if (global_instance.mixer) {

        MIX_DestroyMixer(global_instance.mixer);
        global_instance.mixer = nullptr;

        global_instance.samples.clear();
        global_instance.streams.clear();
        global_instance.tracks.clear();
    }
    MIX_Quit();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

DLL_API void F_API FSOUND_Update()
{
    STUB();
    return;
}

DLL_API void F_API FSOUND_SetSpeakerMode(unsigned int speakermode)
{
    STUB();
    return;
}

DLL_API void F_API FSOUND_SetSFXMasterVolume(int volume)
{
    STUB();
    return;
}

DLL_API void F_API FSOUND_SetPanSeperation(float pansep)
{
    STUB();
    return;
}

DLL_API void F_API FSOUND_File_SetCallbacks(FSOUND_OPENCALLBACK useropen, FSOUND_CLOSECALLBACK userclose, FSOUND_READCALLBACK userread, FSOUND_SEEKCALLBACK userseek, FSOUND_TELLCALLBACK usertell)
{
    STUB();
    return;
}

DLL_API int F_API FSOUND_GetError()
{
    STUB();
    return 0;
}

DLL_API float F_API FSOUND_GetVersion()
{
    return FMOD_VERSION;
}

DLL_API int F_API FSOUND_GetOutput()
{
    STUB();
    return 0;
}

DLL_API void * F_API FSOUND_GetOutputHandle()
{
    STUB();
    return NULL;
}

DLL_API int F_API FSOUND_GetDriver()
{
    STUB();
    return 0;
}

DLL_API int F_API FSOUND_GetMixer()
{
    STUB();
    return 0;
}

DLL_API int F_API FSOUND_GetNumDrivers()
{
    /* Tell the application we have one driver */
    return 1;
}

DLL_API const char * F_API FSOUND_GetDriverName(int id)
{
    if(id != 0)
        return "(unknown)";

    return "FLESSD";
}

DLL_API signed char F_API FSOUND_GetDriverCaps(int id, unsigned int *caps)
{
    if (caps) {
        *caps = 0;
    }
    return TRUE;
}

DLL_API int F_API FSOUND_GetOutputRate()
{
    STUB();
    return 0;
}

DLL_API int F_API FSOUND_GetMaxChannels()
{
    STUB();
    return 0;
}

DLL_API int F_API FSOUND_GetMaxSamples()
{
    STUB();
    return 0;
}

DLL_API int F_API FSOUND_GetSFXMasterVolume()
{
    STUB();
    return 0;
}

DLL_API signed char F_API FSOUND_GetNumHWChannels(int *num2d, int *num3d, int *total)
{
    STUB();
    return TRUE;
}

DLL_API int F_API FSOUND_GetChannelsPlaying()
{
    STUB();
    return 0;
}

DLL_API float F_API FSOUND_GetCPUUsage()
{
    STUB();
    return 0.0;
}

DLL_API void F_API FSOUND_GetMemoryStats(unsigned int *currentalloced, unsigned int *maxalloced)
{
    STUB();
    return;
}

DLL_API FSOUND_SAMPLE *F_API
FSOUND_Sample_Load(int index, const char *name_or_data, unsigned int mode,
                   int offset, int length)
{
    MIX_Audio *wave = nullptr;

    bool load_from_memory = FSOUND_LOADMEMORY == (mode & FSOUND_LOADMEMORY);
    bool is_managed = index == FSOUND_UNMANAGED;

    if (load_from_memory) {
        SDL_IOStream* io = SDL_IOFromMem(const_cast<void*>(reinterpret_cast<const void*>(name_or_data)), length);
        if (!io) {
			return nullptr;
        }
        wave = MIX_LoadAudio_IO(global_instance.mixer, io, false, true);
    } else {
        wave = MIX_LoadAudio(global_instance.mixer, name_or_data, false);
    }

    if(!wave) {
        return nullptr;
    }

    auto sample = std::make_unique<Sample>();

    sample->index = index;
    sample->wave = wave;
    sample->mode = mode;
    sample->is_managed = is_managed;

    auto & sample_ref = global_instance.samples.emplace_back(std::move(sample));

    return sample_ref.get();
}

DLL_API FSOUND_SAMPLE * F_API FSOUND_Sample_Alloc(int index, int length, unsigned int mode, int deffreq, int defvol, int defpan, int defpri)
{
    STUB();
    return NULL;
}

DLL_API void F_API FSOUND_Sample_Free(FSOUND_SAMPLE *sptr)
{
    STUB();
    return;
}

DLL_API signed char F_API FSOUND_Sample_Upload(FSOUND_SAMPLE *sptr, void *srcdata, unsigned int mode)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_Sample_Lock(FSOUND_SAMPLE *sptr, int offset, int length, void **ptr1, void **ptr2, unsigned int *len1, unsigned int *len2)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_Sample_Unlock(FSOUND_SAMPLE *sptr, void *ptr1, void *ptr2, unsigned int len1, unsigned int len2)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_Sample_SetMode(FSOUND_SAMPLE *sptr, unsigned int mode)
{
    if (!sptr) {
        return false;
    }
    if (mode == FSOUND_LOOP_NORMAL) {
        sptr->loop = true;
    } else if (mode == FSOUND_LOOP_OFF) {
        sptr->loop = false;
    }

    return true;
}

DLL_API signed char F_API FSOUND_Sample_SetLoopPoints(FSOUND_SAMPLE *sptr, int loopstart, int loopend)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_Sample_SetDefaults(FSOUND_SAMPLE *sptr, int deffreq, int defvol, int defpan, int defpri)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_Sample_SetDefaultsEx(FSOUND_SAMPLE *sptr, int deffreq, int defvol, int defpan, int defpri, int varfreq, int varvol, int varpan)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_Sample_SetMinMaxDistance(FSOUND_SAMPLE *sptr, float min, float max)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_Sample_SetMaxPlaybacks(FSOUND_SAMPLE *sptr, int max)
{
    STUB();
    return TRUE;
}

DLL_API FSOUND_SAMPLE * F_API FSOUND_Sample_Get(int sampno)
{
    STUB();
    return NULL;
}

DLL_API const char * F_API FSOUND_Sample_GetName(FSOUND_SAMPLE *sptr)
{
    STUB();
    return NULL;
}

DLL_API unsigned int F_API FSOUND_Sample_GetLength(FSOUND_SAMPLE *sptr) {
    if (sptr && sptr->wave) {
        return MIX_GetAudioDuration(sptr->wave);
    }
    return 0;
}

DLL_API signed char F_API FSOUND_Sample_GetLoopPoints(FSOUND_SAMPLE *sptr, int *loopstart, int *loopend)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_Sample_GetDefaults(FSOUND_SAMPLE *sptr, int *deffreq, int *defvol, int *defpan, int *defpri)
{
    return FSOUND_Sample_GetDefaultsEx(sptr, deffreq, defvol, defpan, defpri, 0, 0, 0);
}

DLL_API signed char F_API FSOUND_Sample_GetDefaultsEx(FSOUND_SAMPLE *sptr, int *deffreq, int *defvol, int *defpan, int *defpri, int *varfreq, int *varvol, int *varpan)
{
    if (!sptr || !sptr->wave) {
        return false;
    }

    SDL_AudioSpec spec;
    if (!MIX_GetAudioFormat(sptr->wave, &spec)) {
        return false;
    }

    if (deffreq) {
        *deffreq = spec.freq;
    }
    if (defvol) {
        *defvol = 255;
    }
    if (defpan) {
        *defpan = 128;
    }

    return true;
}

DLL_API unsigned int F_API FSOUND_Sample_GetMode(FSOUND_SAMPLE *sptr)
{
    if(!sptr)
        return 0;

    return sptr->mode;
}

DLL_API signed char F_API FSOUND_Sample_GetMinMaxDistance(FSOUND_SAMPLE *sptr, float *min, float *max)
{
    STUB();
    return TRUE;
}

DLL_API int F_API FSOUND_PlaySound(int channel, FSOUND_SAMPLE *sptr)
{
    return FSOUND_PlaySoundEx(channel, sptr, NULL, FALSE);
}

DLL_API int F_API FSOUND_PlaySoundEx(int channel, FSOUND_SAMPLE *sptr, FSOUND_DSPUNIT *dsp, signed char startpaused)
{
    if(!sptr)
        return -1;

    if (channel == FSOUND_FREE) {
        for (const auto & track: global_instance.tracks) {

            if (track->channel == 0) {
                // never return channel zero, keep it for internal use
                continue;
            }

            if (!track->is_used) {

                if (!MIX_SetTrackAudio(track->handle, sptr->wave)) {
                    return -1;
                }
                track->is_used = true;

                SDL_PropertiesID properties = SDL_CreateProperties();
                if (!properties) {
                    return -1;
                }
                if (!SDL_SetNumberProperty(properties, MIX_PROP_PLAY_LOOPS_NUMBER, sptr->loop ? -1 : 0)) {
                    return -1;
                }

                if (!MIX_PlayTrack(track->handle, properties)) {
                    return -1;
                }

                SDL_DestroyProperties(properties);

                if (startpaused) {
                    if (!MIX_PauseTrack(track->handle)) {
                        return -1;
                    }
                }
                return track->channel;
            }
        }
    } else {
        abort();
    }

    return -1;
}

static bool check_channel(int channel) {
    if (channel < 0 || channel >= global_instance.tracks.size()) {
        return false;
    }
    return true;
}

DLL_API signed char F_API FSOUND_StopSound(int channel)
{
    if (!check_channel(channel)) {
        return false;
    }
    if (!MIX_StopTrack(global_instance.tracks[channel]->handle, FALSE)) {
        return false;
    }
    global_instance.tracks[channel]->is_used = false;

    return true;
}

DLL_API signed char F_API FSOUND_SetFrequency(int channel, int freq)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_SetVolume(int channel, int vol)
{
    // FSOUND range: from 0 (silent) to 255 (full volume)
    // SDL: 0 (silent) to 1.0
    if (!check_channel(channel)) {
        return false;
    }
    if (!MIX_SetTrackGain(global_instance.tracks[channel]->handle, vol / 255.f)) {
        return false;
    }

    return true;
}

DLL_API signed char F_API FSOUND_SetVolumeAbsolute(int channel, int vol)
{
    //Mix_Volume(channel, vol * MIX_MAX_VOLUME / 255);

    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_SetPan(int channel, int pan)
{
    // FSOUND: 0 (full left) to 255 (full right)
    // SDL: 0.0f produces silence, 1.0f produces no change
    if (!check_channel(channel)) {
        return false;
    }

    MIX_StereoGains gains;
    gains.left = (255 - pan) / 128.f;
    gains.right = pan / 128.f;
    if (!MIX_SetTrackStereo(global_instance.tracks[channel]->handle, &gains)) {
        return false;
    }
    return true;
}

DLL_API signed char F_API FSOUND_SetSurround(int channel, signed char surround)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_SetMute(int channel, signed char mute)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_SetPriority(int channel, int priority)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_SetReserved(int channel, signed char reserved)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_SetPaused(int channel, signed char paused)
{
    if (!check_channel(channel)) {
        return false;
    }
    if (paused) {
        if (!MIX_PauseTrack(global_instance.tracks[channel]->handle)) {
            return false;
        }
    } else {
        if (!MIX_ResumeTrack(global_instance.tracks[channel]->handle)) {
            return false;
        }
    }
    return true;
}

DLL_API signed char F_API FSOUND_SetLoopMode(int channel, unsigned int loopmode)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_SetCurrentPosition(int channel, unsigned int offset)
{
    // in SAMPLES (SDL also takes samples)
    if (!check_channel(channel)) {
        return false;
    }
    if (!MIX_SetTrackPlaybackPosition(global_instance.tracks[channel]->handle, offset)) {
        return false;
    }
    return true;
}

DLL_API signed char F_API FSOUND_3D_SetAttributes(int channel, const float *pos, const float *vel)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_3D_SetMinMaxDistance(int channel, float min, float max)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_IsPlaying(int channel)
{
    if (!check_channel(channel)) {
        return false;
    }
    return MIX_TrackPlaying(global_instance.tracks[channel]->handle);
}

DLL_API int F_API FSOUND_GetFrequency(int channel)
{
    STUB();
    return 0;
}

DLL_API int F_API FSOUND_GetVolume(int channel)
{
    // FSOUND range: from 0 (silent) to 255 (full volume)
    // SDL: 0 (silent) to 1.0
    if (!check_channel(channel)) {
        return 0;
    }
    return static_cast<int>(MIX_GetTrackGain(global_instance.tracks[channel]->handle) * 255.f);
}

DLL_API int F_API FSOUND_GetAmplitude(int channel)
{
    STUB();
    return 0;
}

DLL_API int F_API FSOUND_GetPan(int channel)
{
    STUB();
    return 0;
}

DLL_API signed char F_API FSOUND_GetSurround(int channel)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_GetMute(int channel)
{
    STUB();
    return TRUE;
}

DLL_API int F_API FSOUND_GetPriority(int channel)
{
    STUB();
    return 0;
}

DLL_API signed char F_API FSOUND_GetReserved(int channel)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_GetPaused(int channel)
{
    if (!check_channel(channel)) {
        return false;
    }
    return MIX_TrackPaused(global_instance.tracks[channel]->handle) ? 1 : 0;
}

DLL_API unsigned int F_API FSOUND_GetLoopMode(int channel)
{
    STUB();
    return 0;
}

DLL_API unsigned int F_API FSOUND_GetCurrentPosition(int channel)
{
    // in SAMPLES (SDL also takes samples)
    if (!check_channel(channel)) {
        return 0;
    }
    return MIX_GetTrackPlaybackPosition(global_instance.tracks[channel]->handle);
}

DLL_API FSOUND_SAMPLE * F_API FSOUND_GetCurrentSample(int channel)
{
    STUB();
    return NULL;
}

DLL_API signed char F_API FSOUND_GetCurrentLevels(int channel, float *l, float *r)
{
    STUB();
    return TRUE;
}

DLL_API int F_API FSOUND_GetNumSubChannels(int channel)
{
    STUB();
    return 0;
}

DLL_API int F_API FSOUND_GetSubChannel(int channel, int subchannel)
{
    STUB();
    return 0;
}

DLL_API signed char F_API FSOUND_3D_GetAttributes(int channel, float *pos, float *vel)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_3D_GetMinMaxDistance(int channel, float *min, float *max)
{
    STUB();
    return TRUE;
}

DLL_API void F_API FSOUND_3D_Listener_SetAttributes(const float *pos, const float *vel, float fx, float fy, float fz, float tx, float ty, float tz)
{
    // The listener is always at coordinate (0,0,0) and can't be changed.
}

DLL_API void F_API FSOUND_3D_Listener_GetAttributes(float *pos, float *vel, float *fx, float *fy, float *fz, float *tx, float *ty, float *tz)
{
    STUB();
    return;
}

DLL_API void F_API FSOUND_3D_Listener_SetCurrent(int current, int numlisteners)
{
    STUB();
    return;
}

DLL_API void F_API FSOUND_3D_SetDopplerFactor(float scale)
{
    STUB();
    return;
}

DLL_API void F_API FSOUND_3D_SetDistanceFactor(float scale)
{
    STUB();
    return;
}

DLL_API void F_API FSOUND_3D_SetRolloffFactor(float scale)
{
    STUB();
    return;
}

DLL_API int F_API FSOUND_FX_Enable(int channel, unsigned int fxtype)
{
    STUB();
    return 0;
}

DLL_API signed char F_API FSOUND_FX_Disable(int channel)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_FX_SetChorus(int fxid, float WetDryMix, float Depth, float Feedback, float Frequency, int Waveform, float Delay, int Phase)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_FX_SetCompressor(int fxid, float Gain, float Attack, float Release, float Threshold, float Ratio, float Predelay)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_FX_SetDistortion(int fxid, float Gain, float Edge, float PostEQCenterFrequency, float PostEQBandwidth, float PreLowpassCutoff)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_FX_SetEcho(int fxid, float WetDryMix, float Feedback, float LeftDelay, float RightDelay, int PanDelay)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_FX_SetFlanger(int fxid, float WetDryMix, float Depth, float Feedback, float Frequency, int Waveform, float Delay, int Phase)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_FX_SetGargle(int fxid, int RateHz, int WaveShape)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_FX_SetI3DL2Reverb(int fxid, int Room, int RoomHF, float RoomRolloffFactor, float DecayTime, float DecayHFRatio, int Reflections, float ReflectionsDelay, int Reverb, float ReverbDelay, float Diffusion, float Density, float HFReference)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_FX_SetParamEQ(int fxid, float Center, float Bandwidth, float Gain)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_FX_SetWavesReverb(int fxid, float InGain, float ReverbMix, float ReverbTime, float HighFreqRTRatio)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_Stream_SetBufferSize(int ms)
{
    STUB();
    return TRUE;
}

DLL_API FSOUND_STREAM * F_API FSOUND_Stream_Open(const char *name_or_data, unsigned int mode, int offset, int length)
{
    auto stream = std::make_unique<Stream>();

    bool load_from_memory = FSOUND_LOADMEMORY == (mode & FSOUND_LOADMEMORY);

    MIX_Audio *wave = nullptr;
    if (!stream) {
        return nullptr;
    }
    if (load_from_memory) {
        SDL_IOStream* io = SDL_IOFromMem(const_cast<void*>(reinterpret_cast<const void*>(name_or_data)), length);
        if (!io) {
            return nullptr;
        }
        wave = MIX_LoadAudio_IO(global_instance.mixer, io, false, true);
    } else {
        wave = MIX_LoadAudio(global_instance.mixer, name_or_data, false);
    }
    if (!wave) {
        return nullptr;
    }
    stream->wave = wave;
    stream->mode = mode;

    auto & stream_ref = global_instance.streams.emplace_back(std::move(stream));

    return stream_ref.get();
}

DLL_API FSOUND_STREAM * F_API FSOUND_Stream_Create(FSOUND_STREAMCALLBACK callback, int length, unsigned int mode, int samplerate, void *userdata)
{
    STUB();
    return NULL;
}

DLL_API signed char F_API FSOUND_Stream_Close(FSOUND_STREAM *stream)
{
    if (stream && stream->wave) {
        MIX_DestroyAudio(stream->wave);
        stream->wave = nullptr;
        return true;
    }
    return false;
}

DLL_API int F_API FSOUND_Stream_Play(int channel, FSOUND_STREAM *stream)
{
	return FSOUND_Stream_PlayEx(channel, stream, nullptr, false);
}

DLL_API int F_API FSOUND_Stream_PlayEx(int channel, FSOUND_STREAM *stream, FSOUND_DSPUNIT *dsp, signed char startpaused)
{
    if(!stream || !stream->wave)
        return -1;

    if (channel == FSOUND_FREE) {
        for (const auto & track: global_instance.tracks) {

            if (track->channel == 0) {
                // never return channel zero, keep it for internal use
                continue;
            }
            if (!track->is_used) {

                if (!MIX_SetTrackAudio(track->handle, stream->wave)) {
                    return -1;
                }
                track->is_used = true;
                stream->track = track->handle;

                if (!MIX_PlayTrack(track->handle, 0)) {
                    return -1;
                }

                if (startpaused) {
                    if (!MIX_PauseTrack(track->handle)) {
                        return -1;
                    }
                }
                return track->channel;
            }
        }
    } else {
        abort();
    }
    return -1;
}

DLL_API signed char F_API FSOUND_Stream_Stop(FSOUND_STREAM *stream)
{
    if (!stream || !stream->track) {
        return -1;
    }
    if (!MIX_StopTrack(stream->track, false)) {
        return false;
    }
    return true;
}

static Sint64 get_frame_size(FSOUND_STREAM * stream) {

    SDL_AudioSpec spec;
    if (!MIX_GetAudioFormat(stream->wave, &spec)) {
        return 0;
    }
    return spec.channels * SDL_AUDIO_BITSIZE(spec.format) / 8;
}

DLL_API signed char F_API FSOUND_Stream_SetPosition(FSOUND_STREAM *stream, unsigned int position)
{
    // in BYTES (SDL takes frames)
    if (!stream || !stream->track) {
        return false;
    }

    Sint64 frame_size = get_frame_size(stream);
    if (frame_size == 0) {
        return false;
    }

    Sint64 position_frames = position / frame_size;
    if (!MIX_SetTrackPlaybackPosition(stream->track, position_frames)) {
        return false;
    }

    return true;
}

DLL_API unsigned int F_API FSOUND_Stream_GetPosition(FSOUND_STREAM *stream)
{
    // in BYTES (SDL takes frames)
    if (!stream || !stream->track) {
        return 0;
    }

    Sint64 position_frames = MIX_GetTrackPlaybackPosition(stream->track);

    return position_frames * get_frame_size(stream);
}

DLL_API signed char F_API FSOUND_Stream_SetTime(FSOUND_STREAM *stream, int ms)
{
    // in MILLISECONDS (SDL takes frames)
    if (!stream || !stream->track) {
        return false;
    }

    Sint64 position_ms = MIX_AudioMSToFrames(stream->wave, ms);
    MIX_SetTrackPlaybackPosition(stream->track, position_ms);

    return true;
}

DLL_API int F_API FSOUND_Stream_GetTime(FSOUND_STREAM *stream)
{
    // in MILLISECONDS (SDL takes frames)
    if (stream && stream->track) {
        Sint64 position = MIX_GetTrackPlaybackPosition(stream->track);
        return MIX_AudioFramesToMS(stream->wave, position);
    }
    return 0;
}

DLL_API int F_API FSOUND_Stream_GetLength(FSOUND_STREAM *stream)
{
    // in BYTES (SDL takes frames)
    if (stream && stream->wave) {
        Sint64 duration_frames = MIX_GetAudioDuration(stream->wave);
        size_t frame_size = get_frame_size(stream);
        return frame_size * duration_frames;
    }
    return 0;
}

DLL_API int F_API FSOUND_Stream_GetLengthMs(FSOUND_STREAM *stream)
{
    // in MILLISECONDS (SDL takes frames)
    if (stream && stream->wave) {
        Sint64 duration_frames = MIX_GetAudioDuration(stream->wave);
        return MIX_AudioFramesToMS(stream->wave, duration_frames);
    }
    return 0;
}

DLL_API signed char F_API FSOUND_Stream_SetMode(FSOUND_STREAM *stream, unsigned int mode)
{
    STUB();
    return TRUE;
}

DLL_API unsigned int F_API FSOUND_Stream_GetMode(FSOUND_STREAM *stream)
{
    STUB();
    return 0;
}

DLL_API signed char F_API FSOUND_Stream_SetLoopPoints(FSOUND_STREAM *stream, unsigned int loopstartpcm, unsigned int loopendpcm)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_Stream_SetLoopCount(FSOUND_STREAM *stream, int count)
{
    STUB();
    return TRUE;
}

DLL_API int F_API FSOUND_Stream_GetOpenState(FSOUND_STREAM *stream)
{
    STUB();
    return 0;
}

DLL_API FSOUND_SAMPLE * F_API FSOUND_Stream_GetSample(FSOUND_STREAM *stream)
{
    STUB();
    return NULL;
}

DLL_API FSOUND_DSPUNIT * F_API FSOUND_Stream_CreateDSP(FSOUND_STREAM *stream, FSOUND_DSPCALLBACK callback, int priority, void *userdata)
{
    STUB();
    return NULL;
}

void SDLCALL TrackStoppedCallback(void *userdata, MIX_Track *track) {

    CallbackData * callback = reinterpret_cast<CallbackData *>(userdata);

    if (callback->inner_callback) {
        callback->inner_callback(callback->stream, nullptr, 0, callback->userdata);
    }
}

DLL_API signed char F_API FSOUND_Stream_SetEndCallback(FSOUND_STREAM *stream, FSOUND_STREAMCALLBACK callback, void *userdata)
{
    STUB();

    if (!stream || !stream->track) {
        return false;
    }

	stream->callback_data.stream = stream;
	stream->callback_data.inner_callback = callback;
	stream->callback_data.userdata = userdata;
    if (!MIX_SetTrackStoppedCallback(stream->track, TrackStoppedCallback, &stream->callback_data)) {
        return false;
    }

    return true;
}

DLL_API signed char F_API FSOUND_Stream_SetSyncCallback(FSOUND_STREAM *stream, FSOUND_STREAMCALLBACK callback, void *userdata)
{
    STUB();
    return TRUE;
}

DLL_API FSOUND_SYNCPOINT * F_API FSOUND_Stream_AddSyncPoint(FSOUND_STREAM *stream, unsigned int pcmoffset, const char *name)
{
    STUB();
    return NULL;
}

DLL_API signed char F_API FSOUND_Stream_DeleteSyncPoint(FSOUND_SYNCPOINT *point)
{
    STUB();
    return TRUE;
}

DLL_API int F_API FSOUND_Stream_GetNumSyncPoints(FSOUND_STREAM *stream)
{
    STUB();
    return 0;
}

DLL_API FSOUND_SYNCPOINT * F_API FSOUND_Stream_GetSyncPoint(FSOUND_STREAM *stream, int index)
{
    STUB();
    return NULL;
}

DLL_API char * F_API FSOUND_Stream_GetSyncPointInfo(FSOUND_SYNCPOINT *point, unsigned int *pcmoffset)
{
    STUB();
    return NULL;
}

DLL_API signed char F_API FSOUND_Stream_SetSubStream(FSOUND_STREAM *stream, int index)
{
    STUB();
    return TRUE;
}

DLL_API int F_API FSOUND_Stream_GetNumSubStreams(FSOUND_STREAM *stream)
{
    STUB();
    return 0;
}

DLL_API signed char F_API FSOUND_Stream_SetSubStreamSentence(FSOUND_STREAM* stream, const int* sentencelist, int numitems)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_Stream_GetNumTagFields(FSOUND_STREAM *stream, int *num)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_Stream_GetTagField(FSOUND_STREAM *stream, int num, int *type, char **name, void **value, int *length)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_Stream_FindTagField(FSOUND_STREAM *stream, int type, const char *name, void **value, int *length)
{
    STUB();
    return TRUE;
}


DLL_API signed char F_API FSOUND_Stream_Net_SetProxy(const char *proxy)
{
    STUB();
    return TRUE;
}

DLL_API char * F_API FSOUND_Stream_Net_GetLastServerStatus()
{
    STUB();
    return NULL;
}

DLL_API signed char F_API FSOUND_Stream_Net_SetBufferProperties(int buffersize, int prebuffer_percent, int rebuffer_percent)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_Stream_Net_GetBufferProperties(int *buffersize, int *prebuffer_percent, int *rebuffer_percent)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_Stream_Net_SetMetadataCallback(FSOUND_STREAM *stream, FSOUND_METADATACALLBACK callback, void *userdata)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_Stream_Net_GetStatus(FSOUND_STREAM *stream, int *status, int *bufferpercentused, int *bitrate, unsigned int *flags)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_CD_Play(char drive, int track)
{
    STUB();
    return TRUE;
}

DLL_API void F_API FSOUND_CD_SetPlayMode(char drive, signed char mode)
{
    STUB();
    return;
}

DLL_API signed char F_API FSOUND_CD_Stop(char drive)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_CD_SetPaused(char drive, signed char paused)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_CD_SetVolume(char drive, int volume)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_CD_SetTrackTime(char drive, unsigned int ms)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_CD_OpenTray(char drive, signed char open)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_CD_GetPaused(char drive)
{
    STUB();
    return TRUE;
}

DLL_API int F_API FSOUND_CD_GetTrack(char drive)
{
    STUB();
    return TRUE;
}

DLL_API int F_API FSOUND_CD_GetNumTracks(char drive)
{
    STUB();
    return TRUE;
}

DLL_API int F_API FSOUND_CD_GetVolume(char drive)
{
    STUB();
    return 0;
}

DLL_API int F_API FSOUND_CD_GetTrackLength(char drive, int track)
{
    STUB();
    return TRUE;
}

DLL_API int F_API FSOUND_CD_GetTrackTime(char drive)
{
    STUB();
    return 0;
}

DLL_API FSOUND_DSPUNIT * F_API FSOUND_DSP_Create(FSOUND_DSPCALLBACK callback, int priority, void *userdata)
{
    STUB();
    return NULL;
}

DLL_API void F_API FSOUND_DSP_Free(FSOUND_DSPUNIT *unit)
{
    STUB();
    return;
}

DLL_API void F_API FSOUND_DSP_SetPriority(FSOUND_DSPUNIT *unit, int priority)
{
    STUB();
    return;
}

DLL_API int F_API FSOUND_DSP_GetPriority(FSOUND_DSPUNIT *unit)
{
    STUB();
    return 0;
}

DLL_API void F_API FSOUND_DSP_SetActive(FSOUND_DSPUNIT *unit, signed char active)
{
    STUB();
    return;
}

DLL_API signed char F_API FSOUND_DSP_GetActive(FSOUND_DSPUNIT *unit)
{
    STUB();
    return TRUE;
}

DLL_API FSOUND_DSPUNIT * F_API FSOUND_DSP_GetClearUnit()
{
    STUB();
    return NULL;
}

DLL_API FSOUND_DSPUNIT * F_API FSOUND_DSP_GetSFXUnit()
{
    STUB();
    return NULL;
}

DLL_API FSOUND_DSPUNIT * F_API FSOUND_DSP_GetMusicUnit()
{
    STUB();
    return NULL;
}

DLL_API FSOUND_DSPUNIT * F_API FSOUND_DSP_GetFFTUnit()
{
    STUB();
    return NULL;
}

DLL_API FSOUND_DSPUNIT * F_API FSOUND_DSP_GetClipAndCopyUnit()
{
    STUB();
    return NULL;
}

DLL_API signed char F_API FSOUND_DSP_MixBuffers(void *destbuffer, void *srcbuffer, int len, int freq, int vol, int pan, unsigned int mode)
{
    STUB();
    return TRUE;
}

DLL_API void F_API FSOUND_DSP_ClearMixBuffer()
{
    STUB();
    return;
}

DLL_API int F_API FSOUND_DSP_GetBufferLength()
{
    STUB();
    return 0;
}

DLL_API int F_API FSOUND_DSP_GetBufferLengthTotal()
{
    STUB();
    return 0;
}

DLL_API float * F_API FSOUND_DSP_GetSpectrum()
{
    STUB();
    return NULL;
}


DLL_API signed char F_API FSOUND_Reverb_SetProperties(const FSOUND_REVERB_PROPERTIES *prop)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_Reverb_GetProperties(FSOUND_REVERB_PROPERTIES *prop)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_Reverb_SetChannelProperties(int channel, const FSOUND_REVERB_CHANNELPROPERTIES *prop)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_Reverb_GetChannelProperties(int channel, FSOUND_REVERB_CHANNELPROPERTIES *prop)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_Record_SetDriver(int outputtype)
{
    STUB();
    return TRUE;
}

DLL_API int F_API FSOUND_Record_GetNumDrivers()
{
    STUB();
    return 0;
}

DLL_API const char * F_API FSOUND_Record_GetDriverName(int id)
{
    STUB();
    return NULL;
}

DLL_API int F_API FSOUND_Record_GetDriver()
{
    STUB();
    return 0;
}

DLL_API signed char F_API FSOUND_Record_StartSample(FSOUND_SAMPLE *sptr, signed char loop)
{
    STUB();
    return TRUE;
}

DLL_API signed char F_API FSOUND_Record_Stop()
{
    STUB();
    return TRUE;
}

DLL_API int F_API FSOUND_Record_GetPosition()
{
    STUB();
    return 0;
}
