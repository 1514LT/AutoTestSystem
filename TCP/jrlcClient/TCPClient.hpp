#ifndef TCPCLIENT_HPP
#define TCPCLIENT_HPP
#include "../../include/dataProcess.hpp"
#include "Message.hpp"
#include <memory>
#include <string>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <pthread.h>
#include <fstream>
#include <thread>

class TCPClient {

template <typename Function, typename... Args>
void start(Function&& f, Args&&... args)
{
    std::thread(std::forward<Function>(f), std::forward<Args>(args)...).join();
}

enum MSG_TYPE{
TMP_MSG,
CPU_MSG,
HEARTBEAT_MSG,
SEND_FILE_MSG = 4,
DOWNLOAD_FILE_MSG = 5,
FINISH_TEST_TASK_RECORD = 6,
ADD_TEST_CASE_RECORDE = 7,
TESTCASE_MSG = 10

};
private:
  int m_clientSocket;
  std::string josnPath = "/root/AutoTestSystem/source";

private:
  std::string cmd_cpu_info = "lscpu";
  std::string cmd_cpu_serial = "cat /proc/cpuinfo | grep 'Serial'";
  std::string cmd_cpu_os = "uname -a";
public:
  TCPClient(const char *serverIP, int serverPort);
  ~TCPClient();

public:
  int getSocket();
  void sendMessage(const std::shared_ptr<Message>& msg,std::string json_buf,std::string payload);
  bool receiveMessage(std::string &strJson,int &msgType);
  bool receiveMessage(Json::Value &back);
  std::shared_ptr<Message> InitMsg(char type,std::string json,std::string payload);
public:
  std::string extractFileName(const std::string& url);
public:
  void test(Json::Value root); //执行测试任务
  void Init(); // 获取cpu信息
  void Heartbeat();
  void Run();
  void SendFile(Json::Value root);
  void RecvFile(Json::Value root);

public:
  void upTestOutPut(Json::Value root,std::string uploadFile,std::string endTimeStr, std::string starTime, long long usedTime,Json::Value rootFirstBack,int& successFrequency,int& failFrequency,int& actualTestedNumber);
  void addTestCaseRecorde(Json::Value root);
};

#endif
