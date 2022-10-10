#pragma once
#include <sys/epoll.h>
#include <sys/utsname.h>
#include <linux/input.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <android/input.h>
#include <regex.h>
#include "Common.h"

int getTouchDevice();

void getLinuxRelease(int *major, int *minor);

int createEpoll(int fd);

int hasEvent(int mEpollFd, epoll_event *events, int size);

motion_event *process(input_event event);
