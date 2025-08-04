#include "datamanager.h"

void DataManager::AddData(const StockDataPoint& data) {
	std::unique_lock<std::mutex> lock(mtxDatapoints);
	datapoints.push(data);
	cvNewData.notify_one();
}

bool DataManager::isQEmpty() {
	std::unique_lock<std::mutex> lock(mtxDatapoints);
	return datapoints.size() == 0;
}

StockDataPoint DataManager::GetData() {
	std::unique_lock<std::mutex> lock(mtxDatapoints);
	// wait for new data
	cvNewData.wait(lock, [this] { return !datapoints.empty(); });
	// get the data from the queue
	auto d = datapoints.front();
	// remove the data from queue after fetch
	datapoints.pop();
	return d;
}
