#include "poll.h"

bool initPollSystem(PollSystem *poll, void (*func)(void)) {
	poll->pfd = epoll_create1(0);
	if (poll->pfd == -1) {
		perror("epoll_create1 for output loop");
		return false;
	}
	poll->handler.fd = eventfd(0, EFD_NONBLOCK |  EFD_CLOEXEC);
	if (poll->handler.fd == -1) {
		perror("term handler fd");
		return false;
	}
	poll->handler.func = func;
	addFdToPoll(&poll->handler, poll->pfd);
	return true;
}

bool addFdToPoll(PollHandler *handler, int poll) {
	struct epoll_event ev = {
		.events = EPOLLIN,
		.data.ptr = handler
	};
	if (epoll_ctl(poll, EPOLL_CTL_ADD, handler->fd, &ev) == -1) {
		perror("epoll_ctl");
		return false;
	} else {
		return true;
	}
}

bool runPolls(int pfd, struct epoll_event* polls, int count) {
	//int n = epoll_wait(corePoll.pfd, polls, 16, 100); //use for test
	int n = epoll_wait(pfd, polls, count, -1);
	if (n == -1) {
		if (errno == EINTR) {
			//printf("EINTER error game loop\n");
		} else {
			return false;
		}
	}
	for (int i = 0; i < n; i++) {
		PollHandler *handler = polls[i].data.ptr;
		handler->func();
	}
	return true;
}

void closePoll(PollSystem poll) {
	if (poll.handler.fd != -1) {
		close(poll.handler.fd);
	}
	if (poll.pfd != -1) {
		close(poll.pfd);
	}
}

bool initTimerFd(PollHandler *handler, int ticksPerSecond, void (*func)(void)) {
	handler->fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	if (handler->fd == -1) {
		perror("timerfd_create");
		return false;
	}

	uint64_t tickNS = 1000000000ULL / ticksPerSecond; 

	struct itimerspec ts = {
		.it_interval = {
			.tv_sec = 0,
			.tv_nsec = tickNS
		},
		.it_value= {
			.tv_sec = 0,
			.tv_nsec = tickNS
		}
	};
	if (timerfd_settime(handler->fd, 0, &ts, NULL) == -1) {
		perror("timeerfd_settime");
		return false;
	}
	handler->func = func;
	return true;
}


