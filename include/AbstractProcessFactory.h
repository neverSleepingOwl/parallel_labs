#ifndef MULTYTHREADING_ABSTRACTPROCESSFACTORY_H
#define MULTYTHREADING_ABSTRACTPROCESSFACTORY_H

#include "AbstractTemplatedParallelRunner.h"
#include "PlatformDefiner.h"

class AbstractProcessFactory{
    virtual AbstractParallelRunner * get_process() = 0;
    virtual AbstractParallelRunner * get_thread() = 0;
};
#if defined(PLATFORM_CODE)
    #if PLATFORM_CODE == unix
        AbstractProcessFactory * get_factory();
    #elif PLATFORM_CODE == windows
        AbstractProcessFactory * get_factory();
    #endif
#endif
#endif //MULTYTHREADING_ABSTRACTPROCESSFACTORY_H
