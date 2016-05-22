#include "EXider.h"
using namespace EXider;
RemotePC::RemotePC( boost::asio::io_service& io, const std::string& IP )
    : m_socket( io )
    , m_endpoint( boost::asio::ip::address::from_string( IP ), EXIDER_PORT )
    , m_io( io )
    , m_status( NotConencted )
    , m_id( 0 ) {

}

RemotePC::RemotePC( boost::asio::io_service & io, const boost::asio::ip::address & IP )
    : m_socket( io )
    , m_endpoint( IP, EXIDER_PORT )
    , m_io( io )
    , m_status( NotConencted )
    , m_id( 0 ) {

}
RemotePC::~RemotePC() {

}

bool RemotePC::connect()
{
    boost::system::error_code er;
    m_socket.connect( m_endpoint, er );
    if ( er ) {
        m_status = NotConencted;
        return false;
    }
    else {
        m_status = Available;
        //   readRequest();
        return true;
    }
}

bool RemotePC::reconnect() {
    disconnect();
    return connect();
}

void RemotePC::disconnect() {
    if ( m_socket.is_open() )
        m_socket.close();
    m_status = NotConencted;
}

const rpcStatus RemotePC::status() {
    if ( m_status == Available )
        checkConnection();
    return m_status;
}

void RemotePC::sendRequest( const std::string& request ) {
    if ( !m_socket.is_open() )
        std::cerr << "Socket isn't open" << std::endl;
    else if ( m_status == NotConencted )
        std::cerr << "Connection ism't available" << std::endl;

    reqToSend = boost::str( boost::format( "ID %1% %2%\n" ) % getID() % request );     // Adding ID to request
    m_socket.async_write_some( boost::asio::buffer( reqToSend ), boost::bind( &RemotePC::sendHandler, this, _1 ) );
}
void RemotePC::readRequest() {
    if ( !m_socket.is_open() )
        std::cerr << "Socket isn't open" << std::endl;
    else if ( m_status == NotConencted )
        std::cerr << "Connection ism't available" << std::endl;

    boost::asio::async_read_until( m_socket, m_buffer, '\n', boost::bind( &RemotePC::readHandler, this, _1, _2 ) );
}
void RemotePC::setInWork( bool busy ){
    if ( busy ) {
        m_status = Busy;
    }
    else
        m_status = Available;


}
void RemotePC::setID( size_t ID ) {
    m_id = ID;
}
void RemotePC::setCallBackFunction( const boost::function<void( boost::shared_ptr<RemotePC>, std::string )>& cb ) {
    m_callback = cb;
}
void RemotePC::clearCallBackFunction() {
    m_callback.clear();
}
const size_t RemotePC::getID() const {
    return m_id;
}

inline const boost::asio::ip::address RemotePC::getIP() const {
    return m_endpoint.address();
}

bool RemotePC::operator==( const RemotePC & rhrp ) const {
    return getIP() == rhrp.getIP();
}

bool RemotePC::operator<( const RemotePC & rhrp ) const {
    return getIP() < rhrp.getIP();
}
void RemotePC::readHandler( const boost::system::error_code& error, size_t bytes ) {
    if ( boost::asio::error::eof == error ||
         boost::asio::error::connection_reset == error ) {
        m_status = NotConencted;
        if ( !m_callback.empty() ) {
            m_callback( getSelfPtr(), "Disconnected" );
            return;
        }
    }

    if ( error ) {
        if ( !m_callback.empty() )
            m_callback( getSelfPtr(), "Reading ERROR" );
        return;
    }
    std::istream is( &m_buffer );
    std::string result;
    std::getline( is, result );
    if ( !m_callback.empty() )
        m_callback( getSelfPtr(), result );
    //   readRequest();
}
void RemotePC::sendHandler( const boost::system::error_code& error ) {
    if ( boost::asio::error::eof == error ||
         boost::asio::error::connection_reset == error ) {
        m_status = NotConencted;
        if ( !m_callback.empty() )
            m_callback( getSelfPtr(), "Disconnected" );
    }

    if ( !m_callback.empty() )
        if ( error )
            m_callback( getSelfPtr(), "Wriing ERROR" );
        else
            m_callback( getSelfPtr(), "Writing OK" );
}
void RemotePC::connectedHandler( const boost::system::error_code& error ) {
    if ( error ) {
        if ( !m_callback.empty() )
            m_callback( getSelfPtr(), "Connecting ERROR" );
        m_status = ConnectionError;
    }
    else {
        if ( !m_callback.empty() )
            m_callback( getSelfPtr(), "Connection OK" );
        m_status = Available;
    }
}
void RemotePC::checkConnection() {

    m_socket.write_some( boost::asio::buffer( "ID 0 Check\n" ) );
    boost::asio::streambuf buf;
    boost::system::error_code error;
    boost::asio::read_until( m_socket, buf, "\n", error );
    if ( boost::asio::error::eof == error ||
         boost::asio::error::connection_reset == error ) {
        m_status = NotConencted;
    }
}
boost::shared_ptr<RemotePC> RemotePC::getSelfPtr() {
    return shared_from_this();
}
