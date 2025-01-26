#include "MysqlDao.h"
#include "ConfigMgr.h"
MysqlDao::MysqlDao() {
	auto& configMgr = ConfigMgr::GetInstance();
	const std::string& host = configMgr["Mysql"]["Host"];
	const std::string& port = configMgr["Mysql"]["Port"];
	const std::string& passwd = configMgr["Mysql"]["Passwd"];
	const std::string& schema = configMgr["Mysql"]["Schema"];
	const std::string& user = configMgr["Mysql"]["User"];
	//_pool.reset(new MysqlPool(host + ":" + port, user, passwd, schema, 5));
	_pool = std::make_unique<MysqlPool>(host + ":" + port, user, passwd, schema, 5);
}
MysqlDao::~MysqlDao() {
	std::cout << "MysqlDao destoried!" << std::endl;
}

int MysqlDao::RegUser(const std::string& name, const std::string& password, const std::string& email) {
	auto con = _pool->getConnection();
	try {
		if (con == nullptr) {
			return false;
		}
		// 准备调用存储过程
		/*std::make_unique 用于在堆上创建对象的新实例。
		它需要一个类型和构造函数参数，
		然后它会在内部使用 new 来分配内存并调用构造函数。 
		由于 prepareStatement 已经为你创建了对象，
		你不能再使用 std::make_unique，否则会导致重复释放内存的错误。
		由于 prepareStatement 返回的是一个指针，
		你应该使用 std::unique_ptr 的构造函数或者 reset 方法来管理它*/
		std::unique_ptr<sql::PreparedStatement> stmt(con->_con->prepareStatement("CALL reg_user(?,?,?,@result)"));
		// 错误
		//auto stmt = std::make_unique<sql::PreparedStatement>(con->_con->prepareStatement("CALL reg_user(?,?,?,@result)")); 
		
		// 设置输入参数
		stmt->setString(1, name);
		stmt->setString(2, email);
		stmt->setString(3, password);

		////执行存储过程
		stmt->execute();
		std::unique_ptr<sql::Statement> stmtResult(con->_con->createStatement());
		std::unique_ptr<sql::ResultSet> res(stmtResult->executeQuery("SELECT @result AS result"));
		if (res->next()) {
			int result = res->getInt("result");
			std::cout << result << std::endl;
			_pool->returnConnection(std::move(con));
			return result;
		}
		_pool->returnConnection(std::move(con));
		return -1;
	}
	catch (sql::SQLException& e) {
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return -1;
	}
}

