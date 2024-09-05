#ifndef MESSAGE_HPP
#define MESSAGE_HPP
#include "../../include/dataProcess.hpp"

class Message
{
private:
  int m_bodySize;
  char m_type;
  int m_jsonSize;
  int m_payloadSize;
public:
  Message(/* args */);
  Message(int bodySize,unsigned char type,int jsonSize,int payloadSize):m_bodySize(bodySize),m_type(type),m_jsonSize(jsonSize),m_payloadSize(payloadSize){}
  ~Message();

public:
  int getBodySize();
  char getType();
  int getJsonSize();
  int getPayloadSize();

};



#endif