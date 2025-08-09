#include <gtest/gtest.h>
#include "dataingestor.h"
#include "datamanager.h"
#include "dbmanager.h"

class MockDataManager : public DataManager {
    // Optionally override DataManager methods for controlled testing
};

class MockDataIngestor : public DataIngestor {
public:
	MockDataIngestor(DataManager *dm) : DataIngestor(dm) {
		DataIngestor::symbols.push_back("SHUTDOWN");
	}
};

TEST(DatabaseManagerTest, ConstructorInitializesDataManager) {
    MockDataManager dm;
    DatabaseManager dbm(&dm);
    SUCCEED();
}

TEST(DatabaseManagerTest, ConnectCreateTableCloseDoesNotCrash) {
    MockDataManager dm;
    DatabaseManager dbm(&dm);
    EXPECT_NO_THROW(dbm.Connect());
    EXPECT_NO_THROW(dbm.CreateTable());
    EXPECT_NO_THROW(dbm.Close());
}

TEST(DatabaseManagerTest, StartWriterDoesNotCrash) {
    MockDataManager dm;
	MockDataIngestor di(&dm);
	di.Start();
	{
		DatabaseManager dbm(&dm);
		dbm.Connect();
		dbm.CreateTable();
		EXPECT_NO_THROW(dbm.StartWriter());
		dm.AddData(StockDataPoint("SHUTDOWN", 0, 0, ""));
		dbm.Close();
	}
	di.Stop();
	SUCCEED();
}

// This test ensures correct destruction order: DatabaseManager before DataManager
TEST(DatabaseManagerTest, WriterThreadShutdownSafe) {
    MockDataManager dm;
    {
        DatabaseManager dbm(&dm);
        dbm.Connect();
        dbm.CreateTable();
        dbm.StartWriter();
        // Push shutdown sentinel to allow thread to exit
        dm.AddData(StockDataPoint("SHUTDOWN", 0, 0, ""));
        dbm.Close(); // join thread before dm is destroyed
    }
    // dm is destroyed after dbm, so no segfault
    SUCCEED();
}
