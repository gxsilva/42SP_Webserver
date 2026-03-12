#ifndef CGI_PROCESS_EXECUTOR_HPP
#define CGI_PROCESS_EXECUTOR_HPP

#include "../../interfaces/port/InitCgiGateway.hpp"
#include <string>
#include <sys/types.h>
#include <ctime>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>

class CgiProcessExecutor : public InitCgiGateway
{
    public:
        explicit CgiProcessExecutor(std::size_t timeoutSeconds);
        ~CgiProcessExecutor();

        bool start(const std::string& scriptPath,
                   const std::string& interpreterPath,
                   const CgiEnvironment& env,
                   const std::string& requestBody);

        int getReadFd() const;
        int getWriteFd() const;

        bool onWriteReady();
        bool onReadReady();

        CgiProcessState checkState();
        CgiResponse     getResponse();
        void            cleanup();

    private:
        CgiProcessExecutor();
        CgiProcessExecutor(const CgiProcessExecutor&);
        CgiProcessExecutor& operator=(const CgiProcessExecutor&);

        void closeFdIfOpen(int& fd);
        void killChildIfAlive();

        pid_t       _childPid;
        int         _pipeToChild[2];
        int         _pipeFromChild[2];
        std::string _requestBody;
        std::size_t _bodyBytesSent;
        std::string _outputBuffer;
        std::time_t _startTime;
        std::size_t _timeoutSeconds;
        bool        _finished;
};

#endif