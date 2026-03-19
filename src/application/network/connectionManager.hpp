#ifndef CONNECTIONMANAGER_HPP
#define CONNECTIONMANAGER_HPP

#include "../../domain/entities/HttpRequest.hpp"
#include "../../domain/entities/server/ServerBlock.hpp"
#include "../../domain/events/epollEvents.hpp"
#include "../../infrastructure/network/clientSocket.hpp"
#include "../../infrastructure/network/serverSocket.hpp"
#include "../../infrastructure/network/testHttpResponse.hpp"
#include "../CGI/CgiOrchestrator.hpp"
#include "../methods/HttpMethodOrchestrator.hpp"
#include "../use_cases/ParseAndValidateHttpRequestUseCase.hpp"
#include "epollManager.hpp"

#include <iostream>
#include <map>
#include <poll.h>
#include <unistd.h>
#include <vector>

class ConnectionManager
{
	private:
		EpollManager&					   _epollManager;
		std::vector< ClientSocket* >	   _clients;
		ParseAndValidateHttpRequestUseCase _parseUseCase;
		CgiOrchestrator*				   _cgiOrchestrator;
		HttpMethodOrchestrator			   _methodOrchestrator;
		std::map< int, ServerBlock >	   _listenerServerConfigs;
		std::map< int, ServerBlock >	   _clientServerConfigs;
		std::map< int, std::string >	   _requestReadBuffers;
		ServerBlock						   _defaultServerConfig;
		bool							   _hasDefaultServerConfig;

		void			   queueResponse(int fd, ClientSocket& client, const HttpResponse& response);
		bool			   readRawRequestOrDisconnect(int fd, ClientSocket& client, std::string& rawRequest);
		bool			   parseRequestOrRespondBadRequest(int fd, ClientSocket& client, const std::string& rawRequest,
														   HttpRequest& request);
		bool			   handleCgiOrRespondBadGateway(int fd, ClientSocket& client, const HttpRequest& request,
														const ServerBlock& serverConfig);
		const ServerBlock* findClientServerConfig(int clientFd) const;
		const ServerBlock* findListenerServerConfig(int listenerFd) const;
		const ServerBlock& resolveServerConfigForClient(int clientFd) const;
		size_t			   resolveMaxBodySizeForClient(int clientFd) const;
		bool			   popCompleteRequestFromBuffer(int clientFd, std::string& rawRequest);
		void			   disconnectClient(int fd);

	public:
		ConnectionManager(EpollManager& epollManager, const PollCapacity& maxEvents);
		~ConnectionManager();

		void setCgiOrchestrator(CgiOrchestrator* orch);
		void configureMethodOrchestrator(const ServerBlock& serverConfig);
		void registerListenerConfig(int listenerFd, const ServerBlock& serverConfig);

		void acceptNewClient(ServerSocket& serverSocket);
		void handleClientRead(int fd);
		void handleClientWrite(int fd);

		bool isCgiFd(int fd) const;
		void handleCgiEvent(int fd, unsigned int flags);
		void dispatchCgiResponses();
};

#endif
