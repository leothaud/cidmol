import core;
import ast;
import frontend;

core::Mutex mutex;
static constexpr int x_size = 1024 * 1024;
volatile int x[x_size];

struct X {};

void f(int a, X) {
  mutex.acquire();
  core::sout << "f: " << a << "\n";
}

void g() {
  // mutex.acquire();
  for (volatile int i = x_size - 1; i >= 0; i = i - 1) {
    core::sout << x[i] << "\n";
  }
  // mutex.release();
}

int main() {
  X x;
  (void)x;
  mutex.acquire();
  core::startThread(f, 1, x);
  core::sleep(3);
  mutex.release();
  core::sleep(2);
  core::sout << "Ok!\n";
  return 0;
}