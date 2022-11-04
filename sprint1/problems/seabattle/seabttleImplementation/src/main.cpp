#ifdef WIN32
#include <sdkddkver.h>
#endif

#include "seabattle.h"

#include <atomic>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <iostream>
#include <optional>
#include <string>
#include <thread>
#include <string_view>
#include <sstream>
#include <iomanip>

namespace net = boost::asio;
using net::ip::tcp;
using namespace std::literals;

void PrintFieldPair(const SeabattleField& left, const SeabattleField& right) {
    auto left_pad = "  "s;
    auto delimeter = "    "s;
    std::cout << left_pad;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << delimeter;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << std::endl;
    for (size_t i = 0; i < SeabattleField::field_size; ++i) {
        std::cout << left_pad;
        left.PrintLine(std::cout, i);
        std::cout << delimeter;
        right.PrintLine(std::cout, i);
        std::cout << std::endl;
    }
    std::cout << left_pad;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << delimeter;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << std::endl;
}

template <size_t sz>
static std::optional<std::string> ReadExact(tcp::socket& socket) {
    boost::array<char, sz> buf;
    boost::system::error_code ec;

    net::read(socket, net::buffer(buf), net::transfer_exactly(sz), ec);

    if (ec) {
        return std::nullopt;
    }

    return {{buf.data(), sz}};
}

static bool WriteExact(tcp::socket& socket, std::string_view data) {
    boost::system::error_code ec;

    net::write(socket, net::buffer(data), net::transfer_exactly(data.size()), ec);

    return !ec;
}

class SeabattleAgent {
public:
    SeabattleAgent(const SeabattleField& field)
        : my_field_(field) {
    }

    void StartGame(tcp::socket& socket, bool my_initiative) {
        // TODO: реализуйте самостоятельно
        bool myTurn = my_initiative;
        while(!IsGameEnded()){
            if(myTurn){
                myTurn = MakeShot(socket);
            }else{
                myTurn = WaitEnemyShot(socket);
            }
        }
        myTurn ? std::cout << "You win!" << std::endl :
            std::cout << "You lose!" << std::endl;
    }

private:
    static std::optional<std::pair<int, int>> ParseMove(const std::string_view& sv) {
        if (sv.size() != 2) return std::nullopt;

        int p1 = sv[0] - 'A', p2 = sv[1] - '1';

        if (p1 < 0 || p1 > 8) return std::nullopt;
        if (p2 < 0 || p2 > 8) return std::nullopt;

        return {{p1, p2}};
    }

    static std::string MoveToString(std::pair<int, int> move) {
        char buff[] = {static_cast<char>(move.first) + 'A', static_cast<char>(move.second) + '1'};
        return {buff, 2};
    }

    void PrintFields() const {
        PrintFieldPair(my_field_, other_field_);
    }

    bool IsGameEnded() const {
        return my_field_.IsLoser() || other_field_.IsLoser();
    }

    // TODO: добавьте методы по вашему желанию

    bool MakeShot(tcp::socket& socket){
        boost::system::error_code ec;
        std::string shotCoordStr;
        /*Show fields*/
        PrintFields();
        /*Enter shot coordinate*/
        std::cout << "Enter shot coordinate"sv << std::endl;
        std::cin >> shotCoordStr;
        /*Formating shot coordinate*/
        shotCoordStr[0] = std::toupper(shotCoordStr[0]);
        auto shotCoord = ParseMove(shotCoordStr);
        shotCoordStr.push_back('\n');
        /*Send shot coordinate to enemy*/
        socket.write_some(net::buffer(shotCoordStr), ec);
        if (ec) {
            std::cout << "Error sending data"sv << std::endl;
            std::exit(1);
        }
        /*Get result of shot from enemy*/
        net::streambuf stream_buf;
        net::read_until(socket, stream_buf, '\n', ec);
        if (ec) {
            std::cout << "Error reading data"sv << std::endl;
            std::exit(1);
        }
        std::string shotResultStr{std::istreambuf_iterator<char>(&stream_buf),
                                std::istreambuf_iterator<char>()};
        /*Validate result of shot*/
        if(shotResultStr.length() > 2 and 
            (shotResultStr[0] < 0 or shotResultStr[0] > 2)){
            std::cout << "Invalid response"sv << std::endl;
            std::exit(1);
        }
        /*Mark shot on enemy field*/
        SeabattleField::ShotResult shotRes = static_cast<SeabattleField::ShotResult>(std::stoi(shotResultStr));
        MarkShot(other_field_, shotRes, shotCoord);
        return shotRes != SeabattleField::ShotResult::MISS;
    };

