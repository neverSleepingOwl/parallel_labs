#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
//
// Created by owl on 06.04.2020.
//

#ifndef MULTYTHREADING_SHAREDMEMORYSYSTEMINTERRUPTER_H
#define MULTYTHREADING_SHAREDMEMORYSYSTEMINTERRUPTER_H
#include "AbstractSystemInterrupter.h"
#include "PlatformDefiner.h"
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <cstring>


#if defined(PLATFORM_CODE)
    #if PLATFORM_CODE == linux
        #include <sys/mman.h>

        class SharedMemorySystemInterrupter: public AbstractSystemInterrupter{
            using kill_signature = std::function<void ()>;
            static int counter;
            static void * shared_mem;
            static const size_t mem_size = 8;
            static const std::string success;
            std::string read_msg();
            void write_msg(std::string);
        public:
            explicit SharedMemorySystemInterrupter(kill_signature);
            ~SharedMemorySystemInterrupter();
            void handle_success() override ;
            void handle_error() override {};
            bool check_next_sync_call() override ;
        };
    #endif
#endif

#endif //MULTYTHREADING_SHAREDMEMORYSYSTEMINTERRUPTER_H

#pragma clang diagnostic pop