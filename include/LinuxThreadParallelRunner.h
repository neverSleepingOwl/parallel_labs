#ifndef MULTYTHREADING_LINUXTHREADPARALLELRUNNER_H
#define MULTYTHREADING_LINUXTHREADPARALLELRUNNER_H

#include <pthread.h>
#include <tr1/memory>
#include "AbstractTemplatedParallelRunner.h"
#include "LinuxProccessParallelRunner.h"

class LinuxThread: public LinuxProcess{
public:
    LinuxThread(AbstractCallable * callable):LinuxProcess(callable){
    }
};



template <typename AbstractHandler>
class LinuxThreadParallelRunner: public AbstractTemplatedParallelRunner<LinuxThread>{
// Multythreading in linux using libpthread
// threads are created with pthread_create and deleted using
// pthread_cancel
// AbstractHandler must be subclass of AbstractSystemInterrupter
// which either kills all thread with kill_except class method (which invokes pthread_cancel)
// or sets lock/semaphore/writes to pipe/socket to notify other threads of
// first thread being completed
public:
    LinuxThreadParallelRunner() {
        this->_proccesses = std::vector<LinuxThread>();
    }

    static void * run_single(void * data){
        // wrapper with pthread thread callback signature
        // to pass to pthread_create
        // runs, checks and prints result
        LinuxThread * thread = static_cast<LinuxThread *>(data);
        thread->set_running(true);
        thread->executable->run();
        if ( thread->executable->has_result()) {
            std::string result =  thread->executable->to_string();
            std::cout<<"Result: \""<<result<<"\" in thread "<<thread->get_inner_uid()<<";"<<std::endl;
        }
        thread->set_running(false);
    }

    void run() override {
        unsigned long size = this->_proccesses.size() - 1;
        if ((size) < 1000) { // this check to avoid DOS
            // if to many threads are spawned
            // OS hangs
            for (int i = 0; i < this->_proccesses.size(); i++){
                // create threads with pids and set their inner ids
                int code = pthread_create((pthread_t *)&this->_proccesses[i].pid, NULL, LinuxThreadParallelRunner::run_single, (void *)&this->_proccesses[i]);
                std::cout<<"creating thread"<<this->_proccesses[i].pid<<"inner id"<<this->_proccesses[i].get_inner_uid()<<std::endl;
            }
            std::cout<<"weird stuff here"<<std::endl;
            for (int i = 0; i < this->_proccesses.size(); i++){
                // wait for all threads to finish
                pthread_join((pthread_t)this->_proccesses[i].pid, NULL);
            }
            std::cout<<"now exiting"<<std::endl;
        }
    }

    void kill_except(uint16_t inner_process_id, int signal) override {
        // kill all threads except given one
        for (auto i = 0; i < this->_proccesses.size(); ++i){
            auto thread = this->_proccesses[i];
            if (i != inner_process_id && thread.get_running()) {
                // if thread is not running we shouldn't kill it
                pthread_cancel((pthread_t)thread.pid);
                thread.set_running(false);
            }
        }
    }

    std::shared_ptr<AbstractSystemInterrupter> get_signal_handler(AbstractConcurrencyUnit * process){
        static_assert(
                std::is_base_of<AbstractSystemInterrupter, AbstractHandler>::value,
                "AbstractHandler template arg must derive "
                "from AbstractSystemInterrupter");
//        auto handler = std::shared_ptr<AbstractSystemInterrupter>(std::make_shared<AbstractHandler>(
//                std::bind(&LinuxThreadParallelRunner::kill_except, this, process->get_inner_uid(), SIGTERM)
//        ));
        auto handler = std::shared_ptr<AbstractSystemInterrupter>(new AbstractHandler(
                std::bind(&LinuxThreadParallelRunner::kill_except, this, process->get_inner_uid(), SIGTERM)
        ));
        return handler;
    };
};


#endif //MULTYTHREADING_LINUXTHREADPARALLELRUNNER_H
