#include "../include/encryptor.h"



namespace twofish_cryptor{

    std::vector<uint8_t> encrypt(uint32_t key, std::string to_encrypt){
        std::string string_key = std::to_string(key);
        std::array<uint8_t, byte_key_len> byte_key = std::array<uint8_t, byte_key_len>();
        for(uint8_t  i = 0; i < byte_key_len; i++){
            if (i < string_key.size()){
                byte_key[i] = string_key[i];
            }else{
                byte_key[i] = 0;
            }
        }
        crypto::twofish256 twofish(byte_key);
        uint32_t size = to_encrypt.size();
        size += (uint32_t) (block_size - (size % block_size));
        std::vector<uint8_t> to_encrypt_vector = std::vector<uint8_t>(size);
        std::vector<uint8_t> encrypted_vector = std::vector<uint8_t>(size);
        for (int i = 0; i < size; i++){
            if (i < to_encrypt.size()){
                to_encrypt_vector[i] = (uint8_t) to_encrypt[i];
            }else{
                to_encrypt_vector[i] = 0;
            }
        }
        for (int i = 0; i < size;i += block_size){
            auto decrypted_block = std::array<uint8_t, block_size>();
            for (int j = i; j < i + block_size; j++){
                decrypted_block[j % block_size] = to_encrypt_vector[j];
            }
            auto encrypted_block =twofish.encrypt(decrypted_block);
            for (int j = i; j < i + block_size; j++){
                encrypted_vector[j] = encrypted_block[j % block_size];
            }
        }
        return encrypted_vector;
    }

    std::string decrypt(uint32_t key, std::vector<uint8_t> & encrypted){
        std::string string_key = std::to_string(key);
        std::array<uint8_t, byte_key_len> byte_key = std::array<uint8_t, byte_key_len>();
        for(uint8_t  i = 0; i < byte_key_len; i++){
            if (i < string_key.size()){
                byte_key[i] = string_key[i];
            }else{
                byte_key[i] = 0;
            }
        }
        crypto::twofish256 twofish(byte_key);
        uint32_t size = encrypted.size();

        std::vector<uint8_t> decrypted = std::vector<uint8_t>(size);
        for (int i = 0; i < size;i += block_size){
            auto encrypted_block = std::array<uint8_t, block_size>();
            for (int j = i; j < i + block_size; j++){
                encrypted_block[j % block_size] = encrypted[j];
            }
            auto decrypted_block = twofish.decrypt(encrypted_block);
            for (int j = i; j < i + block_size; j++){
                decrypted[j] = decrypted_block[j % block_size];
            }
        }
        std::vector<char> output;
        for (auto a: decrypted){
            if (a == 0){
                break;
            }
            output.push_back((char) a);
        }
        return std::string(output.begin(), output.end());
    }
}
