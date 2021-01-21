#include <tcp_client.h>
#include <tcp_connect.h>
#include <tcp_session.h>
#include <event_socket.h>
#include <event_loop.h>
namespace Evpp
{
    TcpClient::TcpClient(EventLoop* loop, const u96 index) :
        event_loop(loop),
        safe_index(index),
        tcp_client(std::make_shared<socket_tcp>()),
        tcp_socket(std::make_unique<EventSocket>()),
        tcp_connect(std::make_unique<TcpConnect>(loop, tcp_client))
    {
        
    }

    TcpClient::~TcpClient()
    {

    }

    bool TcpClient::CreaterClient()
    {
        if (tcp_socket && tcp_connect)
        {
            return tcp_connect->ConnectServers(tcp_socket, this);
        }
        return false;
    }

    bool TcpClient::AddListenPort(const std::string& server_address, const u16 port)
    {
        return tcp_socket->CreaterSocket(server_address, port);
    }

    void TcpClient::DefaultConnect(socket_connect* request)
    {
        printf("与服务器连接成功 %p\n", request);
        tcp_session.reset(new TcpSession(
            event_loop, 
            tcp_client,
            safe_index,
            std::bind(&TcpClient::DefaultDiscons, this, std::placeholders::_1, std::placeholders::_2),
            std::bind(&TcpClient::DefaultMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)
        ));
    }

    void TcpClient::DefaultFailure(socket_connect* request, int status)
    {
        (void)(request);
        String error_msgs[96] = {};
        String error_name[96] = {};
        printf("与服务器连接失败 [%d - %s - %s]\n", status, uv_err_name_r(status, error_name, 256), uv_strerror_r(status, error_msgs, 256));
    }

    void TcpClient::DefaultDiscons(EventLoop* loop, const u96 index)
    {
        (void)loop, index;
        tcp_session.reset();
        return;
    }

    bool TcpClient::DefaultMessage(EventLoop* loop, const std::shared_ptr<TcpSession>& session, const std::shared_ptr<TcpBuffer>& buffer, const u96 index)
    {
        (void)loop, session, buffer, index;
        return true;
    }

    void TcpClient::DefaultConnect(socket_connect* hanlder, int status)
    {
        if (nullptr != hanlder)
        {
            TcpClient* watcher = static_cast<TcpClient*>(hanlder->data);
            {
                if (nullptr != watcher)
                {
                    status ? watcher->DefaultFailure(hanlder, status)  : watcher->DefaultConnect(hanlder);
                }
            }
        }
    }
}