#pragma once
#include "LogicSystem.h"
#include "HttpConnection.h"
#include "VarifyGrpcClient.h"
#include "StatusGrpcClient.h"
#include "ConfigMgr.h"
#include "RedisMgr.h"
#include "MysqlMgr.h"
LogicSystem::LogicSystem() {
	RegGet("/get_test", [](std::shared_ptr<HttpConnection> con) {
		beast::ostream(con->_response.body()) << "Receive get_test req";
		int i = 0;
		for (auto& elem : con->_get_params) {
			i++;
			beast::ostream(con->_response.body()) << "param" << i << " key is " << elem.first;
			beast::ostream(con->_response.body()) << ", " << " value is " << elem.second << std::endl;
		}
		});

	RegPost("/get_varifycode", [](std::shared_ptr<HttpConnection> con) {
		std::string body_str = boost::beast::buffers_to_string(con->_request.body().data());
		std::cout << "receive body is: " << body_str << std::endl;
		con->_response.set(http::field::content_type, "text/json");
		Json::Value root;
		Json::Reader reader;
		Json::Value src_root;
		bool parse_success = reader.parse(body_str, src_root);

		if (!parse_success ) {
			std::cout << "failed to parse json!" << std::endl;
			root["error"] = ErrorCodes::ERROR_JSON; // json解析错误
			std::string jsonstr = root.toStyledString();
			beast::ostream(con->_response.body()) << jsonstr;
			return;
		}

		if (!src_root.isMember("email")) {
			std::cout << "failed to parse json!" << std::endl;
			root["error"] = ErrorCodes::ERROR_JSON; // json解析错误
			std::string jsonstr = root.toStyledString();
			beast::ostream(con->_response.body()) << jsonstr;
			return;
		}

		std::cout << "success to parse json!" << std::endl;
		auto email = src_root["email"].asString();
		GetVarifyRsp rsp = VarifyGrpcClient::GetInstance().GetVarifyCode(email);
		std::cout << "email is: " << email << std::endl;
		root["error"] = rsp.error();
		root["email"] = src_root["email"];
		std::string jsonstr = root.toStyledString();
		beast::ostream(con->_response.body()) << jsonstr;
		});

	RegPost("/get_register", [](std::shared_ptr<HttpConnection> con) {
		std::string body_str = beast::buffers_to_string(con->_request.body().data());
		std::cout << "body_str is " << body_str << std::endl;
		con->_response.set(http::field::content_type, "text/json");
		Json::Value root;
		Json::Reader reader;
		Json::Value src_root;
		bool parse_success = reader.parse(body_str, src_root);
		if (!parse_success) {
			std::cout << "failed to parse json!" << std::endl;
			root["error"] = ErrorCodes::ERROR_JSON;
			std::string jsonstr = root.toStyledString();
			beast::ostream(con->_response.body()) << jsonstr;
			return;
		}
		// 先查找redis中email对应的验证码是否超时
		std::string verify_code;
		bool b_varify_code = RedisMgr::GetInstance().Get("code_" + src_root["email"].asString(), verify_code);
		if (!b_varify_code) {
			std::cout << "get varify code expire" << std::endl;
			root["error"] = ErrorCodes::VARIFYEXPIRE;
			std::string jsonstr = root.toStyledString();
			beast::ostream(con->_response.body()) << jsonstr;
			return;
		}
		// 验证码输入错误
		if (strcmp(verify_code.c_str(), src_root["varifycode"].asString().c_str())) {
			std::cout << "failed to match varify code" << std::endl;
			root["error"] = ErrorCodes::VARIFYCODEERR;
			std::string jsonstr = root.toStyledString();
			beast::ostream(con->_response.body()) << jsonstr;
			return;
		}
		// 密码和确认密码是否相同
		if (strcmp(src_root["password"].asString().c_str(), src_root["confirm"].asString().c_str())) {
			std::cout << "confirmPasswd and password vary" << std::endl;
			root["error"] = ErrorCodes::CONFIRMPASSWDVARY;
			std::string jsonstr = root.toStyledString();
			beast::ostream(con->_response.body()) << jsonstr;
			return;
		}
		// 当前注册用户是否存在（查询redis）
		bool b_usr_exist = RedisMgr::GetInstance().ExistKey(src_root["user"].asString());
		if (b_usr_exist) {
			std::cout << "user exist" << std::endl;
			root["error"] = ErrorCodes::USEREXIST;
			std::string jsonstr = root.toStyledString();
			beast::ostream(con->_response.body()) << jsonstr;
			return;
		}
		// 当前注册用户是否存在（查询mysql）
		int res = MysqlMgr::GetInstance().RegUser(src_root["user"].asString(), src_root["password"].asString(), src_root["email"].asString(), src_root["icon"].asString());
		if (res == -1 || res == 0) {
			std::cout << "user exist" << std::endl;
			root["error"] = ErrorCodes::USEREXIST;
			std::string jsonstr = root.toStyledString();
			beast::ostream(con->_response.body()) << jsonstr;
			return;
		}

		std::cout << "success to parse json!" << std::endl;
		root["error"] = 0;
		root["user"] = src_root["user"].asString();
		root["password"] = src_root["password"].asString();
		root["email"] = src_root["email"];
		root["confirm"] = src_root["confirm"].asString();
		root["icon"] = src_root["icon"].asString();
		root["varifycode"] = src_root["varifycode"].asString();
		// 遍历json
		for(auto it = root.begin(); it != root.end(); it++) {
			std::cout << it.key().asString() << " : " << it->asString() << std::endl;
		}
		std::string jsonstr = root.toStyledString();
		beast::ostream(con->_response.body()) << jsonstr;
		return;
		});

	RegPost("/reset_pwd", [](std::shared_ptr<HttpConnection> con) {
		std::string body_str = beast::buffers_to_string(con->_request.body().data());
		std::cout << "body_str: " << body_str << std::endl;
		con->_response.set(http::field::content_type, "text/json");
		Json::Value root;
		Json::Reader reader;
		Json::Value src_root;
		bool parse_success = reader.parse(body_str, src_root);
		if (!parse_success) {
			std::cout << "parse failed" << std::endl;
			root["error"] = ErrorCodes::ERROR_JSON;
			std::string jsonstr = root.toStyledString();
			std::cout << "root: " << root << std::endl;
			beast::ostream(con->_response.body()) << jsonstr;
			return;
		}
		// 先查找redis中email对应的验证码是否超时
		std::string verify_code;
		bool b_varify_code = RedisMgr::GetInstance().Get("code_" + src_root["email"].asString(), verify_code);
		if (!b_varify_code) {
			std::cout << "get varify code expire" << std::endl;
			root["error"] = ErrorCodes::VARIFYEXPIRE;
			std::string jsonstr = root.toStyledString();
			beast::ostream(con->_response.body()) << jsonstr;
			return;
		}
		// 验证码输入错误
		if (strcmp(verify_code.c_str(), src_root["varifyCode"].asString().c_str())) {
			std::cout << "failed to match varify code" << std::endl;
			root["error"] = ErrorCodes::VARIFYCODEERR;
			std::string jsonstr = root.toStyledString();
			beast::ostream(con->_response.body()) << jsonstr;
			return;
		}
		// 查询数据库，判断用户和邮箱是否匹配
		bool query_success = MysqlMgr::GetInstance().CheckEmail(src_root["user"].asString(), src_root["email"].asString());
		if (!query_success) {
			std::cout << "name and email match failed" << std::endl;
			root["error"] = ErrorCodes::EMAILNOTMATCH;
			std::string jsonstr = root.toStyledString();
			beast::ostream(con->_response.body()) << jsonstr;
			return;
		}
		// 查询数据库，更新密码
		bool update_success = MysqlMgr::GetInstance().UpdatePwd(src_root["user"].asString(), src_root["newPwd"].asString());
		if (!update_success) {
			std::cout << "update password failed" << std::endl;
			root["error"] = ErrorCodes::PWDUPDATEFAILED;
			std::string jsonstr = root.toStyledString();
			beast::ostream(con->_response.body()) << jsonstr;
			return;
		}
		std::cout << "password reset success" << std::endl;
		root["error"] = ErrorCodes::SUCCESS;
		root["user"] = src_root["user"].asString();
		root["email"] = src_root["email"].asString();
		root["newPwd"] = src_root["newPwd"].asString();
		root["varifyCode"] = src_root["varifyCode"].asString();
		std::string jsonstr = root.toStyledString();
		beast::ostream(con->_response.body()) << jsonstr;
		});

	RegPost("/user_login", [](std::shared_ptr<HttpConnection> con) {
		std::string body_str = beast::buffers_to_string(con->_request.body().data());
		std::cout << "body_str is: " << body_str << std::endl;
		con->_response.set(http::field::content_type, "text/json");
		Json::Value root;
		Json::Reader reader;
		Json::Value src_root;
		bool parse_success = reader.parse(body_str, src_root);
		if (!parse_success) {
			std::cout << "parse failed" << std::endl;
			root["error"] = ErrorCodes::ERROR_JSON;
			std::string jsonstr = root.toStyledString();
			beast::ostream(con->_response.body()) << jsonstr;
			return;
		}
		// 查询数据库，判断邮箱和密码是否匹配
		UserInfo userInfo;
		bool query_success = MysqlMgr::GetInstance().CheckPwd(src_root["email"].asString(), src_root["pwd"].asString(), userInfo);
		if (!query_success) {
			std::cout << "email and pwd match failed" << std::endl;
			root["error"] = ErrorCodes::PWDNOTMATCH;
			std::string jsonstr = root.toStyledString();
			beast::ostream(con->_response.body()) << jsonstr;
			return;
		}
		//查询StatusServer找到合适的连接
		auto reply = StatusGrpcClient::GetInstance().GetChatServer(userInfo.uid);
		if (reply.error()) {
			std::cout << " grpc get chat server failed, error is " << reply.error() << std::endl;
			root["error"] = ErrorCodes::RPCFAILED;
			std::string jsonstr = root.toStyledString();
			beast::ostream(con->_response.body()) << jsonstr;
			return;
		}
		std::cout << "succeed to load userinfo uid is " << userInfo.uid << std::endl;
		root["error"] = ErrorCodes::SUCCESS;
		root["email"] = src_root["email"].asString();
		root["uid"] = userInfo.uid;
		root["token"] = reply.token(); // 将IP和token返回给客户端
		root["host"] = reply.host();
		root["port"] = reply.port();
		std::string jsonstr = root.toStyledString();
		beast::ostream(con->_response.body()) << jsonstr;
		return;
		});
}

bool LogicSystem::HandleGet(std::string url, std::shared_ptr<HttpConnection> con) {
	if (_get_handlers.find(url) == _get_handlers.end()) {
		return false;
	}
	_get_handlers[url](con);
	return true;
}

void LogicSystem::RegGet(std::string url, HttpHandler handler) {
	_get_handlers.insert(std::make_pair(url, handler));
}

bool LogicSystem::HandlePost(std::string url, std::shared_ptr<HttpConnection> con) {
	if (_post_handlers.find(url) == _post_handlers.end()) {
		return false;
	}
	_post_handlers[url](con);
	return true;
}
void LogicSystem::RegPost(std::string url, HttpHandler handler) {
	_post_handlers.insert(std::make_pair(url, handler));
}