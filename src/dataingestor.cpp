#include "dataingestor.h"

void DataIngestor::Start() {
	std::vector<std::string> stocks({"AAPL", "AMZN", "MSFT", "MCHP", "INTL"});
	for (auto& s : stocks) {
		// create a thread to fetch the stock for every 100ms
		// detach it, it will stop until stopFetch flag is set
		std::thread fetchThread(&DataIngestor::FetchStockData, this,
								s, 100);
		fetchThread.detach();
	}
}

void DataIngestor::FetchStockData(const std::string &symbol,
								  int interval_ms) {
	while (!stopFetch.load()) {
		// thread to get the stock data
		// simulate the stock fetch for now
		auto data = StockDataFactory::Get(symbol);
		dataManager->AddData(data);
		std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
	}
	//std::cout << "FetchStockData thread is stopped" << std::endl;
}

void DataIngestor::Stop() {
	// set stopFetch flag to true, so that the threads can stop
	stopFetch.store(true);
}
