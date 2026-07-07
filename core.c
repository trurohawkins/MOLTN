#include "core.h"

//used by poll handlers;
void checkRunning();

atomic_int running = 0;

PollSystem corePoll = {
	.pfd = -1,
	.handler = {
		.fd = -1,
	},
};

PollSystem outputPoll = {
	.pfd = -1,
	.handler = {
		.fd = -1,
	},
};

PollSystem gamePoll = {
	.pfd = -1,
	.handler = {
		.fd = -1,
	},
};

SystemQueue events;

bool initCore() {
	initPollSystem(&corePoll, &checkRunning);
	running = 1;
	return true;
}

void exitCore() {
	if (corePoll.handler.fd != -1) {
		close(corePoll.handler.fd);
	}
	if (corePoll.pfd != -1) {
		close(corePoll.pfd);
	}
}

void coreLoop() {
	while (atomic_load_explicit(&running, memory_order_acquire)) {
		runPolls(corePoll.pfd, corePoll.polls, 16);
	}
}

void wakeEvent() {
	uint64_t v = 1;
	if (write(corePoll.handler.fd, &v, sizeof(v)) == -1) {
		perror("write wakeFd");
	}
	if (outputPoll.handler.fd != -1) {
		if (write(outputPoll.handler.fd, &v, sizeof(v)) == -1) {
			perror("write outpoll fd");
		}
	}
}

void debugWrite(char *message) {
	FILE *fptr;
	fptr = fopen("debug.log", "a");
	fprintf(fptr, message);
	fclose(fptr);
}

void checkRunning() {
	//drain corePoll.handler
	uint64_t v;
	read(corePoll.handler.fd, &v, sizeof(v));

	if (!atomic_load_explicit(&running, memory_order_acquire)) {
		uint64_t v;
		read(corePoll.handler.fd, &v, sizeof(v));
	}
}

bool addFdToCore(PollHandler *handler) {
	return addFdToPoll(handler, corePoll.pfd);
}

bool pushEvent(int type, const void *data, uint32_t size) {
	if (size > EVENT_BUFFER_SIZE) {
		return false;
	}
	SystemEvent se;
	se.type = type;
	se.size = size;
	memcpy(se.data, data, size);

	AqPushResult res = SystemQueue_aqPush(&events, se);
	if (res == AQ_PUSH_SUCCESS_EMPTY) {
		uint64_t v = 1;
		if (write(gamePoll.handler.fd, &v, sizeof(v)) == -1) {
			perror("write to event");
			return false;
		}
	} else if (res == AQ_PUSH_FAILED) {
		perror("event queue push fail");
		return false;
	}
	return true;
}

bool popEvent(SystemEvent *se) {
	bool ret = SystemQueue_aqPop(&events, se);
	return ret;
}

