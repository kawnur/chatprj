#ifndef UTILS_GPGME_HPP
#define UTILS_GPGME_HPP

//#include <gpgme.h>
#include <string.h>

#include "utils_cout.hpp"


extern "C" {
#include <gpgme.h>
};

//extern "C" {
//    gpgme_error_t gpgme_op_createkey(gpgme_ctx_t,
//                                    const char *,
//                                    const char *,
//                                    unsigned long,
//                                    unsigned long,
//                                    gpgme_key_t,
//                                    unsigned int);



//};

namespace TestGpgme {
	template<typename T, typename U> void coutFuncResult(T (*type)(int), U (*func)(T)) {
		int i = 0;

		while(true) {
//			auto type = gpgme_protocol_t(i);
			auto typeVal = type(i);
//			auto protocol = gpgme_get_protocol_name(type);
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
    void coutEngineInfo(gpgme_engine_info_t*);
    std::string getStringFromCharPtr(const char*);

//	template<typename T> std::optional<std::string, T> getPtrOrString(T* t) {
	template<typename T> auto getPtrOrString(T* t) {
		return (t == nullptr) ? std::string("nullptr") : t;
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
            gpgme_ctx_t*,
            gpgme_key_t*,
            gpgme_encrypt_flags_t&,
            gpgme_data_t&,
            gpgme_data_t&);

    void decrypt(gpgme_ctx_t*, gpgme_data_t&, gpgme_data_t&);
    ssize_t readData(gpgme_data_t&, char*, size_t);
    void readData1(gpgme_data_t&, std::string&);
    char* readData2(gpgme_data_t&);
}


#endif // UTILS_GPGME_HPP
