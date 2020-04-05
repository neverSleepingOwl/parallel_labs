#pragma clang diagnostic push
#pragma ide diagnostic ignored "performance-unnecessary-value-param"
#ifndef MULTYTHREADING_ABSTRACTPROCESSFACTORY_H
#define MULTYTHREADING_ABSTRACTPROCESSFACTORY_H

#include "AbstractTemplatedParallelRunner.h"
#include "PlatformDefiner.h"
#include <string>
#include <tr1/memory>
#include <boost/optional.hpp>


const std::string process = "process";
const std::string thread = "thread";
const std::string mpi_process = "mpi_process";
const std::string openmp = "openmp";
const std::string signal_interrupter = "signal";
const std::string cancel_interrupter = "cancel";
const std::string mpi_point_to_point_interrupter = "mpip2p";
const std::string mpi_broadcast_interrupter = "mpibroadcast";


#if defined(PLATFORM_CODE)
    #if PLATFORM_CODE == unix
        #include "LinuxProccessParallelRunner.h"
        #include "LinuxThreadParallelRunner.h"
        #include "OpenMpParallelRunner.h"
        #include "MPIParallelRunner.h"

        boost::optional<std::shared_ptr<AbstractParallelRunner>> get_runner(std::string  name, std::string  interrupter){
            boost::optional<std::shared_ptr<AbstractParallelRunner>> ptr;
            if (name == process){
                if (interrupter == signal_interrupter) {
                    ptr = std::make_shared<LinuxProccessParallelRunner<LinuxSignalSystemInterrupter>>();
                }
            }else if(name == thread){
                if (interrupter == cancel_interrupter) {
                    ptr =  std::make_shared<LinuxThreadParallelRunner<LinuxSignalSystemInterrupter>>();
                }
            }else if (name == openmp){
                ptr = std::make_shared<OpenMpParallelRunner>();
            }else if (name == mpi_process){
                if (interrupter == mpi_point_to_point_interrupter){
                    ptr =  std::make_shared<MPIParallelRunner<MpiPointToPointSystemInterrupter>>();
                } else if(interrupter == mpi_broadcast_interrupter){
                    ptr =  std::make_shared<MPIParallelRunner<MpiBroadcastSystemInterrupter>>();
                }
            }
            return ptr;
        }
    #elif PLATFORM_CODE == windows
        boost::optional<std::shared_ptr<AbstractParallelRunner>> AbstractParallelRunner * get_runner();
    #endif
#endif
#endif //MULTYTHREADING_ABSTRACTPROCESSFACTORY_H

#pragma clang diagnostic pop