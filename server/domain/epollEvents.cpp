#include "includes/epollEvents.hpp"

MaxEvents::MaxEvents(int amount)
: _amount(amount)
{
	if (amount <= 0)
		throw std::invalid_argument("MaxEvents must be greater than zero");
}

int MaxEvents::getAmount() const
{
	return (_amount);
}