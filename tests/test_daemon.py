import os
import subprocess
import time


def run_daemon(action, pid_file, port=None, root=None):
    command = [
        "./httpd",
        "--daemon", action,
        "--pid_file", str(pid_file),
    ]

    if action == "start" or action == "restart":
        command.extend([
            "--server_name", "daemon-test",
            "--port", str(port),
            "--ip", "127.0.0.1",
            "--root_dir", str(root),
            "--log", "false",
        ])

    return subprocess.run(
        command,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        timeout=2,
    )


def read_pid(pid_file):
    if not pid_file.exists():
        return None

    content = pid_file.read_text(encoding="utf-8").strip()

    if content == "":
        return None

    return int(content)


def wait_for_pid(pid_file):
    for _ in range(100):
        pid = read_pid(pid_file)

        if pid is not None:
            return pid

        time.sleep(0.02)

    raise RuntimeError("The daemon did not write its PID")


def wait_for_new_pid(pid_file, old_pid):
    for _ in range(100):
        pid = read_pid(pid_file)

        if pid is not None and pid != old_pid:
            return pid

        time.sleep(0.02)

    raise RuntimeError("The daemon did not restart")


def process_is_running(pid):
    try:
        os.kill(pid, 0)
    except ProcessLookupError:
        return False
    except PermissionError:
        return True

    return True


def wait_for_process_stop(pid):
    for _ in range(100):
        if not process_is_running(pid):
            return

        time.sleep(0.02)

    raise RuntimeError("The daemon did not stop")


def stop_daemon(pid_file):
    pid = read_pid(pid_file)

    run_daemon("stop", pid_file)

    if pid is not None:
        wait_for_process_stop(pid)


def cleanup_daemon(pid_file):
    stop_daemon(pid_file)

    if pid_file.exists():
        pid_file.unlink()


def test_daemon_start(tmp_path):
    pid_file = tmp_path / "start.pid"

    try:
        result = run_daemon(
            "start",
            pid_file,
            port=8401,
            root=tmp_path,
        )

        pid = wait_for_pid(pid_file)

        assert result.returncode == 0
        assert process_is_running(pid)
    finally:
        cleanup_daemon(pid_file)


def test_daemon_stop(tmp_path):
    pid_file = tmp_path / "stop.pid"

    try:
        run_daemon(
            "start",
            pid_file,
            port=8402,
            root=tmp_path,
        )

        pid = wait_for_pid(pid_file)
        result = run_daemon("stop", pid_file)

        wait_for_process_stop(pid)

        assert result.returncode == 0
        assert read_pid(pid_file) is None
    finally:
        cleanup_daemon(pid_file)


def test_daemon_restart(tmp_path):
    pid_file = tmp_path / "restart.pid"

    try:
        run_daemon(
            "start",
            pid_file,
            port=8403,
            root=tmp_path,
        )

        old_pid = wait_for_pid(pid_file)

        result = run_daemon(
            "restart",
            pid_file,
            port=8403,
            root=tmp_path,
        )

        new_pid = wait_for_new_pid(pid_file, old_pid)
        wait_for_process_stop(old_pid)

        assert result.returncode == 0
        assert new_pid != old_pid
        assert process_is_running(new_pid)
    finally:
        cleanup_daemon(pid_file)


def test_restart_without_running_daemon(tmp_path):
    pid_file = tmp_path / "restart-empty.pid"

    try:
        result = run_daemon(
            "restart",
            pid_file,
            port=8404,
            root=tmp_path,
        )

        pid = wait_for_pid(pid_file)

        assert result.returncode == 0
        assert process_is_running(pid)
    finally:
        cleanup_daemon(pid_file)


def test_stop_without_running_daemon(tmp_path):
    pid_file = tmp_path / "stop-empty.pid"

    result = run_daemon("stop", pid_file)

    assert result.returncode == 0
    assert read_pid(pid_file) is None
