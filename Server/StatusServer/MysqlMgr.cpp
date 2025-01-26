#include "MysqlMgr.h"

MysqlMgr::MysqlMgr() {
}

MysqlMgr::~MysqlMgr() {
	std::cout << "MysqlMgr destoried!" << std::endl;
}

int MysqlMgr::RegUser(const std::string& name, const std::string& password, const std::string& email, const std::string& icon) {
	return _dao.RegUserTransaction(name, email, password, icon);
}

bool MysqlMgr::CheckEmail(const std::string& name, const std::string& email) {
	return _dao.CheckEmail(name, email);
}

bool MysqlMgr::UpdatePwd(const std::string& name, const std::string& newPwd) {
	return _dao.UpdatePwd(name, newPwd);
}

bool MysqlMgr::CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userInfo) {
	return _dao.CheckPwd(email, pwd, userInfo);
}