#pragma once
#include <boost\asio.hpp>
#include <boost\beast.hpp>
#include <boost\beast\http.hpp>
#include <iostream>
#include <memory>
#include <map>
#include <functional>
#include <unordered_map>
#include <json\json.h>
#include <json\value.h>
#include <json\reader.h>
#include <boost\filesystem.hpp>
#include <boost\property_tree\ptree.hpp>
#include <boost\property_tree\ini_parser.hpp>
#include <queue>
#include <hiredis\hiredis.h>
#include <atomic>

namespace beast = boost::beast;         
namespace http = beast::http;           
namespace net = boost::asio;            
using tcp = boost::asio::ip::tcp;

enum ErrorCodes {
	SUCCESS = 0,
	ERROR_JSON = 1001, //json解析错误
	RPCFAILED = 1002,  //RPC请求错误
	VARIFYEXPIRE = 1003, //验证码超时
	VARIFYCODEERR = 1004, //验证码输入错误
	USEREXIST = 1005, //用户已存在
	CONFIRMPASSWDVARY = 1006, //密码和确认密码不一致
	EMAILNOTMATCH = 1007, //用户邮箱不匹配
	PWDUPDATEFAILED = 1008, //密码更新失败
	PWDNOTMATCH = 1009, //密码邮箱不匹配
	TOKENINVAILD = 1010,   //Token失效
	UIDINVAILD = 1011,  //uid无效
};

class Defer {
public:
	Defer(std::function<void()> func) :_func(func) {};
	~Defer() {
		_func();
	}
private:
	std::function<void()> _func;
};

#define USERIPPREFIX  "uip_"
#define USERTOKENPREFIX  "utoken_"
#define IPCOUNTPREFIX  "ipcount_"
#define USER_BASE_INFO "ubaseinfo_"
#define LOGIN_COUNT  "logincount"