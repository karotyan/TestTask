#include <boost/asio.hpp>
#include <fstream>
#include <iostream>

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_context io_context;

        // Підключення до сервера
        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", "12345");
        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        // Відкриття файлу для запису отриманих даних
        std::ofstream outputFile("received_file.txt", std::ios::binary);
        if (!outputFile.is_open()) {
            std::cerr << "Could not open the output file!" << std::endl;
            return 1;
        }

        // Буфер для отримання даних
        std::array<char, 1024> buffer;
        boost::system::error_code error;
        std::size_t bytes_transferred;

        // Читання з сокету до тих пір, поки з'єднання не буде закрито
        while ((bytes_transferred = socket.read_some(boost::asio::buffer(buffer), error)) > 0) {
            outputFile.write(buffer.data(), bytes_transferred);
        }

        // Перевірка, чи з'єднання закрилося коректно
        if (error != boost::asio::error::eof) {
            throw boost::system::system_error(error);
        }

        std::cout << "File received and saved.\n";
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}