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
        uint32_t messageId, uint8_t companionId, uint8_t authorId, const std::string &timestampTz,
        const std::string networkId);

    MessageMetaData(MessageMetaDataPtr meta);

    bool setFields(CompanionPtr companion, const nlohmann::json &data);
    bool isValid();
    void update(MessageMetaDataPtr meta);
    bool isMessageFromMe();

    uint32_t messageId() const;
    uint8_t companionId() const;
    uint8_t authorId() const;
    std::string timestampTz() const;
    std::string networkId() const;

    void setMessageId(uint32_t newMessageId);
    void setCompanionId(uint8_t newCompanionId);
    void setTimestampTz(const std::string &newTimestampTz);
    void setNetworkId(const std::string &newNetworkId);

    virtual std::string getLogString();

    void log();

    virtual nlohmann::json getJsonData();

private:
    uint32_t messageId_;
    uint8_t companionId_;
    uint8_t authorId_;
    std::string timestampTz_;
    std::string networkId_;
};

#endif // MESSAGE_META_DATA_HPP
