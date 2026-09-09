/*
 * Copyright (C) 2025-2026 Guy Wilson
 *
 * This file is part of PFM.
 *
 * PFM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PFM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PFM. If not, see <https://www.gnu.org/licenses/>.
 */

#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "logger.h"
#include "posixthread.h"

static void * _threadRunner(void * pThreadArgs) {
	void * pThreadRtn = NULL;

	PosixThread * pThread = (PosixThread *)pThreadArgs;

	Logger & log = Logger::getInstance();

	bool go = true;

	while (go) {
		try {
			pThreadRtn = pThread->run();
		}
		catch (thread_error & e) {
			log.error("_threadRunner: Caught exception %s", e.what());
		}

		if (pThread->isRestartable) {
			log.status("Restarting thread...");
			PosixThread::sleep(1);
		}
		else {
			go = false;
		}
	}

	return pThreadRtn;
}

bool PosixThread::start() {
	return this->start(NULL);
}

bool PosixThread::start(void * p) {
	int			err;

	this->threadParameters = p;

	err = pthread_create(&this->tid, NULL, &_threadRunner, this);

	if (err != 0) {
		log.error("ERROR! Can't create thread :[%s]", strerror(err));
		return false;
	}

	return true;
}

void PosixThread::stop() {
	pthread_kill(this->tid, SIGKILL);
}
