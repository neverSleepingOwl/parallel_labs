# Parallel programming labs

This repo contains set of classes for parallel process/thread execution and results check.
Also there is small example of usage and helper classes, using which we can bruteforce `twofish`
symmetrical encryption algorithm.
Bruteforce checks if decrypted result for key in given range matches string passed to encrypt
## Included parallelism implementations
* Linux fork with kill by signal to process group stop option, shared memory check stop option and socket server stop option
* Linux libpthread with kill by `pthread_cancel` stop option, shared memory check stop option and socket server stop option
* OpenMp 
* Mpi with broadcast stop option and point-to-point communication stop option


## Custom Callable
This repo also contains custom templated wrapper around executable `Callable.h`.
So you can pass your function and your checking callback to Callable constructor,
or add callables to execute them in one parallel entity (thread)/process.
Custom `Callable` or `MultipleCallable` can be passed to `AbstractParallelRunner` `add` method
to add them to execution, which is invoked with `run()` method.
Result of each `Callable`/`MultipleCallable` is written to stdout. 
Also there is MapCallable, which is much like callable, except it takes generator (shared pointer to AbstractArgsGenerator),
which has two methods: `next` and `has_next`, and yields tuple of function args.

**NOTE**: function, passed to `Callable` constructor must have return value **convertable to string**,
so you should define `to_string` method, or `<<` operator with `std::ostringstream` return value
and arg. Also it's recommended to declare this operator as friend of your return value class. 

**NOTE**: function, passed to `Callable` constructor can be `std::function` or non-capture lambdas.

Checking callback checks if we should continue work of the program, or terminate all threads.
If checking callback on function result succeeds, parallel programm execution stops.


### Example with lambdas and singular callables.

```c++
#include <iostream>
#include "include/Callable.h"
#include "include/LinuxProccessParallelRunner.h"
#include "include/LinuxThreadParallelRunner.h"


int main(){
    auto functor = [](bool x)->int{ return (x)?1:0;};
    auto functor_checker = [](int a)->bool{return a == 1;};
    Callable<int, bool> new_callable = Callable<int, bool>(functor_checker, functor, false);
    Callable<int, bool> new_callable1 = Callable<int, bool>(functor_checker, functor, false);
    Callable<int, bool> new_callable2 = Callable<int, bool>(functor_checker, functor, true);
    Callable<int, bool> new_callable3 = Callable<int, bool>(functor_checker, functor, false);
    auto x = LinuxThreadParallelRunner<LinuxSignalSystemInterrupter>();
    x.add(&new_callable);
    x.add(&new_callable2);
    x.add(&new_callable3);
    x.add(&new_callable1);
    x.add(&new_callable);
    x.run();
}
```  
Here, we run functor with 5 posix threads, and finish all threads when one functor returns 1.

## Libs installtion

Posix threads and posix processes, like windows processes and threads doesn't require installations of libraries.
But, you need to install openmp and mpi to run these types of parrallelisation.

### OpenMP installation
Run:
```shell script
sudo apt-get install libomp-dev
```

### OpenMPI and boost mpi installation
We need to install boost mpi library to work with mpi and openmpi binary to run openmpi program.

```shell script
sudo apt-get install -y openmpi-bin
sudo apt-get install libboost-all-dev
sudo apt install libopenmpi-dev
```


## Sanitizing addresses
For more comfortable address sanitizing and debug we include address sanitizer in CMakeLists.txt,
We will need llvm support to get addresses to more quickly find errors.
```shell script
sudo apt-get install llvm
```

## **NOTE:** 
Address sanitizer can break some of functionality, especially related to process kill/pthread_cancel magic.
So it would be better to compile without using it.