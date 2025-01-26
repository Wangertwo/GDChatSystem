#pragma once
#include "const.h"
#include "Singleton.h"
class AsioIOServerPool : public Singleton<AsioIOServerPool>
{
	friend class Singleton<AsioIOServerPool>;
public:
	using IOService = boost::asio::io_context;
	using Work = boost::asio::io_context::work;
	using WorkPtr = std::unique_ptr<Work>;
	void Stop();
	~AsioIOServerPool();
	boost::asio::io_context& GetIOService();
private:
	AsioIOServerPool(std::size_t size = std::thread::hardware_concurrency());
	std::vector<IOService> _IOServices;
	std::vector<WorkPtr> _works;
	std::vector<std::thread> _threads;
	std::size_t _nextIOService;
};

