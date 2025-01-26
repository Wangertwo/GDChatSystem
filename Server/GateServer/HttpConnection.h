#pragma once
#include "const.h"
#include "LogicSystem.h"

class HttpConnection : public std::enable_shared_from_this<HttpConnection>
{
	friend class LogicSystem;
public:
	HttpConnection(boost::asio::io_context& ioc);
	void Start();
	tcp::socket& GetSocket();
private:
	tcp::socket _socket;
	void handleRequest();
	void writeResponse();
	void checkDeadLine();
	void PreParseGetParam();
	boost::asio::steady_timer _deadline{ _socket.get_executor(), std::chrono::seconds(60)};
	beast::flat_buffer _buffer{ 8192 };
	http::request<http::dynamic_body> _request;
	http::response<http::dynamic_body> _response;
	// get²ÎÊý½âÎö
	std::string _get_url;
	std::unordered_map<std::string, std::string> _get_params;
};

