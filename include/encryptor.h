//
// Created by owl on 02.04.2020.
//

#ifndef MULTYTHREADING_ENCRYPTOR_H
#define MULTYTHREADING_ENCRYPTOR_H

#include <vector>
#include <string>
#include "twofish.h"
namespace twofish_cryptor {
    const uint8_t  byte_key_len = crypto::twofish256::seq_size;
    const uint8_t  block_size = crypto::twofish256::block_size;
    std::vector<uint8_t> encrypt(uint32_t , std::string );
    std::string decrypt(uint32_t , std::vector<uint8_t> & );
}
#endif //MULTYTHREADING_ENCRYPTOR_H
