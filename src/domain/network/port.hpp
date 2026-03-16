#ifndef PORT_HPP
#define PORT_HPP

#include <stdexcept>

class Port
{
	private:
		int _value;

	public:
		Port();
		explicit Port(int value);
		~Port();

		int getValue() const;
};

#endif
