#include "tcp_server.hpp"

TcpServer::TcpServer(const string &ip, const int &port) : ip_(ip), port_(port) {}
TcpServer::~TcpServer() {}

const string &TcpServer::get_ip(void) {
  return ip_;
}

const int &TcpServer::get_port(void) {
  return port_;
}

const int &TcpServer::get_socket(void) {
  return socket_;
}