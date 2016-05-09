#pragma once
#include "EXider.h"
namespace EXider {

	class FtpClient {
        const std::string m_login, m_password, m_dir;
        Poco::Net::FTPClientSession m_ftpSession;
        void changeDir( const std::string& Dir );
        void logIn( const std::string& login, const std::string& password );
        const std::pair<std::string,std::string> getDirAndNameFromPath( const std::string& FilePath );                                          // First - Dir, Second - Name
    public:
        FtpClient( const std::string& host, const std::string& login, const std::string& password, const std::string& defaultDir = "/EXider/" );
		bool connected() const;
        void connect();
        void reconnect();
		void disconnect();
		const std::string upload( const std::string& filePathToUpload, const std::string& filePathToSave = "" );
		const std::string download( const std::string& filePathToDownload, const std::string& filePathToSave = "");
	};

}