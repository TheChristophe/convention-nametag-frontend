#ifndef CONVENTION_NAMETAG_SERVER_HPP
#define CONVENTION_NAMETAG_SERVER_HPP

#include <App.h>
#include <libusockets.h>
#include <nlohmann/json.hpp>

#include <string>

class WebServer {
    public:
    explicit WebServer();
    ~WebServer() = default;

    nlohmann::json Handle(int type, std::string_view command, const nlohmann::json &metadata);

    void Run();
    void Halt();

    private:
    template <typename ResponseT, typename RequestT>
    void processData(ResponseT *response, RequestT *request, int method, std::string_view command, std::string_view data, bool last)
    {
        std::string buffer{};
        if (not last) {
            response->writeStatus("400 Bad Request");
            response->end("<!DOCTYPE html><html lang=\"en\"><body>error</body></html>");
        }
        buffer.append(data.data(), data.size());

        auto out = this->Handle(method, command, nlohmann::json::parse(buffer)).dump();

        response->writeStatus("200 OK");
        response->writeHeader("content-type", "application/json");

        response->end(out);
    }

    template <typename ResponseT, typename RequestT>
    void processAborted(ResponseT *response, RequestT *request, int method, std::string_view command)
    {
        // dunno
    }

    us_listen_socket_t *_socket{};

    short _port{ 8080 };
};

#endif