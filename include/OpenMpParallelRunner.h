//
// Created by owl on 02.04.2020.
//

#ifndef MULTYTHREADING_OPENMPPARALLELRUNNER_H
#define MULTYTHREADING_OPENMPPARALLELRUNNER_H

#include "AbstractTemplatedParallelRunner.h"
#include "Callable.h"
#include <omp.h>


class OpenMpSystemInterrupter: public AbstractSystemInterrupter{
// Yet we can't interrupt
// OpenMp threads, but Callable requires AbstractSystemInterrupter
// to run
public:
    explicit OpenMpSystemInterrupter() = default;
    void handle_success() override{};
    void handle_error() override {};
    bool check_next_sync_call() override {return true;};
};

class OpenMpThread: public AbstractConcurrencyUnit{
protected:
    bool running = false;
    uint16_t inner_uid = 0;
public:
    unsigned long pid = 0;
    explicit OpenMpThread(AbstractCallable * callable){
        this->executable = callable;
    };
    void set_pid() override {this->pid = 0;};  // receive pid from getpid method
    unsigned long get_pid() const override{return 0;}; // getter for private pid field
    void set_running(bool _running) override {this->running = _running;}; // set process _running
    bool get_running() const override {return this->running;};  // getter for running field
    void set_inner_uid(uint16_t _inner_uid) override {this->inner_uid = _inner_uid;};
    uint16_t get_inner_uid() const override {return this->inner_uid;};
};

class OpenMpParallelRunner: public AbstractTemplatedParallelRunner<OpenMpThread>{
public:
    #pragma clang diagnostic push
    #pragma ide diagnostic ignored "modernize-loop-convert"
    void run() override {
        omp_set_num_threads(this->_proccesses.size());
        #pragma omp parallel for
        for(int i = 0; i < this->_proccesses.size(); i++){
            auto proc = &this->_proccesses[i];
            proc->set_running(true);
            proc->executable->run();
            proc->set_running(false);
            if ( proc->executable->has_result()) {
                std::string result =  proc->executable->to_string();
                std::cout<<"Result: \""<<result<<"\" in thread "<<proc->get_inner_uid()<<";"<<std::endl;
            }
        }
    }
#pragma clang diagnostic pop
    void kill_except(uint16_t inner_process_id, int signal) override {};
    std::shared_ptr<AbstractSystemInterrupter> get_signal_handler(AbstractConcurrencyUnit *) override{
        std::shared_ptr<AbstractSystemInterrupter> ptr = std::make_shared<OpenMpSystemInterrupter>();
        return ptr;
    }
};


#endif //MULTYTHREADING_OPENMPPARALLELRUNNER_H
