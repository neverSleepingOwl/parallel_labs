#ifndef MULTYTHREADING_TWOFISH_H
#define MULTYTHREADING_TWOFISH_H

#include <cstdint>
#include <array>

namespace crypto {
    class twofish256 {
        using word = uint32_t;
        using k_vector_t = std::array<word, 4>;
        using sbox_t = std::array<word, 1024>;
        using subkeys_t = std::array<word, 40>;
    public:
        const static uint8_t block_size = 16;
        const static uint8_t seq_size =32;
        using byte = uint8_t;
        using user_key_t = std::array<byte, 32>;
        using session_key_t = std::pair<sbox_t, subkeys_t>;
        using block_t = std::array<byte, block_size>;

        template<class Sequence>
        explicit twofish256(Sequence&& seq) noexcept {
//            assert(std::distance(std::begin(seq), std::end(seq)) == seq_size);
            user_key_t user_key;
            auto it = std::begin(seq);
            for(int i = 0; i < 32; ++i) user_key[i] = *it++;
            make_session_key(user_key);
        }

        twofish256(twofish256 const&) = delete;

        twofish256& operator=(twofish256 const&) = delete;

        block_t encrypt(const block_t& p);

        block_t decrypt(const block_t& c);

        static bool assurance_test();

        ~twofish256() = default;

    private:

        session_key_t session_key;

        static const byte P[2][256];

        static const word MDS[4][256];

        void make_session_key(const user_key_t& user_key);

    };
}
#endif //MULTYTHREADING_TWOFISH_H
