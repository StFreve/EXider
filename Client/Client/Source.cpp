#include "EXider.h"
int main() {
    try {
        boost::asio::io_service io;
        EXider::Client client( io );
        client.run();
        return 0;
    }
    catch ( std::exception& ex ) {
        std::cerr << ex.what() << std::endl;
    }
}