/*
 *  client.cpp
 *  v0.0.1
 *  Updated on: May 15, 2017
 *  Updated by: Zheng Xing <zxing@robohome.org>
 */

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <errno.h>

//const char* socket_path = "./socket";
const char* socket_path = "\0hidden";

int main(int argc, char** argv) {
	using namespace std;

	struct sockaddr_un addr;
	char buf[100];
	int data_fd;
	int rc;

	if ((data_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		cerr << "Error: socket() failed: " << strerror(errno) << endl;
		exit (EXIT_FAILURE);
	}

	memset(&addr, 0, sizeof(addr));

	addr.sun_family = AF_UNIX;

	if (*socket_path == '\0') {
		*addr.sun_path = '\0';
		strncpy(addr.sun_path + 1, socket_path + 1, sizeof(addr.sun_path) - 2);
	} else {
		strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
	}

	if (connect(data_fd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
		cerr << "Error: connect() failed: " << strerror(errno) << endl;
		exit (EXIT_FAILURE);
	}
	
	memset(buf, 0, strlen(buf));

	while ((rc = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
		
		if (write(data_fd, buf, rc) == -1) {
			cerr << "Error: write() failed: " << strerror(errno) << endl;
			exit (EXIT_FAILURE);
		} else {
			cout << "wrote " << rc << " Bytes" << " data: " << buf << endl;
		}
		
		memset(buf, 0, strlen(buf));
		
		if ((rc = read(data_fd, buf, sizeof(buf))) > 0) {
			cout << "read " << rc << " Bytes" << " data: " << buf << endl;
		} else if (rc == -1) {
			cerr << "Error: read() failed: " << strerror(errno) << endl;
			exit (EXIT_FAILURE);
		} else if (rc == 0) {
			cout << "read() returned 0" << endl;
			close(data_fd);
		}
		memset(buf, 0, strlen(buf));
	}

	return 0;
}
