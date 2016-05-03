/*
 * Copyright 2015 Twitter, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"

#include "proto/messages.h"
#include "basics/basics.h"
#include "errors/errors.h"
#include "threads/threads.h"
#include "network/network.h"

#include "basics/modinit.h"
#include "errors/modinit.h"
#include "threads/modinit.h"
#include "network/modinit.h"

#include "metrics/metrics.h"

namespace heron {
namespace common {

class MeanMetricTest : public ::testing::Test {
 public:
  MeanMetricTest() {}
  ~MeanMetricTest() {}

  void SetUp() { mean_metric_ = new MeanMetric(); }

  void TearDown() { delete mean_metric_; }

  heron::proto::system::MetricPublisherPublishMessage* CreateEmptyPublishMessage() {
    return new heron::proto::system::MetricPublisherPublishMessage();
  }

 protected:
  MeanMetric* mean_metric_;
};

TEST_F(MeanMetricTest, testRecord) {
  mean_metric_->record(5);
  heron::proto::system::MetricPublisherPublishMessage* message = CreateEmptyPublishMessage();

  sp_string prefix = "TestPrefix";
  double expectedMean = 5.0;
  mean_metric_->GetAndReset(prefix, message);

  // Only one metric datum should be present.
  EXPECT_EQ(1, message->metrics_size());

  // Check that datum.
  heron::proto::system::MetricDatum datum = message->metrics(0);
  EXPECT_STREQ(prefix.c_str(), datum.name().c_str());
  EXPECT_DOUBLE_EQ(expectedMean, atof(datum.value().c_str()));

  // Clean up.
  delete message;
}

TEST_F(MeanMetricTest, testMultipleRecord) {
  mean_metric_->record(5);
  mean_metric_->record(5);
  mean_metric_->record(2);
  heron::proto::system::MetricPublisherPublishMessage* message = CreateEmptyPublishMessage();

  sp_string prefix = "TestPrefix";
  double expectedMean = 4.0;
  mean_metric_->GetAndReset(prefix, message);

  // Only one metric datum should be present.
  EXPECT_EQ(1, message->metrics_size());

  // Check that datum.
  heron::proto::system::MetricDatum datum = message->metrics(0);
  EXPECT_STREQ(prefix.c_str(), datum.name().c_str());
  EXPECT_DOUBLE_EQ(expectedMean, atof(datum.value().c_str()));

  // Clean up.
  delete message;
}

TEST_F(MeanMetricTest, testDoubleMeanValue) {
  mean_metric_->record(2);
  mean_metric_->record(5);
  heron::proto::system::MetricPublisherPublishMessage* message = CreateEmptyPublishMessage();

  sp_string prefix = "TestPrefix";
  double expectedMean = 3.5;
  mean_metric_->GetAndReset(prefix, message);

  // Only one metric datum should be present.
  EXPECT_EQ(1, message->metrics_size());

  // Check that datum.
  heron::proto::system::MetricDatum datum = message->metrics(0);
  EXPECT_STREQ(prefix.c_str(), datum.name().c_str());
  EXPECT_DOUBLE_EQ(expectedMean, atof(datum.value().c_str()));

  // Clean up.
  delete message;
}

TEST_F(MeanMetricTest, testGetAndReset) {
  mean_metric_->record(2);
  mean_metric_->record(4);
  heron::proto::system::MetricPublisherPublishMessage* message = CreateEmptyPublishMessage();

  sp_string prefix = "TestPrefix";
  double expectedMean = 3.0;
  mean_metric_->GetAndReset(prefix, message);

  // Only one metric datum should be present.
  EXPECT_EQ(1, message->metrics_size());

  // Check that datum.
  heron::proto::system::MetricDatum datum = message->metrics(0);
  EXPECT_STREQ(prefix.c_str(), datum.name().c_str());
  EXPECT_DOUBLE_EQ(expectedMean, atof(datum.value().c_str()));

  // Clean up.
  delete message;

  // After last GetAndReset, the value should have been reset.
  // Create another message for next GetAndReset call.
  message = CreateEmptyPublishMessage();

  // Expected count should be zero.
  expectedMean = 0.0;
  mean_metric_->GetAndReset(prefix, message);

  // Only one metric datum should be present.
  EXPECT_EQ(1, message->metrics_size());

  // Check that datum.
  datum = message->metrics(0);
  EXPECT_STREQ(prefix.c_str(), datum.name().c_str());
  EXPECT_DOUBLE_EQ(expectedMean, atof(datum.value().c_str()));

  // Clean up.
  delete message;
}

TEST_F(MeanMetricTest, testMultipleDatum) {
  mean_metric_->record(4);
  heron::proto::system::MetricPublisherPublishMessage* message = CreateEmptyPublishMessage();

  sp_string prefix = "TestPrefix";
  double expectedMean = 4.0;
  mean_metric_->GetAndReset(prefix, message);

  // Only one metric datum should be present.
  EXPECT_EQ(1, message->metrics_size());

  // Check that datum.
  heron::proto::system::MetricDatum datum = message->metrics(0);
  EXPECT_STREQ(prefix.c_str(), datum.name().c_str());
  EXPECT_DOUBLE_EQ(expectedMean, atof(datum.value().c_str()));

  // Next metrics
  mean_metric_->record(2);
  mean_metric_->record(2);

  // Send the same message object.
  mean_metric_->GetAndReset(prefix, message);

  // Two metric datum should be present.
  EXPECT_EQ(2, message->metrics_size());

  // Check the first datum.
  datum = message->metrics(0);
  EXPECT_STREQ(prefix.c_str(), datum.name().c_str());
  EXPECT_DOUBLE_EQ(expectedMean, atof(datum.value().c_str()));

  // Check the second datum.
  expectedMean = 2.0;
  datum = message->metrics(1);
  EXPECT_STREQ(prefix.c_str(), datum.name().c_str());
  EXPECT_DOUBLE_EQ(expectedMean, atof(datum.value().c_str()));

  // Clean up.
  delete message;
}
}  // namespace common
}  // namespace heron

int main(int argc, char** argv) {
  heron::common::Initialize(argv[0]);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
