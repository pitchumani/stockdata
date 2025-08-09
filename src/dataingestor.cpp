#include "dataingestor.h"

void DataIngestor::Start() {
	//std::cout << "symbols.size() = " << symbols.size() << std::endl;
	for (auto& symbol : symbols) {
		// create a thread to fetch the stock for every 100ms
		// detach it, it will stop until stopFetch flag is set
		fetchThreads.emplace_back(&DataIngestor::FetchStockData, this, symbol, 100);
			//std::thread fetchThread(&DataIngestor::FetchStockData, this,
			//					s, 100);
		//fetchThreads.push_back(std::move(fetchThread));
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

	// Track the detached threads and stop
	std::this_thread::sleep_for(std::chrono::seconds(1));
	//std::cout << "Have " << fetchThreads.size() << " threads\n";
	for (auto& fthread : fetchThreads) {
		if (fthread.joinable()) {
			fthread.join();
		}
	}
	fetchThreads.clear();
}
