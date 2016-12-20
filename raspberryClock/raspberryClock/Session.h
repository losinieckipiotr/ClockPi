#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>

#include <memory>
#include <array>
#include <string>

namespace baip = boost::asio::ip;

class Session;

using reciveHandlerT = std::function<void(std::string, std::shared_ptr<Session>)>;

class Session : public std::enable_shared_from_this<Session>
{
public:
	Session(baip::tcp::socket&& socket, reciveHandlerT reciveHandler);
	~Session();

	void Start();

	void Send(const std::string resp, const std::string msg);
	void Recive();

private:
	enum { BUFFER_SIZE = 1024 };

	baip::tcp::socket socket_;

    reciveHandlerT reciveHandler_;

	std::array<char, BUFFER_SIZE> buffer_;
};

#endif // !SESSION_H
