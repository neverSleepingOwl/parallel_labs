#include "include/LinuxProccessParallelRunner.h"
#include "include/LinuxThreadParallelRunner.h"
#include "include/parallel_decryptor.h"
#include <string>
#include "include/OpenMpParallelRunner.h"
#include "include/MPIParallelRunner.h"


int main() {
    auto encrypted_vector = twofish_cryptor::encrypt(10000, "AAAAAAAA");
    auto callables = parrallel_decryptor::prepare_mapped_functions(0, 10000000, 5, std::string("AAAAAAAA"), encrypted_vector);
//    auto runner = LinuxThreadParallelRunner<LinuxSignalSystemInterrupter>();
    auto runner = LinuxProccessParallelRunner<LinuxSignalSystemInterrupter>();
//    auto runner = OpenMpParallelRunner();
//    MpiBroadcastSystemInterrupter
//    auto runner = MPIParallelRunner<MpiPointToPointSystemInterrupter>();
//    auto runner = MPIParallelRunner<MpiBroadcastSystemInterrupter>();
    for (auto & callable : callables) {
        runner.add(callable);
    }
    runner.run();
    return 0;
}
