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

void Session::Send(std::string resp, std::string msg)
{
	auto self = shared_from_this();

	//send response
	ba::async_write(
		socket_,
		ba::buffer(resp.data(), resp.size()),
		ba::transfer_all(),
		[this, self, resp, msg](errorT ec, size_t bytes)
	{
		if (!ec)
		{
			if (bytes != resp.size())
			{
				//TO DO: error handle
				std::cout << "Not all data was transfered" << std::endl;
				return;//session ends
			}

			if (msg.length() == 0) return;
			errorT ec2;
			//send message
			auto bytes2 = ba::write(socket_,
				ba::buffer(msg.data(), msg.size()),
				ba::transfer_all(), ec2);
			if (ec2)
			{
				if (bytes2 != msg.size())
				{
					std::cout << "Not all data was transfered" << std::endl;
				}
				else
				{
					//TO DO: error handle
					std::cout << ec2.message() << std::endl;
				}
			}
		}
		else
		{
			//TO DO: error handle
			std::cout << ec.message() << std::endl;
		}
	});
}
