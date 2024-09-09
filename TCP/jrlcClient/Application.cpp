#include "Application.hpp"

Application::Application()
{
  file_path = JRLC::getUserPath() + "/dataacquisition/";
  if(JRLC::check_and_create_directory(file_path))
  {
    file_path = file_path + "shell/";
    JRLC::check_and_create_directory(file_path);
  }
  shell_path = file_path + "unbuffered.so";
}

Application::~Application()
{
}

bool Application::isProcessRunning(pid_t pid)
{
  if (pid <= 0)
  {
    return false;
  }
  if (kill(pid, 0) == 0)
  {
    printf("应用正在运行\n");
    return true;
  }
  else
  {
    if (errno == EPERM)
    {
      printf("应用正在运行，但我们没有权限发送信号\n");
      return true;
    }
    else if (errno == ESRCH)
    {
      printf("进程不存在\n");
      return false;
    }
    else
    {
      printf("检查进程时发生错误\n");
      return false;
    }
  }
}
void Application::SetPid(pid_t pid)
{
  this->pid = pid;
}
pid_t Application::GetPid(void)
{
  return this->pid;
}
void Application::printExitStatus(int status)
{
  std::ostringstream ss;
  const char* signalName = nullptr;
  if(WIFEXITED(status))
  {
      ss << "Subprocess ended normally, exit status:" << WEXITSTATUS(status);
  } 
  if(WIFSIGNALED(status))
  {
    switch(WTERMSIG(status))
    {
    case SIGABRT:
      signalName = "SIGABRT";
      ss << "A serious error occurred when the abort function was called.";
      break;
    case SIGSEGV:
      signalName = "SIGSEGV";
      ss << "Segmentation fault, invalid memory access.";
      break;
    case SIGILL:
      signalName = "SIGILL";
      ss << "Accessing protected memory space.";
      break;
    case SIGFPE:
      signalName = "SIGFPE";
      ss << "Arithmetic error, division by zero exception.";
      break;
    case SIGINT:
      signalName = "SIGINT";
      ss << "Ctrl+C。";
      break;
    case SIGKILL:
      signalName = "SIGKILL";
      ss << "kill -9。";
      break;
    default:
      break;
    }
    if(signalName)
    {
        ss << "The subprocess is due to the signal" << signalName << " end," << "end single" << WTERMSIG(status);
    }
    else
    {
        ss << "The subprocess ends due to an unknown signal," << "End signal:" << WTERMSIG(status);
    }
    
    if(WCOREDUMP(status))
    {
        ss << "And a coredump file was generated";
    }
    else
    {
        ss << "no coredump file was generated.";
    }
    }
    if(WIFSTOPPED(status))
    {
        ss << "The subprocess has been paused," << "Pause signal:" << WSTOPSIG(status);
    }
    if(WIFCONTINUED(status))
    {
        ss << "Subprocess terminated normally, exit status:" << WEXITSTATUS(status);
    }
    if(!WIFEXITED(status) && !WIFSIGNALED(status) && !WIFSTOPPED(status) && !WIFCONTINUED(status))
    {
        ss << "Unknown exit status.";
    }
    exitStatus = ss.str();
}
std::string Application::getLastTwoLines(const std::string& str) 
{
  std::istringstream iss(str);
  std::vector<std::string> lines;
  std::string line;
  while (std::getline(iss, line)) 
  {
      lines.push_back(line);
  }

  std::string lastTwoLines;
  if (lines.size() >= 2) 
  {
      lastTwoLines += *(lines.end() - 2) + "\n";
      lastTwoLines += *(lines.end() - 1);
  } 
  else if (lines.size() == 1)
  {
      lastTwoLines = lines[0];
  }

  return lastTwoLines;
}

