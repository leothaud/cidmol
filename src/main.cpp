import core;
import ast;
import frontend;

core::Mutex mutex;
static constexpr int x_size = 1024 * 1024;
volatile int x[x_size];

thread_local int local = 2;

struct X {};

void f(u64 n) {
  auto &logger = core::getThreadLogger();
  logger.info("Before: ", local);
  core::sleep(n);
  ++local;
  logger.info("After: ", local);
  core::stdLogger.info("thread ", n, " finished.");
}

int main() {
  int numThread = 8;
  for (int i = 0; i < numThread; ++i) {
    core::startThread<f>(i);
  }
  return 0;
}