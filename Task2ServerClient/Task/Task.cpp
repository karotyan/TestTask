#include <boost/asio.hpp>
#include <fstream>
#include <iostream>

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_context io_context;

        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 12345));
        std::cout << "Server is running on port 12345...\n";

        for (;;) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);

            std::ifstream file("response.txt", std::ios::binary);
            if (!file) {
                std::cerr << "File not found!\n";
                continue;
            }

            std::string file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

            boost::asio::write(socket, boost::asio::buffer(file_content));
            socket.close();
            std::cout << "File sent to client.\n";
        }
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}