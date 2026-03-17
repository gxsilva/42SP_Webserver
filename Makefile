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

DOMAIN_DIR				= $(SRCS_DIR)/domain
D_ENTITIES_DIR			= $(DOMAIN_DIR)/entities
D_AST_DIR				= $(D_ENTITIES_DIR)/ast
D_ENTITIES_COMMON_DIR	= $(D_ENTITIES_DIR)/common
D_ENTITIES_CONFIG_DIR	= $(D_ENTITIES_DIR)/config
D_ERRORS_DIR			= $(DOMAIN_DIR)/errors
D_ERRORS_COMMON_DIR		= $(D_ERRORS_DIR)/common
D_ERRORS_VALIDATOR_DIR	= $(D_ERRORS_DIR)/validator
D_EVENTS_DIR			= $(DOMAIN_DIR)/events
D_NETWORK_DIR			= $(DOMAIN_DIR)/network
D_SERVICES_DIR			= $(DOMAIN_DIR)/services
D_CONFIG_SERVICES_DIR	= $(D_SERVICES_DIR)/config
D_REQUEST_SERVICES_DIR	= $(D_SERVICES_DIR)/request
D_VALIDATOR_DIR			= $(D_SERVICES_DIR)/validator
D_VALUE_OBJECTS_DIR		= $(DOMAIN_DIR)/value_objects
D_VALUE_CONFIG_DIR		= $(D_VALUE_OBJECTS_DIR)/config
D_CGI_DIR				= $(DOMAIN_DIR)/CGI

APP_DIR					= $(SRCS_DIR)/application
A_CGI_DIR				= $(APP_DIR)/CGI
A_SERVER_NET_DIR		= $(APP_DIR)/network
APP_USECASES_DIR		= $(APP_DIR)/use_cases

INTERFACES_DIR			= $(SRCS_DIR)/interfaces
CLI_DIR					= $(INTERFACES_DIR)/cli

INFRA_DIR				= $(SRCS_DIR)/infrastructure
I_COMMON_DIR			= $(INFRA_DIR)/common
I_IO_DIR				= $(INFRA_DIR)/io
I_COMMON_CONFIG_DIR		= $(I_COMMON_DIR)/config
I_IO_CONFIG_DIR			= $(I_IO_DIR)/config
I_IO_REQUEST_DIR		= $(I_IO_DIR)/request
I_NETWORK_DIR			= $(INFRA_DIR)/network

APPLICATION_DIR			= $(SRCS_DIR)/application
USE_CASES_DIR			= $(APPLICATION_DIR)/use_cases

# ---------------- PROVISÒRIO ----------------
HDRS				= $(shell find . -name "*.hpp")
# -------------------------------

# SOURCES

INTERFACE_SRCS		= $(CLI_DIR)/main.cpp

APPLICATION_SRCS	= $(A_SERVER_NET_DIR)/connectionManager.cpp \
						$(A_SERVER_NET_DIR)/epollManager.cpp \
						$(A_SERVER_NET_DIR)/server.cpp \
						$(A_SERVER_NET_DIR)/serverHandlers.cpp \
						$(A_CGI_DIR)/CgiHandler.cpp \
						$(A_CGI_DIR)/CgiOrchestrator.cpp \
						$(APP_USECASES_DIR)/CompileSourceFile.cpp \
						$(APP_USECASES_DIR)/BuildServerConfig.cpp \
						$(APP_USECASES_DIR)/ParseAndValidateHttpRequestUseCase.cpp

DOMAIN_SRCS			= $(D_ENTITIES_COMMON_DIR)/SourceLocation.cpp \
						$(D_ENTITIES_CONFIG_DIR)/Token.cpp \
						$(D_ENTITIES_DIR)/HttpRequest.cpp \
						$(D_ENTITIES_DIR)/HttpResponse.cpp \
						$(D_ERRORS_COMMON_DIR)/CompilerError.cpp \
						$(D_ERRORS_COMMON_DIR)/ErrorList.cpp \
						$(D_ERRORS_VALIDATOR_DIR)/ValidationError.cpp \
						$(D_EVENTS_DIR)/epollEvents.cpp \
						$(D_NETWORK_DIR)/ipAddr.cpp \
						$(D_NETWORK_DIR)/port.cpp \
						$(D_CONFIG_SERVICES_DIR)/ConfigBuilder.cpp \
						$(D_CONFIG_SERVICES_DIR)/Lexer.cpp \
						$(D_CONFIG_SERVICES_DIR)/Parser.cpp \
						$(D_CONFIG_SERVICES_DIR)/Validator.cpp \
						$(D_REQUEST_SERVICES_DIR)/HttpRequestValidator.cpp \
						$(D_VALIDATOR_DIR)/CardinalityRuleService.cpp \
						$(D_VALIDATOR_DIR)/ConflictRuleService.cpp \
						$(D_VALIDATOR_DIR)/ContextRuleService.cpp \
						$(D_VALIDATOR_DIR)/DependencyRuleService.cpp \
						$(D_VALIDATOR_DIR)/ValueRuleService.cpp \
						$(D_SERVICES_DIR)/statusCodeResponse.cpp \
						$(D_AST_DIR)/base/ASTNode.cpp \
						$(D_AST_DIR)/node/ASTValue.cpp \
						$(D_AST_DIR)/node/ASTDirective.cpp \
						$(D_AST_DIR)/node/ASTBlock.cpp \
						$(D_AST_DIR)/node/ASTRoot.cpp \
						$(D_VALUE_CONFIG_DIR)/RuleTable.cpp \
						$(D_CGI_DIR)/CgiEnvironment.cpp \
						$(D_CGI_DIR)/CgiResponse.cpp

