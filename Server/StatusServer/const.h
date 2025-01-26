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
	ERROR_JSON = 1001, //json��������
	RPCFAILED = 1002,  //RPC�������
	VARIFYEXPIRE = 1003, //��֤�볬ʱ
	VARIFYCODEERR = 1004, //��֤���������
	USEREXIST = 1005, //�û��Ѵ���
	CONFIRMPASSWDVARY = 1006, //�����ȷ�����벻һ��
	EMAILNOTMATCH = 1007, //�û����䲻ƥ��
	PWDUPDATEFAILED = 1008, //�������ʧ��
	PWDNOTMATCH = 1009, //�������䲻ƥ��
	TOKENINVAILD = 1010,   //TokenʧЧ
	UIDINVAILD = 1011,  //uid��Ч
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