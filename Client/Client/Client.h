#pragma once
#include "EXider.h"
namespace EXider {

    class Client {
        boost::asio::io_service& m_io;

        friend Information;
        Information m_info;

        size_t m_nextTaskID;
        //		Logger m_logger;
        //		FtpClient m_ftp;
        PCList m_freePC, m_busyPC, m_notConnectedPC;
        std::vector<boost::shared_ptr<Task> > m_tasks;

    public:
        Client( boost::asio::io_service& io );
        void run();
        // RemotePCs
        void addRemotePCs( const std::vector<boost::asio::ip::address>&  IP );
        void deleteRemotePCs( const std::vector<boost::asio::ip::address>&  IP );
        void saveRemotePCs( const std::string& fileToSave );
        void freeRemotePC( const boost::shared_ptr<RemotePC>& pc );

        // Tasks
        void newTask( const std::string& taskName, size_t taskID, const std::string& filePath, const std::string& arguments, int computersToUse, bool autoFree, bool startAfterCreating, bool withoutSendingProgram );
        void startTasks( std::set<boost::shared_ptr<Task> >& tasksToStart );
        void stopTasks( std::set<boost::shared_ptr<Task> >& tasksToStart );
        void discardTasks( std::set<boost::shared_ptr<Task> >& tasksToStart );
        inline void startTask( boost::shared_ptr<Task>& taskToStart );
        inline void stopTask( boost::shared_ptr<Task>& taskToStop );
        inline void discardTask( boost::shared_ptr<Task>& taskToStop );
    };
}