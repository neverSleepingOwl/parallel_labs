#ifndef MULTYTHREADING_ABSTRACTSIGHANDLER_H
#define MULTYTHREADING_ABSTRACTSIGHANDLER_H

#endif //MULTYTHREADING_ABSTRACTSIGHANDLER_H

class AbstractSignalHandler{
/*
 * Base class for all thread finished handlers
 * Inheritors must write to pipe/socket/throw signals
 * or lock mutexes and change env vars
 * to notify other processes to stop their work
 * before each small period of time, callables
 * in each parallel runner must check_nex_syn_call
 * in order to deacrease time of work
 * by the time of work of the fastest parallel entity
 * (process/thread/MPI stuff)
 */
public:
    // handle_success method kills
    // all proccess which are ran parallel
    // by signal
    // or by setting some stuff
    // which is checked by check_sync_call
    // method
    virtual void handle_success() = 0;
    // Callback for error, usually empty void
    virtual void handle_error() = 0;
    // In MultipleCallable class this
    // is checked before running of each callable
    // reading from pipe/socket/MQ
    // must be non blocking
    // true for not finished
    // false for finished
    virtual bool check_next_sync_call()  = 0;
};