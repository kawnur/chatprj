#include "utils_gpgme.hpp"

void checkProtocols() {}

void coutEngineInfo(gpgme_engine_info_t* info)
{
    endline(1);
    coutWithEndl((*info)->next);
    coutWithEndl((*info)->protocol);
    coutStringWithEndl((*info)->file_name);
    coutStringWithEndl((*info)->home_dir);
    coutStringWithEndl((*info)->version);
    coutStringWithEndl((*info)->req_version);
    endline(1);
}

std::string getStringFromCharPtr(const char* value)
{
    return (value) ? std::string(value) : std::string("nullptr");
}

void coutKeyInfo(const gpgme_key_t* const key)
{
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

    coutArgsWithSpaceSeparator("issuer_serial:", getStringFromCharPtr((*key)->issuer_serial));
    coutArgsWithSpaceSeparator("issuer_name:", getStringFromCharPtr((*key)->issuer_name));
    coutArgsWithSpaceSeparator("chain_id:", getStringFromCharPtr((*key)->chain_id));

    coutArgsWithSpaceSeparator("owner_trust:", (*key)->owner_trust);
    coutArgsWithSpaceSeparator("subkeys:", (*key)->subkeys);
    coutArgsWithSpaceSeparator("uids:", (*key)->uids);

    coutArgsWithSpaceSeparator("fpr:", getStringFromCharPtr((*key)->fpr));

    coutArgsWithSpaceSeparator("last_update:", (*key)->last_update);

    coutArgsWithSpaceSeparator("key->subkeys->length:", (*key)->subkeys->length);

    endline(1);
}

void coutUserIdInfo(gpgme_key_t* key)
{
    _gpgme_user_id* uids = (*key)->uids;

    endline(1);

    coutArgsWithSpaceSeparator("next:", uids->next);
    coutArgsWithSpaceSeparator("revoked:", (int)(uids->revoked));
    coutArgsWithSpaceSeparator("invalid:", (int)(uids->invalid));
    coutArgsWithSpaceSeparator("_unused:", (int)(uids->_unused));
    coutArgsWithSpaceSeparator("origin:", (int)(uids->origin));
    coutArgsWithSpaceSeparator("validity:", uids->validity);

    coutArgsWithSpaceSeparator("uid:", getStringFromCharPtr(uids->uid));
    coutArgsWithSpaceSeparator("name:", getStringFromCharPtr(uids->name));
    coutArgsWithSpaceSeparator("email:", getStringFromCharPtr(uids->email));
    coutArgsWithSpaceSeparator("comment:", getStringFromCharPtr(uids->comment));

    coutArgsWithSpaceSeparator("signatures:", uids->signatures);
    coutArgsWithSpaceSeparator("_last_keysig:", uids->_last_keysig);

    coutArgsWithSpaceSeparator("address:", getStringFromCharPtr(uids->address));

    coutArgsWithSpaceSeparator("tofu:", uids->tofu);
    coutArgsWithSpaceSeparator("last_update:", uids->last_update);

    coutArgsWithSpaceSeparator("uidhash:", getStringFromCharPtr(uids->uidhash));

    endline(1);
}

void createKey(gpgme_ctx_t* contextPtr, const char* algoName)
{
    // create key
    const char* userId = "user2";
    unsigned long reserved = 0;
    unsigned long expires = 1000000000;
    gpgme_key_t extrakey = NULL;
    unsigned int flags = GPGME_CREATE_ENCR;

    auto errorCreateKey = gpgme_op_createkey(
            *contextPtr, userId, algoName, reserved, expires, extrakey, flags
            );

    coutWithEndl(errorCreateKey);

    if(errorCreateKey == 0)
    {
        coutWithEndl("key created successfully");
    }
    else if(errorCreateKey == GPG_ERR_NOT_SUPPORTED)
    {
        coutWithEndl("engine does not support the command");
    }
    else
    {
        coutWithEndl("key creation: some other error");
    }
}