INFRA_SRCS			= $(I_COMMON_CONFIG_DIR)/TokenResult.cpp \
						$(I_COMMON_CONFIG_DIR)/LexerResult.cpp \
						$(I_COMMON_CONFIG_DIR)/ASTResult.cpp \
						$(I_COMMON_CONFIG_DIR)/ConfigResult.cpp \
						$(I_COMMON_CONFIG_DIR)/ValidatorResult.cpp \
						$(I_COMMON_CONFIG_DIR)/RuleRegistry.cpp \
						$(I_IO_CONFIG_DIR)/FileReader.cpp \
						$(I_IO_CONFIG_DIR)/FileValidator.cpp \
						$(I_IO_CONFIG_DIR)/SemanticAnalyzer.cpp \
						$(I_IO_REQUEST_DIR)/HttpRequestParser.cpp \
						$(INFRA_DIR)/logging/Logger.cpp \
						$(INFRA_DIR)/CGI/CgiProcessExecutor.cpp \
						$(I_NETWORK_DIR)/clientSocket.cpp \
						$(I_NETWORK_DIR)/serverSocket.cpp \
						$(I_NETWORK_DIR)/fileDescriptor.cpp \
						$(I_NETWORK_DIR)/testHttpResponse.cpp

# TEST DEFINITIONS
TEST_NAME		= test_http_request
TEST_SRC		= test/HttpRequestTest.cpp
TEST_VALIDATION_NAME	= test_http_validation
TEST_VALIDATION_SRC	= test/HttpRequestValidationTest.cpp
TEST_USECASE_NAME	= test_parse_validate_usecase
TEST_USECASE_SRC	= test/ParseAndValidateHttpRequestUseCaseTest.cpp
TEST_OBJS		= $(OBJ_DIR)/test/HttpRequest.o \
					$(OBJ_DIR)/test/HttpRequestParser.o

TEST_DIR					= test

TEST_HTTP_REQUEST_BIN			= test_http_request
TEST_HTTP_REQUEST_MAIN			= $(TEST_DIR)/HttpRequestTest.cpp
TEST_HTTP_REQUEST_SRCS			= $(TEST_HTTP_REQUEST_MAIN) \
						  $(D_ENTITIES_DIR)/HttpRequest.cpp \
						  $(I_IO_REQUEST_DIR)/HttpRequestParser.cpp

TEST_HTTP_VALIDATION_BIN		= test_http_validation
TEST_HTTP_VALIDATION_MAIN		= $(TEST_DIR)/HttpRequestValidationTest.cpp
TEST_HTTP_VALIDATION_SRCS		= $(TEST_HTTP_VALIDATION_MAIN) \
						  $(D_ENTITIES_DIR)/HttpRequest.cpp \
						  $(I_IO_REQUEST_DIR)/HttpRequestParser.cpp \
						  $(D_REQUEST_SERVICES_DIR)/HttpRequestValidator.cpp \
						  $(D_ERRORS_VALIDATOR_DIR)/ValidationError.cpp

TEST_PARSE_VALIDATE_USECASE_BIN		= test_parse_validate_usecase
TEST_PARSE_VALIDATE_USECASE_MAIN		= $(TEST_DIR)/ParseAndValidateHttpRequestUseCaseTest.cpp
TEST_PARSE_VALIDATE_USECASE_SRCS		= $(TEST_PARSE_VALIDATE_USECASE_MAIN) \
											$(D_ENTITIES_DIR)/HttpRequest.cpp \
											$(I_IO_REQUEST_DIR)/HttpRequestParser.cpp \
											$(D_REQUEST_SERVICES_DIR)/HttpRequestValidator.cpp \
											$(D_ERRORS_VALIDATOR_DIR)/ValidationError.cpp \
											$(APP_USECASES_DIR)/ParseAndValidateHttpRequestUseCase.cpp

