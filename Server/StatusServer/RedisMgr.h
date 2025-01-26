#pragma once
#include "const.h"
#include "Singleton.h"
#include "RedisConnectPool.h"
#include "ConfigMgr.h"
class RedisMgr : public Singleton<RedisMgr>
{
	friend class Singleton<RedisMgr>;
private:
	RedisMgr();
	std::unique_ptr<RedisConnectPool> _pool;
public:
	~RedisMgr();
	bool Get(const std::string& key, std::string& value);
	bool Set(const std::string& key, const std::string& value);
	bool Set(const char* key, const char* value, size_t hvaluelen);
	bool LPush(const std::string& key, const std::string& value);
	bool LPop(const std::string& key, std::string& value);
	bool RPush(const std::string& key, const std::string& value);
	bool RPop(const std::string& key, std::string& value);
	bool HSet(const std::string& key, const std::string& hkey, const std::string& value);
	bool HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen);
	std::string HGet(const std::string& key, const std::string& hkey);
	//bool HGet(const std::string& key, const std::string& hkey, std::string& value);
	bool Del(const std::string& key);
	bool ExistKey(const std::string& key);
};

