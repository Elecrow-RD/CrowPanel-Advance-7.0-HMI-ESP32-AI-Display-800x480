#include "At_Config.hpp"

AT_Config* AT_Config::instance_ = nullptr;

#if 1  //private


#endif

#if 1 //public
AT_Config& AT_Config::getInstance()
{
  if (instance_ == nullptr)
  {
    instance_ = new AT_Config();
  }

  return *instance_;
}

AT_Config::AT_Config(void)
{

}

void AT_Config::begin( void )
{

}

void AT_Config::parseCmd(const char *cmd, uint16_t length)
{
  ATEerror_t status = AT_OK;
  const struct ATCommand_s *Current_ATCommand;

  if (strcmp(cmd, "AT\r\n") == 0)
  {
    status = AT_OK;
  }
  else if (cmd[2] == '\0')
  {
     status = AT_ERROR; 
  }
  else
  {
    /*excluding AT */
    if ((cmd[length - 1] != '\n') || (cmd[length - 2] != '\r'))
    {
      status = AT_PARAM_ERROR;
    }
    else
    {
      status = AT_ERROR;
      cmd += 2;

      for (size_t i = 0; i < (sizeof(ATCommand) / sizeof(struct ATCommand_s)); i++)
      {
        if (strncmp(cmd, ATCommand[i].string, ATCommand[i].size_string) == 0)
        {
          Current_ATCommand = &(ATCommand[i]);
          /* point to the string after the command to parse it */
          cmd += Current_ATCommand->size_string;

          /* parse after the command */
          switch (cmd[0])
          {
            case '\r': /* nothing after the command */
              if (cmd[1] == '\n')
              {
                status = Current_ATCommand->run(cmd);
              }
              break;
            case '=':
              if ((cmd[1] == '?') && (cmd[2] == '\r') && (cmd[3] == '\n')) //)
              {
                status = Current_ATCommand->get(cmd + 1);
              }
              else
              {
                status = Current_ATCommand->set(cmd + 1);
                if (status == AT_OK)
                {

                }
              }
              break;
            case '?':
              status = AT_OK;
              break;
            default:
              /* not recognized */
              break;
          }
          break;
        }
      }
    }
  }
  AT_PPRINTF("%s", ATError_description[status]);
}

bool AT_Config::receiveSerialCmd(uint8_t *buf, uint8_t *size)
{
  uint8_t uart_len = Serial.available();

  if (uart_len > 0 )
  {
    for (uint8_t u8i = 0; u8i < uart_len; u8i++)
    {
      buf[u8i] = Serial.read();
    }
    Serial.flush(); // empty rx fifo
    *size = uart_len;
    return true;
  }
  return false;
}

#endif

#if 1 //protected


#endif

AT_Config at_config;
