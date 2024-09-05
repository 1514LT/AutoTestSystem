#include "TCPServer.hpp"
#include <netinet/in.h>
#include <sys/socket.h>


TCPServer::TCPServer()
{
}

TCPServer::~TCPServer()
{
}

TCPServer::TCPServer(int port)
{
  int sockfd = 0;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    perror("socket");
  }
  int opt = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  struct sockaddr_in my_addr;
  bzero(&my_addr, sizeof(my_addr));
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(port);
  my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  int ret = bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr));
  if (ret != 0)
  {
    perror("bind");
  }
  ret = listen(sockfd, 10);
  if (ret != 0)
  {
    perror("listen");
  }
  try
  {
    while (1)
    {
      struct sockaddr_in cli_addr;
      socklen_t cli_len = sizeof(cli_addr);
      int cli_fd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_len);
      if (cli_fd < 0)
        continue;
      unsigned short port = ntohs(cli_addr.sin_port);
      char ip_str[16] = "";
      inet_ntop(AF_INET, &cli_addr.sin_addr.s_addr, ip_str, 16);
      printf("%s:%hu 的连接已到来.......\n", ip_str, port);
      std::thread handler(&TCPServer::echo, this, cli_fd);
      handler.detach();
    }
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
    close(sockfd);
    this->~TCPServer();
  }
}

void TCPServer::echo(int cli_fd)
{
  printf("cli_fd:%d\n", cli_fd);
  unsigned char date_size[4] = "";
  void *ping = date_size;
  int connectFlage = recv(cli_fd, date_size, sizeof(date_size), 0);
  printf("date_size:%d\n", ntohl(*(int*)ping));

  char type[1] = "";
  recv(cli_fd, type, sizeof(type), 0);
  printf("type:%d\n",*(int*)type);

  char jsonSize[4] = "";
  recv(cli_fd, jsonSize, sizeof(jsonSize), 0);
  printf("jsonSize:%d\n",ntohl(*(int*)jsonSize));
  int jsonSizes = *(int*)jsonSize;

  char payloadSize[4] = "";
  recv(cli_fd, payloadSize, sizeof(payloadSize), 0);
  printf("jsonSize:%d\n",ntohl(*(int*)payloadSize));
  int payloadSizes = *(int*)payloadSize;

  char json[1024] = "";
  recv(cli_fd, json, jsonSizes, 0);
  printf("json:%s\n",json);

  char payload[1024] = "";
  recv(cli_fd, payload, payloadSizes, 0);
  printf("payload:%s\n",payload);


  



}
int main(int argc, char const *argv[])
{
    if(argc != 2)
  {
    perror("./jrlcServer port");
    return -1;
  }

  TCPServer tcp(8000);
  return 0;
}