void Application::GetStatus(pid_t pid)
{
  while(isWatching)
  {
    if (kill(pid, 0) == 0)
    {
      runningStatus = "process is running";
    } 
    else 
    {   
      std::string file_path = "/tmp/" + std::to_string(pid) + ".txt";
      std::string cmd_cat = "cat " + file_path;
      this->exitStatus = getLastTwoLines(JRLC::getCmd(cmd_cat));
      this->runningStatus = "process is quit";
      this->endTime = getCurrentTimeMillis();
      sleep(2);
      this->isWatching = false;
      kill(pid, SIGTERM);
    }
    sleep(1);
  }
}

void Application::FreeChild(pid_t pid)
{
  while (1)
  {
    if (killd)
    {
      if (kill(pid, SIGTERM) == 0)
      {
        int status;
        waitpid(pid, &status, 0);
        this->endTime = getCurrentTimeMillis();
        printExitStatus(status);
        break;
      }
      else
      {
        std::cerr << "Error: Failed to send SIGTERM signal to child process." << std::endl;
        break;
      }
    }
  }
}
bool Application::StartApplication(const char *path)
{

  if (pipe(pipefd) == -1 || pipe(outputPipe) == -1)
  {
    perror("pipe");
    exit(EXIT_FAILURE);
  }
  pid_t pid = fork();
  if (pid == 0) // 子进程
  {
    close(pipefd[0]);// 关闭读端
    close(outputPipe[0]);
    dup2(outputPipe[1], STDOUT_FILENO); // 重定向标准输出
    std::string env = "LD_PRELOAD=" + shell_path;
    char* script_path = (char *)path;
    char* program_path = (char *)path;
    char *argv[] = {
        script_path,
        program_path,
        NULL };
    char *envp[] = {
        (char*)env.c_str(),
        NULL};
    this->startTime = getCurrentTimeMillis();
    write(pipefd[1],&this->startTime,sizeof(this->startTime));
    
    close(pipefd[1]);
    if (execve(script_path, argv, envp) == -1)
    {
      perror("execve");
      this->startTime = -1;
      write(pipefd[1],&this->startTime,sizeof(this->startTime));
      close(pipefd[1]);
      return false;
    }
    exit(EXIT_FAILURE);
  }
  else if (pid < 0)
  {
    std::cerr << "Fork failed!" << std::endl;
    return false;
  }
  else // 父进程
  {
    close(pipefd[1]); 
    close(outputPipe[1]); // 关闭写端
    long long startTime;
    read(pipefd[0], &startTime, sizeof(startTime));
    close(pipefd[0]);  // 关闭读端
    if(startTime == -1)
    {
      SetPid(-1);
    }
    else
    {
      std::cout << "PID of child process: " << pid << std::endl;
      SetPid(pid);
    }
    this->startTime = startTime;
    std::thread(&Application::FreeChild, this, pid).detach();
    std::thread(&Application::recvLog,this,outputPipe).detach();
  }
  return true;
}

bool Application::KillPid(pid_t pid)
{
  this->killd = true;
  return true;
}

long long Application::getCurrentTimeMillis()
{
  auto now = std::chrono::system_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
  return duration.count();
}
long long Application::getStartTime()
{
  return this->startTime;
}
long long Application::getEndTime()
{
  return this->endTime;
}
void Application::setStartTime(long long startTime)
{
  this->startTime = startTime;
}
void Application::setEndTime(long long endTime)
{
  this->endTime = endTime;
}

std::vector<std::string> Application::getOutPut()
{
  return this->outPut;
}
void Application::recvLog(int outputPipe[2])
{
  close(outputPipe[1]); // 关闭写端
  // 从管道读取数据
  char buffer[10240];
  ssize_t bytes;
 while (true) {

    ssize_t bytes = read(outputPipe[0], buffer, sizeof(buffer));
    if (bytes <= 0) break;  // 如果没有数据，退出循环
    buffer[bytes] = '\0';
    outPut.push_back(buffer);
  }
  close(outputPipe[0]);
  return;
}

void Application::appendToFile(const std::string& filename, const std::string& content) {
    std::ofstream file;

    // 以追加模式打开文件
    file.open(filename, std::ios_base::app);

    if (!file) {
        // 文件打开失败，可以在此处进行错误处理
        return;
    }

    file << content;

    file.close();
}
