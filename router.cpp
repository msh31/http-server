#include "router.hpp"
#include <sstream>
#include <string>

void RequestRouter::addRoute(const std::string& path, std::function<std::string(const HttpRequest&)> handler) {
    routes[path] = handler;
}

std::string RequestRouter::handleRequest(const HttpRequest& req) {
    std::string route_key = req.method + " " + req.path;

    auto it = routes.find(route_key);
    if (it != routes.end()) {
        return it->second(req);
    }

    // loop through the web dir for files to serve
    // route_pattern = the key (like "GET *") & handler = the function to call
    for (const auto& [route_pattern, handler] : routes) {
        // string.split, the c++ way, takes that GET and puts that in the method, then the * in the path
        std::istringstream pattern_stream(route_pattern);
        std::string pattern_method, pattern_path;
        pattern_stream >> pattern_method >> pattern_path;

        if (pattern_method == req.method && pattern_path == "*") {
            return handler(req);
        }
    }

    return "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 13\r\n\r\n404 Not Found";
}

HttpRequest RequestRouter::parseHttpRequest(const std::string& raw_request) {
    HttpRequest req;
    std::istringstream stream(raw_request);
    std::string line;
    
    // method + path (e.g. GET /api/users HTTP/1.1)
    if (std::getline(stream, line)) {
        std::istringstream request_line(line);
        request_line >> req.method >> req.path;
    }
    
    // headers (key: value pairs) (e.g. Host: localhost:8004 Content-Type: application/json)
    while (std::getline(stream, line) && line != "\r" && !line.empty()) {
        size_t colon_pos = line.find(':');

        if (colon_pos != std::string::npos) {
            std::string key = line.substr(0, colon_pos);
            std::string value = line.substr(colon_pos + 2);

            if (!value.empty() && value.back() == '\r') {
                value.pop_back();
            }

            req.headers[key] = value;
        }
    }
    
    // body (e.g. {"user": "data"} )
    std::string body_line;

    while (std::getline(stream, body_line)) {
        req.body += body_line + "\n";
    }
    return req;
}