#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <iostream>
#include <map>

std::string process(std::string const& s) {
  std::string::size_type pos = s.find(' ');
  if (pos != std::string::npos) {
    return s.substr(0, pos);
  } else {
    return s;
  }
}

enum requestMethods {
  GET,
  POST,
  PUT,
  PATCH,
  DELETE  
};

int main () {
  int server_fd; 
  int new_socket; 
  long valread;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  const int PORT = 8080;

  std::map<std::string, requestMethods> requestsMethodsMap;
  requestsMethodsMap["GET"] = GET;
  requestsMethodsMap["POST"] = POST;
  requestsMethodsMap["PUT"] = PUT;
  requestsMethodsMap["PATCH"] = PATCH;
  requestsMethodsMap["DELETE"] = DELETE;

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("Error in socket");
    return 0;
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  memset(address.sin_zero, '\0', sizeof(address.sin_zero));

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
    std::string method = process(buffer);
    std::string result;
    switch (requestsMethodsMap[method]) {
      case GET: 
        result = "YOU SENT A GET REQUEST\n";
        break;
      case POST:
        result = "YOU SENT A POST REQUEST\n";
        break;
      case PUT:
        result = "YOU SENT A PUT REQUEST\n";
        break;
      case PATCH:
        result = "YOU SENT A PATCH REQUEST\n";
        break;
      case DELETE:
        result = "YOU SENT A DELETE REQUEST\n";
        break;
    }

    write(new_socket, result.c_str(), result.length());
    close(new_socket);
  }
  return 0;
}
