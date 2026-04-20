import subprocess

def test_missing_pid_file( ):
    r = subprocess.run(["./httpd","--server_name","x","--port","8081" ,"--ip","127.0.0.1","--root_dir","."])
    assert r.returncode == 2

def test_missing_server_name():
    r = subprocess.run(["./httpd","--pid_file","/tmp/c.pid","--port","8082","--ip","127.0.0.1" , "--root_dir","."])
    assert r.returncode == 2

def test_missing_port( ):
    r = subprocess.run(["./httpd","--pid_file","/tmp/c.pid","--server_name","t","--ip","127.0.0.1","--root_dir","."])
    assert r.returncode == 2

def test_unknown_option():
    r = subprocess.run(["./httpd","--pid_file","/tmp/c.pid","--server_name","t","--port","8083","--ip","127.0.0.1","--root_dir",".","--bad",])
    assert r.returncode == 2

def test_invalid_daemon_value( ):
    r = subprocess.run(["./httpd","--pid_file","/tmp/c.pid","--server_name","t","--port","8084","--ip","127.0.0.1","--root_dir",".","--daemon","non"])
    assert r.returncode == 2

def test_missing_pid_file():
    r = subprocess.run(["./httpd","--server_name","x","--port","8081","--ip","127.0.0.1","--root_dir","."])
    assert r.returncode == 2


def test_valid_with_log_false():
    r = subprocess.run(["./httpd","--daemon","start","--pid_file","/tmp/cB.pid","--server_name","ok","--port","8091","--ip","127.0.0.1","--root_dir",".","--log","false"])
    assert r.returncode != 2


def test_valid_with_log_true():
    r = subprocess.run(["./httpd","--daemon","start","--pid_file","/tmp/cC.pid","--server_name","ok","--port","8092","--ip","127.0.0.1","--root_dir",".","--log","true","--log_file","log.txt"])
    assert r.returncode != 2


def test_valid_daemon_start():
    r = subprocess.run(["./httpd","--pid_file","/tmp/cD.pid","--server_name","ok","--port","8093","--ip","127.0.0.1","--root_dir",".","--daemon","start"])
    assert r.returncode != 2


def test_valid_argument():
    r = subprocess.run(["./httpd","--daemon","start","--pid_file","/tmp/cE.pid","--server_name","ok","--port","8094","--ip","127.0.0.1","--root_dir",".","--log","false"])
    assert r.returncode != 2

