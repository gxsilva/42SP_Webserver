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
#    by: @gxsilva & @ailton-bezerra                                                                      

# DEFINES
NAME		= webserver

CXX			= c++
CXXSTD		= -std=c++98
CXXWARN		= -Wall -Wextra -Werror -Wshadow
CXXDEP		= -MMD -MP
CXXFLAGS	= $(CXXSTD) $(CXXWARN) $(CXXDEP)

REQ_TOOLS	= clang-format clang-tidy bear

# DIRECTORIES
SRCS_DIR	= src
OBJ_DIR		= obj

# FILES
SRCS		= main.cpp \
			  utils/Logger.cpp

# EXPANSIONS
SRC			= $(addprefix $(SRCS_DIR)/,$(SRCS))
OBJ			= $(patsubst $(SRCS_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))
DEPS		= $(OBJ:.o=.d)

# TARGET
all: $(NAME)

$(NAME): $(OBJ)
	@echo "⚙️  Linking..."
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

$(OBJ_DIR)/%.o: $(SRCS_DIR)/%.cpp | $(OBJ_DIR)
	@echo "🛠️  Building..."
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@echo "🧹 Removing objects..."
	@rm -f $(OBJ) $(DEPS)

fclean: clean
	@echo "🧹 Removing binary..."
	@rm -f $(NAME)

re: fclean all

# $$ -> to be treat as normal $ in bash
# -n -> not empty
check-tools:
	@echo "🔎 Checking required tools..."
	@missing=""; \
	for t in $(REQ_TOOLS); do \
		if ! command -v $$t >/dev/null 2>&1; then \
			missing="$$missing $$t"; \
		fi; \
	done; \
	if [ -n "$$missing" ]; then \
		echo "❌ Missing required tools:$$missing" >&2; \
		exit 1; \
	else \
		echo "✅ All required tools available."; \
	fi

format: check-tools
	@echo "🔧 Formatting..."
	@hdrs="$$(find . -type f -name '*.hpp' -print)"; \
	files="$(SRC) $$hdrs"; \
	if [ -z "$$files" ]; then \
		echo "Nothing to format."; \
	else \
		clang-format -i $$files; \
		echo "✅ Formatted files."; \
	fi


#gt = greater than | -B force recompile | -s = existe and it size is grater than 0
compile_commands_json:
	@if command -v bear >/dev/null 2>&1; then \
		jobs=$$(nproc 2>/dev/null || echo 1); \
		[ $$jobs -gt 1 ] && jobs=$$((jobs-1)); \
		echo "📋 Rebuilding to generate compile_commands.json (jobs=$$jobs)..."; \
		rm -f compile_commands.json; \
		$(MAKE) clean >/dev/null; \
		bear -- $(MAKE) all -B -j$$jobs; \
		if [ ! -s compile_commands.json ]; then \
			echo "❌ Failed to generate compile_commands.json"; exit 1; \
		else \
			echo "✅ compile_commands.json ready."; \
		fi; \
	else \
		echo "⚠️  Bear not found. Skipping compile database step."; \
	fi

tidy: check-tools compile_commands_json
	@echo "🔍 Running clang-tidy..."
	@clang-tidy -p . $(SRC)

.PHONY: all clean fclean re format check-tools tidy compile_commands_json

-include $(DEPS)
