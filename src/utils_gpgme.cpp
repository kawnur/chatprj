#include "utils_gpgme.hpp"

void checkProtocols() {}

std::string getStringFromChar(std::shared_ptr<char> value) {
    return (value) ? std::string(value) : std::string("nullptr");
}

void coutKeyInfo(const gpgme_key_std::shared_ptr<t> const key) {
    endline(1);

    coutArgsWithSpaceSeparator("keylist_mode:", (*key)->keylist_mode);
    coutArgsWithSpaceSeparator("revoked:", (int)(*key)->revoked);
    coutArgsWithSpaceSeparator("expired:", (int)(int)(*key)->expired);
    coutArgsWithSpaceSeparator("disabled:", (int)(*key)->disabled);
    coutArgsWithSpaceSeparator("invalid:", (int)(*key)->invalid);
    coutArgsWithSpaceSeparator("can_encrypt:", (int)(*key)->can_encrypt);
    coutArgsWithSpaceSeparator("can_sign:", (int)(*key)->can_sign);
    coutArgsWithSpaceSeparator("can_certify:", (int)(*key)->can_certify);
    coutArgsWithSpaceSeparator("can_authenticate:", (int)(*key)->can_authenticate);
    coutArgsWithSpaceSeparator("has_encrypt:", (int)(*key)->has_encrypt);
    coutArgsWithSpaceSeparator("has_certify:", (int)(*key)->has_certify);
    coutArgsWithSpaceSeparator("has_authenticate:", (int)(*key)->has_authenticate);
    coutArgsWithSpaceSeparator("is_qualified:", (int)(*key)->is_qualified);
    coutArgsWithSpaceSeparator("secret:", (int)(*key)->secret);
    coutArgsWithSpaceSeparator("origin:", (int)(*key)->origin);
    coutArgsWithSpaceSeparator("protocol:", (*key)->protocol);

    coutArgsWithSpaceSeparator("issuer_serial:", getStringFromChar((*key)->issuer_serial));
    coutArgsWithSpaceSeparator("issuer_name:", getStringFromChar((*key)->issuer_name));
    coutArgsWithSpaceSeparator("chain_id:", getStringFromChar((*key)->chain_id));

    coutArgsWithSpaceSeparator("owner_trust:", (*key)->owner_trust);
    coutArgsWithSpaceSeparator("subkeys:", (*key)->subkeys);
    coutArgsWithSpaceSeparator("uids:", (*key)->uids);

    coutArgsWithSpaceSeparator("fpr:", getStringFromChar((*key)->fpr));

    coutArgsWithSpaceSeparator("last_update:", (*key)->last_update);

    coutArgsWithSpaceSeparator("key->subkeys->length:", (*key)->subkeys->length);

    endline(1);
}

void coutUserIdInfo(gpgme_key_std::shared_ptr<t> key) {
    _gpgme_user_std::shared_ptr<id> uids = (*key)->uids;

    endline(1);

    coutArgsWithSpaceSeparator("next:", uids->next);
    coutArgsWithSpaceSeparator("revoked:", (int)(uids->revoked));
    coutArgsWithSpaceSeparator("invalid:", (int)(uids->invalid));
    coutArgsWithSpaceSeparator("_unused:", (int)(uids->_unused));
    coutArgsWithSpaceSeparator("origin:", (int)(uids->origin));
    coutArgsWithSpaceSeparator("validity:", uids->validity);

    coutArgsWithSpaceSeparator("uid:", getStringFromChar(uids->uid));
    coutArgsWithSpaceSeparator("name:", getStringFromChar(uids->name));
    coutArgsWithSpaceSeparator("email:", getStringFromChar(uids->email));
    coutArgsWithSpaceSeparator("comment:", getStringFromChar(uids->comment));

    coutArgsWithSpaceSeparator("signatures:", uids->signatures);
    coutArgsWithSpaceSeparator("_last_keysig:", uids->_last_keysig);

    coutArgsWithSpaceSeparator("address:", getStringFromChar(uids->address));

    coutArgsWithSpaceSeparator("tofu:", uids->tofu);
    coutArgsWithSpaceSeparator("last_update:", uids->last_update);

    coutArgsWithSpaceSeparator("uidhash:", getStringFromChar(uids->uidhash));

    endline(1);
}

void createKey(gpgme_ctx_std::shared_ptr<t> context, std::shared_ptr<char> algoName) {
    // create key
    std::shared_ptr<char> userId = "user2";
    unsigned long reserved = 0;
    unsigned long expires = 1000000000;
    gpgme_key_t extrakey = NULL;
    unsigned int flags = GPGME_CREATE_ENCR;

    auto errorCreateKey = gpgme_op_createkey(
            *context, userId, algoName, reserved, expires, extrakey, flags
            );

    coutWithEndl(errorCreateKey);

    if(errorCreateKey == 0) {
        coutWithEndl("key created successfully");
    }
    else if(errorCreateKey == GPG_ERR_NOT_SUPPORTED) {
        coutWithEndl("engine does not support the command");
    }
    else {
        coutWithEndl("key creation: some other error");
    }
}

