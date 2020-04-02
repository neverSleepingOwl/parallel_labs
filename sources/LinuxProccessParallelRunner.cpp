#include "../include/LinuxProccessParallelRunner.h"



LinuxProcess::LinuxProcess(AbstractCallable *function) {
    this->executable = function;
}

void LinuxProcess::set_pid() {
    if (this->running)
        this->pid = getpid();
}

unsigned long LinuxProcess::get_pid() const {
    return this->pid;
}

void LinuxProcess::set_running(bool _running) {
    this->running = _running;
}

bool LinuxProcess::get_running() const {
    return this->running;
}

void LinuxProcess::set_inner_uid(uint16_t _inner_uid) {
    this->inner_uid = _inner_uid;
}

uint16_t LinuxProcess::get_inner_uid() const {
    return this->inner_uid;
}

void LinuxSignalSystemInterrupter::handle_success() {
    this->_kill_except();
}

void LinuxSignalSystemInterrupter::handle_error() {
    std::cerr<<"Error occured, couldn't finish process"<<std::endl;
}

LinuxSignalSystemInterrupter::LinuxSignalSystemInterrupter(kill_signature kill_method) {
    this->_kill_except =  std::move(kill_method);
}

bool LinuxSignalSystemInterrupter::check_next_sync_call() {
    return true;
}
