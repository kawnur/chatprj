#ifndef UTILS_GPGME_HPP
#define UTILS_GPGME_HPP

#include <memory>
#include <sys/types.h>  // std::ssize_t
#include <string.h>
#include <string>

#include "utils_cout.hpp"

using namespace std::string_literals;

extern "C"
{
#include <gpgme.h>
}

namespace TestGpgme
{
    template<typename T, typename U>
    void coutFuncResult(T (*type)(int), U (*func)(T))
    {
		int i = 0;

        while (true) {
			auto typeVal = type(i);
			auto result = func(typeVal);

            if (result != nullptr)
				coutArgsWithSpaceSeparator(i, std::string(result));
            else
				break;

			i++;
		}
	}

    void checkProtocols();
    std::string getStringFromChar(const char *value);

    template<typename T>
    auto getOrString(std::shared_ptr<T> parameter)
    {
        return (parameter) ? parameter : "nullptr"s;  // TODO ???
	}

    void coutKeyInfo(const gpgme_key_t * const key);
    void coutUserIdInfo(gpgme_key_t *key);
    void createKey(gpgme_ctx_t *context, const char *algoName);
    void listKeys(gpgme_ctx_t *context);
    void getKeyByUser(gpgme_ctx_t *context, gpgme_key_t *key, const char *name);
    void createDataObject(gpgme_data_t *data);
    void printAsBytesTillNullTerminator(const char *value);
    void printAsBytes(const char *value, std::size_t size);
    void printAsChars(const char *value, std::size_t size);
    int getTerminatorPosition(const char *value, ssize_t size);
    void seekSetZero(gpgme_data_t &data);

    void encrypt(
        gpgme_ctx_t *context, gpgme_key_t *keys, gpgme_encrypt_flags_t &flags, gpgme_data_t &data,
        gpgme_data_t &dataEncrypt);

    void decrypt(gpgme_ctx_t *context, gpgme_data_t &dataEncrypt, gpgme_data_t &dataDecrypt);
    ssize_t readData(gpgme_data_t &data, char *dataString, std::size_t size);
    void readData1(gpgme_data_t &data, std::string &dataString);
    char *readData2(gpgme_data_t &data);
}

#endif // UTILS_GPGME_HPP
