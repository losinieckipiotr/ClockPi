#include "Server.h"
#include "Session.h"

#include <exception>
#include <iostream>

Server::Server() :
	socket_(io_service_),
	acceptor_(io_service_, baip::tcp::endpoint(baip::address_v4(), SERVER_PORT))
{

}

Server::~Server()
{
	try
	{
		if (!io_service_.stopped())
			io_service_.stop();
		if (serviceTh_.joinable())
			serviceTh_.join();
	}
	catch (const std::exception&) { }//ignore
}

void Server::Start()
{
	Accept();

	serviceTh_ = std::thread([this]() { io_service_.run(); });

	listener_.Start();
}

void Server::Accept()
{
	acceptor_.async_accept(socket_, [this](boost::system::error_code ec)
	{
		if (!ec)
		{
			std::make_shared<Session>(std::move(socket_))->Start();
		}
		else
		{
			std::cout << ec.message() << std::endl;
		}
		Accept();
	});
}
