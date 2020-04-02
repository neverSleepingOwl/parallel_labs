//
// Created by owl on 31.03.2020.
//

#ifndef MULTYTHREADING_ABSTRACTPARALLELRUNNER_H
#define MULTYTHREADING_ABSTRACTPARALLELRUNNER_H

#include <string>
#include "Callable.h"
#include <vector>


class AbstractParallel{
public:
    virtual void set_pid() = 0;  // receive pid from getpid method
    virtual unsigned long get_pid() const = 0; // getter for private pid field
    virtual void set_running(bool _running) = 0; // set process _running
    virtual bool get_running() const = 0;  // getter for running field
    virtual void set_inner_uid(uint16_t inner_uid) = 0;
    virtual uint16_t get_inner_uid() const = 0;
};


template <typename T>
class AbstractParallelRunner {
protected:
    std::vector<T> _proccesses;
public:
    virtual void run() = 0;
    void add(AbstractCallable * func){
        static_assert(std::is_base_of<AbstractParallel, T>::value,
                      "AbstractHandler template arg must derive "
                      "from AbstractSignalHandler");
        T process = T(func);
        process.set_inner_uid(this->_proccesses.size());
        auto handler = this->get_signal_handler(&process);
        process.executable->set_sig_handler(handler);
        this->_proccesses.push_back(process);
    };
    virtual void kill_except(uint16_t inner_process_id, int signal) = 0;
    virtual AbstractSignalHandler * get_signal_handler(AbstractParallel * ) = 0;
};

#endif //MULTYTHREADING_ABSTRACTPARALLELRUNNER_H
