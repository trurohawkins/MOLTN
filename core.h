#ifndef CORE
#define CORE
#include <errno.h>

#include "helper.h"
#include "poll.h"

extern atomic_int running;

extern PollSystem outputPoll;
extern PollSystem gamePoll;

bool initCore();
void exitCore();
void coreLoop();
bool addFdToCore(PollHandler* handler);
bool initTimerFd(PollHandler *handler, int ticksPerSecond, void (*func)(void));
void wakeEvent();
void debugWrite(char *message);

//events
#define EVENT_BUFFER_SIZE 32
typedef struct {
	uint32_t type;
	uint32_t size;
	alignas(max_align_t) unsigned char data[EVENT_BUFFER_SIZE];
} SystemEvent;

DECLARE_SPSC(SystemEvent, SystemQueue, 256);
bool pushEvent(int type, const void *data, uint32_t size);
bool popEvent(SystemEvent *se);

#endif

