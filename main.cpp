#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string>
#include <string.h>
#include <iostream>
#include <map>
#include <vector>
#include <system_error>
#include <execinfo.h>
#include <sstream>
#include <iterator>
#include <sys/epoll.h>
#include <fcntl.h>
#include <netdb.h>
#define MAXEVENTS 64
std::string process(std::string const& s) {
  std::string::size_type pos = s.find(' ');
  if (pos != std::string::npos) {
    return s.substr(0, pos);
  } else {
    return s;
  }
}

int make_socket_non_blocking(int sfd) {
  int flags;

  flags = fcntl(sfd, F_GETFL, 0);
  if (flags == -1) {
    throw system_error_stacktrace(std::error_code(server_fd, std::generic_category()), "Error in fcntl");
    return -1;
  }

  flags |= O_NONBLOCK;
  if (fcntl(sfd, F_SETFL, flags) == -1) {
    throw system_error_stacktrace(std::error_code(server_fd, std::generic_category()), "Error in fcntl");
    return -1;
  }

  return 0;
}

enum requestMethods {
  GET,
  POST,
  PUT,
  PATCH,
  DELETE  
};

class system_error_stacktrace : public std::system_error {
  using std::system_error::system_error;
  public:
  std::string stacktrace() const throw() {
    void * array[1000];
    int nSize = backtrace(array, 1000);
    char** symbols = backtrace_symbols(array, nSize);

    std::vector<std::string> v(symbols, symbols + nSize);

    const char* const delim = "\n";

    std::ostringstream imploded;
    std::copy(v.begin(), v.end(),
        std::ostream_iterator<std::string>(imploded, delim));
    free(symbols);
    return imploded.str();
  }
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

  std::string responseHeader = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: ";
  std::string responseHeaderNotAllowed = "HTTP/1.1 405 Method Not Allowed\nContent-Type: text/plain\nContent-Length: ";

  try {
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
      throw system_error_stacktrace(std::error_code(server_fd, std::generic_category()), "Error in socket");
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    memset(address.sin_zero, '\0', sizeof(address.sin_zero));
    int errno;
    if (errno = bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
      throw system_error_stacktrace(std::error_code(errno, std::generic_category()), "Bind failed");
    }
    if (errno = listen(server_fd, 10) < 0) {
      throw system_error_stacktrace(std::error_code(errno, std::generic_category()), "Error in listen");
    }

    int epoll_fd = epoll_create1(0);

    if (epoll_fd < 0) {
      throw system_error_stacktrace(std::error_code(epoll_fd, std::generic_category()), "Error in epoll_create1");
    }

    struct epoll_event event, *events;

    event.data.fd = server_fd;
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
      throw system_error_stacktrace(std::error_code(epoll_fd, std::generic_category()), "Error in epoll_ctl");
      exit(1);
    }

    events =(epoll_event*)calloc(MAXEVENTS, sizeof(event));

    while (true) {

      int n, i;

      n = epoll_wait(epoll_fd, events, MAXEVENTS, -1);
      for (i =0; i< n; i++) {
        if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP ||
            !(events[i].events & EPOLLIN)) {
          /* An error on this fd or socket not ready */
          throw system_error_stacktrace(std::error_code(epoll_fd, std::generic_category()), "Error in epoll");
          close(events[i].data.fd);
        } else if (events[i].data.fd == server_fd) {
          /* New incoming connection */
          struct epoll_event event;
          struct sockaddr in_addr;
          socklen_t in_len = sizeof(in_addr);
          char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

          //  while ((new_socket = accept(server_fd, &in_addr, &in_len)) != -1) {
          if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            throw system_error_stacktrace(std::error_code(new_socket, std::generic_category()), "Error in accept");
          }
          if (getnameinfo(&in_addr, in_len,
                hbuf, sizeof(hbuf),
                sbuf, sizeof(sbuf),
                NI_NUMERICHOST | NI_NUMERICHOST) == 0) {
            printf("Accepted connection on descriptor %d (host=%s, port=%s)\n",
                new_socket, hbuf, sbuf);
          }
          /* Make the incoming socket non-block
           * and add it to list of fds to
           * monitor*/
          if (make_socket_non_blocking(new_socket) == -1) {
            abort();
          }

          event.data.fd = new_socket;
          event.events = EPOLLIN | EPOLLET;
          if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_socket, &event) == -1) {
            throw system_error_stacktrace(std::error_code(server_fd, std::generic_category()), "Error in epoll_ctl");
            abort();
          }
          in_len = sizeof(in_addr);

          if (errno != EAGAIN && errno != EWOULDBLOCK)
            throw system_error_stacktrace(std::error_code(server_fd, std::generic_category()), "Error in accept");
          /* else
           *
           * We hae processed all incomming connectioins
           *
           */
          /* Data incoming on fd */
          ssize_t count;
          std::vector<char> buffer(30000);

          /*while ((*/count = read(new_socket , &buffer[0], buffer.size());/*)) {*/
          if (count == -1) {
            /* EAGAIN, read all data */
            if (errno == EAGAIN)
              return 1;

            throw system_error_stacktrace(std::error_code(epoll_fd, std::generic_category()), "Error in read");
            break;
          }

          std::string str(buffer.begin(), buffer.end());
          std::cout << str << std::endl;

          std::string method = process(str);
          std::string result;
          std::string body;
          int contentLength;
          std::string contentLengthStr;
          switch (requestsMethodsMap.find(method)->second) {
            case GET: 
              body = "YOU SENT A GET REQUEST\n";
              contentLength = body.length();
              contentLengthStr = std::to_string(contentLength);
              result = responseHeader + contentLengthStr + "\n\n" + body;
              break;
            case POST:
              body = "YOU SENT A POST REQUEST\n";
              contentLength = body.length();
              contentLengthStr = std::to_string(contentLength);
              result = responseHeader + contentLengthStr + "\n\n" + body;
              break;
            case PUT:
              body = "YOU SENT A PUT REQUEST\n";
              contentLength = body.length();
              contentLengthStr = std::to_string(contentLength);
              result = responseHeader + contentLengthStr + "\n\n" + body;
              break;
            case PATCH:
              body = "YOU SENT A PATCH REQUEST\n";
              contentLength = body.length();
              contentLengthStr = std::to_string(contentLength);
              result = responseHeader + contentLengthStr + "\n\n" + body;
              break;
            case DELETE:
              body = "YOU SENT A DELETE REQUEST\n";
              contentLength = body.length();
              contentLengthStr = std::to_string(contentLength);
              result = responseHeader + contentLengthStr + "\n\n" + body;
              break;
            default:
              body = "METHOD NOT ALLOWED\n";
              contentLength = body.length();
              contentLengthStr = std::to_string(contentLength);
              result = responseHeaderNotAllowed + contentLengthStr + "\n\n" + body;
              break;
          }

          write(new_socket, result.c_str(), result.length());


          // printf("Close connection on descriptor: %d\n", events[i].data.fd);
          ////  close(events[i].data.fd);
          //}
          //}
        }
      }
    }
  } catch (system_error_stacktrace& e) {
    std::cout << "Caught system_error with code " << e.code()
      << " meaning " << e.what() << '\n' <<e.stacktrace() << '\n';
  }
  return EXIT_SUCCESS;
}
