#include "Message.hpp"
Message::Message()
{
}

Message::~Message()
{
}
  int Message::getBodySize()
  {
    return m_bodySize;
  }
  char Message::getType()
  {
    return m_type;
  }
  int Message::getJsonSize()
  {
    return m_jsonSize;
  }
  int Message::getPayloadSize()
  {
    return m_payloadSize;
  }
