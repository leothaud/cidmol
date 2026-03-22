import core;
import ast;
import frontend;

core::Mutex mutex;
static constexpr int x_size = 1024 * 1024;
volatile int x[x_size];

int local = 2;

struct X {};

void f(u64 n) {
  core::getStdLogger().info("Before: ", local);
  core::sleep(n);
  ++local;
  core::getStdLogger().info("After: ", local);
}

int main() {
  for (int i = 0; i < 48; ++i) {
    core::startThread<f>(i);
  }
  return 0;
}