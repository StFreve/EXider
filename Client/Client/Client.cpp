#include "Client.h"
using namespace EXider;
Client::Client( boost::asio::io_service & io )
    : m_io( io )
    , m_info( this )
    , m_nextTaskID( 0 )
    , m_ftpHost( "31.170.164.154" )
    , m_ftpLogin( "u823219472" )
    , m_ftpPassword( "459s62nqctm5b" ) {

}

void Client::run() {
    boost::asio::io_service::work work( m_io );
    std::string comLine;
    CommandParser parser;
    boost::system::error_code er;
    while ( true ) {
        std::cout << "EXider >> ";
        std::getline( std::cin, comLine );
        if ( comLine.empty() || comLine.find_first_not_of( ' ' ) == std::string::npos )
            continue;
        parser.parseCommand( comLine );
        if ( parser.command() == "pc" ) {
            auto arg = parser.arguments();
            if ( arg.empty() ) {
                std::cout << "No arguments of command \"pc\"" << std::endl;
            }
            else if ( arg[ 0 ].argument == "list" ) {
                size_t Status = 0;
                bool badStatus = false;
                if ( !arg[ 0 ].parameters.empty() ) {
                    m_info.wrongParameter( arg[ 0 ].parameters[ 0 ] );
                    continue;
                }
                bool params = false;
                for ( int i = 1; i < arg.size(); ++i ) {
                    if ( !arg[ i ].parameters.empty() ) {
                        m_info.wrongParameter( arg[ i ].parameters[ 0 ] );
                        params = true;
                        break;
                    }
                    if ( arg[ i ].argument == "available" )
                        Status |= Available;
                    else if ( arg[ i ].argument == "busy" )
                        Status |= Busy;
                    else if ( arg[ i ].argument == "not-connected" )
                        Status |= NotConencted;
                    else {
                        m_info.wrongArgument( arg[ i ].argument );
                        badStatus = true;
                        break;
                    }
                }
                if ( !badStatus && !params ) {
                    m_info.pcList( Status == 0 ? Available : Status );
                }

            }
            else if ( arg[ 0 ].argument == "add" || arg[ 0 ].argument == "delete" || arg[ 0 ].argument == "reconnect" || arg[ 0 ].argument == "install" ) {
                std::vector<boost::asio::ip::address> pc_ip;
                bool badIP = false;
                for ( int i = 1; i < arg.size(); ++i ) {
                    if ( arg[ i ].argument == "ip" ) {
                        for ( int j = 0; j < arg[ i ].parameters.size(); ++j ) {
                            auto ip = boost::asio::ip::address::from_string( arg[ i ].parameters[ j ], er );
                            if ( er ) {
                                badIP = true;
                            }
                            else
                                pc_ip.push_back( ip );
                        }
                    }
                    else if ( arg[ i ].argument == "f" ) {
                        for ( int j = 0; j < arg[ i ].parameters.size(); ++j ) {
                            std::ifstream is( arg[ i ].parameters[ j ] );
                            if ( is.bad() ) {
                                badIP = true;
                                continue;
                            }
                            std::string strIP;
                            while ( is >> strIP ) {
                                auto ip = boost::asio::ip::address::from_string( strIP, er );
                                if ( er ) {
                                    badIP = true;
                                }
                                else
                                    pc_ip.push_back( ip );
                            }
                        }
                    }
                }
                if ( badIP ) {
                    m_info.warning( "Not all IPs will be processed. Some of them don't look like IP Addresses." );
                }
                if ( arg[ 0 ].argument == "add" )
                    addRemotePCs( pc_ip );
                else if ( arg[ 0 ].argument == "delete" )
                    deleteRemotePCs( pc_ip );
                else if ( arg[ 0 ].argument == "reconnect" ) {
                    if ( pc_ip.empty() ) {
                        for ( auto pc : m_notConnectedPC )
                            pc_ip.push_back( pc->getIP() );
                    }
                    reconnectRemotePCs( pc_ip );
                }
                else if ( arg[ 0 ].argument == "install" ) {
                    installRemotePC( pc_ip );
                }
            }
            else if ( arg[ 0 ].argument == "save" ) {
                if ( arg.size() != 1 ) {
                    m_info.wrongArgument( arg[ 1 ].argument );
                }
                else if ( arg[ 0 ].parameters.size() != 1 ) {
                    m_info.print( "No file name found!" );
                }
                else
                    saveRemotePCs( arg[ 0 ].parameters[ 0 ] );
            }
            else if ( arg[ 0 ].argument == "help" ) {
                m_info.help( Information::CommandType::PC );
            }
            else {
                m_info.wrongArgument( arg[ 0 ].argument );
            }
        }
        else if ( parser.command() == "task" ) {
            auto arg = parser.arguments();
            if ( arg.empty() ) {
                std::cout << "No arguments of command \"task\"" << std::endl;
            }
            else if ( arg[ 0 ].argument == "status" && arg.size() == 1 ) {

                m_info.taskList();

            }
            else if ( arg[ 0 ].argument == "new" ) {
                bool badArgument = false;
                bool moreThanOneName = false;
                bool startAfterCreating = false;
                bool autoFree = false;
                int computersWillBeUsed = 1;
                std::string programArguments = "";
                std::string programPath = "";
                std::string taskName = "";
                std::vector<std::string> filesToUpload;
                if ( !arg[ 0 ].parameters.empty() )
                    programPath = arg[ 0 ].parameters[ 0 ];
                for ( int i = 1; i < arg.size(); ++i ) {
                    if ( ( arg[ i ].argument == "c" || arg[ i ].argument == "counter" ) && arg[ i ].parameters.size() == 1 ) {
                        computersWillBeUsed = std::atoi( arg[ i ].parameters[ 0 ].c_str() );
                    }
                    else if ( ( arg[ i ].argument == "p" || arg[ i ].argument == "path" ) && arg[ i ].parameters.size() == 1 ) {
                        programPath = arg[ i ].parameters[ 0 ];
                    }
                    else if ( ( arg[ i ].argument == "a" || arg[ i ].argument == "arg" ) && arg[ i ].parameters.size() == 1 ) {
                        programArguments = arg[ i ].parameters[ 0 ];
                    }
                    else if ( ( arg[ i ].argument == "f" || arg[ i ].argument == "auto-free" ) && arg[ i ].parameters.empty() ) {
                        autoFree = true;
                    }
                    else if ( ( arg[ i ].argument == "n" || arg[ i ].argument == "name" ) && arg[ i ].parameters.size() == 1 ) {
                        moreThanOneName |= !taskName.empty();
                        taskName = arg[ i ].parameters[ 0 ];
                    }
                    else if ( ( arg[ i ].argument == "s" || arg[ i ].argument == "start" ) && arg[ i ].parameters.empty() ) {
                        startAfterCreating = true;
                    }
                    else if ( ( arg[ i ].argument == "u" || arg[ i ].argument == "upload" ) ) {
                        filesToUpload.insert( filesToUpload.end(), arg[ i ].parameters.begin(), arg[ i ].parameters.end() );
                    }
                    else {
                        m_info.wrongArgument( arg[ i ].argument );
                        badArgument = true;
                        break;
                    }
                }
                if ( badArgument )
                    continue;

                newTask( ( taskName.empty() ? std::string( "Task " ) + boost::lexical_cast<std::string>( m_nextTaskID ) : taskName ), m_nextTaskID, programPath, programArguments, filesToUpload, computersWillBeUsed, autoFree, startAfterCreating );
                ++m_nextTaskID;
            }
            else if ( arg[ 0 ].argument == "start" || arg[ 0 ].argument == "stop" || arg[ 0 ].argument == "discard" || arg[ 0 ].argument == "status" ) {

                std::set<boost::shared_ptr<Task> > taskList;
                bool badArgument = false;
                for ( size_t i = 1; i < arg.size(); ++i ) {
                    if ( arg[ i ].argument == "id" ) {
                        for ( size_t id_i = 0; id_i < arg[ i ].parameters.size(); ++id_i ) {
                            size_t idToFind = std::atoi( arg[ i ].parameters[ id_i ].c_str() );
                            bool taskFound = false;
                            for ( size_t i = 0; i < m_tasks.size(); ++i ) {
                                if ( m_tasks[ i ]->getID() == idToFind ) {
                                    taskList.insert( m_tasks[ i ] );
                                    taskFound = true;
                                    break;
                                }
                            }

                            if ( !taskFound )
                                m_info.warning( boost::str( boost::format( "Task with ID '%1%' wasn't found!" ) % idToFind ) );

                        }
                    }
                    else if ( arg[ i ].argument == "n" || arg[ i ].argument == "name" ) {
                        for ( size_t name_i = 0; name_i < arg[ i ].parameters.size(); ++name_i ) {
                            std::string nameToFind = arg[ i ].parameters[ name_i ];
                            bool taskFound = false;
                            for ( size_t i = 0; i < m_tasks.size(); ++i ) {
                                if ( m_tasks[ i ]->getName() == arg[ i ].parameters[ name_i ] ) {
                                    taskList.insert( m_tasks[ i ] );
                                    taskFound = true;
                                }
                            }

                            if ( !taskFound )
                                m_info.warning( boost::str( boost::format( "Task with name '%1%' wasn't found!" ) % nameToFind ) );

                        }
                    }
                    else {
                        m_info.wrongArgument( arg[ i ].argument );
                        badArgument = true;
                        break;
                    }
                }
                if ( badArgument )
                    continue;
                if ( arg[ 0 ].argument == "start" )
                    startTasks( taskList );
                else if ( arg[ 0 ].argument == "stop" )
                    stopTasks( taskList );
                else if ( arg[ 0 ].argument == "discard" )
                    discardTasks( taskList );
                else if ( arg[ 0 ].argument == "status" )
                    m_info.taskInformation( taskList );
            }
            else if ( arg[ 0 ].argument == "help" ) {
                m_info.help( Information::CommandType::Task );
            }
            else {
                m_info.wrongArgument( arg[ 0 ].argument );
            }
        }
        else if ( parser.command() == "help" ) {
            m_info.help();
        }
        else if ( parser.command() == "exit" ) {
            break;
        }
        else {
            m_info.wrongArgument( parser.command() );
        }
    }


}
bool Client::connectRemotePC( boost::shared_ptr<RemotePC>& pc ) {
    return pc->connect();
}
void Client::installRemotePC( const std::vector<boost::asio::ip::address>& IPs ) {
    SSH ssh;
    std::string login, password;
    std::cout << "Login: ";
    std::cin >> login;
    std::cout << "Password: ";
    std::cin >> password;
    std::string  command = std::string( "rm EXider_Server.zip; rm -r EXider_Server; wget http://blacklogger.hol.es/EXider_Server.zip; unzip EXider_Server.zip; cd EXider_Server; echo " ) + password + " | sudo -S chmod +x install.sh; echo " + password + " | sudo -S ./install.sh;  echo " + password + " | sudo -S service exider_server start > /dev/null 2>&1";
    ssh.run( login, password, IPs, command );

    // Clear cin
    std::getline( std::cin, login );
}
void Client::addRemotePCs( const std::vector<boost::asio::ip::address>& IPs ) {
    for ( auto ip : IPs ) {
        boost::shared_ptr<RemotePC> pc( new RemotePC( m_io, ip ) );
        if ( m_notConnectedPC.find( pc ) != m_notConnectedPC.end() ||
             m_freePC.find( pc ) != m_freePC.end() ||
             m_busyPC.find( pc ) != m_busyPC.end() ) {	// Check, if the IP Address is already in one of the containers.
            m_info.warning( std::string( "Remote PC with IP Address: " ) + ip.to_string() + " has been already added." );
            continue;
        }
        if ( connectRemotePC( pc ) ) {
            m_freePC.insert( pc );
            m_info.print( std::string( "Remote PC with IP " ) + ip.to_string() + " was connected." );
        }
        else {
            m_notConnectedPC.insert( pc );
            m_info.print( std::string( "Remote PC with IP " ) + ip.to_string() + " wasn't connected." );
        }
    }
}
void Client::deleteRemotePCs( const std::vector<boost::asio::ip::address>& IPs ) {
    for ( size_t i = 0; i < IPs.size(); ++i ) {
        boost::shared_ptr<RemotePC> pcToDelete( new RemotePC( m_io, IPs[ i ] ) );
        if ( m_busyPC.find( pcToDelete ) != m_busyPC.end() ) {
            m_info.warning( boost::str( boost::format( "Remote PC with IP: %1% can't be deleted while it is in use." ) % IPs[ i ].to_string() ) );
        }
        else {
            m_freePC.erase( pcToDelete );
            m_notConnectedPC.erase( pcToDelete );
            m_info.warning( boost::str( boost::format( "Remote PC with IP: %1% was deleted." ) % IPs[ i ].to_string() ) );
        }
    }
}

