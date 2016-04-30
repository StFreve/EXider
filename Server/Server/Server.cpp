#include "EXider.h"

using namespace EXider;
Server::Server( boost::asio::io_service& io ) :
    m_io( io ), m_socket( new boost::asio::ip::tcp::socket( m_io ) ),
    m_acceptor( m_io, boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4(), PORT ) ),
    m_executor( io, this ), taskThread( &ProgramExecutor::run, &m_executor ),
    resultSenderThread( &Server::resultSender, this ) {

}

void Server::run() {
    while ( true ) {
        try {
            m_acceptor.accept( *m_socket );
            std::cout << "Connected!" << std::endl;
            while ( true ) {
                std::string task = read_request();
                if ( taskManager( task ) )
                    return;
            }
        }
        catch ( std::exception& e ) {
            std::cout << "Connection was closed!" << std::endl;
            std::cerr << e.what() << std::endl;
            m_socket.reset( new boost::asio::ip::tcp::socket( m_io ) );
        }
    }
}

void Server::executeHandler( int id, std::string output ) {
    boost::recursive_mutex::scoped_lock sl( m_send_mutex );
    m_messagesToSend.push( { id, output } );
}

void Server::resultSender() {
    while ( true ) {
        boost::asio::deadline_timer waiter( m_io, boost::posix_time::seconds( 2 ) );
        waiter.wait();
        boost::recursive_mutex::scoped_lock sl( m_send_mutex );
        while ( !m_messagesToSend.empty() ) {
            send_request( m_messagesToSend.front().first, m_messagesToSend.front().second );
            m_messagesToSend.pop();
        }
    }
}

std::string Server::read_request() {
    boost::asio::streambuf buf;
    boost::asio::read_until( *m_socket, buf, "\n" );
    std::istream is( &buf );
    std::string readRequest;
    std::getline( is, readRequest );
    return readRequest;
}

void Server::send_request( int id, std::string message ) {
    char messageToSend[ 256 ];
    sprintf( messageToSend, "Result %s\n", message.c_str() );
    boost::asio::write( *m_socket, boost::asio::buffer( messageToSend ) );
}

int Server::taskManager( const std::string& str ) { // TODO
    std::istringstream iss( str );
    std::string task;
    iss >> task;
    size_t pcID = 0;
    if ( task == "ID" )
        iss >> pcID;
    else {
        // TODO
        std::cerr << "Wrong command format" << std::endl;
        return -1;
    }
    iss >> task;
    if ( task == "Run" ) {
        std::string path;
        std::string arguments;
        iss >> path;
        std::getline( iss, arguments );
        Program prog( path, "", arguments + "-pcid " + boost::lexical_cast<std::string>( pcID ) );
        m_executor.addProgram( 0, prog );
    }
    else if ( task == "Stop" ) {
        if ( iss >> task && task == "all" ) {
            m_executor.stop();
        }
        else
            m_executor.terminateRunningProcess();
    }
    else if ( task == "Download" ) {
        // TODO
    }
    else if ( task == "Exit" ) {
        return 1;
    }
    else {
        std::cerr << "Unkown command!" << std::endl;
    }
    return 0;
}