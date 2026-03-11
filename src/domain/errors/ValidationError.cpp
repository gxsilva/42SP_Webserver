#include "ValidationError.hpp"

ValidationError::ValidationError(const std::string &message)
    : _message(message) {}

ValidationError::~ValidationError() {}

const std::string &ValidationError::getMessage() const {
    return _message;
}
