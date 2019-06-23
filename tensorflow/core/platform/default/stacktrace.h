/* Copyright 2016 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef TENSORFLOW_CORE_PLATFORM_DEFAULT_STACKTRACE_H_
#define TENSORFLOW_CORE_PLATFORM_DEFAULT_STACKTRACE_H_

// clang-format off
#include "tensorflow/core/platform/platform.h"
// clang-format on

#include <string>
#include "absl/debugging/symbolize.h"
#include "absl/debugging/stacktrace.h"
#include "absl/strings/str_format.h"

namespace tensorflow {

// Function to create a pretty stacktrace.
inline std::string CurrentStackTrace() {
  constexpr int kNumStackFrames = 64;
  void* stack[kNumStackFrames];
  int frame_sizes[kNumStackFrames];
  int depth = absl::GetStackFrames(stack, frame_sizes, kNumStackFrames, 1);

  char buf[1024];
  std::string stacktrace;
  for (int i = 0; i < depth; ++i) {
    const char* symbol = "(unknown)";
    if (absl::Symbolize(stack[i], buf, sizeof(buf))) {
      symbol = buf;
    }
    
    std::string line;
    if (frame_sizes[i] <= 0) {
      line = absl::StrFormat("%p  (unknown)  %s\n", stack[i], symbol);
    } else {
      line = absl::StrFormat("%p  %9d  %s\n", stack[i], frame_sizes[i], symbol);
    }
    
    stacktrace += line;
  }

  return stacktrace;
}

inline void DebugWriteToString(const char* data, void* arg) {
  reinterpret_cast<std::string*>(arg)->append(data);
}

// A dummy class that does nothing.  Someday, add real support.
class SavedStackTrace {
 public:
  SavedStackTrace() {}

  void CreateCurrent(int skip_count) {}

  void Reset() {}

  typedef void DebugWriter(const char*, void*);
  void Dump(DebugWriter* writerfn, void* arg) const {}

  int depth() const { return 0; }
  void* const* stack() const { return stack_; }

 private:
  void* stack_[32];
};

}  // namespace tensorflow

#endif  // TENSORFLOW_CORE_PLATFORM_DEFAULT_STACKTRACE_H_
