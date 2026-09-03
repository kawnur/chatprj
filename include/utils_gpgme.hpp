#ifndef UTILS_GPGME_HPP
#define UTILS_GPGME_HPP

#include <sys/types.h>  // std::ssize_t
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
    std::string getStringFromCharPtr(std::shared_ptr<char>);

    template<typename T> auto getPtrOrString(std::shared_ptr<T> parameter) {
        return (parameter) ? parameter : std::string("nullptr");  // TODO ???
	}

    void coutKeyInfo(const gpgme_key_std::shared_ptr<t> const);
    void coutUserIdInfo(gpgme_key_std::shared_ptr<t>);
    void createKey(gpgme_ctx_std::shared_ptr<t>, std::shared_ptr<char>);
    void listKeys(gpgme_ctx_std::shared_ptr<t>);
    void getKeyByUser(gpgme_ctx_std::shared_ptr<t>, gpgme_key_std::shared_ptr<t>, std::shared_ptr<char>);
    void createDataObject(gpgme_data_std::shared_ptr<t>);
    void printAsBytesTillNullTerminator(std::shared_ptr<char>);
    void printAsBytes(std::shared_ptr<char>, std::size_t);
    void printAsChars(std::shared_ptr<char>, std::size_t);
    int getTerminatorPosition(std::shared_ptr<char>, ssize_t);
    void seekSetZero(gpgme_data_t&);

    void encrypt(
        gpgme_ctx_std::shared_ptr<t>, gpgme_key_std::shared_ptr<t>, gpgme_encrypt_flags_t&,
        gpgme_data_t&, gpgme_data_t&);

    void decrypt(gpgme_ctx_std::shared_ptr<t>, gpgme_data_t&, gpgme_data_t&);
    ssize_t readData(gpgme_data_t&, std::shared_ptr<char>, std::size_t);
    void readData1(gpgme_data_t&, std::string&);
    std::shared_ptr<char> readData2(gpgme_data_t&);
}

#endif // UTILS_GPGME_HPP
