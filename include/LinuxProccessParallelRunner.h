#ifndef MULTYTHREADING_LINUXPROCCESSPARALLELRUNNER_H
#define MULTYTHREADING_LINUXPROCCESSPARALLELRUNNER_H

#include <vector>
#include <cstdint>
#include "Callable.h"
#include "AbstractTemplatedParallelRunner.h"
#include <sys/wait.h>
#include <unistd.h>
#include <tr1/memory>

using kill_signature = std::function<void ()>;


class LinuxSignalSystemInterrupter: public AbstractSystemInterrupter{
private:
    kill_signature _kill_except;
public:
    explicit LinuxSignalSystemInterrupter(kill_signature);
    void handle_success() override ;
    void handle_error() override ;
    bool check_next_sync_call() override ;
};


class LinuxProcess: public AbstractConcurrencyUnit{
protected:
    bool running = false;
    uint16_t inner_uid = 0;
public:
    unsigned long pid = 0;
    explicit LinuxProcess(AbstractCallable *);
    void set_pid() override;  // receive pid from getpid method
    unsigned long get_pid() const override; // getter for private pid field
    void set_running(bool _running) override; // set process _running
    bool get_running() const override;  // getter for running field
    void set_inner_uid(uint16_t inner_uid) override;
    uint16_t get_inner_uid() const override;
};

template <typename AbstractHandler>
class LinuxProccessParallelRunner: public AbstractTemplatedParallelRunner<LinuxProcess>{
private:
    void forker(unsigned long nprocesses) {
        pid_t pid;
        std::cout<<nprocesses<<std::endl;
        if(nprocesses >= 0)
        {
            if ((pid = fork()) < 0)
            {
                std::cerr<<"Invalid forking"<<std::endl;
            }
            else if (pid == 0)
            {
                LinuxProcess proc = this->_proccesses[nprocesses];
                proc.set_pid();
                proc.set_running(true);
                proc.executable->run();
                if (proc.executable->has_result()) {
                    std::string result = proc.executable->to_string();
                    std::cout<<result<<std::endl;
                }
                exit(0);
            }
            else if(pid > 0 && nprocesses != 0)
            {
                std::cout<<nprocesses;
                forker(nprocesses - 1);
            }
        }
    }

public:
    LinuxProccessParallelRunner() {
        this->_proccesses = std::vector<LinuxProcess>();
    }

    void run() override {
        unsigned long size = this->_proccesses.size() - 1;
        std::cout<<size<<std::endl;
        if ((size) < 1000) { // this check to avoid DOS
            std::cout<<this->_proccesses.size()<<std::endl;
            this->forker(size);
            int status = 0;
            for (int i = this->_proccesses.size(); i > 0; --i){
                int pid = wait(&status);
            }
        }
    }

    void kill_except(uint16_t inner_process_id, int signal) override {
        // kill all processes except given one
        for (auto i = 0; i < this->_proccesses.size(); ++i){
            auto process = this->_proccesses[i];
            if (i != inner_process_id && process.get_running()) {
                kill(process.get_pid(), signal);
            }
        }
    }

    std::shared_ptr<AbstractSystemInterrupter> get_signal_handler(AbstractConcurrencyUnit * process){
        static_assert(
                std::is_base_of<AbstractSystemInterrupter, AbstractHandler>::value,
                "AbstractHandler template arg must derive "
                "from AbstractSystemInterrupter");
        auto handler = std::shared_ptr<AbstractSystemInterrupter>(new AbstractHandler(
                std::bind(&LinuxProccessParallelRunner::kill_except, this, process->get_inner_uid(), SIGINT)
        ));
        return handler;
    };
};

#endif //MULTYTHREADING_LINUXPROCCESSPARALLELRUNNER_H
