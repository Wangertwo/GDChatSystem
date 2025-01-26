//#pragma once
//#include "const.h"
//#include <grpcpp\grpcpp.h>
//#include "message.grpc.pb.h"
//
//using message::VarifyService;
//using message::StatusService;
//using grpc::Channel;
//
//// Grpc连接池模板类
//template<typename ServiceType, typename StubType = typename ServiceType::Stub>
//class RPConnectPool
//{
//public:
//	RPConnectPool(std::size_t poolSize, std::string host, std::string port);
//	~RPConnectPool();
//	std::unique_ptr<StubType> getRPConnection();
//	void returnRPConnection(std::unique_ptr<StubType> con);
//	void close();
//
//private:
//	std::mutex _mutex;
//	std::condition_variable _cond_var;
//	std::queue<std::unique_ptr<StubType>> _connections;
//	std::size_t _poolSize;
//	std::string _host;
//	std::string _port;
//	std::atomic_bool _b_stop;
//};

#pragma once
#include "const.h"
#include <grpcpp\grpcpp.h>
#include "message.grpc.pb.h"

using message::VarifyService;
using grpc::Channel;

// Grpc连接池模板类
class RPConnectPool
{
public:
	RPConnectPool(std::size_t poolSize, std::string host, std::string port);
	~RPConnectPool();
	std::unique_ptr<VarifyService::Stub> getRPConnection();
	void returnRPConnection(std::unique_ptr<VarifyService::Stub> con);
	void close();

private:
	std::mutex _mutex;
	std::condition_variable _cond_var;
	std::queue<std::unique_ptr<VarifyService::Stub>> _connections;
	std::size_t _poolSize;
	std::string _host;
	std::string _port;
	std::atomic_bool _b_stop;
};