#ifndef LISTENER_H
#define LISTENER_H

#include <boost/asio.hpp>

#include <thread>
#include <array>
#include <string>

namespace baip = boost::asio::ip;

class Listener
{
public:
	Listener();
	~Listener();

	void Start();

private:
	void Recive();
	void Response();

	bool ValidateMsg(const std::string& msg);

	enum
	{
		BUFFER_SIZE = 1024,
		LISTENER_PORT = 2017
	};

	boost::asio::io_service io_service_;
	baip::udp::socket socket_;
	baip::udp::endpoint remoteEndpoint_;
	
	std::array<char, BUFFER_SIZE> buffer_;

	std::thread serviceTh_;
};

#endif // !LISTENER_H

