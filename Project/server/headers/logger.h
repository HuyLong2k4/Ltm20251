#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <time.h>
#include <string.h>

void logMessage(int connfd, const char* direction, const char* message);
void logResult(int connfd, const char* direction, int result);
void initLogger();
void closeLogger();

// Wrapper functions that log automatically
void sendMessageLogged(int connfd, char *message);
void sendResultLogged(int connfd, int result);

// Macros for easy replacement
#define SEND_MSG(fd, msg) sendMessageLogged(fd, msg)
#define SEND_RESULT(fd, res) sendResultLogged(fd, res)

#endif // LOGGER_H
