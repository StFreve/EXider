#include "EXider.h"

using namespace EXider;
Server::Server( boost::asio::io_service& io )
    : m_io( io )
    , m_socket( new boost::asio::ip::tcp::socket( m_io ) )
    , m_acceptor( m_io, boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4(), PORT ) )
    , m_send_mutex()
    , m_ftp( "31.170.164.154", "u823219472", "459s62nqctm5b" )
    , m_executor( io, this )
    , m_messagesToSend()
    , taskThread( &ProgramExecutor::run, &m_executor )
    , resultSenderThread( &Server::resultSender, this )
    , wasStopped( false ) {

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

void Server::send_message( int id, std::string message ) {
    if ( wasStopped )
        return;
    boost::recursive_mutex::scoped_lock sl( m_send_mutex );
    m_messagesToSend.push( { id, message } );
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
    std::cerr << "Send message: " << message << std::endl;
    char messageToSend[ 256 ];
    sprintf( messageToSend, "%s\n", message.c_str() );
    boost::asio::write( *m_socket, boost::asio::buffer( messageToSend, strlen( messageToSend ) ) );
}

int Server::taskManager( const std::string& str ) { // TODO
    std::cerr << "Get command: " << str << std::endl;
    wasStopped = false;
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
        std::string fullPath;
        std::string arguments;
        iss >> fullPath;
        std::getline( iss, arguments );

        std::string filePath, fileName;
        size_t lastSlashPos = fullPath.find_last_of( '/' );
        if ( lastSlashPos == std::string::npos ) {
            fileName = fullPath;
        }
        else {
            filePath = fullPath.substr( 0, lastSlashPos + 1 );
            fileName = fullPath.substr( lastSlashPos + 1 );
        }
        std::cerr << boost::format( "Run Program\nName: %1%\nPath: %2%\nArguments: %3%" ) % fileName % filePath % arguments << std::endl;
        Program prog( fileName, filePath, arguments + "-pcid " + boost::lexical_cast<std::string>( pcID ) );
        m_executor.addProgram( 0, prog );
    }
    else if ( task == "Stop" ) {
      /*  if ( iss >> task && task == "all" ) {
            m_executor.stop();
        }
        else
            m_executor.terminateRunningProcess();*/
        wasStopped = true;                                                     // Prevents sending other messages
        m_executor.stop();                                                     // Stops running process
        
        boost::recursive_mutex::scoped_lock sl( m_send_mutex ); 
        std::queue<std::pair<int, std::string> >().swap( m_messagesToSend );   // Deletes all messages in queue

        send_request( pcID, "Stopped" );
            
    }
    else if ( task == "Download" ) {
        std::string url;
        std::getline( iss, url );
        url = url.substr( url.find_first_not_of( ' ' ) );
        try {
            m_ftp.download( url );
        }
        catch ( std::exception& e ) {
            m_messagesToSend.push( std::make_pair( pcID, "Downloading failed" ) );
            return 0;
        }
        m_messagesToSend.push( std::make_pair( pcID, "Downloading OK" ) );
    }
    else if ( task == "Exit" ) {
        return 1;
    }
    else {
        std::cerr << "Unkown command!" << std::endl;
    }
    return 0;
}