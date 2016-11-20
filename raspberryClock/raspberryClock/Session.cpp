#include "Session.h"

#include <string>
#include <iostream>
#include <future>

namespace ba = boost::asio;
using errorT = boost::system::error_code;

Session::Session(baip::tcp::socket&& socket, reciveHandlerT reviceHandler) :
	socket_(std::move(socket)),
	reciveHandler_(reviceHandler)
{

}

Session::~Session()
{

}

void Session::Start()
{
	socket_.set_option(ba::ip::tcp::no_delay(true));
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
			std::async(std::launch::async, reciveHandler_, std::move(msg), self);
		}
		else
		{
			std::cout << ec.message() << std::endl;//if error, session ends
		}
	});
}

void Session::Send(std::string msg, bool async)
{
	if (async)
	{
		auto self = shared_from_this();
		ba::async_write(
			socket_,
			ba::buffer(msg.data(), msg.size()),
			[this, self](errorT ec, size_t bytes)
		{
			if (ec)
				std::cout << ec.message() << std::endl;
			//TO DO: error handle

		});
	}
	else
	{
		errorT ec;
		ba::write(
			socket_,
			ba::buffer(msg.data(), msg.size()), ec);
		if(ec)
			std::cout << ec.message() << std::endl;
			//TO DO: error handle
	}
}
