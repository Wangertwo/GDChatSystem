﻿#pragma once
#include "const.h"
#include "CServer.h"
#include "ConfigMgr.h"
#include "RedisMgr.h"
#include <iostream>
#include <string>

void TestRedisMgr() {
    assert(RedisMgr::GetInstance().Set("blogwebsite", "Allen.club"));
    std::string value = "";
    assert(RedisMgr::GetInstance().Get("blogwebsite", value));
    assert(RedisMgr::GetInstance().Get("nonekey", value) == false);
    assert(RedisMgr::GetInstance().HSet("bloginfo", "blogwebsite", "Allen.club"));
    assert(RedisMgr::GetInstance().ExistKey("bloginfo"));
    assert(RedisMgr::GetInstance().Del("bloginfo"));
    assert(RedisMgr::GetInstance().Del("bloginfo"));
    assert(RedisMgr::GetInstance().ExistKey("bloginfo") == false);
    assert(RedisMgr::GetInstance().LPush("lpushkey1", "lpushvalue1"));
    assert(RedisMgr::GetInstance().LPush("lpushkey1", "lpushvalue2"));
    assert(RedisMgr::GetInstance().LPush("lpushkey1", "lpushvalue3"));
    assert(RedisMgr::GetInstance().RPop("lpushkey1", value));
    assert(RedisMgr::GetInstance().RPop("lpushkey1", value));
    assert(RedisMgr::GetInstance().LPop("lpushkey1", value));
    assert(RedisMgr::GetInstance().LPop("lpushkey2", value) == false);
}

int main()
{
	TestRedisMgr();
	std::string gate_port_str = ConfigMgr::GetInstance()["GateServer"]["Port"];
	unsigned short gate_port = atoi(gate_port_str.c_str()); // c_str()->int
	try {
		net::io_context ioc{ 1 };
		boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
		signals.async_wait([&ioc](const boost::system::error_code& error, int signal_number) {
			if (error) {
				return;
			}
			ioc.stop();
			});
		std::make_shared<CServer>(ioc, gate_port)->Start();
		std::cout << "GateServer listen on port: " << gate_port << std::endl;
		ioc.run();
	}
	catch (std::exception& ex) {
		std::cerr << "Error: " << ex.what() << std::endl;
		return EXIT_FAILURE;
	}
}


