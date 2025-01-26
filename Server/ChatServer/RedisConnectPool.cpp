#include "RedisConnectPool.h"

RedisConnectPool::RedisConnectPool(std::size_t poolSize, const std::string& host, const std::string& port, const std::string& password)
	: _poolSize(poolSize), _host(host), _port(port), _b_stop(false)
{
	for (int i = 0; i < _poolSize; i++) {
		// 自定义删除器 delete -> redisFree
		auto context = std::unique_ptr<redisContext, decltype(&redisFree)>(redisConnect(host.c_str(), atoi(port.c_str())), redisFree);
		if (context.get() == nullptr) {
			std::cout << "connect error" << std::endl;
			continue;
		}
		if (context.get() != nullptr && context.get()->err) {
			std::cout << "connect error " << context->errstr << std::endl;
			continue;
		}
		std::cout << "connect success" << std::endl;
		auto reply = (redisReply*)redisCommand(context.get(), "AUTH %s", password.c_str());
		if (reply->type == REDIS_REPLY_ERROR) {
			std::cout << "certify error" << std::endl;
			freeReplyObject(reply);
			continue;
		}
		else {
			std::cout << "certify success" << std::endl;
			freeReplyObject(reply);
		}
		// 连接的创建和认证都已经成功
		_connections.push(std::move(context));
	}
}

RedisConnectPool::~RedisConnectPool() {
	std::lock_guard<std::mutex> lock_g(_mutex);
	Close();
	while (!_connections.empty()) {
		_connections.pop();
	}
}

std::unique_ptr<redisContext, decltype(&redisFree)> RedisConnectPool::getConnection() {
	std::unique_lock<std::mutex> lock_u(_mutex);
	_cond_var.wait(lock_u, [this]() {
		return this->_b_stop || !this->_connections.empty();
		});
	if (_b_stop) {
		// 返回空的unique_ptr
		return std::unique_ptr<redisContext, decltype(&redisFree)>(nullptr, redisFree);
	}
	auto context = std::move(_connections.front());
	_connections.pop();
	return context;
}

void RedisConnectPool::returnConnection(std::unique_ptr<redisContext, decltype(&redisFree)> context) {
	std::lock_guard<std::mutex> lock_g(_mutex);
	if (_b_stop) {
		return;
	}
	_connections.push(std::move(context));
	_cond_var.notify_one();
}

void RedisConnectPool::Close() {
	_b_stop = true;
	_cond_var.notify_all();
}