void Client::reconnectRemotePCs( const std::vector<boost::asio::ip::address>& IPs ) {
    for ( size_t i = 0; i < IPs.size(); ++i ) {
        boost::shared_ptr<RemotePC> pc( new RemotePC( m_io, IPs[ i ] ) );
        if ( m_notConnectedPC.find( pc ) == m_notConnectedPC.end() ) {
            m_info.warning( boost::str( boost::format( "Remote PC with IP: %1% can't be found." ) % IPs[ i ].to_string() ) );
        }
        else {
            if ( connectRemotePC( pc ) ) {
                m_notConnectedPC.erase( pc );
                m_freePC.insert( pc );
                m_info.print( std::string( "Remote PC with IP " ) + IPs[ i ].to_string() + " was connected." );
            }
            else {
                m_info.print( std::string( "Remote PC with IP " ) + IPs[ i ].to_string() + " wasn't connected." );
            }
        }
    }
}


void Client::saveRemotePCs( const std::string& fileToSave ) {
    std::ofstream os( fileToSave );
    if ( os.bad() ) {
        std::cout << "Can't open file " << fileToSave << std::endl;
    }
    else {
        for ( auto it : m_freePC )
            os << it->getIP().to_string() << " ";
        for ( auto it : m_busyPC )
            os << it->getIP().to_string() << " ";
    }
}

