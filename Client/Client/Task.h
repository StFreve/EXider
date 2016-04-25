#pragma once
#include "EXider.h"
namespace EXider {
    class Task {
        boost::asio::io_service& m_io;
        mutable boost::recursive_mutex m_mutexForResult;                                // Mutex to read or write resulting information

        const std::string m_taskName;
        const size_t m_tID;
        PCList m_workingPCs;                                                            // PC's working on the task
        std::vector<std::string> m_result;                                              // Results received from RemotePC's

        const std::string m_downloadURL, m_executeCommand;                              // Download URL (ftp) and Executeing Command for RemotePC
        bool m_downloadsFiles;
        bool m_autoFree;                                                                // Automaticly delete PC from the task, when result is received

        void handler( boost::shared_ptr<RemotePC> fromPC, std::string result );	        // call-back function for RemotePC

        boost::thread m_thread;
    public:
        Task( boost::asio::io_service& io, const PCList& workingPCs, const std::string downloadURL, const std::string executeCommand, const std::string& taskName, int tID, bool autoFree = 0 );
        Task( boost::asio::io_service& io, const PCList& workingPCs, const std::string executeCommand, const std::string& taskName, int tID, bool autoFree = 0 );
        void run();
        void stop();
        const std::string getResult( const std::string& delimeter = " " ) const;
        const std::string getInfromation() const;
        const std::string& getName() const;
        const size_t getID() const;
    };
}