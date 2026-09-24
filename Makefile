CC = gcc
CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla

TARGET = httpd

OBJS = \
    src/main.o \
    src/config/config.o \
    src/config/config_utils.o \
    src/daemon/daemon.o \
    src/daemon/daemon_utils.o \
    src/server/server.o \
    src/server/server_utils.o \
    src/logger/logger.o \
    src/logger/logger_utils.o \
    src/utils/string/string.o \
    src/http/request.o \
    src/http/request_line.o \
    src/http/request_headers.o \
    src/http/request_body.o \
    src/http/request_utils.o \
    src/http/answer_core.o \
    src/http/answer_path.o \
    src/http/answer_format.o


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

check:
	-pytest -vv
	./tests/clean.sh

clean:
	$(RM) $(TARGET) $(OBJS)


