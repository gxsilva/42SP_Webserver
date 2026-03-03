#include "includes/epollEvents.hpp"

PollCapacity::PollCapacity(int amount)
: _amount(amount)
{
	if (amount <= 0)
		throw std::invalid_argument("PollCapacity must be greater than zero");
}

int PollCapacity::getAmount() const
{
	return (_amount);
}