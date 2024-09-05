#include "TCPClient.hpp"
#include "Message.hpp"
#include <cstring>
#include <memory>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

TCPClient::TCPClient(const char *serverIP, int serverPort)
{
  m_clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (m_clientSocket == -1)
  {
    perror("Error creating socket");
    exit(EXIT_FAILURE);
  }

  sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(serverPort);
  if (inet_pton(AF_INET, serverIP, &serverAddress.sin_addr) <= 0)
  {
    perror("Invalid address");
    exit(EXIT_FAILURE);
  }

  if (connect(m_clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
  {
    perror("Connection failed");
    exit(EXIT_FAILURE);
  }
  test();

}
TCPClient::~TCPClient()
{

}


int TCPClient::getSocket()
{
  return m_clientSocket;
}


void TCPClient::sendMessage(const std::shared_ptr<Message>& msg,std::string json_buf,std::string payload)
{
  std::stringstream iss;
  int bodySize = msg->getBodySize();
  
  char type = msg->getType();

  int jsonSize = json_buf.size();
  int payloadSize = payload.size();
  int back_bodySize = jsonSize + payloadSize;
  bodySize =htonl(jsonSize + payloadSize);
  jsonSize = htonl(jsonSize);
  payloadSize = htonl(payloadSize);


#if 1
  char buff[back_bodySize+9];
  char * header = buff;
  int *p_bodySize = (int*)header;
  *p_bodySize = bodySize;
  header += 4;
  char *p_type = (char*)header;
  *p_type = type;
  header += 1;
  int *p_jsonSize = (int*)header;
  *p_jsonSize = jsonSize;
  header += 4;
  int *p_payloadSize = (int*)header;
  *p_payloadSize = payloadSize;
  header += 4;
  memcpy(header, json_buf.c_str(),json_buf.size());
  header += json_buf.size();
  memcpy(header, payload.c_str(), payload.size());
  send(getSocket(), buff, back_bodySize+13, 0);
  #endif

}

// void TCPClient::receiveMessage(Message &msg)
// {
//   // int fd = getSocket();
//   // int *p = (int *)&msg;
//   // recv(fd, &msg, 4, 0);
//   // msg.len = *p;
//   // unsigned char payload_len[4] = "";
//   // recv(fd,payload_len,sizeof(payload_len),0);
//   // int *payload_size = (int*)payload_len;
//   // msg.payload_len = *payload_size;
//   // msg.buf = new char[msg.len];
//   // recv(fd, msg.buf, msg.len, 0);
//   // msg.payload_buf = new char[msg.payload_len];
//   // recv(fd,msg.payload_buf,msg.payload_len,0);
//   // printf("json_len:%d,payload_len:%d,json_buf:%s,payload_buf:%s\n", msg.len, msg.payload_len, msg.buf, msg.payload_buf);
//   return;
// }

void TCPClient::test()
{
  Json::Value root;
  root["MSG_Type"] = "KernelState";
  root["MSG_Body"] = "cmd_create";
  root["sessionName"] = "mysession";
  root["output"] = "/tmp/root-lttng-traces/";
  root["password"] = "111111";
  int bodySize = 0;
  char type = 1;
  int jsonSize = JRLC::JsonToString(root).size();
  std::string payload = "asdasdasd";
  int payloadSize = payload.size();
  bodySize = jsonSize + payloadSize;
  std::shared_ptr<Message> msg(new Message(bodySize,type,jsonSize,payloadSize));

  sendMessage(msg,JRLC::JsonToString(root),payload);
}