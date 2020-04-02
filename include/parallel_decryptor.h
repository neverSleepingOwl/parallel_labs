#ifndef MULTYTHREADING_PARALLEL_DECRYPTOR_H
#define MULTYTHREADING_PARALLEL_DECRYPTOR_H
#include <vector>
#include "Callable.h"
#include "encryptor.h"
#include <cstdlib>
#include <functional>
#include <ostream>


namespace parrallel_decryptor{
    struct decryption_result{
        std::string result;
        uint32_t key;
        friend std::ostringstream&operator<<(std::ostringstream& os, const decryption_result& dt);
    };


    std::ostringstream& operator<<(std::ostringstream& os, const decryption_result & dt)
    {
        // result for valid output
        os<<"String decrypted: \""<<dt.result<<"\", encryption key: "<<dt.key<<".";
        return os;
    }

    decryption_result decrypt_with_key(uint32_t key, std::vector<uint8_t>  encrypted){
        std::string result = twofish_cryptor::decrypt(key, encrypted);
        return decryption_result{result, key};
    }

// couldn't cast std::bind to function pointer
// so instead of binding i simply use global var

    std::string expected;


    bool success(decryption_result result){
        return result.result == expected;
    }


    std::vector<MultipleCallable *> prepare_functions(
            uint32_t range_begin,
            uint32_t range_end,
            uint16_t num_threads,
            std::string _expected,
            std::vector<uint8_t> & encrypted
    ){
        expected = _expected;
        const auto range_size = range_end - range_begin;
        const div_t div_res = div(range_size, num_threads);
        const auto step = div_res.quot + ((div_res.rem == 0)?0:1);
        auto vec = std::vector<MultipleCallable*>();
        for(auto i = 0; i < num_threads; i++){
            auto threadCallable = new MultipleCallable();
            for (auto j = 0; j <= step; j++){
                auto key_value = i * step + j;
                if (key_value > range_end)
                    break;
                AbstractCallable * to_call = new Callable<decryption_result, uint32_t, std::vector<uint8_t >>(success, decrypt_with_key, key_value,  encrypted);
                threadCallable->add_single_callable(to_call);
            }
            vec.push_back(threadCallable);
        }
        return vec;
    }
}
#endif //MULTYTHREADING_PARALLEL_DECRYPTOR_H
