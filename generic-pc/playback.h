#ifndef __PLAYBACK_H
#define __PLAYBACK_H

#include <string>
#include <stdint.h>

typedef enum {
	PLAYMODE_TS = 0,
	PLAYMODE_FILE,
} playmode_t; 

class cPlayback
{
	private:
		bool playing;
		int mAudioStream;
		int mSubtitleStream;
		int mDvbsubtitleStream;
		int mTeletextStream;
		void (*framebuffer_callback)(unsigned char **, unsigned int *, unsigned int *, unsigned int *, int *);
	public:
		cPlayback(int num __attribute__((unused))= 0, void (* /*fbcb*/)(unsigned char **, unsigned int *, unsigned int *, unsigned int *, int *) = NULL) { };
		bool Open(playmode_t PlayMode);
		void Close(void);
		bool Start(char * filename, unsigned short vpid, int vtype, unsigned short apid, bool ac3, int duration, bool no_probe = true);
		bool Stop(void);
		bool SetAPid(unsigned short pid, bool ac3);
		bool SetSubtitlePid(unsigned short pid);
		bool SetDvbsubtitlePid(unsigned short pid);
		bool SetTeletextPid(unsigned short pid);
		unsigned short GetAPid(void) { return mAudioStream; }
		unsigned short GetSubtitlePid(void) { return mSubtitleStream; }
		unsigned short GetDvbsubtitlePid(void) { return mDvbsubtitleStream; }
		unsigned short GetTeletextPid(void);
		void SuspendSubtitle(bool);
		bool SetSpeed(int speed);
		bool GetSpeed(int &speed) const;
		bool GetPosition(int &position, int &duration);
		bool SetPosition(int position, bool absolute = false);
		void FindAllPids(uint16_t *apids, unsigned short *ac3flags, uint16_t *numpida, std::string *language);
		void FindAllSubtitlePids(uint16_t *pids, uint16_t *numpids, std::string *language);
		void FindAllDvbsubtitlePids(uint16_t *pids, uint16_t *numpids, std::string *language);
		void FindAllTeletextsubtitlePids(uint16_t *pids, uint16_t *numpidt, std::string *tlanguage);

		void RequestAbort(void);
		//
		cPlayback(int num = 0);
		~cPlayback();
};

#endif
