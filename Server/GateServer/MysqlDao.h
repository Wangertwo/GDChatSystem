#pragma once
#include "const.h"
#include "MysqlPool.h"

struct UserInfo {
	std::string name;
	std::string password;
	int uid;
	std::string email;
};

class MysqlDao
{
public:
	MysqlDao();
	~MysqlDao();
	int RegUser(const std::string& name, const std::string& password, const std::string& email);
	int RegUserTransaction(const std::string& name, const std::string& email, const std::string& pwd, const std::string& icon);
	bool CheckEmail(const std::string& name, const std::string& email);
	bool UpdatePwd(const std::string& name, const std::string& newPwd);
	bool CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userInfo);

private:
	std::unique_ptr<MysqlPool> _pool;
};

