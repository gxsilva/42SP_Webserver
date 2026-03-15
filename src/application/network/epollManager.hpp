#ifndef EPOLLMANAGER_HPP
#define EPOLLMANAGER_HPP

#include "../../domain/events/epollEvents.hpp"
#include <cstring>
#include <stdexcept>
#include <sys/epoll.h>
#include <unistd.h>
#include <vector>

class EpollManager
{
	private:
		int						 _epollFd;
		PollCapacity			 _maxEvents;
		std::vector<epoll_event> _triggeredEvents;
		int						 _readyEventsCount;

	public:
		explicit EpollManager(const PollCapacity& maxEvents);
		~EpollManager();

		void addFd(int fd, short event);
		void modifyFd(int fd, unsigned int event);
		void removeFd(int fd);
		int	 waitForEvents();
		int	 waitForEvents(int timeoutMs);

		int			 getEventFd(int index) const;
		unsigned int getEventFlags(int index) const;
};

#endif
