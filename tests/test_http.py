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


def request_server(port, root, request):
    server = start_server(port=port, root=str(root))

    try:
        response = send_raw_request(port, request)
    finally:
        stop_server(server)

    return response


def test_get_file(tmp_path):
    body = "Hello from a file"
    write_file(tmp_path / "hello.txt", body)

    response = request_server(
        8201,
        tmp_path,
        "GET /hello.txt HTTP/1.1\r\nHost: test\r\n\r\n",
    )

    assert response.startswith("HTTP/1.1 200")
    assert get_content_length(response) == len(body)
    assert get_body(response) == body


def test_get_nested_file(tmp_path):
    directory = tmp_path / "documents"
    directory.mkdir()

    write_file(directory / "notes.txt", "HTTP notes")

    response = request_server(
        8202,
        tmp_path,
        "GET /documents/notes.txt HTTP/1.1\r\nHost: test\r\n\r\n",
    )

    assert response.startswith("HTTP/1.1 200")
    assert get_body(response) == "HTTP notes"


def test_missing_file(tmp_path):
    response = request_server(
        8203,
        tmp_path,
        "GET /missing.txt HTTP/1.1\r\nHost: test\r\n\r\n",
    )

    assert response.startswith("HTTP/1.1 404")


def test_directory_without_default_file(tmp_path):
    directory = tmp_path / "empty"
    directory.mkdir()

    response = request_server(
        8204,
        tmp_path,
        "GET /empty/ HTTP/1.1\r\nHost: test\r\n\r\n",
    )

    assert response.startswith("HTTP/1.1 404")


def test_head_file(tmp_path):
    body = "12345"
    write_file(tmp_path / "file.txt", body)

    response = request_server(
        8205,
        tmp_path,
        "HEAD /file.txt HTTP/1.1\r\nHost: test\r\n\r\n",
    )

    assert response.startswith("HTTP/1.1 200")
    assert get_content_length(response) == len(body)
    assert get_body(response) == ""


def test_head_missing_file(tmp_path):
    response = request_server(
        8206,
        tmp_path,
        "HEAD /missing.txt HTTP/1.1\r\nHost: test\r\n\r\n",
    )

    assert response.startswith("HTTP/1.1 404")


def test_missing_host_header(tmp_path):
    write_file(tmp_path / "index.html", "Home")

    response = request_server(
        8207,
        tmp_path,
        "GET / HTTP/1.1\r\n\r\n",
    )

    assert response.startswith("HTTP/1.1 400")


def test_unsupported_http_version(tmp_path):
    response = request_server(
        8208,
        tmp_path,
        "GET / HTTP/2.0\r\nHost: test\r\n\r\n",
    )

    assert response.startswith("HTTP/1.1 505")


def test_method_not_allowed(tmp_path):
    response = request_server(
        8209,
        tmp_path,
        "POST / HTTP/1.1\r\nHost: test\r\n\r\n",
    )

    assert response.startswith("HTTP/1.1 405")


def test_parent_directory_is_forbidden(tmp_path):
    response = request_server(
        8210,
        tmp_path,
        "GET /../ HTTP/1.1\r\nHost: test\r\n\r\n",
    )

    assert response.startswith("HTTP/1.1 403")


def test_file_outside_root_is_forbidden(tmp_path):
    response = request_server(
        8211,
        tmp_path,
        "GET /../../etc/passwd HTTP/1.1\r\nHost: test\r\n\r\n",
    )

    assert response.startswith("HTTP/1.1 403")


def test_spaces_in_path(tmp_path):
    response = request_server(
        8212,
        tmp_path,
        "GET /hello world HTTP/1.1\r\nHost: test\r\n\r\n",
    )

    assert response.startswith("HTTP/1.1 400")


def test_missing_method(tmp_path):
    response = request_server(
        8213,
        tmp_path,
        "/ HTTP/1.1\r\nHost: test\r\n\r\n",
    )

    assert response.startswith("HTTP/1.1 400")


def test_empty_request(tmp_path):
    response = request_server(
        8214,
        tmp_path,
        "\r\n\r\n",
    )

    assert response.startswith("HTTP/1.1 400")
