#include "utils_gpgme.hpp"

#include <map>

#include "utils.hpp"

void checkProtocols() {}

std::string getStringFromCharPtr(const char *value)
{
    return (value) ? std::string(value) : "nullptr"s;
}

void coutKeyInfo(const gpgme_key_t *const key)
{
    // endline(1);

    // coutArgsWithSpaceSeparator("keylist_mode:", (*key)->keylist_mode);
    // coutArgsWithSpaceSeparator("revoked:", (int)(*key)->revoked);
    // coutArgsWithSpaceSeparator("expired:", (int)(int)(*key)->expired);
    // coutArgsWithSpaceSeparator("disabled:", (int)(*key)->disabled);
    // coutArgsWithSpaceSeparator("invalid:", (int)(*key)->invalid);
    // coutArgsWithSpaceSeparator("can_encrypt:", (int)(*key)->can_encrypt);
    // coutArgsWithSpaceSeparator("can_sign:", (int)(*key)->can_sign);
    // coutArgsWithSpaceSeparator("can_certify:", (int)(*key)->can_certify);
    // coutArgsWithSpaceSeparator("can_authenticate:", (int)(*key)->can_authenticate);
    // coutArgsWithSpaceSeparator("has_encrypt:", (int)(*key)->has_encrypt);
    // coutArgsWithSpaceSeparator("has_certify:", (int)(*key)->has_certify);
    // coutArgsWithSpaceSeparator("has_authenticate:", (int)(*key)->has_authenticate);
    // coutArgsWithSpaceSeparator("is_qualified:", (int)(*key)->is_qualified);
    // coutArgsWithSpaceSeparator("secret:", (int)(*key)->secret);
    // coutArgsWithSpaceSeparator("origin:", (int)(*key)->origin);
    // coutArgsWithSpaceSeparator("protocol:", (*key)->protocol);

    // coutArgsWithSpaceSeparator("issuer_serial:", getStringFromCharPtr((*key)->issuer_serial));
    // coutArgsWithSpaceSeparator("issuer_name:", getStringFromCharPtr((*key)->issuer_name));
    // coutArgsWithSpaceSeparator("chain_id:", getStringFromCharPtr((*key)->chain_id));

    // coutArgsWithSpaceSeparator("owner_trust:", (*key)->owner_trust);
    // coutArgsWithSpaceSeparator("subkeys:", (*key)->subkeys);
    // coutArgsWithSpaceSeparator("uids:", (*key)->uids);

    // coutArgsWithSpaceSeparator("fpr:", getStringFromCharPtr((*key)->fpr));

    // coutArgsWithSpaceSeparator("last_update:", (*key)->last_update);

    // coutArgsWithSpaceSeparator("key->subkeys->length:", (*key)->subkeys->length);

    // endline(1);
}

void coutUserIdInfo(gpgme_key_t *key)
{
    // _gpgme_user_id *uids = (*key)->uids;

    // endline(1);

    // coutArgsWithSpaceSeparator("next:", uids->next);
    // coutArgsWithSpaceSeparator("revoked:", (int)(uids->revoked));
    // coutArgsWithSpaceSeparator("invalid:", (int)(uids->invalid));
    // coutArgsWithSpaceSeparator("_unused:", (int)(uids->_unused));
    // coutArgsWithSpaceSeparator("origin:", (int)(uids->origin));
    // coutArgsWithSpaceSeparator("validity:", uids->validity);

    // coutArgsWithSpaceSeparator("uid:", getStringFromCharPtr(uids->uid));
    // coutArgsWithSpaceSeparator("name:", getStringFromCharPtr(uids->name));
    // coutArgsWithSpaceSeparator("email:", getStringFromCharPtr(uids->email));
    // coutArgsWithSpaceSeparator("comment:", getStringFromCharPtr(uids->comment));

    // coutArgsWithSpaceSeparator("signatures:", uids->signatures);
    // coutArgsWithSpaceSeparator("_last_keysig:", uids->_last_keysig);

    // coutArgsWithSpaceSeparator("address:", getStringFromCharPtr(uids->address));

    // coutArgsWithSpaceSeparator("tofu:", uids->tofu);
    // coutArgsWithSpaceSeparator("last_update:", uids->last_update);

    // coutArgsWithSpaceSeparator("uidhash:", getStringFromCharPtr(uids->uidhash));

    // endline(1);
}

