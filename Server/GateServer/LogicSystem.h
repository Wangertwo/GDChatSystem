#pragma once
#include "const.h"
#include "Singleton.h"
class HttpConnection;
typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;

class LogicSystem : public Singleton<LogicSystem>
{
	friend class Singleton<LogicSystem>;
public:
	~LogicSystem() {};
	bool HandleGet(std::string url, std::shared_ptr<HttpConnection> con);
	void RegGet(std::string url, HttpHandler handler);
	bool HandlePost(std::string url, std::shared_ptr<HttpConnection> con);
	void RegPost(std::string url, HttpHandler handler);
private:
	LogicSystem();
	std::map<std::string, HttpHandler> _get_handlers;
	std::map<std::string, HttpHandler> _post_handlers;
};