int MysqlDao::RegUserTransaction(const std::string& name, const std::string& email, const std::string& pwd, const std::string& icon) {
	auto con = _pool->getConnection();
	if (con == nullptr) {
		return false;
	}

	Defer defer([this, &con] {
		_pool->returnConnection(std::move(con));
		});

	try {
		//开始事务
		con->_con->setAutoCommit(false);
		//执行第一个数据库操作，根据email查找用户
			// 准备查询语句

		std::unique_ptr<sql::PreparedStatement> pstmt_email(con->_con->prepareStatement("SELECT 1 FROM user WHERE email = ?"));

		// 绑定参数
		pstmt_email->setString(1, email);

		// 执行查询
		std::unique_ptr<sql::ResultSet> res_email(pstmt_email->executeQuery());

		auto email_exist = res_email->next();
		std::cout << email_exist << std::endl;
		if (email_exist) {
			con->_con->rollback();
			std::cout << "email " << email << " exist";
			return 0;
		}

		// 准备查询用户名是否重复
		std::unique_ptr<sql::PreparedStatement> pstmt_name(con->_con->prepareStatement("SELECT 1 FROM user WHERE name = ?"));

		// 绑定参数
		pstmt_name->setString(1, name);

		// 执行查询
		std::unique_ptr<sql::ResultSet> res_name(pstmt_name->executeQuery());

		auto name_exist = res_name->next();
		if (name_exist) {
			con->_con->rollback();
			std::cout << "name " << name << " exist";
			return 0;
		}

		// 准备更新用户id
		std::unique_ptr<sql::PreparedStatement> pstmt_upid(con->_con->prepareStatement("UPDATE user_id SET id = id + 1"));

		// 执行更新
		pstmt_upid->executeUpdate();

		// 获取更新后的 id 值
		std::unique_ptr<sql::PreparedStatement> pstmt_uid(con->_con->prepareStatement("SELECT id FROM user_id"));
		std::unique_ptr<sql::ResultSet> res_uid(pstmt_uid->executeQuery());
		int newId = 0;
		// 处理结果集
		if (res_uid->next()) {
			newId = res_uid->getInt("id");
		}
		else {
			std::cout << "select id from user_id failed" << std::endl;
			con->_con->rollback();
			return -1;
		}

		// 插入user信息
		std::unique_ptr<sql::PreparedStatement> pstmt_insert(con->_con->prepareStatement("INSERT INTO user (uid, name, email, pwd, nick, icon) "
			"VALUES (?, ?, ?, ?,?,?)"));
		pstmt_insert->setInt(1, newId);
		pstmt_insert->setString(2, name);
		pstmt_insert->setString(3, email);
		pstmt_insert->setString(4, pwd);
		pstmt_insert->setString(5, name);
		pstmt_insert->setString(6, icon);
		//执行插入
		pstmt_insert->executeUpdate();
		// 提交事务
		con->_con->commit();
		std::cout << "newuser insert into user success" << std::endl;
		return newId;
	}
	catch (sql::SQLException& e) {
		// 如果发生错误，回滚事务
		if (con) {
			con->_con->rollback();
		}
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return -1;
	}
}

bool MysqlDao::CheckEmail(const std::string& name, const std::string& email) {
	auto con = _pool->getConnection();
	Defer defer([this, &con] {
		_pool->returnConnection(std::move(con));
		});
	try {
		if (con == nullptr) {
			return false;
		}
		// 准备查询语句
		std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->
			prepareStatement("SELECT email FROM user WHERE name = ?"));
		pstmt->setString(1, name); // 绑定占位符
		// 执行查询语句
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		// 输出结果集
		while (res->next()) {
			std::cout << "query result: " << res->getString("email") << std::endl;
			if (email != res->getString("email")) {
				return false;
			}
			return true;
		}
	}
	catch (sql::SQLException& e) {
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}
bool MysqlDao::UpdatePwd(const std::string& name, const std::string& newPwd) {
	auto con = _pool->getConnection();
	Defer defer([this, &con]() {
		_pool->returnConnection(std::move(con));
		});
	try {
		if (con == nullptr) {
			return false;
		}
		// 准备查询语句
		std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->
			prepareStatement("UPDATE user SET pwd = ? WHERE name = ?"));
		pstmt->setString(1, newPwd);
		pstmt->setString(2, name);
		// 执行更新
		int res = pstmt->executeUpdate();
		std::cout << "update rows：" << res << std::endl;
		return true;
	}
	catch(sql::SQLException& e){
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

bool MysqlDao::CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userInfo) {
	auto con = _pool->getConnection();
	if (con == nullptr) {
		return false;
	}
	Defer defer([this, &con]() {
		_pool->returnConnection(std::move(con));
		});
	try {
		std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->
			prepareStatement("SELECT * FROM user WHERE email = ?"));
		pstmt->setString(1, email);
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		std::string query_pwd = "";
		while (res->next()) {
			std::cout << "query result: " << res->getString("pwd") << std::endl;
			query_pwd = res->getString("pwd");
			break;
		}
		if (pwd != query_pwd) {
			return false;
		}
		userInfo.email = res->getString("email");
		userInfo.name = res->getString("name");
		userInfo.uid = res->getInt("uid");
		userInfo.password = pwd;
		return true;
	}
	catch (sql::SQLException& e) {
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}