#include "fileDescriptor.hpp"

FileDescriptor::FileDescriptor() 
: _fd(-1)
{}

FileDescriptor::FileDescriptor(int fd) 
: _fd(fd)
{
    if (fd < 0)
        throw std::invalid_argument("File descriptor can't be negative!");
}

FileDescriptor::~FileDescriptor()
{
    invalidate();
}

bool FileDescriptor::isValid() const
{
    return (_fd >= 0);
}

void FileDescriptor::invalidate()
{
    if (_fd != -1)
    {
        close(_fd);
        _fd = -1;
    }
}

bool FileDescriptor::setNonBlocking()
{
    if (!isValid())
        return (false);

    int flag = fcntl(_fd, F_GETFL, 0);
    if (flag == -1)
        return (false);

    if (fcntl(_fd, F_SETFL, flag | O_NONBLOCK) == -1)
        return (false);

    return (true);
}

int  FileDescriptor::get() const
{
    return (_fd);
}

bool FileDescriptor::operator==(const FileDescriptor& other) const
{
    return (_fd == other._fd);
}

bool FileDescriptor::operator<(const FileDescriptor& other) const
{
    return (_fd < other._fd);
}
