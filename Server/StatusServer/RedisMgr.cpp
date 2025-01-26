#include "RedisMgr.h"

RedisMgr::RedisMgr() {
	auto& configMgr = ConfigMgr::GetInstance();
	const std::string host = configMgr["Redis"]["Host"];
	const std::string port = configMgr["Redis"]["Port"];
	const std::string passwd = configMgr["Redis"]["Password"];
	_pool = std::make_unique<RedisConnectPool>(5, host, port, passwd);
}

RedisMgr::~RedisMgr() {
	std::cout << "RedisMgr destroied" << std::endl;
}

bool RedisMgr::Get(const std::string& key, std::string& value) {
	auto context = _pool->getConnection();
	if (context.get() == nullptr) {
		return false;
	}

	auto reply = (redisReply*)redisCommand(context.get(), "Get %s", key.c_str());
	if (reply == nullptr) {
		std::cout << "[ GET  " << key << " ] failed" << std::endl;
		_pool->returnConnection(std::move(context));
		return false;
	}
	if (reply->type != REDIS_REPLY_STRING) {
		std::cout << "[ GET  " << key << " ] failed" << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(std::move(context));
		return false;
	}
	value = reply->str;
	freeReplyObject(reply);
	std::cout << "Succeed to execute command [ GET " << key << "  ]" << std::endl;
	_pool->returnConnection(std::move(context));
	return true;
}

bool RedisMgr::Set(const std::string& key, const std::string& value) {
	auto context = _pool->getConnection();
	if (context.get() == nullptr) {
		return false;
	}
	auto reply = (redisReply*)redisCommand(context.get(), "Set %s %s", key.c_str(), value.c_str());
	if (reply == nullptr) {
		std::cout << "[ SET " << key << " " << value << " ] failed" << std::endl;
		_pool->returnConnection(std::move(context));
		return false;
	}
	if (!(reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0)) {
		std::cout << "[ SET " << key << " " << value << " ] failed" << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(std::move(context));
		return false;
	}
	freeReplyObject(reply);
	std::cout << "Execut command [ SET " << key << "  " << value << " ] success ! " << std::endl;
	_pool->returnConnection(std::move(context));
	return true;
}

bool RedisMgr::Set(const char* key, const char* value, size_t hvaluelen) {
	auto context = _pool->getConnection();
	if (context.get() == nullptr) {
		return false;
	}
	const char* argv[3];
	std::size_t argvlen[3];
	argv[0] = "SET";
	argvlen[0] = 3;
	argv[1] = key;
	argvlen[1] = strlen(key);
	argv[2] = value;
	argvlen[2] = hvaluelen;
	auto reply = (redisReply*)redisCommandArgv(context.get(), 3, argv, argvlen);
	if (reply == nullptr) {
		std::cout << "[ SET " << key << " " << value << " ] failed" << std::endl;
		_pool->returnConnection(std::move(context));
		return false;
	}
	if (!(reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0)) {
		std::cout << "[ SET " << key << " " << value << " ] failed" << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(std::move(context));
		return false;
	}
	freeReplyObject(reply);
	std::cout << "Execut command [ SET " << key << "  " << value << " ] success ! " << std::endl;
	_pool->returnConnection(std::move(context));
	return true;
}

bool RedisMgr::LPush(const std::string& key, const std::string& value) {
	auto context = _pool->getConnection();
	if (context.get() == nullptr) {
		return false;
	}
	auto reply = (redisReply*)redisCommand(context.get(), "LPush %s %s", key.c_str(), value.c_str());
	if (reply == nullptr) {
		std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		_pool->returnConnection(std::move(context));
		return false;
	}
	if (reply->type != REDIS_REPLY_INTEGER) {
		std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(std::move(context));
		return false;
	}
	std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] success ! " << std::endl;
	freeReplyObject(reply);
	_pool->returnConnection(std::move(context));
	return true;
}

bool RedisMgr::LPop(const std::string& key, std::string& value) {
	auto context = _pool->getConnection();
	if (context.get() == nullptr) {
		return false;
	}
	auto reply = (redisReply*)redisCommand(context.get(), "LPOP %s ", key.c_str());
	if (reply == nullptr || reply->type == REDIS_REPLY_NIL) {
		std::cout << "Execut command [ LPOP " << key << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(std::move(context));
		return false;
	}
	value = reply->str;
	std::cout << "Execut command [ LPOP " << key << " ] success ! " << std::endl;
	freeReplyObject(reply);
	_pool->returnConnection(std::move(context));
	return true;
}

bool RedisMgr::RPush(const std::string& key, const std::string& value) {
	auto context = _pool->getConnection();
	if (context.get() == nullptr) {
		return false;
	}
	auto reply = (redisReply*)redisCommand(context.get(), "RPUSH %s %s", key.c_str(), value.c_str());
	if (NULL == reply)
	{
		std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		_pool->returnConnection(std::move(context));
		return false;
	}
	if (reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
		std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(std::move(context));
		return false;
	}
	std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] success ! " << std::endl;
	freeReplyObject(reply);
	_pool->returnConnection(std::move(context));
	return true;
}

