from framework import start_server , stop_server , send_raw_request
import os

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


def test_basic_folder():
    os.makedirs("folder", exist_ok=True)
    body = "42"*15
    open("folder/index.html","w").write(body)

    p = start_server(root=".")
    resp = send_raw_request(8081,"GET /folder/ HTTP/1.1\r\nHost: test\r\n\r\n")
    stop_server(p)

    assert resp.startswith("HTTP/1.1 200")
    assert get_length(resp) == len(body)





def test_default_folder():
    os.makedirs("4242", exist_ok=True)
    body = "42424242"*20
    open("abc/index.html","w").write(body)

    p = start_server(root=".")
    resp = send_raw_request(8081,"GET /abc/ HTTP/1.1\r\nHost: test\r\n\r\n")
    stop_server(p)

    assert resp.startswith("HTTP/1.1 200")
    assert get_length(resp) == len(body)

def test_head_root():
    body = "42Httpd"
    open("index.html", "w").write(body)

    p = start_server(root=".")
    resp = send_raw_request(8081,'HEAD / HTTP/1.1\r\nHost: test\r\n\r\n')
    stop_server(p)

    assert resp.startswith("HTTP/1.1 200")
    assert body not in resp 
    assert get_length(resp) == len(body)


def test_head_folder():
    os.makedirs("www", exist_ok=True)
    open("www/index.html", "w").write("424242")

    p = start_server(root=".")
    resp = send_raw_request(8081,"HEAD /www/ HTTP/1.1\r\nHost: test\r\n\r\n")
    stop_server(p)

    assert resp.startswith("HTTP/1.1 200")
    assert "424242" not in resp
    assert get_length(resp) == 6


def test_head_final():
    os.makedirs("epita", exist_ok=True)
    open("epita/42.html", "w").write("abcd")

    p = start_server(root=".")
    resp = send_raw_request(8081,"HEAD /epita/42.html HTTP/1.1\r\nHost: test\r\n\r\n")
    stop_server(p)

    assert resp.startswith("HTTP/1.1 200")
    assert "abcd" not in resp
    assert get_length(resp) == 4