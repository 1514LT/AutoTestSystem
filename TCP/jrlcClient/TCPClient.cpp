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
std::shared_ptr<Message> TCPClient::InitMsg(char type,std::string json,std::string payload)
{
  int bodySize = 0;
  int jsonSize = json.size();
  int payloadSize = payload.size();
  bodySize = jsonSize + payloadSize;
  std::shared_ptr<Message> msg(new Message(bodySize,type,jsonSize,payloadSize));
  return msg;
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
    Json::Value root = JRLC::StringToJson(reseult);
    switch (msg_type)
    {
    case -1:
      std::cout << "erro msg_type" << std::endl;
      break;
    // case MSG_TYPE::SEND_FILE_MSG:
    //   start(&TCPClient::SendFile,this,root);
    //   break;
    // case MSG_TYPE::DOWNLOAD_FILE_MSG:
    //   start(&TCPClient::RecvFile,this,root);
    //   break;
    case MSG_TYPE::TESTCASE_MSG:
      start(&TCPClient::test,this,root);
    default:
      break;
    }
    

    sleep(1);
  }
  printf("client close\n");
  
}


std::string TCPClient::extractFileName(const std::string& url) {
    std::size_t lastSlash = url.find_last_of('/');
    if (lastSlash != std::string::npos) {
        return url.substr(lastSlash + 1);
    }
    return "";
}

void TCPClient::test(Json::Value root)
{
  /* to do run test case*/
  std::string msg = JRLC::JsonToString(root);
  if(root["testTaskIds"].isNull())
    return;
  std::vector<long> TaskIds;
  for(auto ids:root["testTaskIds"])
  {
    std::string startTaskTime = JRLC::microsecondsToDateTime(JRLC::getCurrentTimeMicro()); 
    int successFrequency = 0;
    int failFrequency = 0;
    int actualTestedNumber = 0;
    TaskIds.push_back(ids.asInt64());
    printf("testTaskId:%ld\n",ids.asInt64());
    while (1)
    {
      Json::Value back;
      back["testTaskRecordId"] = ids.asInt64();
      std::string payload = "";
      std::shared_ptr<Message> msg = InitMsg(5,JRLC::JsonToString(back),payload);
      sendMessage(msg,JRLC::JsonToString(back),payload);
      std::string strJson;
      int msgType;
      receiveMessage(strJson,msgType);
      if(strJson.size() < 20)
      {
        printf("test %ld end\n",ids.asInt64());
        break;
      }
      printf("---->strJson:%s\n",strJson.c_str());
      Json::Value backSecend;
      backSecend =  JRLC::StringToJson(strJson);
      if(backSecend["testCase"]["testCaseName"].isNull() || backSecend["testCase"]["executionParameters"].isNull() || backSecend["testCase"]["fileUrl"].isNull())
      {
        std::cerr << "erro recv" << std::endl;
        return;
      }
      // 判断当前目录是否存在测试文件，不存在下载
      std::string dstName = extractFileName(backSecend["testCase"]["fileUrl"].asString());
      printf("dstName:%s\n",dstName.c_str());
      std::ifstream file_test(dstName);
      if(!file_test.good())
      {
        std::string cmd_curl_download = "curl -o " + dstName + " " + backSecend["testCase"]["fileUrl"].asString();
        std::cout << JRLC::getCmd(cmd_curl_download) << std::endl;
      }

      // 解压压缩包
      std::string cmd_tar = "tar -xf " + dstName;
      std::cout << JRLC::getCmd(cmd_tar) << std::endl;
      // 启动进程执行测试用例
      std::string logFile = backSecend["testCase"]["id"].asString() + "_" + std::to_string(JRLC::getCurrentTimeMicro()) + "_result.txt";
      std::string cmd_exec = backSecend["testCase"]["executionParameters"].asString() + "> " + logFile;
      long long startTime = JRLC::getCurrentTimeMicro();
      std::string startTimeStr = JRLC::microsecondsToDateTime(startTime);
      std::string back_exec = JRLC::getCmd(cmd_exec);
      std::string strRun = JRLC::read_file_contents("result.txt");
      std::cout << strRun << std::endl;
      long long endTime = JRLC::getCurrentTimeMicro();
      std::string endTimeStr = JRLC::microsecondsToDateTime(endTime);
      long long usedTime = endTime - startTime;
      std::string usedTimeStr = JRLC::microsecondsToDateTime(usedTime);
      // 上传测试输出
      Json::Value root_outPut;
      root_outPut["fileName"] = logFile;
      root_outPut["type"] = 3;
      upTestOutPut(root_outPut,root_outPut["fileName"].asString(),endTimeStr,startTimeStr,usedTime,backSecend,successFrequency,failFrequency,actualTestedNumber);
      // 删除测试文件
      std::string cmd_rm = "rm -rf " + logFile;
      JRLC::getCmd(cmd_rm);
    }
    std::string endTaskTime = JRLC::microsecondsToDateTime(JRLC::getCurrentTimeMicro()); 
    // 返回测试任务执行情况
    Json::Value rootTask;
    rootTask["endTime"] = endTaskTime;
    rootTask["startTime"] = startTaskTime;
    rootTask["successFrequency"] = successFrequency;
    rootTask["actualTestedNumber"] = actualTestedNumber;
    rootTask["failFrequency"] = failFrequency;
    rootTask["testTaskRecordId"] = ids.asInt64();
    std::string payload = "";
    std::shared_ptr<Message> msgTask = InitMsg(MSG_TYPE::FINISH_TEST_TASK_RECORD,JRLC::JsonToString(rootTask),payload);
    sendMessage(msgTask,JRLC::JsonToString(rootTask),payload);
  }

}

