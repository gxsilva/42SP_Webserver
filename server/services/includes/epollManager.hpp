#ifndef EPOLLMANAGER_HPP
#define EPOLLMANAGER_HPP

#include <sys/epoll.h>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <stdexcept>
#include "../../domain/includes/epollEvents.hpp"

class	EpollManager
{
	private:
		int							_epollFd;
		MaxEvents					_maxEvents;
		std::vector<epoll_event>	_triggeredEvents;
		int							_readyEventsCount;

	public:
		explicit EpollManager(const MaxEvents& maxEvents);
		~EpollManager();
		
		void	addFd(int fd, short event);
		void	removeFd(int fd);
		int		waitForEvents();
		
		int		getEventFd(int index) const;
};

#endif