#ifndef HTTPSESSION_HPP
#define HTTPSESSION_HPP

#include "../../infra/includes/clientSocket.hpp"

class HttpSession 
{
    public:
        enum	connectionState
	    {
	    	READING,
	    	PROCESSING,
	    	WRITING,
	    	CLOSED
	    };

    private:
        ClientSocket*    _socket;
        std::string      _buffer;
        connectionState  _state;

    public:
        explicit HttpSession(ClientSocket* socket);
        ~HttpSession();

        ClientSocket*       getSocket() const;
        connectionState     getState() const;
        void                setState(connectionState state);
        void                appendToBuffer(const char* data, ssize_t size);
        const std::string&  getBuffer() const;
        bool                isReadComplete() const;
};

#endif