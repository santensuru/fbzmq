/**
 * Copyright (c) 2014-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#include "LogSample.h"

#include <folly/DynamicConverter.h>
#include <folly/json.h>

namespace {

/**
 * These are well defined constants. DO-NOT change these.
 */
const std::string INT_KEY{"int"};
const std::string DOUBLE_KEY{"double"};
const std::string STRING_KEY{"normal"};
const std::string STRINGVECTOR_KEY{"normvector"};
const std::string STRINGTAGSET_KEY{"tagset"};

const std::string kTimeCol{"time"};

} // anonymous namespace

namespace fbzmq {

LogSample::LogSample()
  : LogSample(std::chrono::system_clock::now()) {}

LogSample::LogSample(std::chrono::system_clock::time_point timestamp)
  : timestamp_(timestamp) {
  json_ = folly::dynamic::object;

  // Initialize most frequently used column types
  json_[INT_KEY] = folly::dynamic::object;
  json_[STRING_KEY] = folly::dynamic::object;

  // Initialize the timestamps
  json_[INT_KEY][kTimeCol] = std::chrono::duration_cast<std::chrono::seconds>(
      timestamp_.time_since_epoch()).count();
}

std::string
LogSample::toJson() const {
  return folly::toJson(json_);
}

void
LogSample::addInt(folly::StringPiece key, int64_t value) {
  json_[INT_KEY][key] = value;
}

void
LogSample::addDouble(folly::StringPiece key, double value) {
  if (json_.find(DOUBLE_KEY) == json_.items().end()) {
    json_.insert(DOUBLE_KEY, folly::dynamic::object());
  }

  json_[DOUBLE_KEY][key] = value;
}

void
LogSample::addString(folly::StringPiece key, folly::StringPiece value) {
  json_[STRING_KEY][key] = value;
}

void
LogSample::addStringVector(
    folly::StringPiece key,
    const std::vector<std::string>& values) {
  if (json_.find(STRINGVECTOR_KEY) == json_.items().end()) {
    json_.insert(STRINGVECTOR_KEY, folly::dynamic::object());
  }

  json_[STRINGVECTOR_KEY][key] = folly::dynamic(values.begin(), values.end());
}

void
LogSample::addStringTagset(
    folly::StringPiece key,
    const std::set<std::string>& tags) {
  if (json_.find(STRINGTAGSET_KEY) == json_.items().end()) {
    json_.insert(STRINGTAGSET_KEY, folly::dynamic::object());
  }

  json_[STRINGTAGSET_KEY][key] = folly::dynamic(tags.begin(), tags.end());
}

int64_t
LogSample::getInt(folly::StringPiece key) const {
  return folly::convertTo<int64_t>(getInnerValue(INT_KEY, key));
}

double
LogSample::getDouble(folly::StringPiece key) const {
  return folly::convertTo<double>(getInnerValue(DOUBLE_KEY, key));
}

std::string
LogSample::getString(folly::StringPiece key) const {
  return folly::convertTo<std::string>(getInnerValue(STRING_KEY, key));
}

std::vector<std::string>
LogSample::getStringVector(folly::StringPiece key) const {
  return folly::convertTo<std::vector<std::string>>(
      getInnerValue(STRINGVECTOR_KEY, key));
}

std::set<std::string>
LogSample::getStringTagset(folly::StringPiece key) const {
  return folly::convertTo<std::set<std::string>>(
      getInnerValue(STRINGTAGSET_KEY, key));
}

bool
LogSample::isIntSet(folly::StringPiece key) const {
  return isInnerValueSet(INT_KEY, key);
}

bool
LogSample::isDoubleSet(folly::StringPiece key) const {
  return isInnerValueSet(DOUBLE_KEY, key);
}

bool
LogSample::isStringSet(folly::StringPiece key) const {
  return isInnerValueSet(STRING_KEY, key);
}

bool
LogSample::isStringVectorSet(folly::StringPiece key) const {
  return isInnerValueSet(STRINGVECTOR_KEY, key);
}

bool
LogSample::isStringTagsetSet(folly::StringPiece key) const {
  return isInnerValueSet(STRINGTAGSET_KEY, key);
}

bool
LogSample::isInnerValueSet(
    folly::StringPiece keyType,
    folly::StringPiece key) const {
  if (auto obj = json_.get_ptr(keyType)) {
    if (auto data = obj->get_ptr(key)) {
      (void)data;
      return true;
    }
  }

  return false;
}

const folly::dynamic&
LogSample::getInnerValue(
    folly::StringPiece keyType,
    folly::StringPiece key) const {
  if (auto obj = json_.get_ptr(keyType)) {
    if (auto data = obj->get_ptr(key)) {
      return *data;
    }
  }

  throw std::invalid_argument(folly::sformat(
        "invalid key: {} with keyType: {} ", key, keyType));
}

} // namespace fbzmq
