from framework import send_raw_request
from framework import start_server
from framework import stop_server


def write_file(path, content):
    with open(path, "w", encoding="utf-8") as file:
        file.write(content)


def get_content_length(response):
    headers = response.split("\r\n\r\n", 1)[0]

    for line in headers.split("\r\n"):
        if line.lower().startswith("content-length:"):
            value = line.split(":", 1)[1].strip()
            return int(value)

    return None


def get_body(response):
    parts = response.split("\r\n\r\n", 1)

    if len(parts) == 2:
        return parts[1]

    return ""


def test_get_default_file(tmp_path):
    body = "Hello from HTTPd!"
    write_file(tmp_path / "index.html", body)

    server = start_server(port=8181, root=str(tmp_path))

    try:
        response = send_raw_request(
            8181,
            "GET / HTTP/1.1\r\nHost: test\r\n\r\n",
        )
    finally:
        stop_server(server)

    assert response.startswith("HTTP/1.1 200")
    assert get_content_length(response) == len(body)
    assert get_body(response) == body


def test_get_default_file_from_directory(tmp_path):
    directory = tmp_path / "folder"
    directory.mkdir()

    body = "File inside a directory"
    write_file(directory / "index.html", body)

    server = start_server(port=8182, root=str(tmp_path))

    try:
        response = send_raw_request(
            8182,
            "GET /folder/ HTTP/1.1\r\nHost: test\r\n\r\n",
        )
    finally:
        stop_server(server)

    assert response.startswith("HTTP/1.1 200")
    assert get_content_length(response) == len(body)
    assert get_body(response) == body


def test_custom_default_file(tmp_path):
    body = "Custom home page"
    write_file(tmp_path / "home.html", body)

    server = start_server(
        port=8183,
        root=str(tmp_path),
        extra_arguments=["--default_file", "home.html"],
    )

    try:
        response = send_raw_request(
            8183,
            "GET / HTTP/1.1\r\nHost: test\r\n\r\n",
        )
    finally:
        stop_server(server)

    assert response.startswith("HTTP/1.1 200")
    assert get_body(response) == body


def test_head_default_file(tmp_path):
    body = "This body must not be returned"
    write_file(tmp_path / "index.html", body)

    server = start_server(port=8184, root=str(tmp_path))

    try:
        response = send_raw_request(
            8184,
            "HEAD / HTTP/1.1\r\nHost: test\r\n\r\n",
        )
    finally:
        stop_server(server)

    assert response.startswith("HTTP/1.1 200")
    assert get_content_length(response) == len(body)
    assert get_body(response) == ""


def test_head_default_file_from_directory(tmp_path):
    directory = tmp_path / "documents"
    directory.mkdir()

    body = "Document index"
    write_file(directory / "index.html", body)

    server = start_server(port=8185, root=str(tmp_path))

    try:
        response = send_raw_request(
            8185,
            "HEAD /documents/ HTTP/1.1\r\nHost: test\r\n\r\n",
        )
    finally:
        stop_server(server)

    assert response.startswith("HTTP/1.1 200")
    assert get_content_length(response) == len(body)
    assert get_body(response) == ""
