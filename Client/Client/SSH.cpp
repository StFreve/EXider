#include "EXider.h"
using namespace EXider;
int SSH::connect( ssh_channel & channel, const std::string & login, const std::string & password, const std::string & ip, int port ) {
    //Initilization
    int rc;

    //Set options for SSH connection
    ssh_options_set( m_session, SSH_OPTIONS_HOST, ip.c_str() );
#ifndef NDEBUG 
   /* int verbosity = SSH_LOG_PROTOCOL;
    ssh_options_set( m_session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity );*/
#endif
    ssh_options_set( m_session, SSH_OPTIONS_PORT, &port );
    ssh_options_set( m_session, SSH_OPTIONS_USER, login.c_str() );

    //Connect to server

    rc = ssh_connect( m_session );
    if ( rc != SSH_OK )
        return rc;

    rc = ssh_userauth_password( m_session, NULL, password.c_str() );

    if ( rc != SSH_AUTH_SUCCESS )
        return rc;


    channel = ssh_channel_new( m_session );

    if ( channel == NULL )
        return SSH_ERROR;

    rc = ssh_channel_open_session( channel );
    if ( rc != SSH_OK )
        return rc;


    return SSH_OK;
}
int SSH::sendCommand( ssh_channel & channel, const std::string & command ) {
    int rc = ssh_channel_request_exec( channel, command.c_str() );
    if ( rc != SSH_OK )
        return rc;

    return SSH_OK;
}
int SSH::readResult( ssh_channel & channel, std::string & result ) {
    char buffer[ 64 ];
    unsigned int nbytes;

    while ( ( nbytes = ssh_channel_read( channel, buffer, sizeof( buffer ), 0 ) ) > 0 ) {
     /*   if ( fwrite( buffer, 1, nbytes, stdout ) != nbytes )
            return SSH_ERROR;*/

        result += std::string( buffer );
    }
    return SSH_OK;
}
SSH::SSH()
    : m_session( ssh_new() ) {

}
void SSH::run( const std::string & login, const std::string & password, const std::vector<boost::asio::ip::address>& vecToConnect, const std::string& command ) {
    ssh_channel channel;
    int rc;
    std::string result;
    for ( auto ip : vecToConnect ) {
        if ( connect( channel, login, password, ip.to_string(), 22 ) == SSH_OK ) {
            rc = sendCommand( channel, command );
            if ( readResult( channel, result ) != SSH_OK );
        }
        ssh_channel_free( channel );
    }
}