bool RedisMgr::RPop(const std::string& key, std::string& value) {
	auto context = _pool->getConnection();
	if (context.get() == nullptr) {
		return false;
	}
	auto reply = (redisReply*)redisCommand(context.get(), "RPOP %s ", key.c_str());
	if (reply == nullptr || reply->type == REDIS_REPLY_NIL) {
		std::cout << "Execut command [ RPOP " << key << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(std::move(context));
		return false;
	}
	value = reply->str;
	std::cout << "Execut command [ RPOP " << key << " ] success ! " << std::endl;
	freeReplyObject(reply);
	_pool->returnConnection(std::move(context));
	return true;
}

bool RedisMgr::HSet(const std::string& key, const std::string& hkey, const std::string& value) {
	auto context = _pool->getConnection();
	if (context.get() == nullptr) {
		return false;
	}
	auto reply = (redisReply*)redisCommand(context.get(), "HSET %s %s %s", key.c_str(), hkey.c_str(), value.c_str());
	if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
		std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(std::move(context));
		return false;
	}
	std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << value << " ] success ! " << std::endl;
	freeReplyObject(reply);
	_pool->returnConnection(std::move(context));
	return true;
}

bool RedisMgr::HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen) {
	auto context = _pool->getConnection();
	if (context.get() == nullptr) {
		return false;
	}
	const char* argv[4];
	size_t argvlen[4];
	argv[0] = "HSET";
	argvlen[0] = 4;
	argv[1] = key;
	argvlen[1] = strlen(key);
	argv[2] = hkey;
	argvlen[2] = strlen(hkey);
	argv[3] = hvalue;
	argvlen[3] = hvaluelen;
	auto reply = (redisReply*)redisCommandArgv(context.get(), 4, argv, argvlen);
	if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
		std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << hvalue << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(std::move(context));
		return false;
	}
	std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << hvalue << " ] success ! " << std::endl;
	freeReplyObject(reply);
	_pool->returnConnection(std::move(context));
	return true;
}

std::string RedisMgr::HGet(const std::string& key, const std::string& hkey) {
	auto connect = _pool->getConnection();
	if (connect == nullptr) {
		return "";
	}
	const char* argv[3];
	size_t argvlen[3];
	argv[0] = "HGET";
	argvlen[0] = 4;
	argv[1] = key.c_str();
	argvlen[1] = key.length();
	argv[2] = hkey.c_str();
	argvlen[2] = hkey.length();

	auto reply = (redisReply*)redisCommandArgv(connect.get(), 3, argv, argvlen);
	if (reply == nullptr) {
		std::cout << "Execut command [ HGet " << key << " " << hkey << "  ] failure ! " << std::endl;
		_pool->returnConnection(std::move(connect));
		return "";
	}

	if (reply->type == REDIS_REPLY_NIL) {
		freeReplyObject(reply);
		std::cout << "Execut command [ HGet " << key << " " << hkey << "  ] failure ! " << std::endl;
		_pool->returnConnection(std::move(connect));
		return "";
	}

	std::string value = reply->str;
	freeReplyObject(reply);
	_pool->returnConnection(std::move(connect));
	std::cout << "Execut command [ HGet " << key << " " << hkey << " ] success ! " << std::endl;
	return value;
}

//bool RedisMgr::HGet(const std::string& key, const std::string& hkey, std::string& value) {
//	auto context = _pool->getConnection();
//	if (context.get() == nullptr) {
//		return false;
//	}
//	auto reply = (redisReply*)redisCommand(context.get(), "HGET %s %s", key.c_str(), hkey.c_str());
//	if (reply == nullptr || reply->type == REDIS_REPLY_NIL) {
//		freeReplyObject(reply);
//		std::cout << "Execut command [ HGet " << key << " " << hkey << "  ] failure ! " << std::endl;
//		_pool->returnConnection(std::move(context));
//		return false;
//	}
//	value = reply->str;
//	freeReplyObject(reply);
//	std::cout << "Execut command [ HGet " << key << " " << hkey << " ] success ! " << std::endl;
//	_pool->returnConnection(std::move(context));
//	return true;
//}

bool RedisMgr::Del(const std::string& key) {
	auto context = _pool->getConnection();
	if (context.get() == nullptr) {
		return false;
	}
	auto reply = (redisReply*)redisCommand(context.get(), "DEL %s", key.c_str());
	if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
		std::cout << "Execut command [ Del " << key << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(std::move(context));
		return false;
	}
	std::cout << "Execut command [ Del " << key << " ] success ! " << std::endl;
	freeReplyObject(reply);
	_pool->returnConnection(std::move(context));
	return true;
}

bool RedisMgr::ExistKey(const std::string& key) {
	auto context = _pool->getConnection();
	if (context.get() == nullptr) {
		return false;
	}
	auto reply = (redisReply*)redisCommand(context.get(), "exists %s", key.c_str());
	if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER || reply->integer == 0) {
		std::cout << "Not Found [ Key " << key << " ]  ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(std::move(context));
		return false;
	}
	std::cout << " Found [ Key " << key << " ] exists ! " << std::endl;
	freeReplyObject(reply);
	_pool->returnConnection(std::move(context));
	return true;
}





