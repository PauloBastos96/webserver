#include "http_handler.hpp"
#include "webserver.hpp"
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

HttpHandler::HttpHandler(const std::string &request, Server &server,
                         Client &client)
    : server_(&server), client_(&client) {
    HttpParser parser(request);
    headers_ = parser.get_headers();
}

HttpHandler::~HttpHandler() {}

/// @brief Check if the request has a redirection
/// @param uri The URI of the request
/// @return True if the request has a redirection, false otherwise
bool HttpHandler::has_redirection(const std::string &uri) {
    std::vector<Location> locations = server_->get_locations();
    for (size_t i = 0; i < locations.size(); i++) {
        if (uri == locations.at(i).get_path() &&
            !locations.at(i).get_config().get_redirection().empty())
            return true;
    }
    if (!server_->get_config().get_redirection().empty())
        return true;
    return false;
}

std::string HttpHandler::create_redirection_response() {
    enum redirectionType type;
    std::string response;
    std::string redirection;

    redirection = server_->get_config().get_redirection();
    for (size_t i = 0; i < server_->get_locations().size(); i++) {
        if (headers_.at("uri") == server_->get_locations().at(i).get_path()) {
            redirection =
                server_->get_locations().at(i).get_config().get_redirection();
            break;
        }
    }
    if (redirection.substr(redirection.find_last_of(' ') + 1) == "redirect")
        type = REDIRECT_307;
    else
        type = REDIRECT_308;
    if (type == REDIRECT_307)
        response = "HTTP/1.1 307 Temporary Redirect\r\n";
    else
        response = "HTTP/1.1 308 Permanent Redirect\r\n";
    response +=
        "Location: " + redirection.substr(0, redirection.find_last_of(' ')) +
        "\r\nContent-Type: text/html\r\nContent-Length: 0\r\n\r\n";
    WebServer::log(std::string(type == REDIRECT_307 ? HTTP_307 : HTTP_308) +
                       headers_.at("uri"),
                   info);
    return response;
}

#pragma region HTTP Methods

/// @brief Process the request
std::string HttpHandler::process_request() {
    if (!is_method_allowed(headers_.at("method")))
        return get_error_page(405);
    if (has_redirection(headers_.at("uri")))
        return create_redirection_response();
    if (headers_.at("method") == "GET")
        return process_get();
    else if (headers_.at("method") == "POST")
        return process_post();
    else if (headers_.at("method") == "DELETE")
        return process_delete();
    else if (IS_VALID_BUT_NOT_SUPPORTED(headers_.at("method")))
        return get_error_page(501);
    else
        return get_error_page(400);
    return "";
}

/// @brief Process a GET request
std::string HttpHandler::process_get() {
    std::fstream file;
    std::string content;
    std::string file_path;
    std::stringstream ss;
    Stat buffer;

    try {
        file_path = get_file_path(headers_.at("uri"));
        if (*(headers_.at("uri").end() - 1) == '/' &&
            server_->get_config().get_auto_index()) {
            file_path = server_->get_config().get_root() + headers_.at("uri");
            content = create_autoindex(file_path, headers_.at("uri"));
            if (content.empty())
                return get_error_page(404);
            ss << content.length();
            std::string response =
                response_builder("200", "OK", "text/html", ss.str());
            WebServer::log(std::string(HTTP_200) + headers_.at("uri"), info);
            return response + content;
        }
        if (stat(file_path.c_str(), &buffer) != 0 || S_ISDIR(buffer.st_mode))
            throw std::runtime_error("404");
        content = read_file(file_path);
        ss << content.length();
        std::string response = response_builder(
            "200", "OK", get_content_type(file_path), ss.str());
        WebServer::log(std::string(HTTP_200) + headers_.at("uri"), info);
        return response + content;
    } catch (const std::runtime_error &e) {
        return get_error_page(404);
    }
}

/// @brief Process a POST request
std::string HttpHandler::process_post() {
    size_t max_size =
        get_max_size(server_->get_config().get_max_client_body_size());
    if (headers_.at("body").size() > max_size)
        return get_error_page(413);
    std::string response = "Received" + headers_.at("body");
    std::stringstream ss;
    ss << response.length();
    return response_builder("201", "Created", "text/plain", ss.str()) +
           response;
}

