#include "TCP/jrlcClient/TCPClient.hpp"

int main(int argc, char const *argv[])
{
  if (argc != 3)
  {
    printf("input Ip and Port!\n");
    return -1;
  }
  const char *ip = argv[1];
  int port = std::stoi(argv[2]);
  TCPClient client(ip, port);
  while (1)
  {
    /* code */
  }
  
  return 0;
}