void createKey(gpgme_ctx_t *contextPtr, const char *algoName)
{
    // create key
    const char *userId = "user2";
    unsigned long reserved = 0;
    unsigned long expires = 1000000000;
    gpgme_key_t extrakey = NULL;
    unsigned int flags = GPGME_CREATE_ENCR;

    auto key =
        gpgme_op_createkey(*contextPtr, userId, algoName, reserved, expires, extrakey, flags);

    coutWithEndl(key);

    std::map<gpgme_error_t, std::string> map {
        { GPG_ERR_NO_ERROR, "key created successfully" },
        { GPG_ERR_NOT_SUPPORTED, "engine does not support the command" }
    };

    coutWithEndl(getMappingValueOrDefault(map, key, "key creation: some other error"s));
}

void listKeys(gpgme_ctx_t *contextPtr)
{
    // list keys
    const char *pattern = NULL;
    int secret_only = 0;

    int i = 0;
    coutArgsWithSpaceSeparator("i:", i);

    auto errorStart = gpgme_op_keylist_start(*contextPtr, pattern, secret_only);

    if (errorStart == GPG_ERR_INV_VALUE)
        coutWithEndl("key listing: context is not a valid pointer");

    gpgme_error_t errorNext = GPG_ERR_CODE_DIM;

    while (errorNext != GPG_ERR_EOF && i < 4) {
        gpgme_key_t r_key;
        //			coutArgsWithSpaceSeparator("&r_key:", &r_key);

        errorNext = gpgme_op_keylist_next(*contextPtr, &r_key);

        std::map<gpgme_error_t, std::string> mapNext {
            { GPG_ERR_INV_VALUE, "key listing: context or r_key is not a valid pointer" },
            { GPG_ERR_ENOMEM, "key listing: there is not enough memory for the operation" }
        };

        coutMappingValue(mapNext, errorNext);

        if (r_key != nullptr) {
            coutKeyInfo(&r_key);
            coutUserIdInfo(&r_key);
        }

        i++;
        coutArgsWithSpaceSeparator("i:", i);
    }

    auto errorEnd = gpgme_op_keylist_end(*contextPtr);

    std::map<gpgme_error_t, std::string> mapEnd {
        { GPG_ERR_INV_VALUE, "key listing: context is not a valid pointer" },
        { GPG_ERR_ENOMEM, "key listing: there is not enough memory for the operation" }
    };

    coutMappingValue(mapEnd, errorEnd);

    i++;
    coutArgsWithSpaceSeparator("i:", i);
}

void getKeyByUser(gpgme_ctx_t *contextPtr, gpgme_key_t *keyPtr, const char *name)
{
    const char *pattern = NULL;
    int secret_only = 0;

    auto errorStart = gpgme_op_keylist_start(*contextPtr, pattern, secret_only);

    std::map<gpgme_error_t, std::string> mapStart {
        { GPG_ERR_INV_VALUE, "getKeyByUser: context is not a valid pointer" }
    };

    coutMappingValue(mapStart, errorStart);

    gpgme_error_t errorNext = GPG_ERR_CODE_DIM;

    std::map<gpgme_error_t, std::string> mapNext {
        { GPG_ERR_INV_VALUE, "key listing: context or r_key is not a valid pointer" },
        { GPG_ERR_ENOMEM, "key listing: there is not enough memory for the operation" }
    };

    while (true) {
        errorNext = gpgme_op_keylist_next(*contextPtr, keyPtr);

        coutMappingValue(mapNext, errorNext);

        if (strcmp((*keyPtr)->uids->name, name) == 0) {
            coutWithEndl("key found");
            break;
        }
    }
}

void createDataObject(gpgme_data_t *dataPtr)
{
    auto error = gpgme_data_new(dataPtr);

    std::map<gpgme_error_t, std::string> map {
        { GPG_ERR_NO_ERROR, "data object was successfully created" },
        { GPG_ERR_INV_VALUE, "is not a valid pointer" },
        { GPG_ERR_ENOMEM, "not enough memory" }
    };

    coutMappingValue(map, error);
}

void printAsBytesTillNullTerminator(const char *value)
{
    const char *p = value;

    int i = 0;

    while (*p != '\0') {
        printf("%x ", *p);
        p++;
        i++;
    }

    endline(1);
}

void printAsBytes(const char *value, std::size_t size)
{
    const char *p = value;

    std::size_t i = 0;

    while (i < size) {
        printf("%x ", *p);
        p++;
        i++;
    }

    endline(1);
}

void printAsChars(const char *value, std::size_t size)
{
    const char *p = value;

    std::size_t i = 0;

    while (i < size) {
        printf("%c ", *p);
        p++;
        i++;
    }

    endline(1);
}

int getTerminatorPosition(const char *value, ssize_t size)
{
    const char *p = value;

    int i = 0;

    while (i < size) {
        if (*p == '\0') {
            return i;
        }

        p++;
        i++;
    }

    return -1;
}

