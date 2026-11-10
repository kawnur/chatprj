#ifndef MESSAGE_META_DATA_HPP
#define MESSAGE_META_DATA_HPP

#include <cstdint>
#include <string>

#include <nlohmann/json.hpp>

#include "alias.hpp"
#include "constants.hpp"

class MessageMetaData
{
public:
    MessageMetaData() = default;

    MessageMetaData(
        NetworkMessageType networkMessageType, uint32_t messageId,
        uint8_t companionId, /*const std::string &companionName, */uint8_t authorId,
        /*const std::string &authorName, */const std::string &timestampTz,/* const std::string &hashMD5,*/
        const std::string networkId);

    MessageMetaData(MessageMetaDataPtr meta);

    bool setFields(CompanionPtr companion, const nlohmann::json &data);
    bool isValid();
    void update(MessageMetaDataPtr meta);
    bool isMessageFromMe();

    NetworkMessageType networkMessageType() const;
    uint32_t messageId() const;
    uint8_t companionId() const;
    // std::string companionName() const;
    uint8_t authorId() const;
    // std::string authorName() const;
    std::string timestampTz() const;
    // std::string hashMD5() const;
    std::string networkId() const;

    void setMessageId(uint32_t newMessageId);
    void setCompanionId(uint8_t newCompanionId);
    void setTimestampTz(const std::string &newTimestampTz);
    void setNetworkId(const std::string &newNetworkId);
    void setNetworkMessageType(NetworkMessageType newNetworkMessageType);
    // void setCompanionName(const std::string &newCompanionName);
    // void setAuthorName(const std::string &newAuthorName);

    virtual std::string getLogString();

    void log();

private:
    NetworkMessageType networkMessageType_;
    uint32_t messageId_;
    uint8_t companionId_;
    // std::string companionName_;
    uint8_t authorId_;
    // std::string authorName_;
    std::string timestampTz_;
    // std::string hashMD5_;
    std::string networkId_;
};

#endif // MESSAGE_META_DATA_HPP
