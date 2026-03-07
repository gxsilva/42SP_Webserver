NAME = server

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g

# Diretório para os arquivos objetos
OBJECT_DIRECTORY = obj

SRCS = main.cpp \
       services/server.cpp \
       services/serverHandlers.cpp \
       services/epollManager.cpp \
       services/connectionManager.cpp \
       infra/serverSocket.cpp \
       infra/clientSocket.cpp \
       infra/fileDescriptor.cpp \
       infra/testHttpResponse.cpp

# Mapeia os fontes para a pasta de objetos, preservando a estrutura de subpastas
OBJS = $(addprefix $(OBJECT_DIRECTORY)/, $(SRCS:.cpp=.o))

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

# Regra para compilar os .cpp em .o dentro da pasta OBJECT_DIRECTORY
$(OBJECT_DIRECTORY)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJECT_DIRECTORY)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re