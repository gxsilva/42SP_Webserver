#ifndef CGIORCHESTRATOR_HPP
#define CGIORCHESTRATOR_HPP

#include "../../domain/entities/HttpRequest.hpp"
#include "../../domain/entities/HttpResponse.hpp"
#include "../../infrastructure/CGI/CgiProcessExecutor.hpp"
#include "../network/epollManager.hpp"
#include "CgiHandler.hpp"
#include <sys/epoll.h>
#include <utility>
#include <vector>

class CgiOrchestrator
{
	public:
		explicit CgiOrchestrator(EpollManager& epoll);
		~CgiOrchestrator();

		bool startCgi(int clientFd, const HttpRequest& request, const CgiRouteConfig& config);
		bool isCgiFd(int fd) const;
		void handleEvent(int fd, unsigned int flags);
		std::vector<std::pair<int, HttpResponse> > collectFinished();
		void cancelForClient(int clientFd);

	private:
		struct CgiSession
		{
			int					clientFd;
			CgiProcessExecutor* executor;
			CgiHandler*			handler;
		};

		EpollManager&			_epoll;
		std::vector<CgiSession> _sessions;

		CgiSession* findByPipeFd(int fd);
		void		removeSession(size_t index);
		HttpResponse buildErrorResponse(int statusCode, const std::string& body);

		CgiOrchestrator();
		CgiOrchestrator(const CgiOrchestrator&);
		CgiOrchestrator& operator=(const CgiOrchestrator&);
};

#endif
