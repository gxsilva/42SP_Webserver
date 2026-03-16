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
CXXDEBUG	= -g -O0
CXXFLAGS	= $(CXXSTD) $(CXXWARN) $(CXXDEP) $(CXXDEBUG)

REQ_TOOLS	= clang-format clang-tidy bear

# DIRECTORIES
SRCS_DIR			= src
OBJ_DIR				= obj

APPLICATION_DIR			= $(SRCS_DIR)/application
SERVER_NET_DIR			= $(APPLICATION_DIR)/network

DOMAIN_DIR				= $(SRCS_DIR)/domain
D_ENTITIES_DIR			= $(DOMAIN_DIR)/entities
D_AST_DIR				= $(D_ENTITIES_DIR)/ast
D_ERRORS_DIR			= $(DOMAIN_DIR)/errors
D_EVENTS_DIR			= $(DOMAIN_DIR)/events
D_NETWORK_DIR			= $(DOMAIN_DIR)/network
D_SERVICES_DIR			= $(DOMAIN_DIR)/services
D_VALIDATOR_DIR			= $(D_SERVICES_DIR)/validator
D_VALUE_OBJECTS_DIR		= $(DOMAIN_DIR)/value_objects

APP_DIR					= $(SRCS_DIR)/application
APP_USECASES_DIR		= $(APP_DIR)/use_cases

INTERFACES_DIR			= $(SRCS_DIR)/interfaces
CLI_DIR					= $(INTERFACES_DIR)/cli

INFRA_DIR				= $(SRCS_DIR)/infrastructure
I_COMMON_DIR			= $(INFRA_DIR)/common
I_IO_DIR				= $(INFRA_DIR)/io

APPLICATION_DIR			= $(SRCS_DIR)/application
USE_CASES_DIR			= $(APPLICATION_DIR)/use_cases

# ---------------- PROVISÒRIO ----------------
HDRS				= $(shell find . -name "*.hpp")
# -------------------------------

# SOURCES

DOMAIN_SRCS		= $(D_ENTITIES_DIR)/SourceLocation.cpp \
					$(D_ENTITIES_DIR)/Token.cpp \
					$(D_ERRORS_DIR)/CompilerError.cpp \
					$(D_ERRORS_DIR)/ErrorList.cpp \
					$(D_SERVICES_DIR)/Lexer.cpp \
					$(D_SERVICES_DIR)/Parser.cpp \
					$(D_SERVICES_DIR)/validator/CardinalityRuleService.cpp \
					$(D_SERVICES_DIR)/validator/ConflictRuleService.cpp \
					$(D_SERVICES_DIR)/validator/ContextRuleService.cpp \
					$(D_SERVICES_DIR)/validator/DependencyRuleService.cpp \
					$(D_SERVICES_DIR)/validator/ValueRuleService.cpp \
					$(D_SERVICES_DIR)/Validator.cpp \
					$(D_AST_DIR)/base/ASTNode.cpp \
					$(D_AST_DIR)/node/ASTValue.cpp \
					$(D_AST_DIR)/node/ASTDirective.cpp \
					$(D_AST_DIR)/node/ASTBlock.cpp \
					$(D_AST_DIR)/node/ASTRoot.cpp \
					$(D_VALUE_OBJECTS_DIR)/RuleTable.cpp

INTERFACE_SRCS		= $(CLI_DIR)/main.cpp

INFRA_SRCS		= $(I_COMMON_DIR)/TokenResult.cpp \
					$(I_COMMON_DIR)/LexerResult.cpp \
					$(I_COMMON_DIR)/ASTResult.cpp \
					$(I_COMMON_DIR)/ValidatorResult.cpp \
					$(I_IO_DIR)/FileReader.cpp \
					$(I_IO_DIR)/FileValidator.cpp \
					$(I_IO_DIR)/SemanticAnalyzer.cpp \
					$(INFRA_DIR)/logging/Logger.cpp \
					$(I_COMMON_DIR)/RuleRegistry.cpp \
					$(I_COMMON_DIR)/builder/ConfigBuilder.cpp

