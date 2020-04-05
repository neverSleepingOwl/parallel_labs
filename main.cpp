#include <string>
#include "include/parallel_decryptor.h"
#include "include/AbstractProcessFactory.h"


int main() {
    auto encrypted_vector = twofish_cryptor::encrypt(10000, "AAAAAAAA");
    auto callables = parrallel_decryptor::prepare_mapped_functions(0, 10000000, 5, std::string("AAAAAAAA"), encrypted_vector);
//    auto to_run = get_runner("thread", "cancel");
    auto to_run = get_runner("process", "signal");
//    auto to_run = get_runner("openmp", "");
//    auto to_run = get_runner("mpi_process", "mpip2p");
//    auto to_run = get_runner("mpi_process", "mpibroadcast");
    if (to_run){
        auto runner = to_run.get();
        for (auto & callable : callables) {
            runner->add(callable);
        }
        runner->run();
        return 0;
    }
}
