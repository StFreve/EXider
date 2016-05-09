#include "EXider.h"
using namespace EXider;// TODO
FtpClient::FtpClient( const std::string& host, const std::string& login, const std::string& password, const std::string& dir )
    : m_login( login )
    , m_password( password )
    , m_dir( dir )
    , m_ftpSession( host ) {
    Poco::Timespan time( 0, 0, 1, 0, 0 );
    m_ftpSession.setTimeout( time );

}
void FtpClient::logIn( const std::string& login, const std::string& password ) {
    m_ftpSession.login( login, password );
}
void FtpClient::changeDir( const std::string& dir ) {
    m_ftpSession.setWorkingDirectory( dir );

}
const std::pair<std::string, std::string> FtpClient::getDirAndNameFromPath( const std::string& FilePath ) {
    size_t lastSlash = FilePath.find_last_of( '/' );
    std::string fileName;
    std::string dir;
    if ( lastSlash != std::string::npos ) {
        dir = FilePath.substr( 0, lastSlash + 1 );
        fileName = FilePath.substr( lastSlash + 1 );
    }
    else {
        fileName = FilePath;
    }
    return make_pair( dir, fileName );
}
bool FtpClient::connected() const {
    return m_ftpSession.isLoggedIn();
}
void FtpClient::connect() {
    logIn( m_login, m_password );
    changeDir( m_dir );
}
void FtpClient::disconnect() {
    m_ftpSession.close();
}
void FtpClient::reconnect() {
    if ( connected() )
        disconnect();
    connect();
}
const std::string FtpClient::upload( const std::string& filePathToUpload, const std::string& filePathToSave ) {
    if ( !connected() )
        connect();

    auto dirAndNameToUpload = getDirAndNameFromPath( filePathToUpload );
    auto dirAndNameToSave = getDirAndNameFromPath( filePathToSave );

    if ( dirAndNameToUpload.first.empty() ) {
        dirAndNameToUpload.first = boost::filesystem::current_path().string() + "/";
    }
    if ( dirAndNameToSave.first.empty() ) {
        dirAndNameToSave.first = m_dir;
    }
    if ( dirAndNameToSave.second.empty() ) {
        dirAndNameToSave.second = dirAndNameToUpload.second;
    }


    changeDir( dirAndNameToSave.first );

    m_ftpSession.setFileType( Poco::Net::FTPClientSession::TYPE_BINARY );
    std::ostream& os = m_ftpSession.beginUpload( dirAndNameToSave.second );
    std::ifstream ifs( dirAndNameToUpload.first + dirAndNameToUpload.second, std::ofstream::binary );
    os << ifs.rdbuf();
    m_ftpSession.endUpload();

    return dirAndNameToSave.first + dirAndNameToSave.second;
}
const std::string FtpClient::download( const std::string& filePathToDownload, const std::string& filePathToSave ) {
    if ( !connected() )
        connect();

    auto dirAndNameToDownload = getDirAndNameFromPath( filePathToDownload );
    auto dirAndNameToSave = getDirAndNameFromPath( filePathToSave );

    if ( dirAndNameToDownload.first.empty() ) {
        dirAndNameToDownload.first = m_dir;
    }
    if ( dirAndNameToSave.first.empty() ) {
        dirAndNameToSave.first = boost::filesystem::current_path().string() + "/";
    }
    if ( dirAndNameToSave.second.empty() ) {
        dirAndNameToSave.second = dirAndNameToDownload.second;
    }

    changeDir( dirAndNameToDownload.first );

    m_ftpSession.setFileType( Poco::Net::FTPClientSession::TYPE_BINARY );
    std::istream& is = m_ftpSession.beginDownload( dirAndNameToDownload.second );
    std::ofstream ofs( dirAndNameToSave.first + dirAndNameToSave.second, std::ofstream::out | std::ofstream::app | std::ofstream::binary );
    ofs << is.rdbuf();
    m_ftpSession.endDownload();
    return dirAndNameToSave.first + dirAndNameToSave.second;
}

