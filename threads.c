#include "threads.h"

/****
	func ran on thread
	data should heap allocated
		and will be sent to the func
****/
pthread_t createThread(void*(*func)(void*), void *data, bool detached) {
	// specifies proeprties of thread
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(
		&attr, 
		detached ? PTHREAD_CREATE_DETACHED : PTHREAD_CREATE_JOINABLE
	);
	
	// create fork point
	pthread_t threadHandle;
	int err = pthread_create(&threadHandle, &attr, func, data);

	pthread_attr_destroy(&attr);

	if (err != 0) {
		printf("error creating thread: %i\n", err);
	}

	return threadHandle;
}
