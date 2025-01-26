#include "MysqlPool.h"

MysqlPool::MysqlPool(const std::string& url, const std::string& user, const std::string& passwd, const std::string& schema, std::size_t poolSize)
	: _url(url), _user(user), _password(passwd), _schema(schema), _poolSize(poolSize), _b_stop(false)
{
	try {
		for (int i = 0; i < _poolSize; i++) {
			sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
			sql::Connection* con = driver->connect(_url, _user, _password);
			con->setSchema(_schema);
			// 获取当前时间戳
			auto current_time = std::chrono::system_clock::now().time_since_epoch();
			// 将时间戳转换为秒
			long long time_stamp = std::chrono::duration_cast<std::chrono::seconds>(current_time).count();
			_queue.push(std::make_unique<SqlConnection>(con, time_stamp));
			std::cout << "MysqlConnection create success" << std::endl;
		}
		// 检查连接可用性
		_check_thread = std::thread([this]() {
			while (!_b_stop) {
				this->CheckConnection();
				std::this_thread::sleep_for(std::chrono::seconds(60));
			}
			});
		_check_thread.detach();
	}
	catch (sql::SQLException& e) {
		// 处理异常
		std::cout << "mysql pool init failed, error is " << e.what() << std::endl;
	}
}

void MysqlPool::CheckConnection() {
	std::lock_guard<std::mutex> lock_g(_mutex);
	auto poolSize = this->_queue.size();
	auto current_time = std::chrono::system_clock::now().time_since_epoch();
	long long time_stamp = std::chrono::duration_cast<std::chrono::seconds>(current_time).count();
	for (int i = 0; i < poolSize; i++) {
		auto con = std::move(_queue.front());
		_queue.pop();
		
		Defer defer([this, &con]() {
			this->_queue.push(std::move(con));
			});

		if (time_stamp - con->_last_oper_time < 5) {
			continue;
		}

		try {
			std::unique_ptr<sql::Statement> stmt(con->_con->createStatement());
			stmt->executeQuery("SELECT 1");
			con->_last_oper_time = time_stamp;
		}
		catch (sql::SQLException& e) {
			std::cout << "Error keeping connection alive: " << e.what() << std::endl;
			// 重新创建连接并替换旧的连接
			sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
			auto* newcon = driver->connect(_url, _user, _password);
			newcon->setSchema(_schema);
			con->_con.reset(newcon);
			con->_last_oper_time = time_stamp;
		}
	}
}

std::unique_ptr<SqlConnection> MysqlPool::getConnection() {
	std::unique_lock<std::mutex> lock_u(_mutex);
	_cond_var.wait(lock_u, [this]() {
		return this->_b_stop || !this->_queue.empty(); // false 阻塞 true 返回
		});
	if (this->_b_stop) {
		return nullptr;
	}
	auto con = std::move(_queue.front());
	_queue.pop();
	return con;
}

void MysqlPool::returnConnection(std::unique_ptr<SqlConnection> con) {
	std::unique_lock<std::mutex> lock_u(_mutex);
	if (_b_stop) {
		return;
	}
	_queue.push(std::move(con));
	_cond_var.notify_one();
}

void MysqlPool::Close() {
	_b_stop = true;
	_cond_var.notify_all();
}

MysqlPool::~MysqlPool() {
	std::lock_guard<std::mutex> lock_g(_mutex);
	Close();
	while (!_queue.empty()) {
		_queue.pop();
	}
}