/// @brief Process a DELETE request
std::string HttpHandler::process_delete() {
    std::string file_path;
    Stat buffer;

    file_path = server_->get_config().get_root() + headers_.at("uri");
    if (stat(file_path.c_str(), &buffer) != 0)
        return get_error_page(404);
    if (!(buffer.st_mode & S_IWOTH))
        return get_error_page(403);
    if (std::remove(file_path.c_str()))
        return get_error_page(403);
    return response_builder("204", "No Content", "text/plain", "0");
}

#pragma endregion

#pragma region Utility Functions

/// @brief Check if the method is allowed for the location
/// @param method The method to check
/// @return True if the method is allowed, false otherwise
bool HttpHandler::is_method_allowed(const std::string &method) {
    std::string location =
        headers_.at("uri").substr(0, headers_.at("uri").find_last_of('/'));
    std::vector<Location> locations = server_->get_locations();
    for (size_t i = 0; i < locations.size(); i++) {
        if (location == locations.at(i).get_path()) {
            for (size_t j = 0; j < locations.at(i).get_allowed_methods().size();
                 j++) {
                if (method == locations.at(i).get_allowed_methods().at(j))
                    return true;
            }
            return false;
        }
    }
    return true;
}

/// @brief Check if the server should generate an autoindex page
/// @param uri The URI of the request
/// @param server A reference to the server
/// @return True if the server should generate an autoindex page, false
/// otherwise
bool should_generate_autoindex(const std::string &uri, Server &server) {
    if (uri == "/" && server.get_config().get_auto_index())
        return true;
    else if (*(uri.end() - 1) == '/') {
        std::vector<Location> locations = server.get_locations();
        if (locations.empty() && server.get_config().get_auto_index())
            return true;
        for (size_t i = 0; i < locations.size(); i++) {
            if (uri == locations.at(i).get_path() &&
                locations.at(i).get_config().get_auto_index())
                return true;
        }
    }
    return false;
}

/// @brief Get the path of the location
/// @param uri The URI of the request
/// @return The path of the location
std::string HttpHandler::get_location_path(const std::string &uri) {
    std::string path;
    std::vector<Location> locations = server_->get_locations();
    Stat buffer;

    for (size_t i = 0; i < locations.size(); i++) {
        if (uri == locations.at(i).get_path()) {
            std::vector<std::string> indexes =
                locations.at(i).get_config().get_indexes();
            for (size_t j = 0; j < indexes.size(); j++) {
                path = server_->get_config().get_root() +
                       locations.at(i).get_config().get_root() + "/" +
                       indexes.at(j);
                if (stat(path.c_str(), &buffer) == 0)
                    return path;
            }
        } else {
            path = server_->get_config().get_root() + uri;
            if (stat(path.c_str(), &buffer) == 0)
                break;
        }
    }
    return path;
}

/// @brief Get the path of the requested file
/// @param uri The URI of the request
/// @return The path of the requested file
std::string HttpHandler::get_file_path(const std::string &uri) {
    std::string file_path;
    Stat buffer;

    if (uri == "/") {
        for (size_t i = 0; i < server_->get_config().get_indexes().size();
             i++) {
            file_path = server_->get_config().get_root() + "/" +
                        server_->get_config().get_indexes().at(i);
            if (stat(file_path.c_str(), &buffer) == 0)
                break;
        }
    } else {
        if (server_->get_locations().empty())
            file_path = server_->get_config().get_root() + uri;
        else
            file_path = get_location_path(uri);
    }
    return file_path;
}

/// @brief Get the path of the error page
/// @param status_code The status code
/// @return The path of the error page
std::string HttpHandler::get_error_page_path(const int status_code) {
    std::string path;
    std::string location;
    Stat buffer;
    bool hasCustomErrorPage = false;
    bool isValidPath = false;

    get_route_error_page(hasCustomErrorPage, status_code, path);
    isValidPath = stat(path.c_str(), &buffer) == 0;
    switch (status_code) {
    case 400:
        return (!hasCustomErrorPage || !isValidPath)
                   ? "default_pages/bad_request.html"
                   : path;
    case 403:
        return (!hasCustomErrorPage || !isValidPath)
                   ? "default_pages/forbidden.html"
                   : path;
    case 404:
        return (!hasCustomErrorPage || !isValidPath)
                   ? "default_pages/not_found.html"
                   : path;
    case 405:
        return (!hasCustomErrorPage || !isValidPath)
                   ? "default_pages/not_allowed.html"
                   : path;
    case 413:
        return (!hasCustomErrorPage || !isValidPath)
                   ? "default_pages/content_too_large.html"
                   : path;
    case 501:
        return (!hasCustomErrorPage || !isValidPath)
                   ? "default_pages/not_implemented.html"
                   : path;
    default:
        return (!hasCustomErrorPage || !isValidPath)
                   ? "default_pages/internal_server_error.html"
                   : path;
    }
}

