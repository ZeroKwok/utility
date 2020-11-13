#ifdef UTILITY_DISABLE_HEADONLY
#   include "../encryption.hpp"
#endif

namespace util {
namespace encryption {
namespace detail {

/// TEA(Tiny Encryption Algorithm)

inline void _encrypt_with_tea32(
    uint32_t* t, uint32_t *s, const uint32_t * k)
{
    uint32_t y=s[0], z=s[1], sum=0, i;               /* set up */
    uint32_t delta=0x9e3779b9;                       /* a key schedule constant */
    uint32_t a=k[0], b=k[1], c=k[2], d=k[3];         /* cache key */

    for (i=0; i < 32; i++) {                         /* basic cycle start */
        sum +=  delta;
        y += ((z<<4) + a) ^ (z + sum) ^ ((z>>5) + b);
        z += ((y<<4) + c) ^ (y + sum) ^ ((y>>5) + d);/* end cycle */
     }

    t[0]= y;
    t[1]= z;
}

inline void _decrypt_with_tea32(
    uint32_t* t, uint32_t *s, const uint32_t * k)
{
    uint32_t y=s[0], z=s[1], sum=0xC6EF3720, i;      /* set up */
    uint32_t delta=0x9e3779b9;                       /* a key schedule constant */
    uint32_t a=k[0], b=k[1], c=k[2], d=k[3];         /* cache key */

    for(i=0; i<32; i++) {                            /* basic cycle start */
        z -= ((y<<4) + c) ^ (y + sum) ^ ((y>>5) +  d);
        y -= ((z<<4) + a) ^ (z + sum) ^ ((z>>5) +  b);
        sum -= delta;                                /* end cycle */
     }

    t[0] = y;
    t[1] = z;
}

} // detail

bytedata& encrypt_with_tea32(bytedata& bytes, const bytedata& key)
{
    uint64_t origin =  bytes.size();
    uint64_t length = (bytes.size() + 7) & (~7);

    if (bytes.size() < length)
        bytes.append(static_cast<size_t>(length - bytes.size()) + 8u, 0);
    else
        bytes.append(8u, 0);

    bytedata password = key;
    if (password.size() < 16u)
        password.append(16u - password.size(), 0);

    uint64_t now  = origin;
    uint64_t next = *reinterpret_cast<uint64_t*>(&bytes[0]);

    for (size_t i = 0; i < bytes.size(); i += 8)
    {
        detail::_encrypt_with_tea32(
            reinterpret_cast<uint32_t*>(&bytes[i]), 
            reinterpret_cast<uint32_t*>(&now), 
            reinterpret_cast<const uint32_t*>(password.data()));

        if(i + 8 < bytes.size())
        {
            now = next;
            next = *reinterpret_cast<uint64_t*>(&bytes[i + 8]);
        }
    }

    return bytes;
}

bytedata& decrypt_with_tea32(bytedata& bytes, const bytedata& key)
{
    uint64_t length = (bytes.size() + 7) & (~7);

    if (length != bytes.size())
        throw std::runtime_error("The length of the decrypted data is incorrect.");

    bytedata password = key;
    if (password.size() < 16u)
        password.append(16u - password.size(), 0);

    detail::_decrypt_with_tea32(
        reinterpret_cast<uint32_t*>(&length),
        reinterpret_cast<uint32_t*>(&bytes[0]),
        reinterpret_cast<const uint32_t*>(password.data()));

    if(bytes.size() - 8 < length)
        throw std::runtime_error("The decrypted data is incorrect.");

    for (size_t i = 0; i < bytes.size(); i += 8)
    {
        detail::_decrypt_with_tea32(
            reinterpret_cast<uint32_t*>(&bytes[i]),
            reinterpret_cast<uint32_t*>(&bytes[i+8]),
            reinterpret_cast<const uint32_t*>(password.data()));
    }

    bytes.resize(static_cast<size_t>(length));

    return bytes;
}

} // encryption
} // util