TEST_BINS					= $(TEST_HTTP_REQUEST_BIN) \
						  $(TEST_HTTP_VALIDATION_BIN) \
						  $(TEST_PARSE_VALIDATE_USECASE_BIN)

TEST_FILES					= $(TEST_HTTP_REQUEST_MAIN) \
						  $(TEST_HTTP_VALIDATION_MAIN) \
						  $(TEST_PARSE_VALIDATE_USECASE_MAIN)

TEST_OBJ_DIR					= $(OBJ_DIR)/test
TEST_ALL_SRCS					= $(sort $(TEST_HTTP_REQUEST_SRCS) \
						  $(TEST_HTTP_VALIDATION_SRCS) \
						  $(TEST_PARSE_VALIDATE_USECASE_SRCS))

TEST_HTTP_REQUEST_OBJ			= $(patsubst %.cpp,$(TEST_OBJ_DIR)/%.o,$(TEST_HTTP_REQUEST_SRCS))
TEST_HTTP_VALIDATION_OBJ		= $(patsubst %.cpp,$(TEST_OBJ_DIR)/%.o,$(TEST_HTTP_VALIDATION_SRCS))
TEST_PARSE_VALIDATE_USECASE_OBJ		= $(patsubst %.cpp,$(TEST_OBJ_DIR)/%.o,$(TEST_PARSE_VALIDATE_USECASE_SRCS))

TEST_OBJ					= $(patsubst %.cpp,$(TEST_OBJ_DIR)/%.o,$(TEST_ALL_SRCS))
TEST_DEPS					= $(TEST_OBJ:.o=.d)

# EXPANSIONS
SRC_SET				= $(INTERFACE_SRCS) \
						$(APPLICATION_SRCS) \
						$(DOMAIN_SRCS) \
						$(INFRA_SRCS)

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
	@rm -f $(TEST_OBJ) $(TEST_DEPS)

fclean: clean
	@echo "🧹 Removing binary..."
	@rm -f $(NAME)
	@rm -f $(TEST_BINS)

re: fclean all

test: $(TEST_NAME) $(TEST_VALIDATION_NAME) $(TEST_USECASE_NAME) $(TEST_CGI_NAME)
	@echo "🧪 Running parser tests..."
	@./$(TEST_HTTP_REQUEST_BIN)
	@echo ""
	@echo "🧪 Running validation tests..."
	@./$(TEST_HTTP_VALIDATION_BIN)
	@echo ""
	@echo "🧪 Running use case tests..."
	@./$(TEST_PARSE_VALIDATE_USECASE_BIN)

$(TEST_OBJ_DIR)/%.o: %.cpp | $(TEST_OBJ_DIR)
	@echo "🛠️  Building test object..."
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_OBJ_DIR):
	@mkdir -p $(TEST_OBJ_DIR)
	@./$(TEST_USECASE_NAME)
	@echo ""
	@echo "🧪 Running CGI tests..."
	@./$(TEST_CGI_NAME)

$(TEST_HTTP_REQUEST_BIN): $(TEST_HTTP_REQUEST_OBJ) | $(TEST_OBJ_DIR)
	@echo "🛠️  Building parser test..."
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_HTTP_VALIDATION_BIN): $(TEST_HTTP_VALIDATION_OBJ) | $(TEST_OBJ_DIR)
	@echo "🛠️  Building validation test..."
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_PARSE_VALIDATE_USECASE_BIN): $(TEST_PARSE_VALIDATE_USECASE_OBJ) | $(TEST_OBJ_DIR)
	@echo "🛠️  Building use case test..."
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_CGI_NAME): $(TEST_CGI_SRC) $(D_ENTITIES_DIR)/HttpRequest.cpp $(D_CGI_DIR)/CgiEnvironment.cpp $(D_CGI_DIR)/CgiResponse.cpp | $(OBJ_DIR)
	@echo "🛠️  Building CGI test..."
	@mkdir -p $(OBJ_DIR)/test
	$(CXX) $(CXXFLAGS) -o $@ $(TEST_CGI_SRC) $(D_ENTITIES_DIR)/HttpRequest.cpp $(D_CGI_DIR)/CgiEnvironment.cpp $(D_CGI_DIR)/CgiResponse.cpp

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
	@clang-format -i $(SRC_SET) $(TEST_ALL_SRCS) $(HDRS)


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

.PHONY: all clean fclean re format check-tools tidy compile_commands_json clean_logs test test_http_request test_http_validation test_parse_validate_usecase

-include $(DEPS) $(TEST_DEPS)
