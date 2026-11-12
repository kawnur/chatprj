#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <cstdint>
#include <string>

#include <nlohmann/json.hpp>

#include "alias.hpp"
#include "constants.hpp"
#include "message_state.hpp"

class Message
{
public:
    using meta_type = MessageMetaData;
    using data_type = MessageData;
    using state_type = MessageState;

    using MetaPtr = std::shared_ptr<meta_type>;
    using DataPtr = std::shared_ptr<data_type>;
    using StatePtr = std::shared_ptr<state_type>;

    Message(MetaPtr meta, DataPtr data, StatePtr state);
    Message(MessagePtr message);

    virtual ~Message();

    uint32_t id() const;
    uint8_t companionId() const;
    uint8_t authorId() const;
    std::string time() const;
    std::string _data() const;
    bool isMessageFromMe() const;
    bool isSent() const;
    bool isReceived() const;
    bool isAntecedent() const;
    std::string networkId() const;

    void setId(int id);
    void setCompanionId(int id);
    void setTime(const std::string &time);
    void setReceived(bool value);

    friend bool operator<(const Message &message1, const Message &message2);

    MetaPtr meta() const;
    DataPtr data() const;
    StatePtr state() const;

    void updateMeta(MessageMetaDataPtr meta);
    // virtual std::string getNetworkReadyData();
    std::string getNetworkReadyData();

protected:
    virtual nlohmann::json getJsonData() { return nlohmann::json::object(); }
    nlohmann::json buildJsonData(NetworkMessageType type);
    virtual std::string typeMark();
    virtual void log();

    MetaPtr meta_;
    DataPtr data_;
    StatePtr state_;
};

class TextMessage : public Message
{
public:
    using meta_type = MessageMetaData;
    using data_type = TextMessageData;
    using state_type = MessageState;

    TextMessage(MetaPtr meta, DataPtr data, StatePtr state);
    ~TextMessage() = default;

    nlohmann::json getJsonData() override;
    std::string typeMark() override;
};

class ReceiveConfirmationMessage : public Message
{
public:
    using meta_type = MessageMetaData;
    using data_type = MessageData;
    using state_type = ReceiveConfirmationMessageState;

    ReceiveConfirmationMessage(MetaPtr meta, DataPtr data, StatePtr state);
    ~ReceiveConfirmationMessage() = default;

    nlohmann::json getJsonData() override;
    std::string typeMark() override;
};

class ReceiveConfirmationRequestMessage : public Message
{
public:
    using meta_type = MessageMetaData;
    using data_type = MessageData;
    using state_type = MessageState;

    ReceiveConfirmationRequestMessage(MetaPtr meta, DataPtr data, StatePtr state);
    ~ReceiveConfirmationRequestMessage() = default;

    nlohmann::json getJsonData() override;
    std::string typeMark() override;
};

#endif // MESSAGE_HPP
