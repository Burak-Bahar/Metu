import copy
import json
import socket
import sys
import threading
import time

local_host = '127.0.0.1'
#######
######  Note
###### it may take longer than expected

def read_text_file(filep):
    with open(filep, 'r') as f:
        vv = {}
        line = f.readline()
        port_count = int(line.split()[0])
        line = (f.readlines()[0:])
        for l in line:
            x = l.split()
            vv.update({x[0]: x[1]})

    return vv, port_count


def is_changed(vector, data):
    if vector == data.get("vector"):
        return False
    tmp = vector
    a = False
    # if not equal append new ones to dict
    for x in data.get("vector").keys():
        if x not in vector.keys():
            tmp[x] = data.get("vector").get(x)
            a = True
    return a


def find_data_port(data):
    for x in data.items():
        if x[1] == '0':
            return x

def connection_cost(vector, data_port):
    for v in vector.items():
        if v[0] == data_port[0]:
            con_cost = int(v[1])
            return con_cost
    return 0

def change(vector, data, p_count):
    tmp = copy.deepcopy(vector)
    data_port = find_data_port(data)
    a = False
    # find cost between the current connection
    con_cost = connection_cost(vector, data_port)
    # if there are new items
    for d in data.items():
        if d[0] not in vector.keys():
            tmp.update({d[0]: str(con_cost + int(d[1]))})
            a = True
    # if there are updates
    for v in vector.items():
        if v[0] != data_port[0] and v[1] != "0":
            for d in data.items():
                if d[0] == v[0]:
                    mmin = min(int(v[1]), con_cost + int(d[1]))
                    if mmin == con_cost + int(d[1]):
                        mmin = str(mmin)
                        tmp.update({v[0]: mmin})
                        a = True
    return tmp, a


def receive_send(conn, vector, p_count):
    recv_data = conn.recv(2024)  # Should be ready to read
    data = bytes.decode(recv_data)
    data = json.loads(data)
    if data:
        v, a = change(vector, data.get("vector"), p_count)
        if a:
            # if changed return
            start_connections(v, sys.argv[1], data.get("port"))
            return v
        else:
            return vector
    return vector


def start_connections(vector, port, sending):
    for port_num in vector.keys():
        #kendisine gondermesin
        if sending is not port_num:
            server_addr = (local_host, port_num)
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(10)
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            sock.connect((server_addr[0], int(server_addr[1])))
            data = {"port": port, "vector": vector}
            data = json.dumps(data)
            ss = str.encode(data)
            try:
                sock.send(ss)
                time.sleep(0.1)
                sock.close()
            except TimeoutError:
                sock.close()


def last_print(vector):
    a = [len(vector)]
    for x in vector.keys():
        a.append(int(x))
        a.sort()
    for b in a:
        for x in vector.keys():
            if int(x) == b:
                print(sys.argv[1], "-", x, "|", vector.get(x))


def init_sockets(vector, p_count):
    # set up listening socket
    host, port = local_host, int(sys.argv[1])
    l_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    l_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    l_sock.bind((host, port))
    l_sock.settimeout(5)
    l_sock.listen()
    vector.update({sys.argv[1]: '0'})
    try:
        while True:
            # listening
            conn, addr = l_sock.accept()  # Should be ready to read

            vector = receive_send(conn, vector, p_count)

    except KeyboardInterrupt:
        l_sock.close()
    except TimeoutError:
        l_sock.close()
    except socket.timeout:
        l_sock.close()
    finally:
        last_print(vector)


file = sys.argv[1]
file_path = f"{file}.costs"
dist_vec, p_count = read_text_file(file_path)

thread = threading.Thread(target=init_sockets, args=(dist_vec, p_count))
thread.start()
time.sleep(0.5)
start_connections(dist_vec, file, 0)
