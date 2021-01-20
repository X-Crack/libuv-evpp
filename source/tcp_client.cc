#include <tcp_client.h>
#include <tcp_socket.h>
#include <tcp_connect.h>
#include <event_loop.h>
namespace Evpp
{
    TcpClient::TcpClient(EventLoop* loop) : 
        event_loop(loop),
        tcp_socket(std::make_unique<TcpSocket>()),
        tcp_connect(std::make_unique<TcpConnect>(loop))
    {
        
    }

    TcpClient::~TcpClient()
    {

    }

    bool TcpClient::CreaterClient()
    {
        if (tcp_socket && tcp_socket->InitializeTcpSocket() && tcp_connect)
        {
            return tcp_connect->ConnectServers(tcp_socket, this);
        }
        return false;
    }

    TcpClient& TcpClient::AddListenPort(const std::string& server_address, const u16 port)
    {
        if (false == tcp_socket->AddListenPort(server_address, port))
        {
            printf("server address is not\n");
        }
        return *this;
    }

    void TcpClient::DefaultConnect(socket_connect* request)
    {
        printf("与服务器连接成功 %p\n", request);
    }

    void TcpClient::DefaultRefuser(socket_connect* request, int status)
    {
        (void)(request);
        String error_msgs[96] = {};
        String error_name[96] = {};
        printf("与服务器连接失败 [%d - %s - %s]\n", status, uv_err_name_r(status, error_name, 256), uv_strerror_r(status, error_msgs, 256));
    }

    void TcpClient::DefaultConnect(socket_connect* hanlder, int status)
    {
        if (nullptr != hanlder)
        {
            TcpClient* watcher = static_cast<TcpClient*>(hanlder->data);
            {
                if (nullptr != watcher)
                {
                    if (0 == status)
                    {
                        watcher->DefaultConnect(hanlder);
                    }
                    else
                    {
                        watcher->DefaultRefuser(hanlder, status);
                    }
                }
            }
        }
    }
}