#include "RPConnectPool.h"

RPConnectPool::RPConnectPool(std::size_t poolSize, std::string host, std::string port)
	: _host(host), _port(port), _poolSize(poolSize), _b_stop(false)
{
	for (int i = 0; i < poolSize; i++) {
		std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port,
			grpc::InsecureChannelCredentials());
		_connections.push(VarifyService::NewStub(channel));
	}
}
std::unique_ptr<VarifyService::Stub> RPConnectPool::getRPConnection() {
	std::unique_lock<std::mutex> lock_u(_mutex);
	_cond_var.wait(lock_u, [this]() {
		return this->_b_stop || !this->_connections.empty();
		});
	if (_b_stop) {
		return nullptr;
	}
	auto con = std::move(_connections.front());
	_connections.pop();
	return con;
}

void RPConnectPool::returnRPConnection(std::unique_ptr<VarifyService::Stub> con) {
	std::lock_guard<std::mutex> lock_g(_mutex);
	if (_b_stop) {
		return;
	}
	_connections.push(std::move(con));
	_cond_var.notify_one();
}
void RPConnectPool::close() {
	_b_stop = true;
	_cond_var.notify_all();
}
RPConnectPool::~RPConnectPool() {
	std::lock_guard<std::mutex> lock_g(_mutex);
	close();
	while (!_connections.empty()) {
		_connections.pop();
	}
}