#pragma once
#include "EXider.h"
namespace EXider {
	
	class Client {
//		Logger m_logger;
//		Information m_info;

		std::list<RemotePC> m_pc;
		std::list<Task> m_jobs;
		boost::asio::io_service& m_io;
		FtpClient m_ftp;
		void commandHandler( const std::string command );
	public:
		Client( const boost::asio::io_service& io );
		void run();
		size_t addRemotePC(const std::string IP);
		void deleteRemotePC(size_t pcID);
		void deleteRemotePc( std::vector<size_t> pcIDs );
		void loadRemotePCs(const std::string fileToLoad);
		void saveRemotePCs(const std::string fileToSave);

		int createJob();
		void cancelJob( size_t jobID );
		void cancelJobs( std::vector<size_t> jobIDs );
	};
}