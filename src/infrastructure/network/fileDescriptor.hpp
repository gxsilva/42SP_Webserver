#ifndef FILE_DESCRIPTOR_HPP
#define FILE_DESCRIPTOR_HPP

#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>

class FileDescriptor
{
    private:
        int _fd;

        FileDescriptor(const FileDescriptor&);
        FileDescriptor& operator=(const FileDescriptor&);

    public:
        FileDescriptor();
        explicit FileDescriptor(int fd);
		~FileDescriptor();

        bool    isValid() const;
        void    invalidate();

        bool    setNonBlocking();

        int     get() const;

        bool    operator==(const FileDescriptor& other) const;
        bool    operator<(const FileDescriptor& other) const;
        
};

#endif
