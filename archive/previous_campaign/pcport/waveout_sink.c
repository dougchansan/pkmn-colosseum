/**
 * @file waveout_sink.c
 * @brief Win32 waveOut streaming sink -- see waveout_sink.h.
 *
 * Double-buffered: NUM_BUFFERS WAVEHDR blocks cycle between "queued to device"
 * and "free". A manual-reset event (CALLBACK_EVENT) is signalled by the driver
 * whenever a buffer finishes; Submit waits on it (with a timeout) for a free
 * buffer. Each buffer is sized for the worst-case THP frame so a single Submit
 * never overflows it.
 */
#ifdef __MWERKS__
/* GCN build: host waveOut sink not applicable. */
#else
#if defined(_WIN32)

#include "waveout_sink.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <string.h>
#include <stdlib.h>

#define WAVEOUT_NUM_BUFFERS 4
/* THP audio frames are ~1078 samples/ch; allow generous headroom (4096 frames
 * stereo = 16 KB) so one Submit always fits a single buffer. */
#define WAVEOUT_BUFFER_FRAMES 4096

typedef struct {
    HWAVEOUT  hwo;
    HANDLE    doneEvent;
    WAVEHDR   hdr[WAVEOUT_NUM_BUFFERS];
    int16_t*  data[WAVEOUT_NUM_BUFFERS];
    uint32_t  bufBytes;       /* capacity of each buffer in bytes */
    int       channels;
    int       opened;
    int       next;           /* round-robin index of the buffer to fill next */
} WaveOutSink;

static WaveOutSink g_sink;

int WaveOutSink_Open(uint32_t sampleRate, int channels, int bitsPerSample) {
    WAVEFORMATEX fmt;
    int i;

    if (g_sink.opened) {
        return 1;
    }
    if (channels <= 0 || (bitsPerSample != 8 && bitsPerSample != 16)) {
        return 0;
    }
    memset(&g_sink, 0, sizeof(g_sink));
    g_sink.channels = channels;
    g_sink.bufBytes = (uint32_t)WAVEOUT_BUFFER_FRAMES *
                      (uint32_t)channels * ((uint32_t)bitsPerSample / 8u);

    g_sink.doneEvent = CreateEventA(NULL, FALSE /* auto-reset */, TRUE /* signalled */, NULL);
    if (g_sink.doneEvent == NULL) {
        return 0;
    }

    memset(&fmt, 0, sizeof(fmt));
    fmt.wFormatTag      = WAVE_FORMAT_PCM;
    fmt.nChannels       = (WORD)channels;
    fmt.nSamplesPerSec  = (DWORD)sampleRate;
    fmt.wBitsPerSample  = (WORD)bitsPerSample;
    fmt.nBlockAlign     = (WORD)(channels * (bitsPerSample / 8));
    fmt.nAvgBytesPerSec = sampleRate * fmt.nBlockAlign;
    fmt.cbSize          = 0;

    if (waveOutOpen(&g_sink.hwo, WAVE_MAPPER, &fmt,
                    (DWORD_PTR)g_sink.doneEvent, 0,
                    CALLBACK_EVENT) != MMSYSERR_NOERROR) {
        CloseHandle(g_sink.doneEvent);
        g_sink.doneEvent = NULL;
        return 0;
    }

    for (i = 0; i < WAVEOUT_NUM_BUFFERS; ++i) {
        g_sink.data[i] = (int16_t*)malloc(g_sink.bufBytes);
        if (g_sink.data[i] == NULL) {
            WaveOutSink_Close();
            return 0;
        }
        memset(&g_sink.hdr[i], 0, sizeof(WAVEHDR));
        g_sink.hdr[i].lpData = (LPSTR)g_sink.data[i];
        g_sink.hdr[i].dwBufferLength = g_sink.bufBytes;
        /* Mark prepared+done so each slot is considered free initially. */
        g_sink.hdr[i].dwFlags = WHDR_DONE;
    }

    g_sink.opened = 1;
    g_sink.next = 0;
    return 1;
}

