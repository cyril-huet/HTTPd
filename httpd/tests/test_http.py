import os
from framework import start_server, stop_server, send_raw_request

def get_length(resp):
    headers = resp.split("\r\n\r\n", 1)[0]
    for line in headers.split("\r\n"):
        if line.lower().startswith("content-length"):
            valeur = line.split(":", 1)[1].strip()
            return int(valeur)
    return None


def test_basic():
    body = "42"
    open("index.html","w").write(body)
    p = start_server(root=".")
    resp = send_raw_request(8081 , "GET / HTTP/1.1\r\nHost: test\r\n\r\n")
    stop_server(p)

    assert resp.startswith("HTTP/1.1 200")
    assert get_length(resp) == len(body)

def test_get_root_basic():
    open("index.html", "w").write("hello epita")
    p = start_server(root=".")
    resp = send_raw_request(8081, "GET / HTTP/1.1\r\nHost: test\r\n\r\n")
    stop_server(p)
    assert resp.startswith("HTTP/1.1 200")
    assert "hello epita" in resp


def test_missing_file():
    p = start_server(root=".")
    resp = send_raw_request(8081, "GET /nofile HTTP/1.1\r\nHost: test\r\n\r\n")
    stop_server(p)
    assert resp.startswith("HTTP/1.1 404")


def test_get_folder():
    os.makedirs("epita42", exist_ok=True)
    open("epita42/index.html", "w").write("Hello World")
    p = start_server(root=".")
    resp = send_raw_request(8081, "GET /z/ HTTP/1.1\r\nHost: test\r\n\r\n")
    stop_server(p)
    assert resp.startswith("HTTP/1.1 200")
    assert "Hello World" in resp


def test_get_file():
    os.makedirs("a", exist_ok=True)
    open("a/t.txt", "w").write("abc")
    p = start_server(root=".")
    resp = send_raw_request(8081, "GET /a/t.txt HTTP/1.1\r\nHost: test\r\n\r\n")
    stop_server(p)
    assert resp.startswith("HTTP/1.1 200")
    assert "abc" in resp

def test_get_emtpy():
    os.makedirs("empty", exist_ok=True)
    p = start_server(root=".")
    resp = send_raw_request(8081, "GET /empty/ HTTP/1.1\r\nHost: test\r\n\r\n")
    stop_server(p)
    assert resp.startswith("HTTP/1.1 404")

def test_head_root():
    open("index.html", "w").write("ppex3")
    p = start_server(root=".")
    resp = send_raw_request(8081, "HEAD / HTTP/1.1\r\nHost: test\r\n\r\n")
    stop_server(p)
    assert resp.startswith("HTTP/1.1 200")
    assert "ppex3" not in resp
    assert get_length(resp) == 3


def test_head_file():
    open("epita.html", "w").write("12345")
    p = start_server(root=".")
    resp = send_raw_request(8081, "HEAD /epita.html HTTP/1.1\r\nHost: test\r\n\r\n")
    stop_server(p)
    assert resp.startswith("HTTP/1.1 200")
    assert "12345" not in resp
    assert get_length(resp) == 5


def test_head_missing():
    p = start_server(root=".")
    resp = send_raw_request(8081, "HEAD /nofile HTTP/1.1\r\nHost: test\r\n\r\n")
    stop_server(p)
    assert resp.startswith("HTTP/1.1 404")


def test_head_folder_default():
    os.makedirs("head", exist_ok=True)
    open("head/index.html", "w").write("ok")
    p = start_server(root=".")
    resp = send_raw_request(8081, "HEAD /h/ HTTP/1.1\r\nHost: test\r\n\r\n")
    stop_server(p)
    assert resp.startswith("HTTP/1.1 200")
    assert "ok" not in resp
    assert get_length(resp) == 2


def test_head_folder_missing_default():
    os.makedirs("n", exist_ok=True)
    p = start_server(root=".")
    resp = send_raw_request(8081, "HEAD /n/ HTTP/1.1\r\nHost: test\r\n\r\n")
    stop_server(p)
    assert resp.startswith("HTTP/1.1 404")


def test_missing_host_header():
    open("index.html", "w").write("a")
    p = start_server(root=".")
    resp = send_raw_request(8081, "GET / HTTP/1.1\r\n\r\n")
    stop_server(p)
    assert resp.startswith("HTTP/1.1 400")


def test_bad_http():
    open("index.html", "w").write("a")
    p = start_server(root=".")
    resp = send_raw_request(8081, "GET / HTTP/2.0\r\nHost: test\r\n\r\n")
    stop_server(p)
    assert resp.startswith("HTTP/1.1 505")


def test_method_not_allowed():
    p = start_server(root=".")
    resp = send_raw_request(8081, "EPITA / HTTP/1.1\r\nHost: test\r\n\r\n")
    stop_server(p)
    assert resp.startswith("HTTP/1.1 405")


def test_forbidden():
    p = start_server(root=".")
    resp = send_raw_request(8081, "GET /../ HTTP/1.1\r\nHost: test\r\n\r\n")
    stop_server(p)
    assert resp.startswith("HTTP/1.1 403")


def test_forbidden_file():
    p = start_server(root=".")
    resp = send_raw_request(8081,"GET /../../etc/passwd HTTP/1.1\r\nHost: test\r\n\r\n")
    stop_server(p)
    assert resp.startswith("HTTP/1.1 403")


def test_double_slash_path():
    open("index.html", "w").write("ok")
    p = start_server(root=".")
    resp = send_raw_request(8081,"GET // HTTP/1.1\r\nHost: test\r\n\r\n")
    stop_server(p)
    assert resp.startswith("HTTP/1.1")


def test_spaces_in_path():
    p = start_server(root=".")
    resp = send_raw_request(8081,"GET /a b HTTP/1.1\r\nHost: test\r\n\r\n")
    stop_server(p)
    assert resp.startswith("HTTP/1.1 400")

def test_no_method():
    p = start_server(root=".")
    resp = send_raw_request(8081,  "/ HTTP/1.1\r\nHost: test\r\n\r\n" )
    stop_server(p)
    assert resp.startswith("HTTP/1.1 400")


def test_only_crlf():
    p = start_server(root=".")
    resp = send_raw_request(8081, "\r\n\r\n")
    stop_server(p)
    assert resp.startswith("HTTP/1.1 400")
