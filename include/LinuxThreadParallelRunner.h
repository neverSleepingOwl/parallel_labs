//
// Created by owl on 01.04.2020.
//

#ifndef MULTYTHREADING_LINUXTHREADPARALLELRUNNER_H
#define MULTYTHREADING_LINUXTHREADPARALLELRUNNER_H

#include <pthread.h>
#include "AbstractParallelRunner.h"
#include "LinuxProccessParallelRunner.h"

class LinuxThread: public LinuxProcess{
public:
    LinuxThread(AbstractCallable * callable):LinuxProcess(callable){

    }
};


template <typename AbstractHandler>
class LinuxThreadParallelRunner: public AbstractParallelRunner<LinuxThread>{
private:
    uint32_t _process_to_run;
public:
    LinuxThreadParallelRunner() {
        this->_proccesses = std::vector<LinuxThread>();
        this->_process_to_run = 0;
    }
    void _run_single(){
        LinuxThread proc = this->_proccesses[this->_process_to_run];
        proc.set_running(true);
        proc.executable->run();
        if (proc.executable->has_result()) {
            std::string result = proc.executable->to_string();
            std::cout<<result<<std::endl;
        }
    }

    static void * run_single(void * data){
        LinuxThreadParallelRunner * runner = static_cast<LinuxThreadParallelRunner *>(data);
        runner->_run_single();
        return (void *)"";
    }

    void run() override {
        for (int i = 0; i <  this->_proccesses.size(); i++){
        }
        unsigned long size = this->_proccesses.size() - 1;
        if ((size) < 1000) { // this check to avoid DOS
            for (auto process: this->_proccesses){
                std::cout<<"creating thread"<<std::endl;
                int code = pthread_create((pthread_t *)&process.pid, NULL, LinuxThreadParallelRunner::run_single, (void *)this);
                this->_process_to_run += 1;
            }
            int status = 0;
            for (auto process: this->_proccesses){
                pthread_join((pthread_t)process.pid, NULL);
            }
        }
    }

    void kill_except(uint16_t inner_process_id, int signal) override {
        // kill all threads except given one
        for (auto i = 0; i < this->_proccesses.size(); ++i){
            auto process = this->_proccesses[i];
            if (i != inner_process_id && process.get_running()) {
                // kill(process.get_pid(), signal);
            }
        }
    }

    AbstractSignalHandler * get_signal_handler(AbstractParallel * process){
        static_assert(
                std::is_base_of<AbstractSignalHandler, AbstractHandler>::value,
                "AbstractHandler template arg must derive "
                "from AbstractSignalHandler");
        auto handler = new AbstractHandler(
                std::bind(&LinuxThreadParallelRunner::kill_except, this, process->get_inner_uid(), SIGKILL)
        );
        return handler;
    };
};


#endif //MULTYTHREADING_LINUXTHREADPARALLELRUNNER_H
