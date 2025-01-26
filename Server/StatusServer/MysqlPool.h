#pragma once
#include "const.h"
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/prepared_statement.h>
#include "jdbc/mysql_driver.h"

class SqlConnection {
public:
	SqlConnection(sql::Connection* con, int64_t lasttime) : _con(con), _last_oper_time(lasttime){};
	std::unique_ptr<sql::Connection> _con;
	int64_t _last_oper_time;
};

class MysqlPool
{
public:
	MysqlPool(const std::string& url, const std::string& user, const std::string& passwd, const std::string& schema, std::size_t poolSize );
	~MysqlPool();
	void CheckConnection();
	std::unique_ptr<SqlConnection> getConnection();
	void returnConnection(std::unique_ptr<SqlConnection> con);
	void Close();
private:
	std::mutex _mutex;
	std::condition_variable _cond_var;
	std::queue<std::unique_ptr<SqlConnection>> _queue;
	std::atomic_bool _b_stop;
	std::size_t _poolSize;
	std::string _url;
	std::string _user;
	std::string _password;
	sql::SQLString _schema;
	std::thread _check_thread;
};

