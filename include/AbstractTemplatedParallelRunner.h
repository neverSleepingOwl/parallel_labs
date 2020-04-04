//
// Created by owl on 31.03.2020.
//

#ifndef MULTYTHREADING_ABSTRACTTEMPLATEDPARALLELRUNNER_H
#define MULTYTHREADING_ABSTRACTTEMPLATEDPARALLELRUNNER_H

#include <string>
#include "Callable.h"
#include <vector>
#include <tr1/memory>


class AbstractConcurrencyUnit{
public:
    AbstractCallable * executable = nullptr;
    virtual void set_pid() = 0;  // receive pid from getpid method
    virtual unsigned long get_pid() const = 0; // getter for private pid field
    virtual void set_running(bool _running) = 0; // set process _running
    virtual bool get_running() const = 0;  // getter for running field
    virtual void set_inner_uid(uint16_t inner_uid) = 0;
    virtual uint16_t get_inner_uid() const = 0;
};

class BaseThread: public AbstractConcurrencyUnit{
protected:
    bool running = false;
    uint16_t inner_uid = 0;
public:
    unsigned long pid = 0;
    explicit BaseThread(AbstractCallable * callable){
        this->executable = callable;
    };
    void set_pid() override {this->pid = 0;};  // receive pid from getpid method
    unsigned long get_pid() const override{return 0;}; // getter for private pid field
    void set_running(bool _running) override {this->running = _running;}; // set process _running
    bool get_running() const override {return this->running;};  // getter for running field
    void set_inner_uid(uint16_t _inner_uid) override {this->inner_uid = _inner_uid;};
    uint16_t get_inner_uid() const override {return this->inner_uid;};
};


class AbstractParallelRunner{
public:
    virtual void run() = 0;
    virtual void add(AbstractCallable * func) = 0;
    virtual void kill_except(uint16_t inner_process_id, int signal) = 0;
    virtual std::shared_ptr<AbstractSystemInterrupter> get_signal_handler(AbstractConcurrencyUnit *) = 0;
};

template <typename T>
class AbstractTemplatedParallelRunner {
protected:
    std::vector<T> _proccesses;
public:
    virtual void run() = 0;
    void add(AbstractCallable * func){
        static_assert(std::is_base_of<AbstractConcurrencyUnit, T>::value,
                      "AbstractHandler template arg must derive "
                      "from AbstractSystemInterrupter");
        T process = T(func);
        process.set_inner_uid(this->_proccesses.size());
        auto handler = this->get_signal_handler(&process);
        process.executable->set_sig_handler(handler);
        this->_proccesses.push_back(process);
    };
    virtual void kill_except(uint16_t inner_process_id, int signal) = 0;

    virtual std::shared_ptr<AbstractSystemInterrupter> get_signal_handler(AbstractConcurrencyUnit *) = 0;

    ~AbstractTemplatedParallelRunner(){
        std::cout<<"Deleting AbstractTemplatedParallelRunner instance."<<std::endl;
        while(this->_proccesses.size() > 0){
            std::cout<<"Deleting AbstractTemplatedParallelRunner. a"<<std::endl;
            auto item = this->_proccesses.back();
            this->_proccesses.pop_back();
            delete item.executable;
        }
    }
};

#endif //MULTYTHREADING_ABSTRACTTEMPLATEDPARALLELRUNNER_H
