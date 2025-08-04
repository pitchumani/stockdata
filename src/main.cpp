// project: real time stock market data aggregator and notifier

// phase1: microservice that simulates real-time ingestion of
// stock data and persists it into a database

#include "dataingestor.h"
#include "datamanager.h"
#include "dbmanager.h"

#include <chrono>

int main() {
	// create data manager
	DataManager dataManager;
	// initialize database manager
	DatabaseManager dbManager(&dataManager);
	DataIngestor dataIngestor(&dataManager);

	// initialize database
	if (dbManager.Connect() == false) {
		std::cerr << "Failed to initialize database. Exiting." << std::endl;
		return 1;
	}

	if (dbManager.CreateTable() == false) {
		std::cerr << "Failed to create table in database. Exiting." << std::endl;
		return 1;
	}

	// start database writer and the data ingestion threads
	dbManager.StartWriter();
	dataIngestor.Start();

	// continue the services for 5 seconds
	std::chrono::seconds duration(5);
	auto start_time = std::chrono::high_resolution_clock::now();
	while(std::chrono::high_resolution_clock::now() - start_time < duration) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	std::cout << "Shutting down the services..." << std::endl;
	// shutdown sequence
	// 1. signal DataIngestor threads to stop fetching
	// 2. wait for them to finish
	// 3. push a sentinel value to the DataManager queue
	// 4. wait for DatabaseManager writer thread to finish
	// 5. close the database connection

	dataIngestor.Stop();
	// TODO: shutdown sequence
	dbManager.Close();

	return 0;
}
