#ifndef APPLICATION_HPP
#define APPLICATION_HPP
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <chrono>
#include <thread>
#include <sstream>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "dataProcess.hpp"
class Application
{
private:
  pid_t pid = -1;
  bool killd = false;
  long long startTime = 0;
  long long endTime = 0;
  std::vector<std::string> outPut;
  int pipefd[2];
  int outputPipe[2];
  std::string file_path;
  std::string shell_path;
public:
  Application();
  ~Application();

public:
  bool isruning = true;
  bool istimeout = false;
  std::string exitStatus = "";
  std::string runningStatus = "";
  int stop_RequestApplicationState = 0;
  bool isWatching = false;

public:
  bool isProcessRunning(pid_t pid);
  bool StartApplication(const char* path);
  void SetPid(pid_t pid);
  pid_t GetPid(void);
  bool KillPid(pid_t pid);
  long long getCurrentTimeMillis();
  long long getStartTime();
  void setStartTime(long long startTime);
  void setEndTime(long long endTime);
  long long getEndTime();
  void FreeChild(pid_t pid);
  void GetStatus(pid_t pid);
  void printExitStatus(int status);
  std::string getLastTwoLines(const std::string& str);
  std::vector<std::string> getOutPut();
  void recvLog(int outputPipe[2]);
  void appendToFile(const std::string& filename, const std::string& content);
};




#endif