/// @brief Get the error page for the route
/// @param hasCustomErrorPage Set value to true if the route has a custom error
/// page
/// @param status_code The status code
/// @param path Set the path of the error page
void HttpHandler::get_route_error_page(bool &hasCustomErrorPage,
                                       const int status_code,
                                       std::string &path) {
    std::string location;
    location =
        headers_.at("uri").substr(0, headers_.at("uri").find_last_of('/'));
    std::vector<Location> locations = server_->get_locations();
    try {
        for (size_t i = 0; i < locations.size(); i++) {
            if (location == locations.at(i).get_path()) {
                hasCustomErrorPage =
                    locations.at(i).get_config().get_error_pages().find(
                        status_code) !=
                    locations.at(i).get_config().get_error_pages().end();
                if (locations.at(i).get_config().get_root().empty())
                    path = server_->get_config().get_root() +
                           locations.at(i).get_config().get_error_pages().at(
                               status_code);
                else
                    path = server_->get_config().get_root() +
                           locations.at(i).get_config().get_root() +
                           locations.at(i).get_config().get_error_pages().at(
                               status_code);
                break;
            }
            if (i == locations.size() - 1) {
                hasCustomErrorPage =
                    server_->get_config().get_error_pages().find(status_code) !=
                    server_->get_config().get_error_pages().end();
                path = server_->get_config().get_root() +
                       server_->get_config().get_error_pages().at(status_code);
            }
        }
    } catch (const std::out_of_range &e) {
        hasCustomErrorPage = false;
    }
}

/// @brief Get the error page
/// @param status_code The status code
/// @return The error page content
std::string HttpHandler::get_error_page(const int status_code) {
    std::string content;
    std::string path;
    std::stringstream ss;

    try {
        switch (status_code) {
        case 400:
            content = read_file(get_error_page_path(400));
            ss << content.length();
            WebServer::log(std::string(HTTP_400) + headers_.at("uri"), warning);
            return response_builder("400", "Bad Request", "text/html",
                                    ss.str()) +
                   content;
        case 403:
            content = read_file(get_error_page_path(403));
            ss << content.length();
            WebServer::log(std::string(HTTP_403) + headers_.at("uri"), warning);
            return response_builder("403", "Forbidden", "text/html", ss.str()) +
                   content;
        case 404:
            content = read_file(get_error_page_path(404));
            ss << content.length();
            WebServer::log(std::string(HTTP_404) + headers_.at("uri"), warning);
            return response_builder("404", "Not Found", "text/html", ss.str()) +
                   content;
        case 405:
            content = read_file(get_error_page_path(405));
            ss << content.length();
            WebServer::log(std::string(HTTP_405) + headers_.at("uri"), warning);
            return response_builder("405", "Method Not Allowed", "text/html",
                                    ss.str()) +
                   content;
        case 413:
            content = read_file(get_error_page_path(413));
            ss << content.length();
            WebServer::log(std::string(HTTP_413) + headers_.at("uri"), warning);
            return response_builder("413", "Content Too Large", "text/html",
                                    ss.str()) +
                   content;
        case 501:
            content = read_file(get_error_page_path(501));
            ss << content.length();
            WebServer::log(std::string(HTTP_501) + headers_.at("uri"), warning);
            return response_builder("501", "Not Implemented", "text/html",
                                    ss.str()) +
                   content;
        default:
            content = read_file(get_error_page_path(500));
            ss << content.length();
            WebServer::log(std::string(HTTP_500) + headers_.at("uri"), warning);
            return response_builder("500", "Internal Server Error", "text/html",
                                    ss.str()) +
                   content;
        }
    } catch (...) {
        return response_builder("500", "Internal Server Error", "text/plain",
                                "22") +
               " Internal Server Error";
    }
}

