#ifndef UTILS_GPGME_HPP
#define UTILS_GPGME_HPP

#include <string.h>

#include "utils_cout.hpp"

extern "C" {
#include <gpgme.h>
}

namespace TestGpgme {
    template<typename T, typename U>
    void coutFuncResult(T (*type)(int), U (*func)(T)) {
		int i = 0;

        while(true) {
			auto typeVal = type(i);
			auto result = func(typeVal);

            if(result != nullptr) {
				coutArgsWithSpaceSeparator(i, std::string(result));
			}
            else {
				break;
			}

			i++;
		}
	}

    void checkProtocols();
    std::string getStringFromCharPtr(const char*);

    template<typename T> auto getPtrOrString(T* parameter) {
        return (parameter) ? parameter : std::string("nullptr");  // TODO ???
	}

    void coutKeyInfo(const gpgme_key_t* const);
    void coutUserIdInfo(gpgme_key_t*);
    void createKey(gpgme_ctx_t*, const char*);
    void listKeys(gpgme_ctx_t*);
    void getKeyByUser(gpgme_ctx_t*, gpgme_key_t*, const char*);
    void createDataObject(gpgme_data_t*);
    void printAsBytesTillNullTerminator(const char*);
    void printAsBytes(const char*, size_t);
    void printAsChars(const char*, size_t);
    int getTerminatorPosition(const char*, ssize_t);
    void seekSetZero(gpgme_data_t&);

    void encrypt(
        gpgme_ctx_t*, gpgme_key_t*, gpgme_encrypt_flags_t&,
        gpgme_data_t&, gpgme_data_t&);

    void decrypt(gpgme_ctx_t*, gpgme_data_t&, gpgme_data_t&);
    ssize_t readData(gpgme_data_t&, char*, size_t);
    void readData1(gpgme_data_t&, std::string&);
    char* readData2(gpgme_data_t&);
}

#endif // UTILS_GPGME_HPP
