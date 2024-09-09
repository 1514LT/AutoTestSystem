#ifndef FTPCLIENT_HPP
#define FTPCLIENT_HPP
#include "../../include/dataProcess.hpp"
#include "Message.hpp"
class FTPClient
{
private:
  
public:
  FTPClient();
  ~FTPClient();

public:
  void SendFile(int socketFd,std::string srcFileName);
  void RecvFile(int socketFd,std::string dstFileName);
};




#endif
