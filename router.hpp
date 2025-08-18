#include <string>
#include <map>
#include <functional>

struct HttpRequest {
    std::string method;
    std::string path;
    std::map<std::string, std::string> headers;
    std::string body;
};

class RequestRouter 
{
    std::map<std::string, std::function<std::string(const HttpRequest&)>> routes;

    public:
        void addRoute(const std::string& path, std::function<std::string(const HttpRequest&)> handler);
        std::string handleRequest(const HttpRequest& req);
        HttpRequest parseHttpRequest(const std::string& raw_request);
};