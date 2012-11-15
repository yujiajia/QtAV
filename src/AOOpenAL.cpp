/******************************************************************************
    AOOpenAL.cpp: description
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


#include <QtAV/AOOpenAL.h>
#include <private/AOOpenAL_p.h>
#include <QtCore/QString>
#include <libswresample/swresample.h>
namespace QtAV {

AOOpenAL::AOOpenAL()
    :AudioOutput(*new AOOpenALPrivate())
{
}

AOOpenAL::~AOOpenAL()
{
    close();
}

int AOOpenAL::write(const QByteArray &data)
{
    if (data.isEmpty() || data.isNull())
        return 0;

    DPTR_D(AOOpenAL);
    ALenum err = AL_NO_ERROR;

    ALuint buf;
    alGenBuffers(1, &buf);
    err = alGetError();
    if (err != AL_NO_ERROR) { //return ?
        qWarning("Failed to gen buffers: %s", alGetString(err));
        return 0;
    }
    alBufferData(buf, d.format, data.constData(), data.size(), d.sample_rate);
    alSourceQueueBuffers(d.source, 1, &buf);
    err = alGetError();
    if (err != AL_NO_ERROR) { //return ?
        qWarning("Error: %s", alGetString(err));
        return 0;
    }
    alSourcePlay(d.source);
    alSourceUnqueueBuffers(d.source, 1, &buf);
    alDeleteBuffers(1, &buf);
#if 0
    for (int i = 0; i < kBufferCount; ++i) {
        alBufferData(d.buffer[i], d.format, data.constData(), data.size(), d.sample_rate);
        if (err != AL_NO_ERROR) { //return ?
            qWarning("Error: %s", alGetString(err));
            return 0;
        }
        alSourceQueueBuffers(d.source, 1, &d.buffer[1]);
    }
    alSourcePlay(d.source);
    ALint processed;
    alGetSourcei(d.source, AL_BUFFERS_PROCESSED, &processed);

    if (processed == 0) {
        ALenum state;
        alGetSourcei(d.source, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING) {
            alSourcePlay(d.source);
        }
        err = alGetError();
        if (err != AL_NO_ERROR) { //return ?
            qWarning("Failed to delete source: %s", alGetString(err));
        }
        return 0;
    }
    while (processed) {
        qDebug("processed: %d", processed);
        ALuint buf;
        /*unqueues a set of buffers attached to a source. processed
         * is the maximum number of buffers that can be unqueued.
         * The unqueue operation will only take place if all n buffers
         * can be removed from the queue.
         * buf: a pointer to an array of buffer names that were removed*/
        //扔掉一些播放完的缓冲,并使新的buf和声源的buf相连
        alSourceUnqueueBuffers(d.source, 1, &buf); //unqueue processed?
        if (!buf)
            return 0;
        /* fills a buffer with audio data. All the pre-defined formats are PCM data,
         * but may be used by extensions to load other data types as well*/
        //av_samples_get_buffer_size(0, d.channels, d.sample_rate, );
        alBufferData(buf, d.format, data.constData(), data.size(), d.sample_rate);
        alSourceQueueBuffers(d.source, 1, &buf);
        err = alGetError();
        if (err != AL_NO_ERROR) { //return ?
            qWarning("Error: %s", alGetString(err));
            return 0;
        }
        processed--;
    }
#endif
    return data.size();
}

bool AOOpenAL::open()
{
    DPTR_D(AOOpenAL);
    const ALCchar *default_device = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
    qDebug("Opening default device: %s", default_device);
    if ((d.device = alcOpenDevice(default_device)) == NULL) { //or NULL to open default
        qWarning("Failed to open sound device: %s", alcGetString(0, alcGetError(0)));
        return false;
    }
    d.context = alcCreateContext(d.device, NULL);
    alcMakeContextCurrent(d.context);
    alcProcessContext(d.context); //??
    ALCenum err = alcGetError(d.device);
    if (err != ALC_NO_ERROR) {
        qWarning("Error: %s", alcGetString(d.device, err));
        return false;
    }

    //http://www.chinavideo.org/viewthread.php?tid=15521&highlight=openal
    //init params. move to another func?
    if (d.channels == 1)
        d.format = AL_FORMAT_MONO16;
    else if (d.channels == 2)
        d.format = AL_FORMAT_STEREO16;
    //why not AL_EXT_MCFORMATS directly?
    if (alIsExtensionPresent("AL_EXT_MCFORMATS")) {
        if (d.channels == 4)
            d.format = alGetEnumValue("AL_FORMAT_QUAD16"); //AL_FORMAT_QUAD16?
        else if (d.channels == 6)
            d.format = alGetEnumValue("AL_FORMAT_51CHN16"); //AL_FORMAT_QUAD16?
    }

    return true;
}

bool AOOpenAL::close()
{
    DPTR_D(AOOpenAL);
    if (d.context) {
        alcDestroyContext(d.context);
        ALCenum err = alcGetError(d.device);
        if (err != ALC_NO_ERROR) { //ALC_INVALID_CONTEXT
            qWarning("Failed to destroy context: %s", alcGetString(d.device, err));
            return false;
        }
    }
    if (d.device) {
        alcCloseDevice(d.device);
        ALCenum err = alcGetError(d.device);
        if (err != ALC_NO_ERROR) { //ALC_INVALID_DEVICE
            qWarning("Failed to close device: %s", alcGetString(d.device, err));
            return false;
        }
    }
    return true;
}

QString AOOpenAL::name() const
{
    const ALCchar *name = alcGetString(d_func().device, ALC_DEVICE_SPECIFIER);
    return QString(name);
}

} //namespace QtAV
