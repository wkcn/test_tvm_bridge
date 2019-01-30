#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <cstdio>
#include <dlfcn.h>
#include <cstring>
#include "tvm_packed_func.h"
using namespace std;
using namespace tvm;

using TVMFunc = void (*)(TVMArgs, TVMRetValue*);
PackedFunc WrapAsyncCall;

template <typename T>
void print_args(T *args) {
    char *ss = (char*)(void*)args;
    cout << "SIZEOF: " << sizeof(T) << endl;
    for (int u = 0;u < sizeof(T); ++u) {
      printf("%x, ", ss[u] & 0xFF);
    }
    cout << endl;
}

extern "C" {

void SetMXTVMBridge(int (*MXTVMBridge)(PackedFunc*)) {
  PackedFunc *packed_func = new PackedFunc([](TVMArgs args, TVMRetValue*) {
      cout << "num_args: " << args.num_args << endl;
      // print args.values[0].v_str
      for (int i = 0; i < 13; ++i) {
        cout << args.values[0].v_str[i];
      }
      cout << endl;
      WrapAsyncCall = *static_cast<PackedFunc*>(args.values[1].v_handle);
      if (strncmp(args.values[0].v_str, "WrapAsyncCall", 13) == 0) {
        WrapAsyncCall = *static_cast<PackedFunc*>(args.values[1].v_handle);
        return;
      }
      throw std::runtime_error("Not register WrapAsyncCall");
  });
  MXTVMBridge(packed_func);
}

};

int main() {
  cout << sizeof(TVMValue) << endl;
  cout << sizeof(TVMArgs) << endl;
  cout << "==========" << endl;
  cout << sizeof(const TVMValue*) << ", " << sizeof(const int*) << ", " << sizeof(int) << endl;
  cout << sizeof(TVMRetValue) << endl;

  // the path of libmxnet.so
  const char path[] = "/usr/lib/python3.7/site-packages/mxnet/libmxnet.so";

  typedef int(*MXTVMBridge_)(PackedFunc *); 
  MXTVMBridge_ bridge = nullptr;

  void *handle = dlopen(path, RTLD_LAZY);
  cout << handle << endl;
  bridge = (MXTVMBridge_)dlsym(handle, "MXTVMBridge");

  cout << (void*)bridge << endl;
  SetMXTVMBridge(bridge);
  return 0;
}
