#include "StatusGrpcClient.h"

StatusGrpcClient::StatusGrpcClient() {
	auto& configMgr = ConfigMgr::GetInstance();
	const std::string& host = configMgr["StatusServer"]["Host"];
	const std::string& port = configMgr["StatusServer"]["Port"];
	_pool.reset(new RPConnectionPool(5, host, port));
}

StatusGrpcClient::~StatusGrpcClient() {
	std::cout << "StatusGrpcClient destroied";
}

GetChatServerRsp StatusGrpcClient::GetChatServer(int uid) {
	ClientContext context;
	GetChatServerReq request;
	GetChatServerRsp reply;
	request.set_uid(uid);
	auto stub = _pool->getRPConnection();
	Defer defer([this, &stub] {
		_pool->returnRPConnection(std::move(stub));
		});
	Status status = stub->GetChatServer(&context, request, &reply);
	if (status.ok()) {
		return reply;
	}
	else {
		reply.set_error(ErrorCodes::RPCFAILED);
		return reply;
	}
}

LoginRsp StatusGrpcClient::Login(int uid, std::string token) {
	ClientContext context;
	LoginReq request;
	LoginRsp reply;
	request.set_uid(uid);
	request.set_token(token);
	auto stub = _pool->getRPConnection();
	Defer defer([this, &stub] {
		_pool->returnRPConnection(std::move(stub));
		});
	Status status = stub->Login(&context, request, &reply);
	if (status.ok()) {
		return reply;
	}
	else {
		reply.set_error(ErrorCodes::RPCFAILED);
		return reply;
	}
}