/// @brief Create an autoindex page
/// @param path The path of the directory
/// @param uri The URI of the directory
/// @return The autoindex page content
const std::string HttpHandler::create_autoindex(const std::string &path,
                                                const std::string &uri) {
    std::string autoindex;
    autoindex = "<!DOCTYPE html>\n<html>\n<head>\n<title>Index of " + uri +
                "</title>\n<link rel=\"stylesheet\" href=\"/styles.css\"> "
                "\n</head>\n<body>\n<h1 id=\"autoindex\">Index of " +
                uri + "</h1>\n";
    autoindex += "<table>\n<tr>\n<th>Name</th>\n<th>Last Modified</th>\n";
    autoindex += "<th>Size</th>\n</tr>\n";
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            struct stat st;
            std::stringstream ss;
            std::string file_path = path + "/" + std::string(ent->d_name);
            stat(file_path.c_str(), &st);
            ss << st.st_size;
            std::string itemName = S_ISDIR(st.st_mode)
                                       ? std::string(ent->d_name) + "/"
                                       : std::string(ent->d_name);
            autoindex += "<tr>\n<td><a href=\"" + itemName + "\">" + itemName +
                         "</a></td>\n";
            autoindex += "<td>" + std::string(ctime(&st.st_mtime)) + "</td>\n";
            autoindex += "<td>" + ss.str() + " B</td>\n</tr>\n";
        }
        closedir(dir);
    } else {
        return "";
    }
    autoindex += "</table>\n</body>\n</html>";
    return autoindex;
}

/// @brief Get the maximum size of the client body
/// @param max_size The maximum size set in the configuration file
/// @return The maximum size of the client body in bytes
size_t HttpHandler::get_max_size(const std::string &max_size) {
    size_t size = 0;
    char unit;

    if (isalpha(*(max_size.end() - 1)))
        unit = static_cast<char>(std::tolower(*(max_size.end() - 1)));
    else
        unit = 'b';
    switch (unit) {
    case 'k':
        size = std::atoi(max_size.c_str()) * 1024;
        break;
    case 'm':
        size = std::atoi(max_size.c_str()) * 1024 * 1024;
        break;
    case 'g':
        size = std::atoi(max_size.c_str()) * 1024 * 1024 * 1024;
        break;
    default:
        size = std::atoi(max_size.c_str());
        break;
    }
    return size;
}

/// @brief Build the response header
/// @param status_code The status code
/// @param status_message The status message
/// @param content_type The content type
/// @param content_length The content length
/// @return The response header
std::string HttpHandler::response_builder(const std::string &status_code,
                                          const std::string &status_message,
                                          const std::string &content_type,
                                          const std::string &content_length) {
    std::string response = "HTTP/1.1 " + status_code + " " + status_message +
                           "\r\n" + "Content-Type: " + content_type + "\r\n" +
                           "Content-Length:" + content_length + "\r\n\r\n";
    return (response);
}

/// @brief Read requested file
/// @param file_path The path of the file
/// @return The content of the file
std::string HttpHandler::read_file(const std::string &file_path) {
    std::fstream file;
    std::string content;

    const bool text_file = is_text_file(file_path);
    if (!text_file)
        file.open(file_path.c_str(), std::ios::in | std::ios::binary);
    else
        file.open(file_path.c_str(), std::ios::in);
    if (file.is_open()) {
        char buffer[1024];
        if (!text_file) {
            while (file.read(buffer, 1024))
                content.append(buffer, 1024);
        } else {
            while (file.getline(buffer, 1024))
                content += buffer;
        }
        file.close();
    } else
        throw std::runtime_error("404");
    return (content);
}

/// @brief Check if a file is a text file
/// @param file_path The path of the file
/// @return True if the file is a text file, false otherwise
bool HttpHandler::is_text_file(const std::string &file_path) {
    const std::string extension =
        file_path.substr(file_path.find_last_of('.') + 1);
    if (extension == "html" || extension == "css" || extension == "js" ||
        extension == "json" || extension == "xml" || extension == "svg")
        return (true);
    return (false);
}

/// @brief Get the content type of a file
/// @param file_path The path of the file
/// @return The content type of the file
std::string HttpHandler::get_content_type(const std::string &file_path) {
    const std::string extension =
        file_path.substr(file_path.find_last_of('.') + 1);
    if (extension == "html" || extension == "css")
        return ("text/" + extension);
    if (extension == "js")
        return ("text/javascript");
    if (extension == "jpeg" || extension == "jpg")
        return ("image/jpeg");
    if (extension == "png")
        return ("image/png");
    if (extension == "gif")
        return ("image/gif");
    if (extension == "svg")
        return ("image/svg+xml");
    if (extension == "json")
        return ("application/json");
    if (extension == "xml")
        return ("application/xml");
    if (extension == "form")
        return ("application/x-www-form-urlencoded");
    return ("text/plain");
}

#pragma endregion
