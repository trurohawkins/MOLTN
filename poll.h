#ifndef POLL
#define POLL
#include <errno.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <stdio.h>
#include <unistd.h>

typedef struct {
	int fd;
	void (*func)(void);
} PollHandler;

typedef struct {
	int pfd;
	struct epoll_event polls[16];
	PollHandler handler;
} PollSystem;

bool initPollSystem(PollSystem *poll, void (*func)(void));
bool addFdToPoll(PollHandler* handler, int poll);
bool runPolls(int pfd, struct epoll_event* polls, int count);
void closePoll(PollSystem poll);

bool initTimerFd(PollHandler *handler, int ticksPerSecond, void (*func)(void));
#endif
