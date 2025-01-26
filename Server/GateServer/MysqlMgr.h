#pragma once
#include "Singleton.h"
#include "MysqlDao.h"
class MysqlMgr : public Singleton<MysqlMgr>
{
	friend class Singleton<MysqlMgr>;
public:
	~MysqlMgr();
	int RegUser(const std::string& name, const std::string& password, const std::string& email, const std::string& icon);
	bool CheckEmail(const std::string& name, const std::string& email);
	bool UpdatePwd(const std::string& name, const std::string& newPwd);
	bool CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userInfo);

private:
	MysqlMgr();
	MysqlDao _dao; // �������˽�г�Ա����ֻ�ᱻ��ʼ��һ��
};

