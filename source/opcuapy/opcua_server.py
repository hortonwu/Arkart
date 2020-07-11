import sys
sys.path.insert(0, "..")
import time
from random import randint

from opcua import ua, Server


if __name__ == "__main__":

    # setup our server
    server = Server()
    server.set_endpoint("opc.tcp://192.168.1.25:4840")

    # setup our own namespace, not really necessary but should as spec
    uri = "Arkart_OPCUA_Server"
    idx = server.register_namespace(uri)

    # get Objects node, this is where we should put our nodes
    objects = server.get_objects_node()

    # populating our address space
    myobj = objects.add_object(idx, "MyObject")
    distance = myobj.add_variable(idx, "MyVariable", 6.7)
    distance.set_writable()    # Set MyVariable to be writable by clients

    # starting!
    server.start()
    
    try:
        distance_T = 0
        while True:
            time.sleep(1)
            distance_T = randint(0,100)
            distance.set_value(distance_T)
            print(" Distance is {}", distance)
    finally:
        #close connection, remove subcsriptions, etc
        server.stop()