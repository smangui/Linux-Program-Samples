gcc -g -o test_client test_client.c TCPConnection.c
gcc -g -o test_server test_server.c TCPConnection.c
gcc -fPIC -o libtcpconnection.so -shared TCPConnection.c
