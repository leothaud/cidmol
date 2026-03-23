/* Copyright 2026 Dylan Leothaud
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
export module core:logger;

import :stream;
import :mutex;

namespace core {

export class Logger {
  FdStream *infoStream, *warningStream, *errorStream;
  bool labels;
#ifdef CORE_THREAD
  bool shared;
  Mutex lock;
#endif
  void infoImpl(auto elt) { *infoStream << elt; }
  void warningImpl(auto elt) { *warningStream << elt; }
  void errorImpl(auto elt) { *errorStream << elt; }

public:
  Logger(const Logger &other)
      : infoStream(other.infoStream), warningStream(other.warningStream),
        errorStream(other.errorStream), labels(other.labels)
#ifdef CORE_THREAD
        ,
        shared(other.shared), lock(other.lock)
#endif
  {
  }

  Logger &operator=(const Logger &other) {
    infoStream = other.infoStream;
    warningStream = other.warningStream;
    errorStream = other.errorStream;
    labels = other.labels;
#ifdef CORE_THREAD
    shared = other.shared;
    lock = other.lock;
#endif
    return *this;
  }
  Logger(Logger &&other)
      : infoStream(core::move(other.infoStream)),
        warningStream(core::move(other.warningStream)),
        errorStream(core::move(other.errorStream)),
        labels(core::move(other.labels))
#ifdef CORE_THREAD
        ,
        shared(core::move(other.shared)), lock(core::move(other.lock))
#endif
  {
  }

  Logger &operator=(Logger &&other) {
    infoStream = core::move(other.infoStream);
    warningStream = core::move(other.warningStream);
    errorStream = core::move(other.errorStream);
    labels = core::move(other.labels);
#ifdef CORE_THREAD
    shared = core::move(other.shared);
    lock = core::move(other.lock);
#endif
    return *this;
  }

  constexpr Logger(FdStream *infoStream, FdStream *warningStream,
                   FdStream *errorStream,
#ifdef CORE_THREAD
                   bool shared = true,
#endif
                   bool labels = true)
      : infoStream(infoStream), warningStream(warningStream),
        errorStream(errorStream), labels(labels)
#ifdef CORE_THREAD
        ,
        shared(shared)
#endif
  {
  }
  constexpr Logger() : Logger(nullptr, nullptr, nullptr) {}

  template <typename... T> void info(T... elts) {
#ifdef CORE_THREAD
    if (shared) {
      lock.acquire();
    }
#endif
    if (labels) {
      infoImpl("[Info]: ");
    }
    (infoImpl(forward<T>(elts)), ...);
    infoImpl("\n");
#ifdef CORE_THREAD
    if (shared) {
      lock.release();
    }
#endif
  }

  template <typename... T> void warning(T... elts) {
#ifdef CORE_THREAD
    if (shared) {
      lock.acquire();
    }
#endif
    if (labels) {
      warningImpl("[Warning]: ");
    }
    (warningImpl(forward<T>(elts)), ...);
    warningImpl("\n");
#ifdef CORE_THREAD
    if (shared) {
      lock.release();
    }
#endif
  }

  template <typename... T> void error(T... elts) {
#ifdef CORE_THREAD
    if (shared) {
      lock.acquire();
    }
#endif
    if (labels) {
      errorImpl("[Error]: ");
    }
    (errorImpl(forward<T>(elts)), ...);
    errorImpl("\n");
#ifdef CORE_THREAD
    if (shared) {
      lock.release();
    }
#endif
  }
};

export Logger stdLogger;

#ifdef CORE_THREAD
export Logger &getThreadLogger() {
  static thread_local Logger logger = Logger(&sout, &sout, &serr);
  return logger;
}
#endif

export void initializeLoggers() { stdLogger = Logger(&sout, &sout, &serr); }

} // namespace core