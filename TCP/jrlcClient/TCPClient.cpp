#include "TCPClient.hpp"

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

}
TCPClient::~TCPClient()
{

}


int TCPClient::getSocket()
{
  return m_clientSocket;
}


void TCPClient::sendMessage(std::string json_buf,std::string payload)
{
  // std::
  // char tmp[json_buf.size()];
  // strcpy(tmp, json_buf.c_str());
  // int size = strlen(tmp);
  // char date[size + 8] = "";
  // char *p = date;
  // char *header = p;
  // int *len = (int *)p;
  // *len = size;
  // p += 4;
  // int *payload_len = (int *)p;
  // *payload_len = 2;
  // p += 4;
  // char buff[size];
  // memcpy(p, tmp, size);
  // memcpy(buff, p, size);
  // p += size;
  // char payload_buf[*payload_len];
  // memcpy(p,payload_buf,*payload_len);
  // printf("len:%d date:%s\n", *len, buff);
  // send(getSocket(), header, size + 8 +*payload_len , 0);
}

void TCPClient::receiveMessage(Message &msg)
{
  // int fd = getSocket();
  // int *p = (int *)&msg;
  // recv(fd, &msg, 4, 0);
  // msg.len = *p;
  // unsigned char payload_len[4] = "";
  // recv(fd,payload_len,sizeof(payload_len),0);
  // int *payload_size = (int*)payload_len;
  // msg.payload_len = *payload_size;
  // msg.buf = new char[msg.len];
  // recv(fd, msg.buf, msg.len, 0);
  // msg.payload_buf = new char[msg.payload_len];
  // recv(fd,msg.payload_buf,msg.payload_len,0);
  // printf("json_len:%d,payload_len:%d,json_buf:%s,payload_buf:%s\n", msg.len, msg.payload_len, msg.buf, msg.payload_buf);
  return;
}

void TCPClient::test()
{
  Json::Value root;
    const std::string cmd_create = R"(
    {
      "MSG_Type":"KernelState",
      "MSG_Body":"cmd_create",
      "sessionName":"mysession",
      "output":"/tmp/root-lttng-traces/",
      "password":"111111"
    }
  )";
  Message msg;
  sendMessage(cmd_create);
  bzero(&msg, sizeof(msg));
  receiveMessage(msg, cli_fd);
}