#include "CgiOrchestrator.hpp"

#include <iostream>

static const std::size_t CGI_TIMEOUT_SECONDS = 30;

CgiOrchestrator::CgiOrchestrator(EpollManager& epoll) : _epoll(epoll) {}

CgiOrchestrator::~CgiOrchestrator()
{
	for (size_t i = 0; i < _sessions.size(); ++i)
	{
		_sessions[i].executor->cleanup();
		delete _sessions[i].handler;
		delete _sessions[i].executor;
	}
	_sessions.clear();
}

bool CgiOrchestrator::startCgi(int clientFd, const HttpRequest& request, const CgiRouteConfig& config)
{
	CgiProcessExecutor* executor = new CgiProcessExecutor(CGI_TIMEOUT_SECONDS);
	CgiHandler*			handler	 = new CgiHandler(*executor, config);

	if (!handler->startCgi(request))
	{
		delete handler;
		delete executor;
		return (false);
	}

	int readFd = executor->getReadFd();
	if (readFd >= 0)
		_epoll.addFd(readFd, EPOLLIN);

	int writeFd = executor->getWriteFd();
	if (writeFd >= 0)
		_epoll.addFd(writeFd, EPOLLOUT);

	CgiSession session;
	session.clientFd = clientFd;
	session.executor = executor;
	session.handler	 = handler;
	_sessions.push_back(session);

	std::cout << "[CGI] Started for client fd " << clientFd << " | readFd=" << readFd << " writeFd=" << writeFd
			  << std::endl;
	return (true);
}

bool CgiOrchestrator::isCgiFd(int fd) const
{
	for (size_t i = 0; i < _sessions.size(); ++i)
	{
		if (_sessions[i].executor->getReadFd() == fd)
			return (true);
		if (_sessions[i].executor->getWriteFd() == fd)
			return (true);
	}
	return (false);
}

CgiOrchestrator::CgiSession* CgiOrchestrator::findByPipeFd(int fd)
{
	for (size_t i = 0; i < _sessions.size(); ++i)
	{
		if (_sessions[i].executor->getReadFd() == fd)
			return (&_sessions[i]);
		if (_sessions[i].executor->getWriteFd() == fd)
			return (&_sessions[i]);
	}
	return (NULL);
}

void CgiOrchestrator::handleEvent(int fd, unsigned int flags)
{
	CgiSession* session = findByPipeFd(fd);
	if (session == NULL)
		return;

	if ((flags & EPOLLOUT) && session->executor->getWriteFd() == fd)
	{
		if (!session->executor->onWriteReady())
		{
			_epoll.removeFd(fd);
			return;
		}
		if (session->executor->getWriteFd() < 0)
			_epoll.removeFd(fd);
	}

	if ((flags & EPOLLIN) && session->executor->getReadFd() == fd)
	{
		if (!session->executor->onReadReady())
			_epoll.removeFd(fd);
	}
}

std::vector< std::pair< int, HttpResponse > > CgiOrchestrator::collectFinished()
{
	std::vector< std::pair< int, HttpResponse > > results;

	for (size_t i = 0; i < _sessions.size();)
	{
		CgiProcessState state = _sessions[i].executor->checkState();

		if (state == CGI_RUNNING)
		{
			++i;
			continue;
		}

		int			 clientFd = _sessions[i].clientFd;
		HttpResponse response;

		if (state == CGI_FINISHED)
		{
			response = _sessions[i].handler->buildResponse();
			std::cout << "[CGI] Finished for client fd " << clientFd << std::endl;
		}
		else if (state == CGI_TIMEOUT)
		{
			response = buildErrorResponse(504, "CGI script timed out");
			std::cerr << "[CGI] Timeout for client fd " << clientFd << std::endl;
		}
		else
		{
			response = buildErrorResponse(502, "CGI script error");
			std::cerr << "[CGI] Error for client fd " << clientFd << std::endl;
		}

		results.push_back(std::make_pair(clientFd, response));
		removeSession(i);
	}

	return (results);
}

void CgiOrchestrator::cancelForClient(int clientFd)
{
	for (size_t i = 0; i < _sessions.size(); ++i)
	{
		if (_sessions[i].clientFd == clientFd)
		{
			removeSession(i);
			return;
		}
	}
}

void CgiOrchestrator::removeSession(size_t index)
{
	int readFd	= _sessions[index].executor->getReadFd();
	int writeFd = _sessions[index].executor->getWriteFd();

	if (readFd >= 0)
		_epoll.removeFd(readFd);
	if (writeFd >= 0)
		_epoll.removeFd(writeFd);

	_sessions[index].executor->cleanup();
	delete _sessions[index].handler;
	delete _sessions[index].executor;

	_sessions.erase(_sessions.begin() + static_cast< long >(index));
}

HttpResponse CgiOrchestrator::buildErrorResponse(int statusCode, const std::string& body)
{
	HttpResponse response;
	response.setStatusCode(statusCode);
	response.setHeader("Content-Type", "text/html");
	response.setHeader("Connection", "close");
	response.setBody("<html><body><h1>" + body + "</h1></body></html>");
	return (response);
}
