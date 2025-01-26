#pragma once
#include "ConfigMgr.h"
#include "const.h"
#include <grpcpp\grpcpp.h>
#include "message.grpc.pb.h"
using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::LoginReq;
using message::LoginRsp;
using message::StatusService;
using StatusService = message::StatusService;

class RPConnectionPool {
public:
	RPConnectionPool(std::size_t poolSize, std::string host, std::string port)
		: _host(host), _port(port), _poolSize(poolSize), _b_stop(false)
	{
		for (int i = 0; i < poolSize; i++) {
			std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port,
				grpc::InsecureChannelCredentials());
			_connections.push(StatusService::NewStub(channel));
		}
	}
	~RPConnectionPool() {
		std::lock_guard<std::mutex> lock_g(_mutex);
		close();
		while (!_connections.empty()) {
			_connections.pop();
		}
	}
	std::unique_ptr<StatusService::Stub> getRPConnection() {
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
	void returnRPConnection(std::unique_ptr<StatusService::Stub> con) 
	{
		std::lock_guard<std::mutex> lock_g(_mutex);
		if (_b_stop) {
			return;
		}
		_connections.push(std::move(con));
		_cond_var.notify_one();
	}
	void close() {
		_b_stop = true;
		_cond_var.notify_all();
	}

private:
	std::mutex _mutex;
	std::condition_variable _cond_var;
	std::queue<std::unique_ptr<StatusService::Stub>> _connections;
	std::size_t _poolSize;
	std::string _host;
	std::string _port;
	std::atomic_bool _b_stop;
};

class StatusGrpcClient : public Singleton<StatusGrpcClient>
{
	friend class Singleton<StatusGrpcClient>;
public:
	~StatusGrpcClient();
	GetChatServerRsp GetChatServer(int uid);
	LoginRsp Login(int uid, std::string token);
private:
	StatusGrpcClient();
	std::unique_ptr<RPConnectionPool> _pool;
}; 