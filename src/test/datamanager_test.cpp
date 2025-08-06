#include <gtest/gtest.h>
#include "datamanager.h"

TEST(StockDataPointTest, ConstructorSetsFields) {
    StockDataPoint dp("AAPL", 123.45f, 1000, "2025-08-06 12:00:00");
    EXPECT_EQ(dp.Symbol, "AAPL");
    EXPECT_FLOAT_EQ(dp.Price, 123.45f);
    EXPECT_EQ(dp.Volume, 1000);
    EXPECT_EQ(dp.Timestamp, "2025-08-06 12:00:00");
}

TEST(StockDataFactoryTest, TimestampStringFormat) {
    std::string ts = StockDataFactory::TimestampString();
    EXPECT_FALSE(ts.empty());
    EXPECT_EQ(ts.length(), 19); // "YYYY-MM-DD HH:MM:SS"
}

TEST(StockDataFactoryTest, GetReturnsValidDataPoint) {
    StockDataPoint dp = StockDataFactory::Get("AAPL");
    EXPECT_EQ(dp.Symbol, "AAPL");
    EXPECT_FALSE(dp.Timestamp.empty());
}
