#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

int main () {
  int server_fd; 
  int new_socket; 
  long valread;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  const int PORT = 8080;

  char* hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("Error in socket");
    return 0;
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons( PORT );

  memset(address.sin_zero, '\0', sizeof address.sin_zero);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("Error in bind");
    return 0;
  }

  if (listen(server_fd, 10) < 0) {
    perror("Error in listen");
    return 0;
  }

  while (1) {
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
      perror("Error in accept");
      return 0;
    }

    char buffer[30000] = {0};
    valread = read( new_socket , buffer, 30000);
    printf("%s\n",buffer );
    write(new_socket , hello , strlen(hello));

    close(new_socket);
  }
  return 0;
}
