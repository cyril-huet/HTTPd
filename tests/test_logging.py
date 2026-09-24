from framework import send_raw_request
from framework import start_server
from framework import stop_server


def write_file(path, content):
    with open(path, "w", encoding="utf-8") as file:
        file.write(content)


def test_logging_disabled(tmp_path):
    log_file = tmp_path / "disabled.log"
    write_file(tmp_path / "index.html", "Home")

    server = start_server(
        port=8301,
        root=str(tmp_path),
        extra_arguments=[
            "--log", "false",
            "--log_file", str(log_file),
        ],
    )

    try:
        send_raw_request(
            8301,
            "GET / HTTP/1.1\r\nHost: test\r\n\r\n",
        )
    finally:
        stop_server(server)

    assert not log_file.exists()


def test_request_and_response_are_logged(tmp_path):
    log_file = tmp_path / "httpd.log"
    write_file(tmp_path / "index.html", "Home")

    server = start_server(
        port=8302,
        root=str(tmp_path),
        extra_arguments=[
            "--log", "true",
            "--log_file", str(log_file),
        ],
    )

    try:
        response = send_raw_request(
            8302,
            "GET / HTTP/1.1\r\nHost: test\r\n\r\n",
        )
    finally:
        stop_server(server)

    content = log_file.read_text(encoding="utf-8")

    assert response.startswith("HTTP/1.1 200")
    assert "[test] received GET on '/' from 127.0.0.1" in content
    assert "[test] responding with 200" in content


def test_not_found_response_is_logged(tmp_path):
    log_file = tmp_path / "not-found.log"

    server = start_server(
        port=8303,
        root=str(tmp_path),
        extra_arguments=[
            "--log", "true",
            "--log_file", str(log_file),
        ],
    )

    try:
        response = send_raw_request(
            8303,
            "GET /missing.txt HTTP/1.1\r\nHost: test\r\n\r\n",
        )
    finally:
        stop_server(server)

    content = log_file.read_text(encoding="utf-8")

    assert response.startswith("HTTP/1.1 404")
    assert "received GET on '/missing.txt'" in content
    assert "responding with 404" in content


def test_bad_request_is_logged(tmp_path):
    log_file = tmp_path / "bad-request.log"

    server = start_server(
        port=8304,
        root=str(tmp_path),
        extra_arguments=[
            "--log", "true",
            "--log_file", str(log_file),
        ],
    )

    try:
        response = send_raw_request(
            8304,
            "GET / HTTP/1.1\r\n\r\n",
        )
    finally:
        stop_server(server)

    content = log_file.read_text(encoding="utf-8")

    assert response.startswith("HTTP/1.1 400")
    assert "Bad Request from 127.0.0.1" in content
    assert "responding with 400" in content