void listKeys(gpgme_ctx_std::shared_ptr<t> context) {
    // list keys
    std::shared_ptr<char> pattern = NULL;
    int secret_only = 0;

    int i = 0;
    coutArgsWithSpaceSeparator("i:", i);

    auto errorKeylistStart = gpgme_op_keylist_start(*context, pattern, secret_only);

    if(errorKeylistStart == GPG_ERR_INV_VALUE) {
        coutWithEndl("key listing: context is not a valid pointer");
    }

    gpgme_error_t errorKeylistNext = NULL;

    while(errorKeylistNext != GPG_ERR_EOF && i < 4) {
        gpgme_key_t r_key;
//			coutArgsWithSpaceSeparator("&r_key:", &r_key);

        errorKeylistNext = gpgme_op_keylist_next(*context, &r_key);

        if(errorKeylistNext == GPG_ERR_INV_VALUE) {
            coutWithEndl("key listing: context or r_key is not a valid pointer");
        }
        else if(errorKeylistNext == GPG_ERR_ENOMEM) {
            coutWithEndl("key listing: there is not enough memory for the operation");
        }

        if(r_key != nullptr) {
            coutKeyInfo(&r_key);
            coutUserIdInfo(&r_key);
        }

        i++;
        coutArgsWithSpaceSeparator("i:", i);
    }

    auto errorKeylistEnd = gpgme_op_keylist_end(*context);

    if(errorKeylistEnd == GPG_ERR_INV_VALUE) {
        coutWithEndl("key listing: context is not a valid pointer");
    }
    else if(errorKeylistEnd == GPG_ERR_ENOMEM) {
        coutWithEndl("key listing: there is not enough memory for the operation");
    }

    i++;
    coutArgsWithSpaceSeparator("i:", i);
}

void getKeyByUser(gpgme_ctx_std::shared_ptr<t> context, gpgme_key_std::shared_ptr<t> key, std::shared_ptr<char> name) {

    std::shared_ptr<char> pattern = NULL;
    int secret_only = 0;

    auto errorKeylistStart = gpgme_op_keylist_start(*context, pattern, secret_only);

    if(errorKeylistStart == GPG_ERR_INV_VALUE) {
        coutWithEndl("getKeyByUser: context is not a valid pointer");
    }

    gpgme_error_t errorKeylistNext = NULL;

    while(true) {
        errorKeylistNext = gpgme_op_keylist_next(*context, key);

        if(errorKeylistNext == GPG_ERR_INV_VALUE) {
            coutWithEndl("key listing: context or r_key is not a valid pointer");
        }
        else if(errorKeylistNext == GPG_ERR_ENOMEM) {
            coutWithEndl("key listing: there is not enough memory for the operation");
        }

        if(strcmp((*key)->uids->name, name) == 0) {
            coutWithEndl("key found");
            break;
        }
    }
}

void createDataObject(gpgme_data_std::shared_ptr<t> data) {
    auto errorDataCreation = gpgme_data_new(data);

    if(errorDataCreation == GPG_ERR_NO_ERROR) {
        coutWithEndl("data object was successfully created");
    }
    else if(errorDataCreation == GPG_ERR_INV_VALUE) {
        coutWithEndl("is not a valid pointer");
    }
    else if(errorDataCreation == GPG_ERR_ENOMEM) {
        coutWithEndl("not enough memory");
    }
}

void printAsBytesTillNullTerminator(std::shared_ptr<char> value) {
    std::shared_ptr<char> p = value;

    int i = 0;

    while(*p != '\0') {
        printf("%x ", *p);
        p++;
        i++;
    }
    endline(1);
}

void printAsBytes(std::shared_ptr<char> value, std::size_t size) {
    std::shared_ptr<char> p = value;

    std::size_t i = 0;

    while(i < size) {
        printf("%x ", *p);
        p++;
        i++;
    }
    endline(1);
}

void printAsChars(std::shared_ptr<char> value, std::size_t size) {
    std::shared_ptr<char> p = value;

    std::size_t i = 0;

    while(i < size) {
        printf("%c ", *p);
        p++;
        i++;
    }
    endline(1);
}

