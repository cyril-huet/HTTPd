# HTTPd

A lightweight HTTP/1.1 server written in C.

HTTPd is an educational project built to understand how a web server works: sockets, HTTP request parsing, static file serving, logging and daemon management.

## Features

* HTTP/1.1 request parsing
* `GET` and `HEAD` methods
* Static file serving
* Configurable default file
* Request and response logging
* Foreground and daemon modes
* Daemon start, stop and restart
* Custom IP address, port and document root
* Error responses: `400`, `403`, `404`, `405` and `505`

## Requirements

* Linux
* GCC
* Make
* Python 3 and pytest for the test suite

## Build

```bash
make
```

The executable is generated at the root of the project:

```bash
./httpd
```

## Quick start

Create a directory containing a web page:

```bash
mkdir -p www
echo "<h1>Hello from HTTPd!</h1>" > www/index.html
```

Start the server:

```bash
./httpd \
    --pid_file /tmp/httpd.pid \
    --server_name localhost \
    --port 8080 \
    --ip 127.0.0.1 \
    --root_dir ./www
```

Open `http://127.0.0.1:8080` in a browser or use curl:

```bash
curl http://127.0.0.1:8080
curl -I http://127.0.0.1:8080
```

## Command-line options

| Option                | Description                             |
| --------------------- | --------------------------------------- |
| `--pid_file PATH`     | Path used to store the process ID       |
| `--server_name NAME`  | Name of the HTTP server                 |
| `--port PORT`         | Listening port                          |
| `--ip ADDRESS`        | Listening IP address                    |
| `--root_dir PATH`     | Directory containing the files to serve |
| `--default_file FILE` | Default file, such as `index.html`      |
| `--log true\|false`   | Enable or disable logging               |
| `--log_file PATH`     | Write logs to a file                    |
| `--daemon start`      | Start the server as a daemon            |
| `--daemon stop`       | Stop the running daemon                 |
| `--daemon restart`    | Restart the daemon                      |

## Daemon mode

Start the server in the background:

```bash
./httpd \
    --daemon start \
    --pid_file /tmp/httpd.pid \
    --server_name localhost \
    --port 8080 \
    --ip 127.0.0.1 \
    --root_dir ./www \
    --log_file httpd.log
```

Stop it:

```bash
./httpd --daemon stop --pid_file /tmp/httpd.pid
```

## Tests

Install pytest if necessary:

```bash
python3 -m pip install pytest
```

Run the test suite:

```bash
python3 -m pytest -v
```

The tests cover configuration parsing, HTTP requests, default files, logging and daemon management.

## Project structure

```text
.
├── src/
│   ├── config/     Command-line configuration
│   ├── daemon/     Daemon management
│   ├── http/       HTTP parsing and responses
│   ├── logger/     Request and response logging
│   ├── server/     Socket and client management
│   ├── utils/      Utility functions
│   └── main.c
├── tests/          Integration tests
├── Makefile
└── README.md
```

## Limitations

HTTPd is an educational server and is not intended for production use.

* Linux only
* HTTP/1.1 only
* `GET` and `HEAD` methods only
* One connection is closed after each response

