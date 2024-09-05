#ifndef TCPCLIENT_HPP
#define TCPCLIENT_HPP
#include "dataProcess.hpp"
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

class TCPClient {
  struct Message {
    int bodySize;
    char type;
    int jsonSize;
    int payloadSize;
};
private:
  int m_clientSocket;

public:
  TCPClient(const char *serverIP, int serverPort);
  ~TCPClient();

public:
  int getSocket();
  void sendMessage(std::string json_buf,std::string payload);
  void receiveMessage(Message &msg);

public:
  void test();
};

#endif
