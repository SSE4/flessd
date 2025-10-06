/*
 * $Id: defs.h 24 2004-07-26 18:35:56Z sam $
 *
 *    (c) 2004 Sam Hocevar <sam@zoy.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License as published by Banlu Kemiyatorn. See
 *  http://sam.zoy.org/projects/COPYING.WTFPL for more details.
 */

#define STUB() fprintf(stderr, "stub! (%s)\n", __FUNCTION__);

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

struct FSOUND_SAMPLE
{
    MIX_Audio *wave = nullptr;
    int index = 0;
    Uint32 mode = 0;
    bool is_managed = false;
    bool loop = false;
};

struct CallbackData {
    FSOUND_STREAM *stream = nullptr;
    FSOUND_STREAMCALLBACK inner_callback = nullptr;
    void * userdata = nullptr;
};

struct FSOUND_STREAM {
    MIX_Audio *wave = nullptr;
    MIX_Track *track = nullptr;
    Uint32 mode = 0;

    CallbackData callback_data;
};
