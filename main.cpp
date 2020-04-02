#include "include/LinuxProccessParallelRunner.h"
#include "include/LinuxThreadParallelRunner.h"
#include "include/parallel_decryptor.h"
#include <string>

int main() {
    auto encrypted_vector = twofish_cryptor::encrypt(12323, "AAAAAAAA");
    auto callables = parrallel_decryptor::prepare_functions(0, 100000, 5, std::string("AAAAAAAA"), encrypted_vector);
//    auto runner = LinuxThreadParallelRunner<LinuxSignalHandler>();
    auto runner = LinuxProccessParallelRunner<LinuxSignalHandler>();
    for (auto & callable : callables) {
        runner.add(callable);
    }
    runner.run();
    return 0;
}
