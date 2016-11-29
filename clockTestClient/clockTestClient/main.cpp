#include <iostream>
#include <array>
#include <string>
#include <sstream>

#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <vector>

#include "Response.h"
#include "Result.h"
#include "Request.h"

using namespace std;

namespace ba = boost::asio;
namespace baip = boost::asio::ip;
namespace pt = boost::property_tree;

enum
{
	BUFFER_SIZE = 1024,
	SERVER_PORT = 2016,
	LISTENER_PORT = 2017
};

int ctr = 0;

string getResult()
{
	Request r;
	r.id = ctr++;
	r.name = "getLastResult";

	return r.ToJSON();
}

string getHistory()
{
	Request r;
	r.id = ctr++;
	r.name = "getResultsHistory";

	return r.ToJSON();
}

string getAlarm()
{
	Request r;
	r.id = ctr++;
	r.name = "getAlarmTime";

	return r.ToJSON();
}
string setAlarm()
{
	Request r;
	r.id = ctr++;
	r.name = "setClockAlarm";

	r.params.emplace("hour", to_string(16));
	r.params.emplace("minute", to_string(0));

	return r.ToJSON();
}
string disableAlarm()
{
	Request r;
	r.id = ctr++;
	r.name = "disableClockAlarm";

	return r.ToJSON();
}

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

		string hostRequest = "{\"getClockHost\":false}\n";
		udpSocet.send_to(
			ba::buffer(hostRequest.data(), hostRequest.size()),
			udpEndpoint);

		std::array<char, BUFFER_SIZE> buffArr;

		size_t bytes = udpSocet.receive_from(ba::buffer(buffArr.data(), BUFFER_SIZE), serverEp);
		string response(buffArr.data(), bytes);
		if (response != "{\"getClockHost\":true}\n")
			return 1;

		ba::ip::tcp::socket tcpSocket(io_service);
		ba::ip::tcp::endpoint tcoEndpoint(serverEp.address(), SERVER_PORT);
		tcpSocket.connect(tcoEndpoint);

		string msg;
		while (true)
		{
			cout << "Enter message: " << endl;
			getline(cin, msg);

			if (msg == "result")
			{
				msg = getResult();
			}
			else if (msg == "history")
			{
				msg = getHistory();
			}
			else if (msg == "time")
			{
				msg = getAlarm();
			}
			else if (msg == "alarm")
			{
				msg = setAlarm();
			}
			else if (msg == "disable")
			{
				msg = disableAlarm();
			}
			else
			{
				continue;
			}

			ba::write(tcpSocket, ba::buffer(msg.data(), msg.length()));
			auto recBytes = tcpSocket.read_some(ba::buffer(buffArr.data(), BUFFER_SIZE));

			msg = string(buffArr.data(), recBytes);
			cout << msg << endl;

			Response resp(msg);
			if (resp.state == 1 && resp.length > 0)
			{
				size_t bytesToRecive = resp.length;
				vector<char> respBuff(resp.length);
				boost::system::error_code ec;
				ba::read(
					tcpSocket,
					ba::buffer(respBuff.data(), respBuff.size()),
					ec);
				if (!ec)
				{
					string msg2(respBuff.data(), respBuff.size());
					cout << msg2 << endl;
				}
				else
				{
					cout << ec.message() << endl;
					return 0;
				}
			}
		}
	}
	catch (std::exception& e)
	{
		cout << "Exception: " << e.what() << endl;
	}

	return 0;
}