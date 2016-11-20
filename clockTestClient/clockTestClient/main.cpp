#include <iostream>
#include <array>
#include <string>
#include <sstream>

#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <vector>

#include "Result.hpp"

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

class Response
{
public:
	Response(string json)
	{
		stringstream ss(json);

		pt::ptree tree;
		pt::read_json(ss, tree);
		auto& respNode = tree.get_child("response");

		id = respNode.get<int>("id");
		state = respNode.get<int>("state");
		length = respNode.get<int>("length");
	}
	Response(int id, int state, int length)
		: id(id), state(state), length(length) { }

	string ToJSON()
	{
		pt::ptree tree;
		pt::ptree response;

		response.put("id", id);
		response.put("state", state);
		response.put("length", length);

		tree.add_child("response", response);

		stringstream ss;
		pt::write_json(ss, tree);
		return ss.str();
	}

	int id;
	int state;
	int length;
};

int ctr = 0;

string getRequest(const string& reqName)
{
	pt::ptree tree;
	pt::ptree request;

	request.put("id", ctr++);
	request.put("name", reqName);

	tree.add_child("request", request);

	stringstream ss;
	pt::write_json(ss, tree);
	return ss.str();
}

string getResult()
{
	return getRequest("getLastResult");
}

string getHistory()
{
	return getRequest("getResultsHistory");
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
			if (msg == "result")
			{
				msg = getResult();
				ba::write(tcpSocket, ba::buffer(msg.data(), msg.length()));
				auto recBytes = tcpSocket.read_some(ba::buffer(buffArr.data(), BUFFER_SIZE));

				msg = string(buffArr.data(), recBytes);

				cout << msg << endl;

				Response resp(msg);
				if (resp.state == 1)
				{
					size_t l = resp.length;
					vector<char> buf(l);
					auto recBytes2 = tcpSocket.read_some(ba::buffer(buf.data(), buf.size()));
					string msg2(buf.data(), recBytes2);

					cout << msg2 << endl;
				}
			}
				
			else if (msg == "history")
			{
				msg = getHistory();
				ba::write(tcpSocket, ba::buffer(msg.data(), msg.length()));
				auto recBytes = tcpSocket.read_some(ba::buffer(buffArr.data(), BUFFER_SIZE));

				msg = string(buffArr.data(), recBytes);

				cout << msg << endl;

				Response resp(msg);
				if (resp.state == 1)
				{
					size_t bytesToRecive = resp.length;
					while (bytesToRecive > 0)
					{
						vector<char> buf(bytesToRecive);
						auto recBytes2 = tcpSocket.read_some(ba::buffer(buf.data(), buf.size()));
						bytesToRecive -= recBytes2;

						string msg2(buf.data(), recBytes2);
						cout << msg2 << endl;
					}
				}
			}	
			else
				continue;
		}
	}
	catch (std::exception& e)
	{
		cout << "Exception: " << e.what() << endl;
	}

	return 0;
}