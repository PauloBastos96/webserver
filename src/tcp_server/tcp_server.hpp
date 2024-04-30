#ifndef TCP_SERVER_HPP
#define TCP_SERVER_HPP

#include <string>

using std::string;

class TcpServer {
private:
  string ip_;
  int port_;
  int socket_;

public:
  TcpServer(const string &ip, const int &port);
  ~TcpServer();

  const string &get_ip(void);
  const int &get_port(void);
  const int &get_socket(void);
};

#endif