#include "Session.h"

#include <string>
#include <iostream>

namespace ba = boost::asio;
using errorT = boost::system::error_code;

Session::Session(baip::tcp::socket&& socket) : socket_(std::move(socket))
{

}

Session::~Session()
{

}

void Session::Start()
{
	Recive();
}

void Session::Recive()
{
	auto self = shared_from_this();
	socket_.async_read_some(
		ba::buffer(buffer_.data(), buffer_.size()),
		[this, self](errorT ec, size_t bytes)
	{
		if (!ec)
		{
			std::string msg(buffer_.data(), bytes);
			std::cout << msg << std::endl;

			Send();
		}
		else
		{
			std::cout << ec.message() << std::endl;
		}
	});
}

void Session::Send()
{
	std::string s("{\"clockResponse\":\"response\"}");
	auto self = shared_from_this();
	ba::async_write(
		socket_,
		ba::buffer(s.data(), s.size()),
		[this, self](errorT ec, size_t bytes)
	{
		if (!ec)
			Recive();
		else
			std::cout << ec.message() << std::endl;
	});
}