void listKeys(gpgme_ctx_t* contextPtr)
{
    // list keys
    const char* pattern = NULL;
    int secret_only = 0;

    int i = 0;
    coutArgsWithSpaceSeparator("i:", i);

    auto errorKeylistStart = gpgme_op_keylist_start(*contextPtr, pattern, secret_only);

    if(errorKeylistStart == GPG_ERR_INV_VALUE)
    {
        coutWithEndl("key listing: context is not a valid pointer");
    }

    gpgme_error_t errorKeylistNext = NULL;

    while(errorKeylistNext != GPG_ERR_EOF && i < 4)
    {
        gpgme_key_t r_key;
//			coutArgsWithSpaceSeparator("&r_key:", &r_key);

        errorKeylistNext = gpgme_op_keylist_next(*contextPtr, &r_key);

        if(errorKeylistNext == GPG_ERR_INV_VALUE)
        {
            coutWithEndl("key listing: context or r_key is not a valid pointer");
        }
        else if(errorKeylistNext == GPG_ERR_ENOMEM)
        {
            coutWithEndl("key listing: there is not enough memory for the operation");
        }

        if(r_key != nullptr)
        {
            coutKeyInfo(&r_key);
            coutUserIdInfo(&r_key);
        }

        i++;
        coutArgsWithSpaceSeparator("i:", i);
    }

    auto errorKeylistEnd = gpgme_op_keylist_end(*contextPtr);

    if(errorKeylistEnd == GPG_ERR_INV_VALUE)
    {
        coutWithEndl("key listing: context is not a valid pointer");
    }
    else if(errorKeylistEnd == GPG_ERR_ENOMEM)
    {
        coutWithEndl("key listing: there is not enough memory for the operation");
    }

    i++;
    coutArgsWithSpaceSeparator("i:", i);
}

void getKeyByUser(gpgme_ctx_t* contextPtr, gpgme_key_t* keyPtr, const char* name)
{

    const char* pattern = NULL;
    int secret_only = 0;

    auto errorKeylistStart = gpgme_op_keylist_start(*contextPtr, pattern, secret_only);

    if(errorKeylistStart == GPG_ERR_INV_VALUE)
    {
        coutWithEndl("getKeyByUser: context is not a valid pointer");
    }

    gpgme_error_t errorKeylistNext = NULL;

    while(true)
    {
        errorKeylistNext = gpgme_op_keylist_next(*contextPtr, keyPtr);

        if(errorKeylistNext == GPG_ERR_INV_VALUE)
        {
            coutWithEndl("key listing: context or r_key is not a valid pointer");
        }
        else if(errorKeylistNext == GPG_ERR_ENOMEM)
        {
            coutWithEndl("key listing: there is not enough memory for the operation");
        }

        if(strcmp((*keyPtr)->uids->name, name) == 0)
        {
            coutWithEndl("key found");
            break;
        }
    }
}

void createDataObject(gpgme_data_t* dataPtr)
{
    auto errorDataCreation = gpgme_data_new(dataPtr);

    if(errorDataCreation == GPG_ERR_NO_ERROR)
    {
        coutWithEndl("data object was successfully created");
    }
    else if(errorDataCreation == GPG_ERR_INV_VALUE)
    {
        coutWithEndl("is not a valid pointer");
    }
    else if(errorDataCreation == GPG_ERR_ENOMEM)
    {
        coutWithEndl("not enough memory");
    }
}

void printAsBytesTillNullTerminator(const char* value)
{
    const char* p = value;

    int i = 0;

    while(*p != '\0')
    {
        printf("%x ", *p);
        p++;
        i++;
    }
    endline(1);
}

void printAsBytes(const char* value, size_t size)
{
    const char* p = value;

    size_t i = 0;

    while(i < size)
    {
        printf("%x ", *p);
        p++;
        i++;
    }
    endline(1);
}

void printAsChars(const char* value, size_t size)
{
    const char* p = value;

    size_t i = 0;

    while(i < size)
    {
        printf("%c ", *p);
        p++;
        i++;
    }
    endline(1);
}

int getTerminatorPosition(const char* value, ssize_t size)
{
    const char* p = value;

    int i = 0;

    while(i < size)
    {
        if(*p == '\0')
        {
            return i;
        }

        p++;
        i++;
    }

    return -1;
}

void seekSetZero(gpgme_data_t& data)
{
    auto off = gpgme_data_seek(data, 0, SEEK_SET);
//		coutArgsWithSpaceSeparator("off:", off);

    if(off == -1)
    {
        coutWithEndl("gpgme_data_seek error");
    }
}

