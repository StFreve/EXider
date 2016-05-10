#pragma once
#include "EXider.h"
namespace EXider {

    class RemotePC : public boost::enable_shared_from_this<RemotePC> {
        boost::asio::ip::tcp::socket m_socket;
        boost::asio::ip::tcp::endpoint m_endpoint;
        boost::asio::streambuf m_buffer;
        boost::function<void( boost::shared_ptr<RemotePC>, std::string )> m_callback;
        rpcStatus m_status;
        size_t m_id;
        
        std::string reqToSend;
        // Async Wrtie, Read, Connecting Handlers
        void sendHandler( const boost::system::error_code& error );
        void readHandler( const boost::system::error_code& error, size_t bytes );
        void connectedHandler( const boost::system::error_code& error );

    public:
        RemotePC( boost::asio::io_service& io, const std::string& IP );
        RemotePC( boost::asio::io_service& io, const boost::asio::ip::address& IP );
        ~RemotePC();
        bool connect();	                                        // Sync-Connect to the socket
        bool reconnect();                                       // Reconnecting to the socket
        void disconnect();                                      // Disconnecting the socke
        inline const rpcStatus status() const;                  // PC's current status

        void sendRequest( const std::string& request );	        // Async-Writing message in socket
        void readRequest();                                     // Async-Reading message from socket

        // Setup PC's ID in current Task, and call-back function to communicate with the Task
        void setID( size_t ID );
        void setCallBackFunction( const boost::function<void( boost::shared_ptr<RemotePC>, std::string )>& cb );
        void clearCallBackFunction();
        boost::shared_ptr<RemotePC> getSelfPtr();               // Self Pointer
        const size_t getID() const;                                // PC's ID in current Task
        inline const boost::asio::ip::address getIP() const;    // IP's IP address
        bool operator==( const RemotePC& rhrp ) const;
        bool operator<( const RemotePC& rhrp ) const;
    };
};