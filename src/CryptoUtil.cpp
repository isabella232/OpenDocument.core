#ifdef ODR_CRYPTO
#include "CryptoUtil.h"
#include "cryptopp/hex.h"
#include "cryptopp/filters.h"
#include "cryptopp/base64.h"
#include "cryptopp/pwdbased.h"
#include "cryptopp/sha.h"
#include "cryptopp/modes.h"
#include "cryptopp/aes.h"
#include "cryptopp/des.h"
#include "cryptopp/blowfish.h"
#include "cryptopp/zinflate.h"

namespace odr {

typedef unsigned char byte;

std::string CryptoUtil::base64Encode(const std::string &in) {
    std::string out;
    CryptoPP::Base64Encoder b(new CryptoPP::StringSink(out));
    b.Put((const byte *) in.data(), in.size());
    b.MessageEnd();
    return out;
}

std::string CryptoUtil::base64Decode(const std::string &in) {
    std::string out;
    CryptoPP::Base64Decoder b(new CryptoPP::StringSink(out));
    b.Put((const byte *) in.data(), in.size());
    b.MessageEnd();
    return out;
}

std::string CryptoUtil::sha1(const std::string &in) {
    byte out[CryptoPP::SHA1::DIGESTSIZE];
    CryptoPP::SHA1().CalculateDigest(out, (byte *) in.data(), in.size());
    return std::string((char *) out, CryptoPP::SHA1::DIGESTSIZE);
}

std::string CryptoUtil::sha256(const std::string &in) {
    byte out[CryptoPP::SHA256::DIGESTSIZE];
    CryptoPP::SHA256().CalculateDigest(out, (byte *) in.data(), in.size());
    return std::string((char *) out, CryptoPP::SHA256::DIGESTSIZE);
}

std::string CryptoUtil::pbkdf2(const std::size_t keySize, const std::string &startKey,
        const std::string &salt, const std::size_t iterationCount) {
    std::string result(keySize, '\0');
    CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA1> pbkdf2;
    pbkdf2.DeriveKey((byte *) result.data(), result.size(), false, (byte *) startKey.data(), startKey.size(),
            (byte *) salt.data(), salt.size(), iterationCount);
    return result;
}

std::string CryptoUtil::decryptAES(const std::string &key, const std::string &iv, const std::string &input) {
    std::string result(input.size(), '\0');
    CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption decryptor;
    decryptor.SetKeyWithIV((byte *) key.data(), key.size(), (byte *) iv.data(), iv.size());
    decryptor.ProcessData((byte *) result.data(), (byte *) input.data(), input.size());
    return result;
}

std::string CryptoUtil::decryptTripleDES(const std::string &key, const std::string &iv, const std::string &input) {
    std::string result(input.size(), '\0');
    CryptoPP::CBC_Mode<CryptoPP::DES_EDE3>::Decryption decryptor;
    decryptor.SetKeyWithIV((byte *) key.data(), key.size(), (byte *) iv.data(), iv.size());
    decryptor.ProcessData((byte *) result.data(), (byte *) input.data(), input.size());
    return result;
}

std::string CryptoUtil::decryptBlowfish(const std::string &key, const std::string &iv, const std::string &input) {
    std::string result(input.size(), '\0');
    CryptoPP::CFB_Mode<CryptoPP::Blowfish>::Decryption decryptor;
    decryptor.SetKeyWithIV((byte *) key.data(), key.size(), (byte *) iv.data(), iv.size());
    decryptor.ProcessData((byte *) result.data(), (byte *) input.data(), input.size());
    return result;
}

// discard non deflated content caused by padding
class MyInflator : public CryptoPP::Inflator {
public:
    MyInflator(BufferedTransformation *attachment = NULLPTR, bool repeat = false, int autoSignalPropagation = -1) :
            Inflator(attachment, repeat, autoSignalPropagation) {}

    virtual unsigned int GetPadding() const {return m_padding;}
protected:
    virtual void ProcessPoststreamTail() {
        m_padding = m_inQueue.CurrentSize();
        m_inQueue.Clear();
    }

private:
    unsigned int m_padding = 0;
};

std::string CryptoUtil::inflate(const std::string &input) {
    std::string result;
    MyInflator inflator(new CryptoPP::StringSink(result));
    inflator.Put((byte *) input.data(), input.size());
    inflator.MessageEnd();
    return result;
}

std::size_t CryptoUtil::padding(const std::string &input) {
    MyInflator inflator;
    inflator.Put((byte *) input.data(), input.size());
    inflator.MessageEnd();
    return inflator.GetPadding();
}

}

#endif