    bool WaitEnemyShot(tcp::socket& socket){
        boost::system::error_code ec;
        /*Show fields*/
        PrintFields();
        /*Get shot coordinate from enemy*/
        net::streambuf stream_buf;
        std::cout << "Waiting enemy shot"sv << std::endl;
        net::read_until(socket, stream_buf, '\n', ec);
        if (ec) {
            std::cout << "Error reading data"sv << std::endl;
            std::exit(1);
        }
        std::string shotCoordStr{std::istreambuf_iterator<char>(&stream_buf),
                                std::istreambuf_iterator<char>()};
        /*Formating shot coordinate*/
        shotCoordStr = shotCoordStr.substr(0, 2);
        std::cout << "Enemy shot to: "sv << shotCoordStr << std::endl;
        /*Calculate result of enemy shot*/
        auto shotCoord = ParseMove(shotCoordStr);
        auto shotRes = my_field_.Shoot(shotCoord->second, shotCoord->first);
        /*Send result of enemy shot to enemy*/
        std::stringstream shotResSS;
        shotResSS << static_cast<int>(shotRes) << "\n";
        socket.write_some(net::buffer(shotResSS.str()), ec);
        if (ec) {
            std::cout << "Error sending data"sv << std::endl;
            std::exit(1);
        }
        /*Mark shot on my field*/
        MarkShot(my_field_, shotRes, shotCoord);
        return shotRes == SeabattleField::ShotResult::MISS;
    };

    void MarkShot(SeabattleField& field,
                SeabattleField::ShotResult& shotRes,
                std::optional<std::pair<int, int>>& shotCoord){
        switch(shotRes){
            case SeabattleField::ShotResult::MISS:
                field.MarkMiss(shotCoord->second, shotCoord->first);
                std::cout << "MISS"sv << std::endl;
                break;
            case SeabattleField::ShotResult::HIT:
                field.MarkHit(shotCoord->second, shotCoord->first);
                std::cout << "HIT"sv << std::endl;
                break;
            case SeabattleField::ShotResult::KILL:
                field.MarkKill(shotCoord->second, shotCoord->first);
                std::cout << "KILL"sv << std::endl;
                break;
        }
    };

private:
    SeabattleField my_field_;
    SeabattleField other_field_;
    net::io_context io_context;
};

void StartServer(const SeabattleField& field, unsigned short port) {
    SeabattleAgent agent(field);

    // TODO: реализуйте самостоятельно
    net::io_context io_context;

    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));
    std::cout << "Waiting for connection..."sv << std::endl;

    boost::system::error_code ec;
    tcp::socket socket{io_context};
    acceptor.accept(socket, ec);

    if (ec) {
        std::cout << "Can't accept connection"sv << std::endl;
        std::exit(1);
    }
    
    agent.StartGame(socket, false);
};

void StartClient(const SeabattleField& field, const std::string& ip_str, unsigned short port) {
    SeabattleAgent agent(field);

    // TODO: реализуйте самостоятельно
    boost::system::error_code ec;
    auto endpoint = tcp::endpoint(net::ip::make_address(ip_str, ec), port);
    if (ec) {
        std::cout << "Wrong IP format"sv << std::endl;
        std::exit(1);
    }

    net::io_context io_context;
    tcp::socket socket{io_context};
    socket.connect(endpoint, ec);
    if (ec) {
        std::cout << "Can't connect to server"sv << std::endl;
        std::exit(1);
    }

    agent.StartGame(socket, true);
};

int main(int argc, const char** argv) {
    if (argc != 3 && argc != 4) {
        std::cout << "Usage: program <seed> [<ip>] <port>" << std::endl;
        return 1;
    }

    std::mt19937 engine(std::stoi(argv[1]));
    SeabattleField fieldL = SeabattleField::GetRandomField(engine);

    if (argc == 3) {
        StartServer(fieldL, std::stoi(argv[2]));
    } else if (argc == 4) {
        StartClient(fieldL, argv[2], std::stoi(argv[3]));
    }
}
