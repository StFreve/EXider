#include "EXider.h"

using namespace EXider;
	ProgramExecutor::ProgramExecutor( boost::asio::io_service& m_io, Server* server ) :
		m_io( m_io ), server( server ) {

	}
	void ProgramExecutor::run() {
		while ( true ) {
			boost::asio::deadline_timer timer( m_io, boost::posix_time::seconds( 2 ) );
			timer.wait();
			boost::recursive_mutex::scoped_lock sl( m_mutex );
			if ( !m_executeQueue.empty() ) {
				// Copying data from Queue and unlocking mutex to be able to add new program to Queue
				Program nowWorking = m_executeQueue.front().second;
				int nowWorking_id = m_executeQueue.front().first;

                // Setting up program path
				std::string fullPath = nowWorking.getFullPath();

                // Setting up arguments
                std::vector<std::string> arguments(nowWorking.getArguments());
				m_executeQueue.pop();
				sl.unlock();

				// Creating child process and waiting for result
				boost::process::context ctx;
				ctx.stdout_behavior = boost::process::capture_stream();
				program.reset(new boost::process::child( boost::process::launch( fullPath, arguments, ctx ) ) );
				program->wait();

				// Reading result from stdout
				boost::process::pistream &is = program->get_stdout();
				std::string result;
				std::getline( is, result );
                result = result.substr( 0, result.find_last_of( '\r' ) );
				// Closing the process
				program.reset();

				// Receiving the result
				sendResult( nowWorking_id, std::string("Result ") + result );
			}

		}
	}
	void ProgramExecutor::addProgram( int id, const Program& rhp ) {
		boost::recursive_mutex::scoped_lock sl( m_mutex );
		m_executeQueue.push( { id,rhp } );
	}
	void ProgramExecutor::terminateRunningProcess() {
		if ( program.get() )		// If process is running
			program->terminate();
	}
	void ProgramExecutor::stop() {
		boost::recursive_mutex::scoped_lock sl( m_mutex );
		std::queue<std::pair<int, Program> >().swap( m_executeQueue );
        if ( program.get() )		// If process is running
	    	program->terminate();
	}
	void ProgramExecutor::sendResult( int id, const std::string& result ) {
		server->send_message( id, result );
	}