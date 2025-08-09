#ifndef DBMANAGER_H_
#define DBMANAGER_H_

// handle database interactions

#include <sqlite3.h>

#include <iostream>
#include <mutex>
#include <thread>

#include "datamanager.h"

/*
// stock_data table in database
CREATE TABLE stock_data (id INTEGER PRIMARY KEY AUTOINCREMENT,
symbol TEXT,
price INTEGER,
volume INTEGER,
timestamp TEXT);
*/

class DatabaseManager {
	// db connection handle
	sqlite3* db = nullptr;
	std::mutex mtx_db;
	void insertData(const StockDataPoint& data);
	DataManager* dataManager = nullptr;
	std::thread writerThread;
public:
	DatabaseManager(DataManager *dmgr) : dataManager(dmgr) {}

	bool Connect();
	bool CreateTable();
	// wait for data (condition variable in DataManager)
	void WriterThread();
	void StartWriter();
	void Close();
	~DatabaseManager() {
		Close();
	}
};

#endif  // DBMANAGER_H_
