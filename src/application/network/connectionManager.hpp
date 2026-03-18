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
#include <poll.h>
#include <unistd.h>
#include <vector>

class ConnectionManager
{
	private:
		EpollManager&						 _epollManager;
		std::vector<ClientSocket*>			 _clients;
		ParseAndValidateHttpRequestUseCase	 _parseUseCase;
		CgiOrchestrator*					 _cgiOrchestrator;
		HttpMethodOrchestrator		 		 _methodOrchestrator;

		void queueResponse(int fd, ClientSocket& client, const HttpResponse& response);
		bool readRawRequestOrDisconnect(int fd, ClientSocket& client, std::string& rawRequest);
		bool parseRequestOrRespondBadRequest(int fd, ClientSocket& client, const std::string& rawRequest, HttpRequest& request);
		bool handleCgiOrRespondBadGateway(int fd, ClientSocket& client, const HttpRequest& request);
		void disconnectClient(int fd);

	public:
		ConnectionManager(EpollManager& epollManager, const PollCapacity& maxEvents);
		~ConnectionManager();

		void setCgiOrchestrator(CgiOrchestrator* orch);
		void configureMethodOrchestrator(const ServerBlock& serverConfig);

		void acceptNewClient(ServerSocket& serverSocket);
		void handleClientRead(int fd);
		void handleClientWrite(int fd);

		bool isCgiFd(int fd) const;
		void handleCgiEvent(int fd, unsigned int flags);
		void dispatchCgiResponses();
};

#endif