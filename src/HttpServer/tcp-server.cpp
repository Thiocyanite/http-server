//
// Created by dominik on 05.11.19.
//

#include <HttpServer/tcp-server.h>
#include <HttpServer/SocketReader.h>
#include <HttpServer/SocketReader.h>
#include <HttpServer/ResponseBuilder.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

namespace HttpServer
{
    Threaded_tcp_server::Threaded_tcp_server(const uint16_t server_port,
                                             const std::function<void(int)> &threadBeheaviour,
                                             const int queue_size) : server_port(server_port),
                                                                     queue_size(queue_size),
                                                                     threadBeheaviour(threadBeheaviour)
    {
        init_server_adress();

        init_socket_descriptor_with_error_check();

        bind_with_error_check();

        listen_with_error_check();
    }


    void Threaded_tcp_server::loop()
    {
        while (true)
        {
            handleConnection();
        }
    }

    Threaded_tcp_server::~Threaded_tcp_server()
    {
        close(server_socket_descriptor);
    }

    void Threaded_tcp_server::init_server_adress()
    {
        memset(&server_address, 0, sizeof(struct sockaddr));
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = htonl(INADDR_ANY);
        server_address.sin_port = htons(server_port);
    }

    void Threaded_tcp_server::bind_with_error_check() const
    {
        int bind_result = bind(server_socket_descriptor, (struct sockaddr *) &server_address, sizeof(struct sockaddr));
        if (bind_result < 0)
        {
            std::cerr << "Błąd przy próbie dowiązania adresu IP i numeru portu do gniazda.\n";
            exit(1);
        }
    }

    void Threaded_tcp_server::listen_with_error_check() const
    {
        int listen_result = listen(server_socket_descriptor, QUEUE_SIZE);
        if (listen_result < 0)
        {
            std::cerr << "Błąd przy próbie ustawienia wielkości kolejki.\n";
            exit(1);
        }
    }

    void Threaded_tcp_server::handleConnection()
    {
        int connection_socket_descriptor = accept(server_socket_descriptor, NULL, NULL);
        if (connection_socket_descriptor < 0)
        {
            std::cerr << "Błąd przy próbie utworzenia gniazda dla połączenia.\n";
            exit(1);
        }

        std::thread th(threadBeheaviour, connection_socket_descriptor);
        th.detach();
    }


    void Threaded_tcp_server::init_socket_descriptor_with_error_check()
    {
        long reuse_addr_val=1;

        server_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket_descriptor < 0)
        {
            std::cerr << "Błąd przy próbie utworzenia gniazda..\n";
            exit(1);
        }
        setsockopt(server_socket_descriptor, SOL_SOCKET, SO_REUSEADDR, &reuse_addr_val,
                   sizeof(reuse_addr_val));
    }

}

#pragma clang diagnostic pop