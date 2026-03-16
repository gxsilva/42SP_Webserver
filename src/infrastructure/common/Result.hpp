#ifndef RESULT_HPP
#define RESULT_HPP

#include <string>

template < typename T >
class Result
{
	private:
		bool		_is_ok;
		T			_value;
		std::string _error;

	public:
		Result(const T& value) : _is_ok(true), _value(value), _error("") {}
		Result(const std::string& error) : _is_ok(false), _error(error) {}

		virtual ~Result() {}

		bool isOk() const { return _is_ok; }
		bool isErr() const { return !_is_ok; }

		T			getValue() const { return _value; }
		std::string getError() const { return _error; }
};

#endif // RESULT_HPP
