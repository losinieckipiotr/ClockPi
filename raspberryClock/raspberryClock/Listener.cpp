#include "Listener.h"

namespace ba = boost::asio;
using errorT = boost::system::error_code;

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
	baip::udp::endpoint listen_endpoint(
		baip::address_v4::from_string("0.0.0.0"),
		LISTENER_PORT);
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
			//TO DO: check data?
			Response();
		}
	});
}

void Listener::Response()
{
	//TO DO: WHAT Response?
	socket_.async_send_to(
		ba::buffer(buffer_, buffer_.size()),
		remoteEndpoint_,
		[](const errorT&, size_t) { }); // empty handler
	Recive();
}
