#include <iostream>
#include <array>

#include <boost/asio.hpp>

using namespace std;

namespace ba = boost::asio;
namespace baip = boost::asio::ip;

enum
{
	BUFFER_SIZE = 1024,
	SERVER_PORT = 2016,
	LISTENER_PORT = 2017
};

int main(int argc, char* argv[])
{
	try
	{
		boost::asio::io_service io_service;

		baip::udp::endpoint udpEndpoint( ba::ip::address_v4::broadcast(), LISTENER_PORT);
		baip::udp::socket udpSocet(io_service, udpEndpoint.protocol());
		baip::udp::endpoint serverEp;
		udpSocet.set_option(baip::udp::socket::reuse_address(true));
		udpSocet.set_option(ba::socket_base::broadcast(true));

		string hostRequest = "{\"getClockHost\":false}";
		udpSocet.send_to(
			ba::buffer(hostRequest.data(), hostRequest.size()),
			udpEndpoint);

		std::array<char, BUFFER_SIZE> buffArr;

		size_t bytes = udpSocet.receive_from(ba::buffer(buffArr.data(), BUFFER_SIZE), serverEp);
		string response(buffArr.data(), bytes);
		if (response != "{\"getClockHost\":true}")
			return 1;

		ba::ip::tcp::socket tcpSocket(io_service);
		ba::ip::tcp::endpoint tcoEndpoint(serverEp.address(), SERVER_PORT);
		tcpSocket.connect(tcoEndpoint);

		string msg;
		while (true)
		{
			cout << "Enter message: " << endl;
			getline(cin, msg);

			ba::write(tcpSocket, boost::asio::buffer(msg.data(), msg.length()));

			auto recBytes = tcpSocket.read_some(ba::buffer(buffArr.data(), BUFFER_SIZE));
			cout << "Reply is: " << endl;
			msg = string(buffArr.data(), recBytes);
			cout << msg << endl;
		}
	}
	catch (std::exception& e)
	{
		cout << "Exception: " << e.what() << "\n";
	}

	return 0;
}