int WaveOutSink_Submit(const int16_t* pcmData, uint32_t numFrames) {
    WAVEHDR* h;
    uint32_t bytes;
    int slot;

    if (!g_sink.opened || pcmData == NULL || numFrames == 0) {
        return 0;
    }
    bytes = numFrames * (uint32_t)g_sink.channels * (uint32_t)sizeof(int16_t);
    if (bytes > g_sink.bufBytes) {
        bytes = g_sink.bufBytes; /* clamp: drop the tail rather than overrun */
    }

    slot = g_sink.next;
    h = &g_sink.hdr[slot];

    /* Wait for this slot to be free (driver sets WHDR_DONE and signals event). */
    while ((h->dwFlags & WHDR_DONE) == 0) {
        if (WaitForSingleObject(g_sink.doneEvent, 100) == WAIT_TIMEOUT) {
            /* Buffer still busy; drop this frame to keep video smooth. */
            return 0;
        }
    }

    /* Recycle: unprepare if it was previously written. */
    if (h->dwFlags & WHDR_PREPARED) {
        waveOutUnprepareHeader(g_sink.hwo, h, sizeof(WAVEHDR));
    }

    memcpy(g_sink.data[slot], pcmData, bytes);
    memset(h, 0, sizeof(WAVEHDR));
    h->lpData = (LPSTR)g_sink.data[slot];
    h->dwBufferLength = bytes;

    if (waveOutPrepareHeader(g_sink.hwo, h, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
        h->dwFlags = WHDR_DONE;
        return 0;
    }
    if (waveOutWrite(g_sink.hwo, h, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
        waveOutUnprepareHeader(g_sink.hwo, h, sizeof(WAVEHDR));
        h->dwFlags = WHDR_DONE;
        return 0;
    }

    g_sink.next = (slot + 1) % WAVEOUT_NUM_BUFFERS;
    return 1;
}

int WaveOutSink_IsPlaying(void) {
    int i;
    if (!g_sink.opened) {
        return 0;
    }
    for (i = 0; i < WAVEOUT_NUM_BUFFERS; ++i) {
        if ((g_sink.hdr[i].dwFlags & WHDR_PREPARED) &&
            (g_sink.hdr[i].dwFlags & WHDR_DONE) == 0) {
            return 1;
        }
    }
    return 0;
}

void WaveOutSink_Close(void) {
    int i;
    if (g_sink.hwo != NULL) {
        waveOutReset(g_sink.hwo); /* flush queued buffers, marks them DONE */
        for (i = 0; i < WAVEOUT_NUM_BUFFERS; ++i) {
            if (g_sink.hdr[i].dwFlags & WHDR_PREPARED) {
                waveOutUnprepareHeader(g_sink.hwo, &g_sink.hdr[i], sizeof(WAVEHDR));
            }
        }
        waveOutClose(g_sink.hwo);
        g_sink.hwo = NULL;
    }
    for (i = 0; i < WAVEOUT_NUM_BUFFERS; ++i) {
        if (g_sink.data[i] != NULL) {
            free(g_sink.data[i]);
            g_sink.data[i] = NULL;
        }
    }
    if (g_sink.doneEvent != NULL) {
        CloseHandle(g_sink.doneEvent);
        g_sink.doneEvent = NULL;
    }
    g_sink.opened = 0;
}

#else /* !_WIN32 -- non-Windows host: no waveOut, provide stubs. */

#include "waveout_sink.h"

int  WaveOutSink_Open(uint32_t sampleRate, int channels, int bitsPerSample) {
    (void)sampleRate; (void)channels; (void)bitsPerSample; return 0;
}
int  WaveOutSink_Submit(const int16_t* pcmData, uint32_t numFrames) {
    (void)pcmData; (void)numFrames; return 0;
}
int  WaveOutSink_IsPlaying(void) { return 0; }
void WaveOutSink_Close(void) { }

#endif /* _WIN32 */
#endif /* __MWERKS__ */
