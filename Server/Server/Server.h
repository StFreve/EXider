#pragma once
#include "EXider.h"
namespace EXider {
	class Server :public boost::noncopyable {
		// Variables
        boost::asio::io_service& m_io;
        boost::shared_ptr<boost::asio::ip::tcp::socket> m_socket;
        boost::asio::ip::tcp::acceptor m_acceptor;
        boost::recursive_mutex m_send_mutex;
        
        FtpClient m_ftp;
		ProgramExecutor m_executor;
		std::queue<std::pair<int, std::string> > m_messagesToSend;
	
        boost::thread taskThread;
		boost::thread resultSenderThread;

        bool wasStopped;
		//Functions
		void send_request( int id, std::string message );
		void resultSender();
		std::string read_request();
		int taskManager( const std::string& str );
	public:
		Server( boost::asio::io_service& io );
		void send_message( int id, std::string message );
		void run();

	};
}