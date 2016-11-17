#include "Server.h"
#include <exception>

Server::Server() :
	socket_(io_service_),
	acceptor_(io_service_, baip::tcp::endpoint(baip::address_v4(), 2016))
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
		//TO DO: START SESSION

		Accept();
	});
}