void TCPClient::upTestOutPut(Json::Value root,std::string uploadFile,std::string endTimeStr, std::string startTimeStr, long long usedTime,Json::Value rootFirstBack,int& successFrequency,int& failFrequency,int& actualTestedNumber)
{
  std::string payload;
  std::shared_ptr<Message> msg = InitMsg(MSG_TYPE::SEND_FILE_MSG,JRLC::JsonToString(root),payload);
  sendMessage(msg,JRLC::JsonToString(root),payload);
  std::string backJson;
  int bcakType;
  receiveMessage(backJson,bcakType);
  std::cout << "backJson" << backJson << std::endl;
  Json::Value backRoot = JRLC::StringToJson(backJson);

  std::string cmd_curlT = "curl -X PUT -T " + uploadFile + " \""+ backRoot["uploadUrl"].asString() + "\"";
  printf("cmd_curlT:%s\n",cmd_curlT.c_str());
  std::cout << JRLC::getCmd(cmd_curlT) << std::endl;

  // 添加测试用例记录
  std::stringstream iss(JRLC::getLastLine(JRLC::read_file_contents(root["fileName"].asString())));

  std::string key, value;

  // 读取到分隔符 ':' 之前的部分
  std::getline(iss, key, ':');

  // 读取到分隔符 ':' 之后的部分
  std::getline(iss, value);
  int testResult = 0;
  if(value == "success")
  {
    testResult = 1;
    successFrequency += 1;
  }
  else
  {
    testResult = 0;
    failFrequency += 1;
  }
  
  Json::Value rootAddTestCase;
  rootAddTestCase["endTime"] = endTimeStr;
  rootAddTestCase["executionStatus"] = testResult;
  rootAddTestCase["executionTime"] = uint64_t(usedTime/1000); // 毫秒
  rootAddTestCase["logFileUrl"] = backRoot["downloadUrl"].asString();
  rootAddTestCase["singleBoardId"] =rootFirstBack["singleBoardId"].asUInt64();
  rootAddTestCase["startTime"] = startTimeStr;
  rootAddTestCase["testCaseId"] = rootFirstBack["testCase"]["id"].asUInt64();
  rootAddTestCase["testTaskId"] = rootFirstBack["testTaskId"].asUInt64();
  rootAddTestCase["testTaskRecordId"] = rootFirstBack["testTaskRecordId"].asUInt64();
  std::shared_ptr<Message> msg2 = InitMsg(MSG_TYPE::ADD_TEST_CASE_RECORDE,JRLC::JsonToString(rootAddTestCase),payload);
  sendMessage(msg2,JRLC::JsonToString(rootAddTestCase),payload);
  std::cout << JRLC::JsonToString(rootAddTestCase) << std::endl;
  actualTestedNumber += 1;
}

void TCPClient::addTestCaseRecorde(Json::Value root)
{
  
}

void TCPClient::SendFile(Json::Value root)
{

}
void TCPClient::RecvFile(Json::Value root)
{

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

bool TCPClient::receiveMessage(Json::Value &back)
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
  back = JRLC::StringToJson(result);
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

bool TCPClient::receiveMessage(std::string &strJson,int &msgType)
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
  strJson = result;
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
    root["cpuSerialNumber"] = "";
    return;
  }
  else
  {
    root["cpuSerialNumber"] = vt_serial[2];
  }
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
  printf("Init end\n");
}