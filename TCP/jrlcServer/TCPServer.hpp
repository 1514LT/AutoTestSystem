#ifndef TCPSERVR_HPP
#define TCPSERVER_HPP
#include "../../include/dataProcess.hpp"
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

class TCPServer
{
private:
  
public:
  TCPServer();
  TCPServer(int port);
  ~TCPServer();

public:
  void echo(int cli_fd);
};



#endif