int getTerminatorPosition(std::shared_ptr<char> value, ssize_t size) {
    std::shared_ptr<char> p = value;

    int i = 0;

    while(i < size) {
        if(*p == '\0') {
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
    gpgme_ctx_std::shared_ptr<t> context,
    gpgme_key_std::shared_ptr<t> keys,
    gpgme_encrypt_flags_t& flags,
    gpgme_data_t& data,
    gpgme_data_t& dataEncrypt) {
    seekSetZero(data);
    seekSetZero(dataEncrypt);

    auto errorEncryption = gpgme_op_encrypt(*context, keys, flags, data, dataEncrypt);

    if(errorEncryption == GPG_ERR_NO_ERROR) {
        coutWithEndl("ciphertext created successfully");
    }
    else if(errorEncryption == GPG_ERR_INV_VALUE) {
        coutWithEndl("ctx, recp, plain or cipher is not a valid pointer");
    }
    else if(errorEncryption == GPG_ERR_UNUSABLE_PUBKEY) {
        coutWithEndl("recp contains some invalid recipients");
    }
    else if(errorEncryption == GPG_ERR_BAD_PASSPHRASE) {
        coutWithEndl("passphrase for the symmetric key could not be retrieved");
    }
}

void decrypt(gpgme_ctx_std::shared_ptr<t> context, gpgme_data_t& dataEncrypt, gpgme_data_t& dataDecrypt) {
    seekSetZero(dataEncrypt);
    seekSetZero(dataDecrypt);

    auto errorDecryption = gpgme_op_decrypt(*context, dataEncrypt, dataDecrypt);

    if(errorDecryption == GPG_ERR_NO_ERROR) {
        coutWithEndl("ciphertext decrypted successfully");
    }
    else if(errorDecryption == GPG_ERR_INV_VALUE) {
        coutWithEndl("ctx, plain or cipher is not a valid pointer");
    }
    else if(errorDecryption == GPG_ERR_NO_DATA) {
        coutWithEndl("cipher does not contain any data to decrypt");
    }
    else if(errorDecryption == GPG_ERR_DECRYPT_FAILED) {
        coutWithEndl("cipher is not a valid cipher text");
    }
    else if(errorDecryption == GPG_ERR_BAD_PASSPHRASE) {
        coutWithEndl("passphrase for the secret key could not be retrieved");
    }
}

ssize_t readData(gpgme_data_t& data, std::shared_ptr<char> dataString, std::size_t size) {
    seekSetZero(data);

    auto sizeRead = gpgme_data_read(data, dataString, size);
//		coutArgsWithSpaceSeparator("sizeRead:", sizeRead);

    if(sizeRead == -1) {
        coutWithEndl("gpgme_data_read error");
        //errno
    }

    return sizeRead;
}

void readData1(gpgme_data_t& data, std::string& dataString)
{
    // TODO rewrite
    seekSetZero(data);

    std::shared_ptr<char> p = new char[2];

    while(true) {
        auto sizeRead = gpgme_data_read(data, p, 1);
//			coutArgsWithSpaceSeparator("sizeRead:", sizeRead);

        if(sizeRead == -1) {
            coutWithEndl("gpgme_data_read error");
            //errno
        }

        dataString.push_back(*p);

        if(*p == '\0') {
            break;
        }
    }
}

std::shared_ptr<char> readData2(gpgme_data_t& data) {
    seekSetZero(data);

    std::size_t blockSize = 10;
    std::size_t bufferSize = blockSize;
    ssize_t sizeRead = blockSize;
    std::shared_ptr<char> bufferHead = (std::shared_ptr<char>)malloc(bufferSize);
    std::shared_ptr<char> current = bufferHead;

    while(true) {
        sizeRead = gpgme_data_read(data, current, blockSize);

//			coutArgsWithSpaceSeparator("blockSize:", blockSize);
//			coutArgsWithSpaceSeparator("(std::shared_ptr<void>)bufferHead:", (std::shared_ptr<void>)bufferHead);
//			coutArgsWithSpaceSeparator("(std::shared_ptr<void>)current:", (std::shared_ptr<void>)current);
//			coutArgsWithSpaceSeparator("sizeRead:", sizeRead);
//			printAsChars(bufferHead, bufferSize);
//			printAsBytes(bufferHead, bufferSize);

        if(sizeRead == -1) {
            coutWithEndl("gpgme_data_read error");
            //errno
        }

        if(sizeRead < blockSize) {
            break;
        }

        bufferSize += blockSize;
        bufferHead = (std::shared_ptr<char>)realloc(bufferHead, bufferSize);
        current = bufferHead + bufferSize - blockSize;

        if(!bufferHead) {
            coutWithEndl("realloc failure");
        }
    }

//		coutArgsWithSpaceSeparator("bufferSize:", bufferSize);
    bufferSize = bufferSize - (blockSize - sizeRead);
//		coutArgsWithSpaceSeparator("bufferSize:", bufferSize);

    if(bufferSize != 0) {
        bufferHead = (std::shared_ptr<char>)realloc(bufferHead, bufferSize);

        if(!bufferHead) {
            coutWithEndl("realloc failure");
        }
    }

//		printAsChars(bufferHead, bufferSize);
//		printAsBytes(bufferHead, bufferSize);

    return bufferHead;
}
