#include "TCP/jrlcClient/TCPClient.hpp"
#include "TCP/jrlcServer/TCPServer.hpp"
#include "include/dataProcess.hpp"

void InitServer()
{
  std::string configFile = JRLC::getLocalPath() + "/source/config.json";
  // printf("configFile:%s\n",configFile.c_str());
  Json::Value root_server = JRLC::StringToJson(JRLC::read_file_contents(configFile));
  int serverPort = root_server["localServerPort"].asInt();
  std::shared_ptr<TCPServer> server(new TCPServer(serverPort));
}

void InitClient()
{
  std::string configFile = JRLC::getLocalPath() + "/../source/config.json";
  Json::Value root_server = JRLC::StringToJson(JRLC::read_file_contents(configFile));
  int serverPort = root_server["serverPort"].asInt();
  std::string serverIp = root_server["serverIp"].asString();
  std::shared_ptr<TCPClient> client(new TCPClient(serverIp.c_str(),serverPort));
}

int main(int argc, char const *argv[])
{
  std::cout << "Init server" << std::endl;
  std::thread(InitServer).detach();

  std::cout << "Init client" << std::endl;
  std::thread(InitClient).detach();

  while (1)
  {
    sleep(1);
  }
  
  return 0;
}
