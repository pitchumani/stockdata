#include "dbmanager.h"

bool DatabaseManager::Connect() {
	const char *dbfile = "stockdata.sqlite3.db";
	int rc = sqlite3_open(dbfile, &db);
	if (rc) {
		std::cerr << "Could not open database " << dbfile << std::endl;
		return false;
	}
	return true;
}

void DatabaseManager::Close() {
	if (writerThread.joinable()) {
		// Push sentinel value to queue to signal shutdown
		if (dataManager) {
			dataManager->AddData(StockDataPoint("SHUTDOWN", 0, 0, ""));
		}
		writerThread.join();
	}
	if (db) {
		sqlite3_close(db);
		db = nullptr;
	}
}

bool DatabaseManager::CreateTable() {
	// get lock for db handle
	std::unique_lock<std::mutex> lock(mtx_db);

	if (!db) {
		std::cerr << "Invalid database handle" << std::endl;
		return false;
	}
	const char* create_table_sql = "CREATE TABLE IF NOT EXISTS stock_data"
		"(id INTEGER PRIMARY KEY AUTOINCREMENT,"
		" symbol TEXT,"
		" price INTEGER,"
		" volume INTEGER,"
		"timestamp TEXT);";
	char *err_msg;
	int rc = sqlite3_exec(db, create_table_sql, 0, 0, &err_msg);
	if (rc != SQLITE_OK) {
		std::cerr << "DBMgr(CreateTable): SQL Error: " << err_msg << std::endl;
		sqlite3_free(err_msg);
		return false;
	}
	return true;
}

void DatabaseManager::insertData(const StockDataPoint& data) {
	// get lock for db handle
	std::unique_lock<std::mutex> lock(mtx_db);

	std::stringstream ss;
	ss << "INSERT INTO stock_data (symbol, price, volume, timestamp) ";
	ss << "VALUES ('" << data.Symbol << "'," << data.Price << ",";
	ss << data.Volume << "," << "'" << data.Timestamp << "');";
	std::string query_string = ss.str();
	const char *insert_sql = query_string.c_str();
	char *err_msg;
	int rc = sqlite3_exec(db, insert_sql, 0, 0, &err_msg);
	if (rc != SQLITE_OK) {
		std::cerr << "DBMgr(insertData): SQLError: " << err_msg << std::endl;
		sqlite3_free(err_msg);
	}
}

void DatabaseManager::WriterThread() {
    while (true) {
        std::optional<StockDataPoint> data = dataManager->GetData();
        if (data.has_value()) {
            if (data->Symbol == "SHUTDOWN") {
                break;
            }
            insertData(data.value());
        }
    }
}

void DatabaseManager::StartWriter() {
    if (writerThread.joinable()) {
        // Already running
        return;
    }
    writerThread = std::thread(&DatabaseManager::WriterThread, this);
}
