# HTTPd

A simple HTTP/1.1 server written in C as part of the EPITA Assistants project.

## Features

- Basic HTTP/1.1 server (GET, HEAD)
- Request parsing and response generation
- Configurable via command-line arguments
- Daemon mode (start / stop / restart)
- Logging system
- Error handling (400, 403, 404, 405, 505)
- Default file support (index.html)

## Project Structure
src/
config/ # Argument parsing
daemon/ # Daemon management
http/ # HTTP parsing & responses
server/ # Socket handling
logger/ # Logging system
utils/ # Helper functions
main.c



## Build

```sh
make
```

## Run
```sh
./httpd --ip 127.0.0.1 --port 8080 --root_dir ./www --server_name my_server
```
### With config file
```sh
./config_reader.sh --path-bin ./httpd --path-config config.txt
```
### Daemon mode
```sh
./httpd --daemon start
./httpd --daemon stop
./httpd --daemon restart
```
