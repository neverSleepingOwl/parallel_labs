#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
#pragma ide diagnostic ignored "performance-unnecessary-value-param"
#include "../include/SharedMemorySystemInterrupter.h"

int SharedMemorySystemInterrupter::counter = 0;
void * SharedMemorySystemInterrupter::shared_mem = nullptr;
const std::string SharedMemorySystemInterrupter::success = "finished";


void* create_shared_memory(size_t size) {
    // Our memory buffer will be readable and writable:
    int protection = PROT_READ | PROT_WRITE;

    // The buffer will be shared (meaning other processes can access it), but
    // anonymous (meaning third-party processes cannot obtain an address for it),
    // so only this process and its children will be able to use it:
    int visibility = MAP_SHARED | MAP_ANONYMOUS;

    // The remaining parameters to `mmap()` are not important for this use case,
    // but the manpage for `mmap` explains their purpose.
    return mmap(nullptr, size, protection, visibility, -1, 0);
}


SharedMemorySystemInterrupter::SharedMemorySystemInterrupter(SharedMemorySystemInterrupter::kill_signature) {
    if (SharedMemorySystemInterrupter::counter == 0){
        SharedMemorySystemInterrupter::shared_mem = create_shared_memory(SharedMemorySystemInterrupter::mem_size);
    }
    SharedMemorySystemInterrupter::counter++;
}

SharedMemorySystemInterrupter::~SharedMemorySystemInterrupter() {
    // TODO maybe find a way to destroy shared memory
    SharedMemorySystemInterrupter::counter--;
}

std::string SharedMemorySystemInterrupter::read_msg() {
    return std::string((char *)SharedMemorySystemInterrupter::shared_mem);
}

void SharedMemorySystemInterrupter::write_msg(std::string str) {
    auto c_str = str.c_str();
    std::memcpy(SharedMemorySystemInterrupter::shared_mem, c_str, sizeof(c_str));
}

void SharedMemorySystemInterrupter::handle_success() {
    this->write_msg(SharedMemorySystemInterrupter::success);
}

bool SharedMemorySystemInterrupter::check_next_sync_call() {
    return this->read_msg() != SharedMemorySystemInterrupter::success;
}


#pragma clang diagnostic pop