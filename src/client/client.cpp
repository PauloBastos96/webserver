#include "client.hpp"

Client::Client(int socket) : socket_(socket) {}

Client::~Client() {}

void Client::set_request(const std::string &request) { request_ = request; }

void Client::set_response(const std::string &response) { response_ = response; }

std::string &Client::get_request() { return request_; }

std::string &Client::get_response() { return response_; }

int Client::get_socket() { return socket_; }

void Client::set_socket(int socket) { socket_ = socket; }