void Client::newTask( const std::string& taskName, size_t taskID, const std::string & filePath, const std::string & arguments, const std::vector<std::string>& filesToUpload, int computersToUse, bool autoFree, bool startAfterCreating ) {
    if ( computersToUse > m_freePC.size() ) {
        return;
    }
    std::vector<std::string> commands;

    // Creating working PC List
    PCList listForTask;
    for ( size_t i = 0; i < computersToUse; ++i ) {
        if ( ( *m_freePC.begin() )->status() == NotConencted ) {
            m_notConnectedPC.insert( *m_freePC.begin() );
            m_freePC.erase( m_freePC.begin() );
            continue;
        }
        listForTask.insert( *m_freePC.begin() );
        m_freePC.erase( m_freePC.begin() );
    }

    // Checking if PC's qty is enough.
    if ( listForTask.size() != computersToUse ) {
        for ( auto pc : listForTask ) {
            m_freePC.insert( pc );
        }
        m_info.error( std::string( "Not enough computers to process current task. " ) + boost::lexical_cast<std::string>( m_freePC.size() ) + " available." );
        return;
    }
    else {
        for ( auto pc : listForTask ) {
            m_busyPC.insert( pc );
        }
    }

    // Uploading file to FTP
    if ( !filesToUpload.empty() ) {
        try {
            FtpClient ftp( m_ftpHost, m_ftpLogin, m_ftpPassword );
            for ( size_t i = 0; i < filesToUpload.size(); ++i )
                commands.push_back( boost::str( boost::format( "Download %1%" ) % ( ftp.upload( filesToUpload[ i ] ) ) ) );
        }
        catch ( std::exception& e ) {
            m_info.error( "Cannot upload file to FTP! Task won't be created." );
            return;
        }
    }

    commands.push_back( boost::str( boost::format( "Run %1% %2%" ) % filePath % arguments ) );

    m_tasks.push_back( boost::shared_ptr<Task>( new Task( m_io, boost::bind( &Client::freeRemotePC, this, _1 ), listForTask, commands, taskName, taskID, autoFree ) ) );

    m_info.print( str( boost::format( "Task '%1%' created." ) % m_tasks.back()->getFullName() ) );

    if ( startAfterCreating ) {
        startTask( m_tasks.back() );
    }

}
void Client::freeRemotePC( const boost::shared_ptr<RemotePC>& pc ) {
    pc->clearCallBackFunction();
    if ( m_busyPC.find( pc ) != m_busyPC.end() ) { // If wasn't already deleted from the PC list
        m_busyPC.erase( pc );
        if ( pc->status() == NotConencted ) {
            m_notConnectedPC.insert( pc );
        }
        else
            m_freePC.insert( pc );
    }
    pc->setInWork( false );
}
inline void Client::startTask( boost::shared_ptr<Task>& task ) {
    task->start();
}
inline void Client::stopTask( boost::shared_ptr<Task>& task ) {
    task->stop();
}
inline void Client::discardTask( boost::shared_ptr<Task>& task ) {
    // TODO: Task Deleting needs improving
    for ( size_t i = 0; i < m_tasks.size(); ++i ) {
        if ( task == m_tasks[ i ] ) {
            try {
                m_tasks.erase( m_tasks.begin() + i );
            }
            catch ( std::exception& e ) {
                m_info.error( "Task was discarded with problems." );
            }
        }
    }
}
void Client::startTasks( std::set<boost::shared_ptr<Task> >& taskList ) {
    for ( auto task : taskList ) {
        startTask( task );
    }
}
void Client::stopTasks( std::set<boost::shared_ptr<Task> >& taskList ) {
    for ( auto task : taskList ) {
        stopTask( task );
    }
}
void Client::discardTasks( std::set<boost::shared_ptr<Task> >& taskList ) {
    for ( auto task : taskList ) {
        discardTask( task );
    }
}
