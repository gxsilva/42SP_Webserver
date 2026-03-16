#ifndef VALIDATIONERROR_HPP
#define VALIDATIONERROR_HPP

#include <string>

class ValidationError
{
	public:
		ValidationError(const std::string& message);
		~ValidationError();

		const std::string& getMessage() const;

	private:
		std::string _message;
};

#endif // VALIDATIONERROR_HPP
