#include "port.hpp"

Port::Port() : _value(8080) {}

Port::Port(int v)
{
	if (v < 1 || v > 65535)
		throw std::invalid_argument("Port must be between 1 and 65535");
	_value = v;
}

Port::~Port() {}

int Port::getValue() const { return (_value); }
