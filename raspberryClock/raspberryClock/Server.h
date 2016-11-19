#ifndef SERVER_H
#define SERVER_H

#include "Listener.h"

#include <boost/asio.hpp>

#include <thread>
#include <functional>
#include <string>
#include <memory>

namespace baip = boost::asio::ip;

class Session;

using reciveHandlerT = std::function<void(std::string, std::shared_ptr<Session>)>;

class Server
{
public:
	Server();
	~Server();

	void Start(reciveHandlerT reciveHandler);
	void Stop();

private:
	void Accept();

	enum
	{
		SERVER_PORT = 2016
	};

	reciveHandlerT reciveHandler_;

	Listener listener_;

	boost::asio::io_service io_service_;
	baip::tcp::socket socket_;
	baip::tcp::acceptor acceptor_;
	std::thread serviceTh_;
};

#endif // !SERVER_H
