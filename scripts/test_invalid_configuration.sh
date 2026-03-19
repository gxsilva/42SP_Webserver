# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    test_invalid_configuration.sh                      :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/03/19 20:32:06 by lsilva-x          #+#    #+#              #
#    Updated: 2026/03/19 20:32:41 by lsilva-x         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/bash

# To better view enable Debug mode in make file

# Navigate to project root (../ from scripts/)
cd "$(dirname "$0")/.."

WEB_SERVER_EXEC="./webserver"
WEB_SERVER_FLAGS="valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --quiet --track-fds=all"


RED="\e[31m"
RESET="\e[0m"
GREEN="\e[32m"


# Check if webserver exists
if [ ! -f "$WEB_SERVER_EXEC" ]; then
    echo "webserver not found, running make..."
    make
    if [ ! -f "$WEB_SERVER_EXEC" ]; then
        echo "Make failed or webserver still not found."
        exit 1
    fi
fi

echo -e "
  _____ _   ___      __     _      _____ _____  
 |_   _| \ | \ \    / /\   | |    |_   _|  __ \ 
   | | |  \| |\ \  / /  \  | |      | | | |  | |
   | | | .   | \ \/ / /\ \ | |      | | | |  | |
  _| |_| |\  |  \  / ____ \| |____ _| |_| |__| |
 |_____|_| \_|   \/_/    \_\______|_____|_____/ 
"

# Run for all bad maps
echo -e "${RED}Running Valgrind on INVALID file configuration:${RESET}"
for file in config/invalid/*.conf; do
    if [ -f "$file" ]; then
        printf "$RED\nRunning:\n$file\n$WEB_SERVER_FLAGS $WEB_SERVER_EXEC  $RESET\n"
        $WEB_SERVER_FLAGS $WEB_SERVER_EXEC "$file"
    fi
done