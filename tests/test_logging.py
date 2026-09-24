import subprocess, os, time


def cleanup():
    if os.path.exists("log.txt"):
        os.remove("log.txt")
    subprocess.run(["./httpd","--daemon","stop","--pid_file","/tmp/lg.pid"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if os.path.exists("/tmp/lg.pid"):
        os.remove("/tmp/lg.pid")

def test_no_logging():
    cleanup()
    r = subprocess.run(["./httpd","--daemon","start","--pid_file","/tmp/lg.pid","--server_name","t1","--port","9001","--ip","127.0.0.1","--root_dir",".","--log","false"])
    subprocess.run(["./httpd","--daemon","stop","--pid_file","/tmp/lg.pid"])
    assert not os.path.exists("log.txt")

def test_no_logging_2():
    cleanup()
    r = subprocess.run(["./httpd","--daemon","start","--pid_file","/tmp/lg.pid","--server_name","t2","--port","9002","--ip","127.0.0.1","--root_dir",".","--log","false"])
    subprocess.run(["./httpd","--daemon","stop","--pid_file","/tmp/lg.pid"])
    assert not os.path.exists("log.txt")

def test_no_log_file():
    cleanup()
    r = subprocess.run(["./httpd","--daemon","start","--pid_file","/tmp/lg.pid","--server_name","t3","--port","9003","--ip","127.0.0.1","--root_dir",".","--log","true"])
    assert r.returncode == 2

def test_with_logs_file():
    cleanup()
    r = subprocess.run(["./httpd","--daemon","start","--pid_file","/tmp/lg.pid","--server_name","t4","--port","9004","--ip","127.0.0.1","--root_dir",".","--log","true","--log_file","log.txt"])
    subprocess.run(["./httpd","--daemon","stop","--pid_file","/tmp/lg.pid"])
    assert os.path.exists("log.txt")

