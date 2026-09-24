import signal
import socket
import subprocess
import time


def wait_for_server(process, port):
    request = b"HEAD / HTTP/1.1\r\nHost: test\r\n\r\n"

    for _ in range(50):
        if process.poll() is not None:
            raise RuntimeError("The HTTP server stopped unexpectedly")

        try:
            with socket.create_connection(("127.0.0.1", port), timeout=0.1) as client:
                client.sendall(request)
                client.recv(1024)
                return
        except OSError:
            time.sleep(0.02)

    process.kill()
    process.wait()
    raise RuntimeError("The HTTP server did not start")


def start_server(port=8081, root=".", extra_arguments=None):
    command = [
        "./httpd",
        "--pid_file", f"/tmp/httpd-test-{port}.pid",
        "--server_name", "test",
        "--port", str(port),
        "--ip", "127.0.0.1",
        "--root_dir", root,
    ]

    if extra_arguments is not None:
        command.extend(extra_arguments)

    process = subprocess.Popen(
        command,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )

    wait_for_server(process, port)
    return process


def stop_server(process):
    if process.poll() is not None:
        return

    process.send_signal(signal.SIGINT)

    try:
        process.wait(timeout=2)
    except subprocess.TimeoutExpired:
        process.kill()
        process.wait()


def send_raw_request(port, request):
    response = b""

    with socket.create_connection(("127.0.0.1", port), timeout=2) as client:
        client.sendall(request.encode())

        while True:
            data = client.recv(4096)

            if data == b"":
                break

            response += data

    return response.decode(errors="replace")
