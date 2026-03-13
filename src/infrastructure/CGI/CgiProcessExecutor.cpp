#include "CgiProcessExecutor.hpp"

static const std::size_t READ_BUFFER_SIZE = 4096;

CgiProcessExecutor::CgiProcessExecutor(std::size_t timeoutSeconds)
	: _childPid(-1), _bodyBytesSent(0), _startTime(0), _timeoutSeconds(timeoutSeconds),
	  _finished(false)
{
	_pipeToChild[0]	  = -1;
	_pipeToChild[1]	  = -1;
	_pipeFromChild[0] = -1;
	_pipeFromChild[1] = -1;
}

CgiProcessExecutor::~CgiProcessExecutor() { cleanup(); }

bool CgiProcessExecutor::start(const std::string& scriptPath, const std::string& interpreterPath,
							   const CgiEnvironment& env, const std::string& requestBody)
{
	_requestBody   = requestBody;
	_bodyBytesSent = 0;
	_outputBuffer.clear();
	_finished = false;

	if (pipe(_pipeToChild) == -1)
		return (false);
	if (pipe(_pipeFromChild) == -1)
	{
		close(_pipeToChild[0]);
		_pipeToChild[0] = -1;
		close(_pipeToChild[1]);
		_pipeToChild[1] = -1;
		return (false);
	}

	int flagsToChild = fcntl(_pipeToChild[1], F_GETFL, 0);
	if (flagsToChild != -1)
		fcntl(_pipeToChild[1], F_SETFL, flagsToChild | O_NONBLOCK);

	int flagsFromChild = fcntl(_pipeFromChild[0], F_GETFL, 0);
	if (flagsFromChild != -1)
		fcntl(_pipeFromChild[0], F_SETFL, flagsFromChild | O_NONBLOCK);
	char** envp = env.toEnvArray();

	_childPid = fork();
	if (_childPid == -1)
	{
		CgiEnvironment::freeEnvArray(envp);
		cleanup();
		return false;
	}

	if (_childPid == 0)
	{
		// === CHILD PROCESS ===
		close(_pipeToChild[1]);
		close(_pipeFromChild[0]);

		dup2(_pipeToChild[0], STDIN_FILENO);
		dup2(_pipeFromChild[1], STDOUT_FILENO);

		close(_pipeToChild[0]);
		close(_pipeFromChild[1]);

		char* argv[3];
		argv[0] = const_cast<char*>(interpreterPath.c_str());
		argv[1] = const_cast<char*>(scriptPath.c_str());
		argv[2] = NULL;

		execve(interpreterPath.c_str(), argv, envp);

		CgiEnvironment::freeEnvArray(envp);
		_exit(1);
	}

	// === PARENT PROCESS ===
	CgiEnvironment::freeEnvArray(envp);
	close(_pipeToChild[0]);
	_pipeToChild[0] = -1;
	close(_pipeFromChild[1]);
	_pipeFromChild[1] = -1;

	_startTime = std::time(NULL);

	if (_requestBody.empty())
		closeFdIfOpen(_pipeToChild[1]);

	return (true);
}

int CgiProcessExecutor::getReadFd() const { return (_pipeFromChild[0]); }

int CgiProcessExecutor::getWriteFd() const { return (_pipeToChild[1]); }

bool CgiProcessExecutor::onWriteReady()
{
	if (_pipeToChild[1] == -1)
		return (true);

	std::size_t remaining = _requestBody.size() - _bodyBytesSent;
	if (remaining == 0)
	{
		closeFdIfOpen(_pipeToChild[1]);
		return (true);
	}

	ssize_t written = write(_pipeToChild[1], _requestBody.c_str() + _bodyBytesSent, remaining);
	if (written > 0)
	{
		_bodyBytesSent += static_cast<std::size_t>(written);
		if (_bodyBytesSent >= _requestBody.size())
			closeFdIfOpen(_pipeToChild[1]);
		return (true);
	}
	if (written == 0)
		return (true);
	if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
        return (true);
	closeFdIfOpen(_pipeToChild[1]);
	_finished = true;
	return (false);
}

bool CgiProcessExecutor::onReadReady()
{
	if (_pipeFromChild[0] == -1)
		return (false);

	char	buf[READ_BUFFER_SIZE];
	ssize_t bytesRead = read(_pipeFromChild[0], buf, sizeof(buf));

	if (bytesRead > 0)
	{
		_outputBuffer.append(buf, static_cast<std::size_t>(bytesRead));
		return (true);
	}
	if (bytesRead == 0)
	{
		_finished = true;
		closeFdIfOpen(_pipeFromChild[0]);
		return (false);
	}
	_finished = true;
	closeFdIfOpen(_pipeFromChild[0]);
	return (false);
}

CgiProcessState CgiProcessExecutor::checkState()
{
	if (_finished)
		return (CGI_FINISHED);

	std::time_t elapsed = std::time(NULL) - _startTime;
	if (static_cast<std::size_t>(elapsed) >= _timeoutSeconds)
	{
		killChildIfAlive();
		cleanup();
		return (CGI_TIMEOUT);
	}

	if (_childPid > 0)
	{
		int	  status = 0;
		pid_t result = waitpid(_childPid, &status, WNOHANG);
		if (result == _childPid)
		{
			_childPid = -1;
			if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
			{
				cleanup();
				return (CGI_ERROR);
			}
		}
	}

	return (CGI_RUNNING);
}

CgiResponse CgiProcessExecutor::getResponse()
{
	CgiResponse response;
	bool		parseOk = response.parse(_outputBuffer);
	if (!parseOk)
		response.setStatusCode(500);
	return (response);
}

void CgiProcessExecutor::cleanup()
{
	killChildIfAlive();
	closeFdIfOpen(_pipeToChild[0]);
	closeFdIfOpen(_pipeToChild[1]);
	closeFdIfOpen(_pipeFromChild[0]);
	closeFdIfOpen(_pipeFromChild[1]);
}

void CgiProcessExecutor::closeFdIfOpen(int& fd)
{
	if (fd != -1)
	{
		close(fd);
		fd = -1;
	}
}

void CgiProcessExecutor::killChildIfAlive()
{
	if (_childPid <= 0)
		return;

	pid_t pid = _childPid;
	if (kill(pid, SIGKILL) == -1)
	{
		if (errno == ESRCH)
			_childPid = -1;
		return;
	}

	int	  status = 0;
	pid_t result = -1;
	do
	{
		result = waitpid(pid, &status, WNOHANG);
	} 
	while (result == -1 && errno == EINTR);

	if (result == pid || (result == -1 && errno == ECHILD))
		_childPid = -1;
}
