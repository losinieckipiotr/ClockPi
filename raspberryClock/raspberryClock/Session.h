#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>

#include <memory>
#include <array>

namespace baip = boost::asio::ip;

class Session : public std::enable_shared_from_this<Session>
{
public:
	Session(baip::tcp::socket&& socket);
	~Session();

	void Start();

private:
	void Recive();
	void Send();

	enum { BUFFER_SIZE = 1024 * 1024 };

	baip::tcp::socket socket_;
	
	std::array<char, BUFFER_SIZE> buffer_;
};

#endif // !SESSION_H

