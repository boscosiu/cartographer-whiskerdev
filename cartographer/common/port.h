/*
 * Copyright 2016 The Cartographer Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CARTOGRAPHER_COMMON_PORT_H_
#define CARTOGRAPHER_COMMON_PORT_H_

#include <zlib.h>

#include <cinttypes>
#include <cmath>
#include <memory>
#include <string>

namespace cartographer {

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

namespace common {

inline int RoundToInt(const float x) { return std::lround(x); }

inline int RoundToInt(const double x) { return std::lround(x); }

inline int64 RoundToInt64(const float x) { return std::lround(x); }

inline int64 RoundToInt64(const double x) { return std::lround(x); }

inline void GzipOp(const std::string& src, std::string* dest, bool compress) {
  const auto buf_size = 65536;
  const auto buf = std::make_unique<Bytef[]>(buf_size);
  int response;
  z_stream zstream{};

  if (compress) {
    deflateInit2(&zstream, Z_BEST_SPEED, Z_DEFLATED, 31, 8, Z_DEFAULT_STRATEGY);
  } else {
    inflateInit2(&zstream, 31);
  }

  zstream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(src.data()));
  zstream.avail_in = src.size();
  dest->clear();

  do {
    zstream.next_out = buf.get();
    zstream.avail_out = buf_size;

    if (compress) {
      response = deflate(&zstream, Z_FINISH);
    } else {
      response = inflate(&zstream, Z_FINISH);
    }

    dest->append(reinterpret_cast<const char*>(buf.get()),
                 buf_size - zstream.avail_out);
  } while (response != Z_STREAM_END);

  if (compress) {
    deflateEnd(&zstream);
  } else {
    inflateEnd(&zstream);
  }
}

inline void FastGzipString(const std::string& uncompressed,
                           std::string* compressed) {
  GzipOp(uncompressed, compressed, true);
}

inline void FastGunzipString(const std::string& compressed,
                             std::string* decompressed) {
  GzipOp(compressed, decompressed, false);
}

}  // namespace common
}  // namespace cartographer

#endif  // CARTOGRAPHER_COMMON_PORT_H_
