#include "FTPClient.hpp"

FTPClient::FTPClient()
{
}

FTPClient::~FTPClient()
{
}
void FTPClient::SendFile(int socketFd,std::string srcFileName)
{
  std::string cmdSend = "";
  JRLC::getCmd(cmdSend);
}
void FTPClient::RecvFile(int socketFd,std::string dstFileName)
{
  std::string cmdRecv = "";
  JRLC::getCmd(cmdRecv);
}
