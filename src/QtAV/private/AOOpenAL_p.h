/******************************************************************************
    AOOpenAL_p.h: description
    Copyright (C) 2012 Wang Bin <wbsecg1@gmail.com>
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/


#ifndef QTAV_AOOPENAL_P_H
#define QTAV_AOOPENAL_P_H

#include <private/AOPortAudio_p.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

namespace QtAV {

const int kBufferSize = 4096*4;
const int kBufferCount = 2;

class Q_EXPORT AOOpenALPrivate : public AOPortAudioPrivate
{
public:
    AOOpenALPrivate():format(AL_FORMAT_STEREO16),source(0),device(0)
    {
        alGenBuffers(kBufferCount, buffer);
        alGenSources(1, &source);

        alSource3f(source, AL_POSITION, 0.0, 0.0, 0.0);
        alSource3f(source, AL_VELOCITY, 0.0, 0.0, 0.0);

        alListener3f(AL_POSITION, 0.0, 0.0, 0.0);
    }
    ~AOOpenALPrivate() {
        alDeleteSources(1, &source);
        ALenum err = alGetError();
        if (err != AL_NO_ERROR) {
            qWarning("Failed to delete source: %s", alGetString(err));
        }
        alDeleteBuffers(kBufferCount, buffer);
        err = alGetError();
        if (err != AL_NO_ERROR) {
            qWarning("Failed to delete source: %s", alGetString(err));
        }
    }

    ALenum format;
    ALuint buffer[kBufferCount];
    ALuint source;
    ALCdevice *device;
    ALCcontext *context;
};

} //namespace QtAV
#endif // QTAV_AOOPENAL_P_H
