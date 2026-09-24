# Executable
NAME = httpd

# Compilation
CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Wextra -Werror -Wvla

# Source files
SRC = src/main.c \
      src/config/config.c \
      src/config/config_utils.c \
      src/daemon/daemon.c \
      src/daemon/daemon_utils.c \
      src/server/server.c \
      src/server/server_utils.c \
      src/logger/logger.c \
      src/logger/logger_utils.c \
      src/utils/string/string.c \
      src/http/request.c \
      src/http/request_line.c \
      src/http/request_headers.c \
      src/http/request_body.c \
      src/http/request_utils.c \
      src/http/answer_core.c \
      src/http/answer_path.c \
      src/http/answer_format.c

OBJ = $(SRC:.c=.o)

# Build the project
all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(OBJ) -o $(NAME)

# Compile each source file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run the test suite
check: $(NAME)
	python3 -m pytest -v tests

test: check

format:
	find src tests -type f \( -name "*.h" -o -name "*.c" \) -exec clang-format -i {} +

# Remove object files
clean:
	$(RM) $(OBJ)

# Remove every generated file
fclean: clean
	$(RM) $(NAME)

# Rebuild the project
re: fclean all

.PHONY: all check test clean fclean re
