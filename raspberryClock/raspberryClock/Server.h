#ifndef SERVER_H
#define SERVER_H

#include "Listener.h"

#include <boost/asio.hpp>

#include <thread>

namespace baip = boost::asio::ip;

class Server
{
public:
	Server();
	~Server();

	void Start();

private:
	void Accept();

	Listener listener_;

	boost::asio::io_service io_service_;
	baip::tcp::socket socket_;
	baip::tcp::acceptor acceptor_;
	std::thread serviceTh_;
};

#endif // !SERVER_H
