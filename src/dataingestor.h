#ifndef DATAINGESTOR_H_
#define DATAINGESTOR_H_

// fetch data and populate the DataManager

#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

#include "datamanager.h"

class DataIngestor {
	std::vector<std::thread> threads;
	DataManager* dataManager;

protected:
	std::vector<std::string> symbols;
	std::atomic<bool> stopFetch{false};

public:
    DataIngestor(DataManager* dmgr) : dataManager(dmgr) {
		symbols.push_back("AAPL");
		symbols.push_back("AMZN");
		symbols.push_back("MSFT");
		symbols.push_back("MCHP");
		symbols.push_back("INTL");
	}

	// create and detach a thread for each stock symbol
	void Start();
	// Fetch stock data for given symbol
	void FetchStockData(const std::string &symbol, int interval_ms);
	// signal threads to terminate gracefully
	void Stop();
};

#endif  // DATAINGESTOR_H_
