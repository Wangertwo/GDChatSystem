#pragma once
#include "const.h"
class RedisConnectPool
{
public:
	RedisConnectPool(std::size_t poolSize, const std::string& host, const std::string& port, const std::string& password);
	~RedisConnectPool();
	std::unique_ptr<redisContext, decltype(&redisFree)> getConnection();
	void returnConnection(std::unique_ptr<redisContext, decltype(&redisFree)> context);
	void Close();
private:
	std::mutex _mutex;
	std::condition_variable _cond_var;
	std::string _host;
	std::string _port;
	std::size_t _poolSize;
	std::atomic_bool _b_stop;
	// RAII 和智能指针来管理资源
	std::queue<std::unique_ptr<redisContext, decltype(&redisFree)>> _connections; 
};

