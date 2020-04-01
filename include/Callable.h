//
// Created by owl on 31.03.2020.
//

#ifndef MULTYTHREADING_CALLABLE_H
#define MULTYTHREADING_CALLABLE_H


#include <functional>
#include <vector>
#include "AbstractSignalHandler.h"
#include "to_string_helper.h"


class AbstractCallable{
public:
    virtual void run() = 0;
    virtual std::string to_string() const = 0;
    virtual bool has_result() const = 0;
    virtual void set_sig_handler(AbstractSignalHandler * handler) = 0;
};


class ThreadFinishedUnexpected: public std::exception{
    const char * what() const noexcept override{
        return "Thread was finished unexpectedly";
    }
};


class ThreadNotReady: public std::exception{
    const char * what() const noexcept override{
        return "Thread was not finished.";
    }
};

class RunWithNoSighandler: public std::exception{
    const char * what() const noexcept override{
        return "No signal handler available";
    }
};


class MultipleCallable: public AbstractCallable{
private:
    std::vector<AbstractCallable *> _callables;
    bool _finished = false;
    bool _has_result = false;
    bool _sighandler_set = false;
    std::string _result_string;
    AbstractSignalHandler * _sighandler;
public:
    explicit MultipleCallable() {
        this->_callables = std::vector<AbstractCallable *>();
    }

    void run() override {
        if (!this->_sighandler_set)
            throw RunWithNoSighandler();
        try{
            for (auto runner: this->_callables){
                if (this->_sighandler->check_next_sync_call()) {
                    runner->run();
                    if (runner->has_result()){
                        this->_finished = true;
                        this->_has_result = true;
                        this->_result_string = runner->to_string();
                        this->_sighandler->handle_success();
                    }
                }
            }
        }catch (std::exception & e){
            std::cerr<<e.what()<<std::endl;
        }
    }

    std::string to_string() const override {
        return this->_result_string;
    }

    bool has_result() const override {
        return false;
    }

    void set_sig_handler(AbstractSignalHandler * handler) override{
        this->_sighandler = handler;
        this->_sighandler_set = true;
    }

    void add_single_callable(AbstractCallable * runner) {
        runner->set_sig_handler(this->_sighandler);
        this->_callables.push_back(runner);
    }
};


template<typename return_type, typename ... args>
class Callable: public AbstractCallable{
    // Class which receives function pointer
    // and function params as ...args
    // and has operator to call func with params any time
    // suitable for custom threads and callbacks
    // works also for std::function
    // and capture less lambdas as callbacks
    using function_signature = return_type (*)(args...);
    using function_params = std::tuple<args...>;
    using result_checker_signature = bool (*) (return_type);
    using lambda_checker_signature = std::function<bool (return_type)>;
    using lambda_signature = std::function<return_type (args...)>;
    private:
        function_signature _function;
        result_checker_signature _result_checker;
        function_params _params;
        return_type _result;
        bool _finished = false;
        bool _has_result = false;
        bool _sighandler_set = false;
        AbstractSignalHandler * _sighandler;
        // here we create basic seq
        // which is sequence with int templates
        // it would be filled further
        template<int ...>
        struct seq { };

        // here we define reqursive template gens
        // which returns itself templated with N - 1 arg
        // so gens<5> -> gens<4, 4> -> gens <3,3,4> -> gens<2,2,3,4>
        // -> gens<1, 1, 2, 3, 4> -> gens<0, 0, 1, 2, 3, 4>
        //then template with specified 0 value is handled
        template<int N, int ...S>
        struct gens : gens<N-1, N-1, S...> { };
        // template with specified value simply
        // contains seq<...> type

        template<int ...S>
        struct gens<0, S...> {
            typedef seq<S...> type;
        };
        // after all that operations call_with_args
        // is being compiled
        // and there std::get is multiply substituted
        // and wi finally call function with valid args
        // inside () operator
        template<int ...S>
        return_type call_with_nargs(seq<S...>) {
            return _function(std::get<S>(this->_params) ...);
        }
    public:
        explicit Callable(
                result_checker_signature result_checker,
                function_signature function,
                args... params
        ){
            this->_result_checker = result_checker;
            this->_function = function;
            this->_params = std::make_tuple(params...);
        }

        void set_sig_handler(AbstractSignalHandler * handler) override{
            this->_sighandler = handler;
            this->_sighandler_set = true;
        }

        void run() override {
            if (!this->_sighandler_set)
                throw RunWithNoSighandler();
            this->_result = this->call_with_nargs(typename gens<sizeof...(args)>::type());
            this->_finished = true;
            this->_has_result = true;
            if(this->_result_checker(this->_result)){
                this->_sighandler->handle_success();
            }
        }

        return_type get_result(){
            if (this->_finished && this->_has_result) {
                return this->_result;
            }
            throw(ThreadNotReady());
        }
        bool has_result() const override {
            return this->_finished && this->_has_result;
        }

        std::string to_string() const override{
            static_assert(
                    string_caster::can_cast_to_string<return_type>,
                    "Can't pass function pointer with not castable "
                    "to str return type, please, define << operator."
            );
            if (this->_finished && this->_has_result) {
                return string_caster::toString(this->_result);
            }
            throw(ThreadNotReady());
        }
};


#endif //MULTYTHREADING_CALLABLE_H
