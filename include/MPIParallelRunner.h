#ifndef MULTYTHREADING_MPIPARALLELRUNNER_H
#define MULTYTHREADING_MPIPARALLELRUNNER_H

#include <boost/mpi.hpp>
#include "AbstractTemplatedParallelRunner.h"


using kill_signature = std::function<void ()>;

class MpiSystemInterrupter: public AbstractSystemInterrupter{
protected:
    boost::mpi::communicator * _world;
public:
    explicit MpiSystemInterrupter(boost::mpi::communicator * world){
        this->_world = world;
    }
};

class MpiPointToPointSystemInterrupter: public MpiSystemInterrupter{
// We can interrupt MPI threads
// either via point-to-point communication
// or via broadcast communication
// so here's point to point
private:
    // We run recv for non blocking
    // request only once, and than
    // after each iteration
    // we check non-blocking response result
    boost::mpi::request resp;
    bool responding = false;
public:
    explicit MpiPointToPointSystemInterrupter(boost::mpi::communicator * world): MpiSystemInterrupter(world){
    }
    void handle_success() override{
        const auto size = this->_world->size();
        for (int i = 0; i < size; i++){
            auto to_send = std::string("stop");
            this->_world->send<std::string>(i, 0, to_send);
        }
    };

    void handle_error() override {};
    bool check_next_sync_call() override {
        const auto size = this->_world->size();
        std::string msg;
        if (!this->responding) {
            this->resp = this->_world->irecv<std::string>(boost::mpi::any_source, boost::mpi::any_tag, msg);
            this->responding = true;
        }
        if (resp.test()){
            if (msg == "stop"){
                return false;
            }
        }
        return true;
    };
};


class MpiBroadcastSystemInterrupter: public MpiSystemInterrupter{
private:
    boost::mpi::request resp;
    bool responding = false;
public:
    static std::string finished;
    explicit MpiBroadcastSystemInterrupter(boost::mpi::communicator * world): MpiSystemInterrupter(world){
    }
    void handle_success() override{
        MpiBroadcastSystemInterrupter::finished = "finished";
        boost::mpi::broadcast(*(this->_world), this->finished, 0);
    };
    void handle_error() override {};
    bool check_next_sync_call() override {
        const auto size = this->_world->size();
        std::string msg;
        if (!this->responding) {
            this->resp = this->_world->irecv<std::string>(boost::mpi::any_source, boost::mpi::any_tag, msg);
            this->responding = true;
        }
        if (resp.test()){
            if (msg == "finished"){
                return false;
            }
        }
        return true;
    };
};

std::string MpiBroadcastSystemInterrupter::finished = "";


class MpiThread: public BaseThread{
public:
    explicit MpiThread(AbstractCallable * callable): BaseThread(callable){
    }
};

template <typename AbstractHandler>
class MPIParallelRunner: public AbstractTemplatedParallelRunner<MpiThread>{
private:
    boost::mpi::communicator * world;
    boost::mpi::environment * env;
public:
    explicit MPIParallelRunner() {
        this->_proccesses = std::vector<MpiThread>();
        this->world = new boost::mpi::communicator();
        int argc = 0;
        char ** argv = nullptr;
        this->env = new boost::mpi::environment(argc, argv);
    }

    ~MPIParallelRunner(){
        delete this->env;
        delete this->world;
    }
    void run() override{
        // Balance abstract processes by MPI threads
        // each abstract process is callable, which meant
        // to run in separate thread
        // but we can't set amount of MPI threads inside code, so
        // We can have more values in this->_processes
        // than world->size
        // so we need to put several processes
        // on each MPI thread
        std::cout<<"Starting run method"<<std::endl;
        const auto size = this->world->size();
        std::cout<<size<<std::endl;
        for (int i = 0; i < size; i++){
            if (world->rank() == i){
                std::cout<<world->rank()<<std::endl;
                int counter = i;
                while (counter < this->_proccesses.size()){
                    auto runner = this->_proccesses[counter];
                    runner.set_running(true);
                    runner.set_inner_uid(world->rank());
                    runner.executable->run();
                    runner.set_running(false);
                    std::cout<<"Result: \""<<"thr finished"<<"\" in thread "<<runner.get_inner_uid()<<";"<<std::endl;
                    if ( runner.executable->has_result()) {
                        std::string result =  runner.executable->to_string();
                        std::cout<<"Result: \""<<result<<"\" in thread "<<runner.get_inner_uid()<<";"<<std::endl;
                    }
                    counter += size;
                }
            }
        }
    }
    void kill_except(uint16_t inner_process_id, int signal) override {};
    std::shared_ptr<AbstractSystemInterrupter> get_signal_handler(AbstractConcurrencyUnit * process) override{
        static_assert(
                std::is_base_of<MpiSystemInterrupter, AbstractHandler>::value,
                "AbstractHandler template arg must derive "
                "from MpiSystemInterrupter");
        auto handler = std::shared_ptr<AbstractSystemInterrupter>(std::make_shared<AbstractHandler>(world));
        return handler;
    }
};


#endif //MULTYTHREADING_MPIPARALLELRUNNER_H
