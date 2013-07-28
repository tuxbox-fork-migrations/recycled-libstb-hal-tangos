/*
 * linuxdvb output/writer handling.
 *
 * konfetti 2010 based on linuxdvb.c code from libeplayer2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

/* ***************************** */
/* Includes                      */
/* ***************************** */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <linux/dvb/video.h>
#include <linux/dvb/audio.h>
#include <linux/dvb/stm_ioctls.h>
#include <memory.h>
#include <asm/types.h>
#include <pthread.h>
#include <errno.h>

#include "common.h"
#include "output.h"
#include "debug.h"
#include "misc.h"
#include "pes.h"
#include "writer.h"
#include "pcm.h"

/* ***************************** */
/* Makros/Constants              */
/* ***************************** */
#define PCM_DEBUG

#ifdef PCM_DEBUG

static short debug_level = 0;

#define pcm_printf(level, fmt, x...) do { \
if (debug_level >= level) printf("[%s:%s] " fmt, __FILE__, __FUNCTION__, ## x); } while (0)
#else
#define pcm_printf(level, fmt, x...)
#endif

#ifndef PCM_SILENT
#define pcm_err(fmt, x...) do { printf("[%s:%s] " fmt, __FILE__, __FUNCTION__, ## x); } while (0)
#else
#define pcm_err(fmt, x...)
#endif

/* ***************************** */
/* Types                         */
/* ***************************** */

/* ***************************** */
/* Varaibles                     */
/* ***************************** */

static int initialHeader = 1;

static unsigned int SubFrameLen = 0;
static unsigned int SubFramesPerPES = 0;

// reference: search for TypeLpcmDVDAudio in player/frame_parser/frame_parser_audio_lpcm.cpp
static const unsigned char clpcm_prv[14] = {   0xA0,   //sub_stream_id
					0, 0,   //resvd and UPC_EAN_ISRC stuff, unused
					0x0A,   //private header length
					0, 9,   //first_access_unit_pointer
					0x00,   //emph,rsvd,stereo,downmix
					0x0F,   //quantisation word length 1,2
					0x0F,   //audio sampling freqency 1,2
					0,      //resvd, multi channel type
					0,      //bit shift on channel GR2, assignment
					0x80,   //dynamic range control
					0, 0    //resvd for copyright management
			};

static unsigned char lpcm_pes[18];
static unsigned char lpcm_prv[14];

static unsigned char breakBuffer[8192];
static unsigned int breakBufferFillSize = 0;

/* ***************************** */
/* Prototypes                    */
/* ***************************** */

/* ***************************** */
/* MISC Functions                */
/* ***************************** */

static int prepareClipPlay(int uNoOfChannels, int uSampleRate, int uBitsPerSample, int bLittleEndian __attribute__((unused)))
{
	printf("rate: %d ch: %d bits: %d (%d bps)\n",
		uSampleRate/*Format->dwSamplesPerSec*/,
		uNoOfChannels/*Format->wChannels*/,
		uBitsPerSample/*Format->wBitsPerSample*/,
		(uBitsPerSample/*Format->wBitsPerSample*/ / 8)
	);

	SubFrameLen = 0;
	SubFramesPerPES = 0;
	breakBufferFillSize = 0;

	memcpy(lpcm_prv, clpcm_prv, sizeof(lpcm_prv));

	//figure out size of subframe
	//and set up sample rate
	switch(uSampleRate) {
		case 48000:	SubFrameLen = 40;
				break;
		case 96000:	lpcm_prv[8] |= 0x10;
				SubFrameLen = 80;
				break;
		case 192000:	lpcm_prv[8] |= 0x20;
				SubFrameLen = 160;
				break;
		case 44100:	lpcm_prv[8] |= 0x80;
				SubFrameLen = 40;
				break;
		case 88200:	lpcm_prv[8] |= 0x90;
				SubFrameLen = 80;
				break;
		case 176400:	lpcm_prv[8] |= 0xA0;
				SubFrameLen = 160;
				break;
		default:	break;
	}

	SubFrameLen *= uNoOfChannels;
	SubFrameLen *= (uBitsPerSample / 8);

	//rewrite PES size to have as many complete subframes per PES as we can
	// FIXME: PES header size was hardcoded to 18 in previous code. Actual size returned by InsertPesHeader is 14.
	SubFramesPerPES = ((2048 - 18) - sizeof(lpcm_prv))/SubFrameLen;
	SubFrameLen *= SubFramesPerPES;

	//set number of channels
	lpcm_prv[10] = uNoOfChannels - 1;

	switch(uBitsPerSample) {
		case 24:
			lpcm_prv[7] |= 0x20;
		case 16:
			break;
		default:
			printf("inappropriate bits per sample (%d) - must be 16 or 24\n",uBitsPerSample);
			return 1;
	}

	return 0;
}

static int reset()
{
	initialHeader = 1;
	return 0;
}

static int writeData(void* _call)
{
	WriterAVCallData_t* call = (WriterAVCallData_t*) _call;

	unsigned char  PesHeader[PES_MAX_HEADER_SIZE];

	pcm_printf(10, "\n");

	if (!call) {
		pcm_err("call data is NULL...\n");
		return 0;
	}

	pcm_printf(10, "AudioPts %lld\n", call->Pts);

	if ((call->data == NULL) || (call->len <= 0)) {
		pcm_err("parsing NULL Data. ignoring...\n");
		return 0;
	}

	if (call->fd < 0) {
		pcm_err("file pointer < 0. ignoring ...\n");
		return 0;
	}

	pcmPrivateData_t*         pcmPrivateData          = (pcmPrivateData_t*)call->private_data;

	if (initialHeader) {
		initialHeader = 0;
		prepareClipPlay(pcmPrivateData->uNoOfChannels, pcmPrivateData->uSampleRate, 
			pcmPrivateData->uBitsPerSample, pcmPrivateData->bLittleEndian);
	}

	unsigned char * buffer = call->data;
	unsigned int size = call->len;

	unsigned int n;
	unsigned char * injectBuffer = (unsigned char *)malloc(SubFrameLen);
	unsigned int pos;

	for(pos = 0; pos < size; )
	{
		//printf("PCM %s - Position=%d\n", __FUNCTION__, pos);
		if((size - pos) < SubFrameLen)
		{
			breakBufferFillSize = size - pos;
			memcpy(breakBuffer, &buffer[pos], sizeof(unsigned char) * breakBufferFillSize);
			//printf("PCM %s - Unplayed=%d\n", __FUNCTION__, breakBufferFillSize);
			break;
		}

		//get first PES's worth
		if(breakBufferFillSize > 0)
		{
			memcpy(injectBuffer, breakBuffer, sizeof(unsigned char)*breakBufferFillSize);
			memcpy(&injectBuffer[breakBufferFillSize], &buffer[pos], sizeof(unsigned char)*(SubFrameLen - breakBufferFillSize));
			pos += (SubFrameLen - breakBufferFillSize);
			breakBufferFillSize = 0;
		} else
		{
		        memcpy(injectBuffer, &buffer[pos], sizeof(unsigned char)*SubFrameLen);
			pos += SubFrameLen;
		}

		struct iovec iov[3];
		iov[0].iov_base = PesHeader;
		iov[1].iov_base = lpcm_prv;
		iov[1].iov_len = sizeof(lpcm_prv);

		iov[2].iov_base = injectBuffer;
		iov[2].iov_len = SubFrameLen;

		//write the PCM data
		if(pcmPrivateData->uBitsPerSample == 16) {
			for(n=0; n<SubFrameLen; n+=2) {
				unsigned char tmp;
				tmp=injectBuffer[n];
				injectBuffer[n]=injectBuffer[n+1];
				injectBuffer[n+1]=tmp;
			}
		} else {
			//      0   1   2   3   4   5   6   7   8   9  10  11
			//    A1c A1b A1a-B1c B1b B1a-A2c A2b A2a-B2c B2b B2a
			// to A1a A1b B1a B1b.A2a A2b B2a B2b-A1c B1c A2c B2c
			for(n=0; n<SubFrameLen; n+=12) {
				unsigned char t, *p = &injectBuffer[n];
				t = p[0];
				p[ 0] = p[ 2];
				p[ 2] = p[ 5];
				p[ 5] = p[ 7];
				p[ 7] = p[11];
				p[11] = p[ 9];
				p[ 9] = p[ 3];
				p[ 3] = p[ 4];
				p[ 4] = p[ 8];
				p[ 8] = t;
			}
		}


		//increment err... subframe count?
		lpcm_prv[1] = ((lpcm_prv[1]+SubFramesPerPES) & 0x1F);

		iov[0].iov_len = InsertPesHeader (PesHeader, iov[1].iov_len + iov[2].iov_len, 0xBD, call->Pts, 0);
		int len = writev(call->fd, iov, 3);
		if (len < 0)
			break;
	}
	free(injectBuffer);

	return size;
}

/* ***************************** */
/* Writer  Definition            */
/* ***************************** */

static WriterCaps_t caps_pcm = {
	"pcm",
	eAudio,
	"A_PCM",
	AUDIO_ENCODING_LPCMA
};

struct Writer_s WriterAudioPCM = {
	&reset,
	&writeData,
	NULL,
	&caps_pcm
};

static WriterCaps_t caps_ipcm = {
	"ipcm",
	eAudio,
	"A_IPCM",
	AUDIO_ENCODING_LPCMA
};

struct Writer_s WriterAudioIPCM = {
	&reset,
	&writeData,
	NULL,
	&caps_ipcm
};
