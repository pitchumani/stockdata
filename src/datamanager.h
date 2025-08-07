#ifndef DATAMANAGER_H_
#define DATAMANAGER_H_

#include <condition_variable>
#include <cstdlib> // rand() srand()
#include <ctime>   // time() to see srand()
#include <iomanip> // put_time()
#include <mutex>
#include <optional>
#include <queue>
#include <random>
#include <sstream>

struct StockDataPoint {
	std::string Symbol;
	float Price;
	int Volume;
	std::string Timestamp;
	StockDataPoint(std::string s, float p, int v, std::string ts) :
		Symbol(s), Price(p), Volume(v), Timestamp(ts) {}
};

class StockDataFactory {
public:
	static std::string TimestampString() {
		auto now = std::chrono::system_clock::now();
		std::time_t time_t_now = std::chrono::system_clock::to_time_t(now);
		std::tm tm_now = *std::localtime(&time_t_now); // Use gmtime for UTC

		std::stringstream ss;
		ss << std::put_time(&tm_now, "%Y-%m-%d %H:%M:%S"); // Example format
		return ss.str();
	}

	static StockDataPoint Get(std::string sym) {
		srand(time(0));
		float p = static_cast<float>(rand());
		int v = rand();
		return StockDataPoint(sym, p, v, TimestampString());
	}
};

class DataManager {
protected:
	std::queue<StockDataPoint> datapoints;
	// helper to check if the queue is empty
	bool isQEmpty();

public:
	DataManager() {}

	// mutext to protect datapoints access
	std::mutex mtxDatapoints;
	// variable to signal the new data
	std::condition_variable cvNewData;
	bool dataAvailable = false;

	// add a datapoint, notify any waiting threads
	void AddData(const StockDataPoint& data);
	// retrieves the next available data point, block is the queue is empty
	std::optional<StockDataPoint> GetData();

};

#endif  // DATAMANAGER_H_