APPLICATION_SRCS	= $(USE_CASES_DIR)/CompileSourceFile.cpp

# EXPANSIONS
SRC_SET				= $(INTERFACE_SRCS) \
						$(APPLICATION_SRCS) \
						$(DOMAIN_SRCS) \
						$(INFRA_SRCS) \
						$(APP_SRCS)

OBJ					= $(patsubst $(SRCS_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_SET))
DEPS				= $(OBJ:.o=.d)

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
	@rm -f $(TEST_NAME) $(TEST_VALIDATION_NAME) $(TEST_USECASE_NAME)

re: fclean all

test: $(TEST_NAME) $(TEST_VALIDATION_NAME) $(TEST_USECASE_NAME)
	@echo "🧪 Running parser tests..."
	@./$(TEST_NAME)
	@echo ""
	@echo "🧪 Running validation tests..."
	@./$(TEST_VALIDATION_NAME)
	@echo ""
	@echo "🧪 Running use case tests..."
	@./$(TEST_USECASE_NAME)

$(TEST_NAME): $(TEST_SRC) $(D_ENTITIES_DIR)/HttpRequest.cpp $(I_IO_DIR)/HttpRequestParser.cpp | $(OBJ_DIR)
	@echo "🛠️  Building parser test..."
	@mkdir -p $(OBJ_DIR)/test
	$(CXX) $(CXXFLAGS) -o $@ $(TEST_SRC) $(D_ENTITIES_DIR)/HttpRequest.cpp $(I_IO_DIR)/HttpRequestParser.cpp

$(TEST_VALIDATION_NAME): $(TEST_VALIDATION_SRC) $(D_ENTITIES_DIR)/HttpRequest.cpp $(I_IO_DIR)/HttpRequestParser.cpp $(D_SERVICES_DIR)/HttpRequestValidator.cpp $(D_ERRORS_DIR)/ValidationError.cpp | $(OBJ_DIR)
	@echo "🛠️  Building validation test..."
	@mkdir -p $(OBJ_DIR)/test
	$(CXX) $(CXXFLAGS) -o $@ $(TEST_VALIDATION_SRC) $(D_ENTITIES_DIR)/HttpRequest.cpp $(I_IO_DIR)/HttpRequestParser.cpp $(D_SERVICES_DIR)/HttpRequestValidator.cpp $(D_ERRORS_DIR)/ValidationError.cpp

$(TEST_USECASE_NAME): $(TEST_USECASE_SRC) $(D_ENTITIES_DIR)/HttpRequest.cpp $(I_IO_DIR)/HttpRequestParser.cpp $(D_SERVICES_DIR)/HttpRequestValidator.cpp $(D_ERRORS_DIR)/ValidationError.cpp $(APP_USECASES_DIR)/ParseAndValidateHttpRequestUseCase.cpp | $(OBJ_DIR)
	@echo "🛠️  Building use case test..."
	@mkdir -p $(OBJ_DIR)/test
	$(CXX) $(CXXFLAGS) -o $@ $(TEST_USECASE_SRC) $(D_ENTITIES_DIR)/HttpRequest.cpp $(I_IO_DIR)/HttpRequestParser.cpp $(D_SERVICES_DIR)/HttpRequestValidator.cpp $(D_ERRORS_DIR)/ValidationError.cpp $(APP_USECASES_DIR)/ParseAndValidateHttpRequestUseCase.cpp

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
	@clang-format -i $(SRC_SET) $(HDRS)


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
	@clang-tidy -p . $(SRC_SET)

clean_logs:
	@echo "🧹 Removing log files..."
	@rm -f log/log_*
	@rm -f log_*

.PHONY: all clean fclean re format check-tools tidy compile_commands_json clean_logs test

-include $(DEPS)
