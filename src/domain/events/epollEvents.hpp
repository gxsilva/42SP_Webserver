#ifndef EPOLLEVENTS_HPP
#define EPOLLEVENTS_HPP

#include <stdexcept>

class PollCapacity
{
	private:
		int _amount;

	public:
		explicit PollCapacity(int amount);

		int getAmount() const;
};

#endif
