#ifndef MULTYTHREADING_LINUXTHREADPARALLELRUNNER_H
#define MULTYTHREADING_LINUXTHREADPARALLELRUNNER_H

#include <tr1/memory>
#include "AbstractTemplatedParallelRunner.h"


#if defined(PLATFORM_CODE)
    #if PLATFORM_CODE == linux
        #include "LinuxProccessParallelRunner.h"
        #include <csignal>
        #include <pthread.h>

        class LinuxThread: public LinuxProcess{
        public:
            LinuxThread(AbstractCallable * callable):LinuxProcess(callable){
            }
        };


        void terminate_handler(){
            // Since c++ 11 pthread_cancel with no cancelation point throws terminate
            std::cout<<"Program finished!!!"<<std::endl;
            exit(0);
        }
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
                std::set_terminate(terminate_handler);
                pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, nullptr);
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
                    }
                    for (int i = 0; i < this->_proccesses.size(); i++){
                        // wait for all threads to finish
                        pthread_join((pthread_t)this->_proccesses[i].pid, NULL);
                    }
                }
            }

            void kill_except(uint16_t inner_process_id, int signal) override {
                // kill all threads except given one
                // Since c++ 11 throws terminate
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
                auto handler = std::shared_ptr<AbstractSystemInterrupter>(new AbstractHandler(
                        std::bind(&LinuxThreadParallelRunner::kill_except, this, process->get_inner_uid(), SIGTERM)
                ));
                return handler;
            };
        };

    #endif
#endif

#endif //MULTYTHREADING_LINUXTHREADPARALLELRUNNER_H
