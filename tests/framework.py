
import subprocess , time
import socket , signal




def start_server(port = 8081 ,  root = "." ):
    
    res = subprocess.Popen(["./httpd" , "--pid_file" , "/tmp/HTTPd.pid" ,"--server_name","test" ,"--port" , str(port) ,"--ip","127.0.0.1" ,"--root_dir" , root] ,stdout = subprocess.PIPE ,stderr = subprocess.PIPE ,)
    return res


def stop_server(res):
    try :
        res.send_signal(signal.SIGINT)
    except Exception :
        res.kill()


def send_raw_request( port , data ):
    s = socket.socket( socket.AF_INET , socket.SOCK_STREAM )
    s.connect( ("127.0.0.1" , port) )
    s.sendall( data.encode() )
    resp = s.recv( 4096 ).decode( errors = "ignore" )
    s.close( )
    return resp
