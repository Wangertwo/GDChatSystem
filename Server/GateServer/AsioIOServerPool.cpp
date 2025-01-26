#include "AsioIOServerPool.h"

AsioIOServerPool::AsioIOServerPool(std::size_t size) 
	:_works(size), _IOServices(size), _nextIOService(0)
{
	for (int i = 0; i < size; i++) {
		_works[i] = std::unique_ptr<Work>(new IOService::work(_IOServices[i]));
	}

	for (int i = 0; i < _IOServices.size(); i++) {
		_threads.emplace_back([i, this]() {
			this->_IOServices[i].run();
			});
	}

}

boost::asio::io_context& AsioIOServerPool::GetIOService() {
	auto& ioservice = _IOServices[_nextIOService++];
	if (_nextIOService == _IOServices.size()) {
		_nextIOService = 0;
	}
	return ioservice;
}

void AsioIOServerPool::Stop() {
	for (auto& work : _works) {
		work->get_io_context().stop();
		work.reset();
	}
	for (auto& t : _threads) {
		t.join();
	}
}

AsioIOServerPool::~AsioIOServerPool() {
	this->Stop();
	std::cout << "AsioIOServerPool destoried!" << std::endl;
}