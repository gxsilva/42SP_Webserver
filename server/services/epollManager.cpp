#include "includes/epollManager.hpp"

EpollManager::EpollManager(const MaxEvents& maxEvents)
: _epollFd(epoll_create1(0)),
_maxEvents(maxEvents),
_triggeredEvents(maxEvents.getAmount()),
_readyEventsCount(0)
{
	if (_epollFd < 0)
		throw std::runtime_error("Failed to create epoll instance");
}

EpollManager::~EpollManager() 
{
	if (_epollFd >= 0)
		close(_epollFd);
}

void	EpollManager::addFd(int fd, short event)
{
	struct epoll_event create;
	memset(&create, 0, sizeof(create));
	
	create.events = event;
	create.data.fd = fd;

	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &create) < 0)
		throw std::runtime_error("Failed to add fd to epoll");
}

void	EpollManager::removeFd(int fd)
{
	if (fd < 0)
        return;
	epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL);
}

int	EpollManager::waitForEvents()
{
	_readyEventsCount = epoll_wait(
		_epollFd,
		_triggeredEvents.data(),
		_maxEvents.getAmount(),
		-1
	);
	return (_readyEventsCount);
}

int EpollManager::getEventFd(int index) const
{
	return (_triggeredEvents[index].data.fd);
}
