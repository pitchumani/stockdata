#include <gtest/gtest.h>
#include "dataingestor.h"
#include "datamanager.h"

class MockDataManager : public DataManager {
    // Optionally override DataManager methods for controlled testing
public:
	bool isQEmpty() {
		return DataManager::isQEmpty();
	}
};

TEST(DataIngestorTest, ConstructorInitializesDataManager) {
    MockDataManager dm;
    DataIngestor ingestor(&dm);
    // No direct getter, but we can check for no crash
    SUCCEED();
}

TEST(DataIngestorTest, StartStopDoesNotCrash) {
    MockDataManager dm;
    DataIngestor ingestor(&dm);
	EXPECT_EQ(dm.isQEmpty(), true);
	EXPECT_EQ(ingestor.SymbolsCount(), 5);
    EXPECT_NO_THROW(ingestor.Start());
	EXPECT_EQ(ingestor.FetchThreadsCount(), 5);
    EXPECT_NO_THROW(ingestor.Stop());
}

TEST(DataIngestorTest, FetchStockDataDoesNotCrash) {
    MockDataManager dm;
    DataIngestor ingestor(&dm);
	EXPECT_EQ(dm.isQEmpty(), true);
	std::string symbol = "AAPL";
    std::thread fetch_thread(&DataIngestor::FetchStockData, &ingestor, symbol, 100);
	// wait for sometime so that fetch_thread can add some data
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	EXPECT_NO_THROW(ingestor.Stop());
    fetch_thread.join();
	EXPECT_EQ(dm.isQEmpty(), false);
}
