import subprocess


def run_httpd(arguments):
    command = ["./httpd"] + arguments

    return subprocess.run(
        command,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        timeout=2,
    )


def test_no_arguments():
    result = run_httpd([])

    assert result.returncode == 2


def test_missing_pid_file():
    result = run_httpd([
        "--server_name", "test",
        "--port", "8081",
        "--ip", "127.0.0.1",
        "--root_dir", ".",
    ])

    assert result.returncode == 2


def test_missing_server_name():
    result = run_httpd([
        "--pid_file", "/tmp/httpd-test.pid",
        "--port", "8082",
        "--ip", "127.0.0.1",
        "--root_dir", ".",
    ])

    assert result.returncode == 2


def test_missing_port():
    result = run_httpd([
        "--pid_file", "/tmp/httpd-test.pid",
        "--server_name", "test",
        "--ip", "127.0.0.1",
        "--root_dir", ".",
    ])

    assert result.returncode == 2


def test_missing_ip_address():
    result = run_httpd([
        "--pid_file", "/tmp/httpd-test.pid",
        "--server_name", "test",
        "--port", "8083",
        "--root_dir", ".",
    ])

    assert result.returncode == 2


def test_missing_root_directory():
    result = run_httpd([
        "--pid_file", "/tmp/httpd-test.pid",
        "--server_name", "test",
        "--port", "8084",
        "--ip", "127.0.0.1",
    ])

    assert result.returncode == 2


def test_unknown_option():
    result = run_httpd([
        "--pid_file", "/tmp/httpd-test.pid",
        "--server_name", "test",
        "--port", "8085",
        "--ip", "127.0.0.1",
        "--root_dir", ".",
        "--unknown",
    ])

    assert result.returncode == 2


def test_invalid_daemon_value():
    result = run_httpd([
        "--pid_file", "/tmp/httpd-test.pid",
        "--server_name", "test",
        "--port", "8086",
        "--ip", "127.0.0.1",
        "--root_dir", ".",
        "--daemon", "invalid",
    ])

    assert result.returncode == 2
