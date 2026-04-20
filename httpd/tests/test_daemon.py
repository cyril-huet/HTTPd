import subprocess , os , time

def cleanup():
    if os.path.exists("/tmp/HTTPd.pid"):
        os.remove("/tmp/HTTPd.pid")

def pid_is_empty():
    return not os.path.exists("/tmp/HTTPd.pid") or open("/tmp/HTTPd.pid").read().strip() == ""

def test_start():
    cleanup()
    subprocess.run(["./httpd","--daemon","start","--pid_file","/tmp/HTTPd.pid","--server_name","d1","--port","8300","--ip","127.0.0.1","--root_dir","."])
    assert not pid_is_empty()

def test_stop():
    cleanup()
    subprocess.run(["./httpd","--daemon","start","--pid_file","/tmp/HTTPd.pid","--server_name","d2","--port","8301","--ip","127.0.0.1","--root_dir","."])
    subprocess.run(["./httpd","--daemon","stop","--pid_file","/tmp/HTTPd.pid"])
    assert pid_is_empty()

def test_restart():
    cleanup()
    subprocess.run(["./httpd","--daemon","start","--pid_file","/tmp/HTTPd.pid","--server_name","d3","--port","8302","--ip","127.0.0.1","--root_dir","."])
    old = open("/tmp/HTTPd.pid").read().strip()
    subprocess.run(["./httpd","--daemon","restart","--pid_file","/tmp/HTTPd.pid","--server_name","d3","--port","8302","--ip","127.0.0.1","--root_dir","."])
    new = open("/tmp/HTTPd.pid").read().strip()
    assert old != new

def test_start_stop():
    cleanup()
    subprocess.run(["./httpd","--daemon","start","--pid_file","/tmp/HTTPd.pid","--server_name","d4","--port","8303","--ip","127.0.0.1","--root_dir","."])
    subprocess.run(["./httpd","--daemon","stop","--pid_file","/tmp/HTTPd.pid"])
    assert pid_is_empty()

def test_start_stop_stop():
    cleanup()
    subprocess.run(["./httpd","--daemon","start","--pid_file","/tmp/HTTPd.pid","--server_name","d5","--port","8304","--ip","127.0.0.1","--root_dir","."])
    subprocess.run(["./httpd","--daemon","stop","--pid_file","/tmp/HTTPd.pid"])
    r = subprocess.run(["./httpd","--daemon","stop","--pid_file","/tmp/HTTPd.pid"])
    assert r.returncode == 0
    assert pid_is_empty()

def test_start_start_stop():
    cleanup()
    subprocess.run(["./httpd","--daemon","start","--pid_file","/tmp/HTTPd.pid","--server_name","d6","--port","8305","--ip","127.0.0.1","--root_dir","."])
    r = subprocess.run(["./httpd","--daemon","start","--pid_file","/tmp/HTTPd.pid"])
    assert r.returncode == 1
    subprocess.run(["./httpd","--daemon","stop","--pid_file","/tmp/HTTPd.pid"])
    assert pid_is_empty()

def test_start_req_stop():
    cleanup()
    subprocess.run(["./httpd","--daemon","start","--pid_file","/tmp/HTTPd.pid","--server_name","d7","--port","8306","--ip","127.0.0.1","--root_dir","."])
    subprocess.run(["./httpd","--daemon","stop","--pid_file","/tmp/HTTPd.pid"])
    assert pid_is_empty()

def test_restart_req_stop():
    cleanup()
    subprocess.run(["./httpd","--daemon","start","--pid_file","/tmp/HTTPd.pid","--server_name","d8","--port","8307","--ip","127.0.0.1","--root_dir","."])
    subprocess.run(["./httpd","--daemon","restart","--pid_file","/tmp/HTTPd.pid","--server_name","d8","--port","8307","--ip","127.0.0.1","--root_dir","."])
    assert not pid_is_empty()

def test_restart_same():
    cleanup()
    subprocess.run(["./httpd","--daemon","restart","--pid_file","/tmp/HTTPd.pid","--server_name","d9","--port","8308","--ip","127.0.0.1","--root_dir","."])
    assert not pid_is_empty()

def test_restart_different():
    cleanup()
    subprocess.run(["./httpd","--daemon","restart","--pid_file","/tmp/HTTPd.pid","--server_name","d10","--port","8310","--ip","127.0.0.1","--root_dir","."])
    assert not pid_is_empty()
