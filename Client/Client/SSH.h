#pragma once
#include "EXider.h"
namespace EXider {
    class SSH {
        ssh_session m_session;
        int connect(ssh_channel& channel, const std::string& login,const std::string& password,const std::string& ip, int port );
        int sendCommand( ssh_channel& channel,const std::string& command );
        int readResult( ssh_channel& channel, std::string& result );
    public:
        SSH();
        void run(const std::string& login,const std::string& password, const std::vector<boost::asio::ip::address>& vecToConnect, const std::string& command );
    };
}