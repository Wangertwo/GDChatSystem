#include "CServer.h"
#include "HttpConnection.h"
#include "AsioIOServerPool.h"
CServer::CServer(net::io_context& ioc, unsigned short& port)
	:_ioc(ioc), _acceptor(ioc, tcp::endpoint(tcp::v4(), port))
{

}

void CServer::Start() {
	auto self = shared_from_this();
	auto& ioc = AsioIOServerPool::GetInstance().GetIOService();
	std::shared_ptr<HttpConnection> new_con = std::make_shared<HttpConnection>(ioc);
	_acceptor.async_accept(new_con->GetSocket(), [self, new_con](boost::system::error_code ec) {
		try {
			if (ec) { // 连接有错误，监听下一个连接
				self->Start();
				return;
			}
			new_con->Start();
			self->Start();
		}
		catch (std::exception& ex) {
			std::cout << "exception is " << ex.what() << std::endl;
			return;
		}
		});
}

