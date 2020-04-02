#include "include/LinuxProccessParallelRunner.h"
#include "include/LinuxThreadParallelRunner.h"
#include "include/parallel_decryptor.h"
#include <string>
#include "include/OpenMpParallelRunner.h"


int main() {
    auto encrypted_vector = twofish_cryptor::encrypt(12323, "AAAAAAAA");
    auto callables = parrallel_decryptor::prepare_functions(0, 100000, 5, std::string("AAAAAAAA"), encrypted_vector);
//    auto runner = LinuxThreadParallelRunner<LinuxSignalSystemInterrupter>();
//    auto runner = LinuxProccessParallelRunner<LinuxSignalSystemInterrupter>();
    auto runner = OpenMpParallelRunner();
    for (auto & callable : callables) {
        runner.add(callable);
    }
    runner.run();
    return 0;
}
