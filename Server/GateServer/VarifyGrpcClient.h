#pragma once
#include "ConfigMgr.h"
#include "RPConnectPool.h"
using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;
using VarifyService = message::VarifyService;
class VarifyGrpcClient : public Singleton<VarifyGrpcClient>
{
	friend class Singleton<VarifyGrpcClient>;
public:
	GetVarifyRsp GetVarifyCode(std::string email) {
		GetVarifyReq request;
		GetVarifyRsp reply;
		ClientContext context;
		request.set_email(email);
		auto _stub = _pool->getRPConnection();
		Status status = _stub->GetVarifyCode(&context, request, &reply);
		if (status.ok()) {
			_pool->returnRPConnection(std::move(_stub));
			return reply;
		}
		else {
			_pool->returnRPConnection(std::move(_stub));
			reply.set_error(ErrorCodes::RPCFAILED);
			return reply;
		}
	}
private:
	VarifyGrpcClient() {
		auto& configMgr = ConfigMgr::GetInstance();
		std::string host = configMgr["VarifyServer"]["Host"];
		std::string port = configMgr["VarifyServer"]["Port"];
		_pool.reset(new RPConnectPool(5, host, port));
	};
	std::unique_ptr<RPConnectPool> _pool;
};