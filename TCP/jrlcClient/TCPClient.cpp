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
  Init();
  std::thread(&TCPClient::Heartbeat,this).detach();
  Run();

}

void TCPClient::Heartbeat()
{
  while (1)
  {
    // printf("Handle Heartbeat\n");

    Json::Value root;
    int bodySize = 0;
    char type = 2;
    int jsonSize = 0;
    std::string payload = "";
    int payloadSize = payload.size();
    bodySize = jsonSize + payloadSize;
    std::shared_ptr<Message> msg(new Message(bodySize,type,jsonSize,payloadSize));
    sendMessage(msg,JRLC::JsonToString(root),payload);
    sleep(1);
  }
  
}

void TCPClient::Run()
{
  while (1)
  {
    printf("Handle Messge\n");
    std::string reseult;
    int msg_type = -1;
    if(!receiveMessage(reseult,msg_type))
      break;

    switch (msg_type)
    {
    case -1:
      std::cout << "erro msg_type" << std::endl;
      break;
    
    default:
      break;
    }
    Json::Value root = JRLC::StringToJson(reseult);

    sleep(1);
  }
  printf("client close\n");
  
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

}

bool TCPClient::receiveMessage(std::string &arg_result,int &msgType)
{
  int cli_fd = getSocket();
  unsigned char date_size[4] = "";
  void *ping = date_size;
  if(recv(cli_fd, date_size, sizeof(date_size), 0)<=0)
  {
    perror("recv date_size\n");
    return false;
  }
  printf("date_size:%d\n", ntohl(*(int*)ping));
  char type[1] = "";
  if(recv(cli_fd, type, sizeof(type), 0)<=0)
  {
    perror("recv type\n");
    return false;
  }
  std::cout << "type:" << (int)type[0] << std::endl; 
  msgType = (int)type[0];
  char jsonSize[4] = "";
  if(recv(cli_fd, jsonSize, sizeof(jsonSize), 0)<= 0)
  {
    perror("recv jsonSize\n");
    return false;
  }
  printf("jsonSize:%d\n",ntohl(*(int*)jsonSize));
  int jsonSizes = ntohl(*(int*)jsonSize);

  char payloadSize[4] = "";
  if(recv(cli_fd, payloadSize, sizeof(payloadSize), 0)<=0)
  {
    perror("recv payloadSize\n");
    return false;
  }
  printf("payloadSize:%d\n",ntohl(*(int*)payloadSize));
  int payloadSizes = ntohl(*(int*)payloadSize);

  char json[jsonSizes] = "";
  if(recv(cli_fd, json, jsonSizes, 0)<=0)
  {
    perror("recv jsonSizes\n");
    return false;
  }
  printf("json:%s\n",json);
  std::string result = json;
  arg_result = result;
  char payload[payloadSizes] = "";
  if(payloadSizes > 0)
  {
    if(recv(cli_fd, payload, payloadSizes, 0)<=0)
    {
      perror("recv payload\n");
      return false;
    }
    printf("payload:%s\n",payload);
  }
  
  return true;
}

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

void TCPClient::Init()
{
  Json::Value root;
  std::string cpu_info = JRLC::getCmd(cmd_cpu_info);
  std::stringstream iss_cpu_info(cpu_info);
  std::string str_cpu_info;
  std::string tmp;
  std::string filePath = josnPath + "/CPUInfo/cpu.json";
  int i = 0;
  while(iss_cpu_info >> tmp)
  {
    if(i > 6)
    {
      break;
    }
    str_cpu_info += tmp;
    i++;
  }
  root["cpu"] = str_cpu_info;
  Json::Value old = JRLC::StringToJson(JRLC::read_file_contents(filePath));
  root["id"] = old["id"].asLargestInt();
  std::string cpuSerialNumber = JRLC::getCmd(cmd_cpu_serial); // cpu序列号
  std::stringstream iss(cpuSerialNumber);
  std::vector<std::string> vt_serial;
  
  while(iss >> tmp)
  {
    vt_serial.push_back(tmp);
    // printf("cpuSerialNumber:%s\n",tmp.c_str());
  }
  if(vt_serial.size()<3)
  {
    perror(cmd_cpu_serial.c_str());
    return;
  }
  root["cpuSerialNumber"] = vt_serial[2];
  std::string os_info = JRLC::getCmd(cmd_cpu_os);
  std::stringstream iss_os(os_info);
  std::vector<std::string> vt_os;
  while(iss_os >> tmp)
  {
    vt_os.push_back(tmp);
  }
  root["singleBoardName"] = vt_os[1]; // 单板名称
  root["operatingSystem"] = vt_os[0]; // 操作系统信息

  std::string result = JRLC::JsonToString(root);
  
  JRLC::wirte_file_contents(result,filePath);
  // printf("sendMsg:%s\n",result.c_str());

  int bodySize = 0;
  char type = 1;
  int jsonSize = result.size();
  std::string payload = "";
  int payloadSize = payload.size();
  bodySize = jsonSize + payloadSize;
  std::shared_ptr<Message> msg(new Message(bodySize,type,jsonSize,payloadSize));

  sendMessage(msg,JRLC::JsonToString(root),payload);

  std::string back;
  int msg_type = -1;
  if(!receiveMessage(back,msg_type))
  {
    perror("recv 0 size pack\n");
    return;
  }
  // printf("back:%s\n",back.c_str());
  Json::Value root_back = JRLC::StringToJson(back);
  if(root_back["id"].isNull())
  {
    perror("root_back[\"id\"]\n");
    return;
  }
  root["id"] = root_back["id"].asLargestInt();
  std::string back_result = JRLC::JsonToString(root);
  // printf("saveMsg:%s\n",back_result.c_str());
  JRLC::wirte_file_contents(back_result,filePath);
  sleep(1);
  printf("Init end\n");
}