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

enum MSG_TYPE{
ERRO_MSG,

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
  bool receiveMessage(std::string &arg_result,int &msgType);

public:
  void test();
  void Init(); // 获取cpu信息
  void Heartbeat();
  void Run();
};

#endif
