#include "Listener.h"

#include <sstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace ba = boost::asio;
using errorT = boost::system::error_code;
using namespace std;
using namespace boost::property_tree;

Listener::Listener() :
	socket_(io_service_)
{
	
}

Listener::~Listener()
{
	try
	{
		if (!io_service_.stopped())
			io_service_.stop();
		if (serviceTh_.joinable())
			serviceTh_.join();
	}
	catch (const std::exception&) {}//ignore
}

void Listener::Start()
{
	baip::udp::endpoint listen_endpoint(baip::address_v4(), LISTENER_PORT);

	socket_.open(listen_endpoint.protocol());
	socket_.set_option(baip::udp::socket::reuse_address(true));
	socket_.bind(listen_endpoint);
	Recive();

	serviceTh_ = std::thread([this]() { io_service_.run(); });
}

void Listener::Recive()
{
	socket_.async_receive_from(
		ba::buffer(buffer_.data(), buffer_.size()),
		remoteEndpoint_,
		[this](const errorT& ec, size_t bytes)
	{
		if (!ec)
		{
			std::string msg(buffer_.data(), bytes);
			if (ValidateMsg(msg))
				Response();
		}
		//TO DO: Error handle
		Recive();
	});
}

void Listener::Response()
{
	std::string s("{\"getClockHost\":true}\n");
	socket_.async_send_to(
		ba::buffer(s.data(), s.size()),
		remoteEndpoint_,
		[](const errorT&, size_t) { }); //empty handler, ignore errors
}

bool Listener::ValidateMsg(const std::string& msg)
{
	try
	{
		stringstream ss(msg);
		ptree tree;
		read_json(ss, tree);
		bool val = tree.get<bool>("getClockHost");
		if (val == false)
			return true;
	}
	catch (exception& ex) {} //ignore

	return false;
}
