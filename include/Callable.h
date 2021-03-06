//
// Created by owl on 31.03.2020.
//

#ifndef MULTYTHREADING_CALLABLE_H
#define MULTYTHREADING_CALLABLE_H


#include <functional>
#include <vector>
#include "AbstractSystemInterrupter.h"
#include "to_string_helper.h"
#include <tr1/memory>


class AbstractCallable{
public:
    bool should_call_sighandler = true;
    virtual void run() = 0;
    virtual std::string to_string() const = 0;
    virtual bool has_result() const = 0;
    virtual void set_sig_handler(std::shared_ptr<AbstractSystemInterrupter> handler) = 0;
    virtual ~AbstractCallable() = default;
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
// Class which stores and executes
// multiple callables sequently
// and stops execution
// when sighandler->check_next_sync_call
// returns false
// has method add_single_callable
// to add AbstractCallable instance to list
private:
    std::vector<AbstractCallable *> _callables;
    bool _finished = false;
    bool _has_result = false;
    bool _sighandler_set = false;
    std::string _result_string;
    std::shared_ptr<AbstractSystemInterrupter> _sighandler;
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
                }else{
                    break;
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
        return this->_has_result && this->_finished;
    }

    void set_sig_handler(std::shared_ptr<AbstractSystemInterrupter> handler) override{
        this->_sighandler = handler;
        this->_sighandler_set = true;
        for (auto & callable: this->_callables)
            callable->set_sig_handler(handler);
    }

    void add_single_callable(AbstractCallable * runner) {
        runner->set_sig_handler(this->_sighandler);
        runner->should_call_sighandler = false;
        this->_callables.push_back(runner);
    }

    ~MultipleCallable() override{
        while(this->_callables.size() > 0){
            auto item = this->_callables.back();
            this->_callables.pop_back();
            delete item;
        }
    }
};


namespace args_dereference{
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
}


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
        function_params _params;
    protected:
        function_signature _function;
        result_checker_signature _result_checker;
        return_type _result;
        bool _finished = false;
        bool _has_result = false;
        bool _sighandler_set = false;
        std::shared_ptr<AbstractSystemInterrupter> _sighandler;

        // after all that operations call_with_args
        // is being compiled
        // and there std::get is multiply substituted
        // and wi finally call function with valid args
        // inside () operator
        template<int ...S>
        return_type call_with_nargs(args_dereference::seq<S...>, function_params __params) {
            return _function(std::get<S>(__params) ...);
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

        explicit Callable(
            result_checker_signature result_checker,
            function_signature function
        ){
            this->_result_checker = result_checker;
            this->_function = function;
        }
        ~Callable() override{
        }
        void set_sig_handler(std::shared_ptr<AbstractSystemInterrupter>  handler) override{
            this->_sighandler = handler;
            this->_sighandler_set = true;
        }

        virtual void run() override {
            if (!this->_sighandler_set)
                throw RunWithNoSighandler();
            this->_result = this->call_with_nargs(typename args_dereference::gens<sizeof...(args)>::type(), this->_params);
            this->_finished = true;
            if(this->_result_checker(this->_result)){
                this->_has_result = true;
                if (this->should_call_sighandler){
                    this->_sighandler->handle_success();
                }
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

template <typename ...args>
class AbstractArgsGenerator{
    //
using argtype = std::tuple<args...>;
public:
    virtual argtype next() = 0;
    virtual bool has_next() = 0;
};

template <typename return_type, typename ...args>
class MapCallable: public Callable<return_type, args...>{
    // Apply callable to a range of params (AbstractArgsGenerator)
    // so we don't have to store all params and many same callables
    // in memory.
    // I prefer custom generator rather than iterators
    // since i like more pythonic way.
    // and i got stuck with compiling boost::coroutine2
    using function_signature = return_type (*)(args...);
    using function_params = std::tuple<args...>;
    using result_checker_signature = bool (*) (return_type);
    using function_params_iter = AbstractArgsGenerator<args...>;
    using function_params_iter_ptr = std::shared_ptr<function_params_iter>;
private:
    function_params_iter_ptr _iterator;
public:
    explicit MapCallable(
            result_checker_signature result_checker,
            function_signature function,
            function_params_iter_ptr  iterator
    ): Callable<return_type, args...>(result_checker, function){
        this->_iterator = iterator;
    }

    ~MapCallable() override{
    }

    virtual void run() override {
        if (!this->_sighandler_set)
            throw RunWithNoSighandler();
        try{
            while (this->_iterator->has_next()){
                auto args_tuple = this->_iterator->next();
                if (this->_sighandler->check_next_sync_call()) {
                    this->_result = this->call_with_nargs(typename args_dereference::gens<sizeof...(args)>::type(), args_tuple);
                    this->_finished = true;
                    if(this->_result_checker(this->_result)){
                        this->_has_result = true;
                        if (this->should_call_sighandler){
                            this->_sighandler->handle_success();
                            break;
                        }
                    }
                }else{
                    break;
                }
            }
        }catch (std::exception & e){
            std::cerr<<e.what()<<std::endl;
        }
    }
};

#endif //MULTYTHREADING_CALLABLE_H
