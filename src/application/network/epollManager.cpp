#include "epollManager.hpp"

EpollManager::EpollManager(const PollCapacity& maxEvents)
	: _epollFd(epoll_create1(0)), _maxEvents(maxEvents), _triggeredEvents(maxEvents.getAmount()), _readyEventsCount(0)
{
	if (_epollFd < 0)
		throw std::runtime_error("Failed to create epoll instance");
}

EpollManager::~EpollManager()
{
	if (_epollFd >= 0)
		close(_epollFd);
}

void EpollManager::addFd(int fd, short event)
{
	struct epoll_event create;
	memset(&create, 0, sizeof(create));

	create.events  = event;
	create.data.fd = fd;

	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &create) < 0)
		throw std::runtime_error("Failed to add fd to epoll");
}

void EpollManager::modifyFd(int fd, unsigned int event)
{
	struct epoll_event create;
	memset(&create, 0, sizeof(create));

	create.events  = event;
	create.data.fd = fd;

	if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, fd, &create) < 0)
		throw std::runtime_error("Failed to modify fd in epoll");
}

void EpollManager::removeFd(int fd)
{
	if (fd < 0)
		return;
	epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL);
}

int EpollManager::waitForEvents()
{
	_readyEventsCount = epoll_wait(_epollFd, _triggeredEvents.data(), _maxEvents.getAmount(), -1);
	return (_readyEventsCount);
}

int EpollManager::getEventFd(int index) const
{
	if (index < 0)
		throw std::out_of_range("EpollManager::getEventFd: Negative index!");
	if (index >= _readyEventsCount)
		throw std::out_of_range("EpollManager::getEventFd: index exceeds ready events!");
	if (static_cast< size_t >(index) >= _triggeredEvents.size())
		throw std::out_of_range("EpollManager::getEventFd: index exceeds triggered events buffer");
	return (_triggeredEvents[index].data.fd);
}

unsigned int EpollManager::getEventFlags(int index) const { return (_triggeredEvents[index].events); }
