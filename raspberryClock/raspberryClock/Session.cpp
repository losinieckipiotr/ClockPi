#include "Session.h"

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
			//TO DO: DO SMOETHING WITH DATA
			Send();
		}
		//TO DO: HANDLE ERROR
	});
}

void Session::Send()
{
	auto self = shared_from_this();
	ba::async_write(
		socket_,
		ba::buffer(buffer_, buffer_.size()),
		[this, self](errorT ec, size_t bytes)
	{
		if (!ec)
		{
			Recive();
		}
		//TO DO: HANDLE ERROR
	});
}
