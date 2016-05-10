#pragma once
#include "EXider.h"
namespace EXider {
    class Task {
        boost::asio::io_service& m_io;
        mutable boost::recursive_mutex m_mutexForResult;                                // Mutex to read or write resulting information
        boost::function<void( const boost::shared_ptr<RemotePC>& ) > m_freePC;          // Function to free RemotePC
        
        const std::string m_taskName;
        const size_t m_tID;
        PCList m_workingPCs;                                                            // PC's working on the task
        std::vector<std::string> m_result;                                              // Results received from RemotePC's
        std::vector<size_t> m_workingStep;
        std::vector<std::string> m_commands;                                            // Download URL (ftp) and Executeing Command for RemotePC
        
        bool m_autoFree;                                                                // Automaticly delete PC from the task, when result is received
        bool m_isStoping;

        void handler( boost::shared_ptr<RemotePC> fromPC, std::string result );	        // call-back function for RemotePC
        void sendNextCommand( const boost::shared_ptr<RemotePC>& pc );
       
        boost::shared_ptr<boost::thread> m_thread;

        mutable boost::recursive_mutex m_mutexForDestroying;                             // Mutex to read or write resulting information

        
        void run();
    public:
        Task( boost::asio::io_service& io, boost::function<void( const boost::shared_ptr<RemotePC>&)> freePC, const PCList& workingPCs, const std::vector<std::string>& commands, const std::string& taskName, int tID, bool autoFree = 0 );
        ~Task();
        void start();
        void stop();
        const std::string getResult( const std::string& delimeter = " " ) const;
        const std::string getInfromation() const;
        const std::string& getName() const;
        const size_t getID() const;
        const std::string getFullName() const;
        const PCList getPCList() const;
    };
}