void encrypt(
        gpgme_ctx_t* contextPtr,
        gpgme_key_t* keys,
        gpgme_encrypt_flags_t& flags,
        gpgme_data_t& data,
        gpgme_data_t& dataEncrypt)
{
    seekSetZero(data);
    seekSetZero(dataEncrypt);

    auto errorEncryption = gpgme_op_encrypt(*contextPtr, keys, flags, data, dataEncrypt);

    if(errorEncryption == GPG_ERR_NO_ERROR)
    {
        coutWithEndl("ciphertext created successfully");
    }
    else if(errorEncryption == GPG_ERR_INV_VALUE)
    {
        coutWithEndl("ctx, recp, plain or cipher is not a valid pointer");
    }
    else if(errorEncryption == GPG_ERR_UNUSABLE_PUBKEY)
    {
        coutWithEndl("recp contains some invalid recipients");
    }
    else if(errorEncryption == GPG_ERR_BAD_PASSPHRASE)
    {
        coutWithEndl("passphrase for the symmetric key could not be retrieved");
    }
}

void decrypt(gpgme_ctx_t* contextPtr, gpgme_data_t& dataEncrypt, gpgme_data_t& dataDecrypt)
{
    seekSetZero(dataEncrypt);
    seekSetZero(dataDecrypt);

    auto errorDecryption = gpgme_op_decrypt(*contextPtr, dataEncrypt, dataDecrypt);

    if(errorDecryption == GPG_ERR_NO_ERROR)
    {
        coutWithEndl("ciphertext decrypted successfully");
    }
    else if(errorDecryption == GPG_ERR_INV_VALUE)
    {
        coutWithEndl("ctx, plain or cipher is not a valid pointer");
    }
    else if(errorDecryption == GPG_ERR_NO_DATA)
    {
        coutWithEndl("cipher does not contain any data to decrypt");
    }
    else if(errorDecryption == GPG_ERR_DECRYPT_FAILED)
    {
        coutWithEndl("cipher is not a valid cipher text");
    }
    else if(errorDecryption == GPG_ERR_BAD_PASSPHRASE)
    {
        coutWithEndl("passphrase for the secret key could not be retrieved");
    }
}

ssize_t readData(gpgme_data_t& data, char* dataString, size_t size)
{
    seekSetZero(data);

    auto sizeRead = gpgme_data_read(data, dataString, size);
//		coutArgsWithSpaceSeparator("sizeRead:", sizeRead);

    if(sizeRead == -1)
    {
        coutWithEndl("gpgme_data_read error");
        //errno
    }

    return sizeRead;
}

void readData1(gpgme_data_t& data, std::string& dataString)
{
    seekSetZero(data);

    char* p = new char[2];

    while(true)
    {
        auto sizeRead = gpgme_data_read(data, p, 1);
//			coutArgsWithSpaceSeparator("sizeRead:", sizeRead);

        if(sizeRead == -1)
        {
            coutWithEndl("gpgme_data_read error");
            //errno
        }

        dataString.push_back(*p);

        if(*p == '\0')
        {
            break;
        }
    }

    delete[] p;
}

char* readData2(gpgme_data_t& data)
{
    seekSetZero(data);

    size_t blockSize = 10;
    size_t bufferSize = blockSize;
    ssize_t sizeRead = blockSize;
    char* bufferHead = (char*)malloc(bufferSize);
    char* current = bufferHead;

    while(true)
    {
        sizeRead = gpgme_data_read(data, current, blockSize);

//			coutArgsWithSpaceSeparator("blockSize:", blockSize);
//			coutArgsWithSpaceSeparator("(void*)bufferHead:", (void*)bufferHead);
//			coutArgsWithSpaceSeparator("(void*)current:", (void*)current);
//			coutArgsWithSpaceSeparator("sizeRead:", sizeRead);
//			printAsChars(bufferHead, bufferSize);
//			printAsBytes(bufferHead, bufferSize);

        if(sizeRead == -1)
        {
            coutWithEndl("gpgme_data_read error");
            //errno
        }

        if(sizeRead < blockSize)
        {
            break;
        }

        bufferSize += blockSize;
        bufferHead = (char*)realloc(bufferHead, bufferSize);
        current = bufferHead + bufferSize - blockSize;

        if(!bufferHead)
        {
            coutWithEndl("realloc failure");
        }
    }

//		coutArgsWithSpaceSeparator("bufferSize:", bufferSize);
    bufferSize = bufferSize - (blockSize - sizeRead);
//		coutArgsWithSpaceSeparator("bufferSize:", bufferSize);

    if(bufferSize != 0)
    {
        bufferHead = (char*)realloc(bufferHead, bufferSize);

        if(!bufferHead)
        {
            coutWithEndl("realloc failure");
        }
    }

//		printAsChars(bufferHead, bufferSize);
//		printAsBytes(bufferHead, bufferSize);

    return bufferHead;
}
