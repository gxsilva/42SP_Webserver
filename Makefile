#                          /$$                                                                          
#                         | $$                                                                          
#  /$$  /$$  /$$  /$$$$$$ | $$$$$$$         /$$$$$$$  /$$$$$$   /$$$$$$  /$$    /$$ /$$$$$$   /$$$$$$   
# | $$ | $$ | $$ /$$__  $$| $$__  $$       /$$_____/ /$$__  $$ /$$__  $$|  $$  /$$//$$__  $$ /$$__  $$  
# | $$ | $$ | $$| $$$$$$$$| $$  \ $$      |  $$$$$$ | $$$$$$$$| $$  \__/ \  $$/$$/| $$$$$$$$| $$  \__/  
# | $$ | $$ | $$| $$_____/| $$  | $$       \____  $$| $$_____/| $$        \  $$$/ | $$_____/| $$        
# |  $$$$$/$$$$/|  $$$$$$$| $$$$$$$/       /$$$$$$$/|  $$$$$$$| $$         \  $/  |  $$$$$$$| $$        
#  \_____/\___/  \_______/|_______//$$$$$$|_______/  \_______/|__/          \_/    \_______/|__/        
#                                 |______/                                                              
#                                                                                                       
#                                                                                                       

CC = c++

CPP_VERSION = -std=c++98
CPP_FLAGS = -Wall -Wextra -Werror -Wshadow $(CPP_VERSION)

SRC =	src/main.cpp

OBJDIR = obj
OBJ = $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(SRC))

NAME = webserver

all: $(NAME)

$(NAME): $(OBJ)
	@echo "⚙️ Compiling..."
	$(CC) $(CPP_FLAGS) -o $(NAME) $(OBJ)

$(OBJDIR)/%.o: src/%.cpp | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CC) $(CPP_FLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

# ---------------- Tidy target using clang-tidy and bear ----------------
tidy:
	@echo "🧹 Tidying up..."
	@$(MAKE) fclean
	@bear -- make
	@run-clang-tidy -p=. -j$$(nproc) -quiet src/

# ---------------- Format target using clang-format ----------------
format:
	@echo "🔧 Formatting code..."
	@clang-format -i $(SRC)

# ---------------- Generic rule for compiling .cpp files to .o files ----------------
%.o: %.cpp
	$(CC) $(CPP_FLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re format