void seekSetZero(gpgme_data_t &data)
{
    auto off = gpgme_data_seek(data, 0, SEEK_SET);
    //		coutArgsWithSpaceSeparator("off:", off);

    if (off == -1)
        coutWithEndl("gpgme_data_seek error");
}

void encrypt(
    gpgme_ctx_t *contextPtr, gpgme_key_t *keys, gpgme_encrypt_flags_t &flags, gpgme_data_t &data,
    gpgme_data_t &dataEncrypt)
{
    seekSetZero(data);
    seekSetZero(dataEncrypt);

    auto error = gpgme_op_encrypt(*contextPtr, keys, flags, data, dataEncrypt);

    std::map<gpgme_error_t, std::string> map {
        { GPG_ERR_NO_ERROR, "ciphertext created successfully" },
        { GPG_ERR_INV_VALUE, "ctx, recp, plain or cipher is not a valid pointer" },
        { GPG_ERR_UNUSABLE_PUBKEY, "recp contains some invalid recipients" },
        { GPG_ERR_BAD_PASSPHRASE, "passphrase for the symmetric key could not be retrieved" }
    };

    coutMappingValue(map, error);
}

void decrypt(gpgme_ctx_t *contextPtr, gpgme_data_t &dataEncrypt, gpgme_data_t &dataDecrypt)
{
    seekSetZero(dataEncrypt);
    seekSetZero(dataDecrypt);

    auto error = gpgme_op_decrypt(*contextPtr, dataEncrypt, dataDecrypt);

    std::map<gpgme_error_t, std::string> map {
        { GPG_ERR_NO_ERROR, "ciphertext decrypted successfully" },
        { GPG_ERR_INV_VALUE, "ctx, plain or cipher is not a valid pointer" },
        { GPG_ERR_NO_DATA, "cipher does not contain any data to decrypt" },
        { GPG_ERR_DECRYPT_FAILED, "cipher is not a valid cipher text" },
        { GPG_ERR_BAD_PASSPHRASE, "passphrase for the secret key could not be retrieved" }
    };

    coutMappingValue(map, error);
}

ssize_t readData(gpgme_data_t &data, char *dataString, std::size_t size)
{
    seekSetZero(data);

    auto sizeRead = gpgme_data_read(data, dataString, size);
    //		coutArgsWithSpaceSeparator("sizeRead:", sizeRead);

    if (sizeRead == -1) {
        coutWithEndl("gpgme_data_read error");
        //errno
    }

    return sizeRead;
}

void readData1(gpgme_data_t &data, std::string &dataString)
{
    seekSetZero(data);

    char *p = new char[2];  // TODO use stack allocation

    while (true) {
        auto sizeRead = gpgme_data_read(data, p, 1);
        //			coutArgsWithSpaceSeparator("sizeRead:", sizeRead);

        if (sizeRead == -1) {
            coutWithEndl("gpgme_data_read error");
            //errno
        }

        dataString.push_back(*p);

        if (*p == '\0')
            break;
    }

    delete[] p;
}

char *readData2(gpgme_data_t &data)
{
    seekSetZero(data);

    std::size_t blockSize = 10;
    std::size_t bufferSize = blockSize;
    ssize_t sizeRead = blockSize;
    char *bufferHead = (char*)malloc(bufferSize);
    char *current = bufferHead;

    while (true) {
        sizeRead = gpgme_data_read(data, current, blockSize);

        //			coutArgsWithSpaceSeparator("blockSize:", blockSize);
        //			coutArgsWithSpaceSeparator("(void*)bufferHead:", (void*)bufferHead);
        //			coutArgsWithSpaceSeparator("(void*)current:", (void*)current);
        //			coutArgsWithSpaceSeparator("sizeRead:", sizeRead);
        //			printAsChars(bufferHead, bufferSize);
        //			printAsBytes(bufferHead, bufferSize);

        if (sizeRead == -1) {
            coutWithEndl("gpgme_data_read error");
            //errno
        }

        if (sizeRead < blockSize)
            break;

        bufferSize += blockSize;
        bufferHead = (char*)realloc(bufferHead, bufferSize);
        current = bufferHead + bufferSize - blockSize;

        if (!bufferHead)
            coutWithEndl("realloc failure");
    }

    //		coutArgsWithSpaceSeparator("bufferSize:", bufferSize);
    bufferSize = bufferSize - (blockSize - sizeRead);
    //		coutArgsWithSpaceSeparator("bufferSize:", bufferSize);

    if (bufferSize != 0) {
        bufferHead = (char*)realloc(bufferHead, bufferSize);

        if (!bufferHead)
            coutWithEndl("realloc failure");
    }

    //		printAsChars(bufferHead, bufferSize);
    //		printAsBytes(bufferHead, bufferSize);

    return bufferHead;
}
