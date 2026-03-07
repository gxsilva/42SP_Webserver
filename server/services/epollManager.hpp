#ifndef EPOLLMANAGER_HPP
#define EPOLLMANAGER_HPP

#include <sys/epoll.h>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <stdexcept>
#include "../domain/epollEvents.hpp"

class	EpollManager
{
	private:
		int							_epollFd;
		PollCapacity				_maxEvents;
		std::vector<epoll_event>	_triggeredEvents;
		int							_readyEventsCount;

	public:
		explicit EpollManager(const PollCapacity& maxEvents);
		~EpollManager();
		
		void	addFd(int fd, short event);
		void	modifyFd(int fd, unsigned int event);
		void	removeFd(int fd);
		int		waitForEvents();
		
		int				getEventFd(int index) const;
		unsigned int	getEventFlags(int index) const;
};

#endif