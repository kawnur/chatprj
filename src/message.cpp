#include "message.hpp"

Message::Message(
    MessageType type, uint32_t id, uint8_t companion_id, uint8_t author_id,
    const std::string& time, const std::string& text) :
    type_(type), id_(id), companion_id_(companion_id), author_id_(author_id),
    time_(time), text_(text) {}

MessageType Message::getType() const {
    return this->type_;
}

uint32_t Message::getId() const {
    return this->id_;
}

uint8_t Message::getCompanionId() const {
    return this->companion_id_;
}

uint8_t Message::getAuthorId() const {
    return this->author_id_;
}

std::string Message::getTime() const {
    return this->time_;
}

std::string Message::getText() const {
    return this->text_;
}

bool Message::isMessageFromMe() const {
    return !(this->author_id_ == this->companion_id_);
}

MessageState::MessageState(
    uint8_t companionId, bool isAntecedent, bool isSent,
    bool isReceived, std::string networkId) :
    isAntecedent_(isAntecedent), isSent_(isSent), isReceived_(isReceived),
    networkId_(networkId) {}

bool MessageState::getIsAntecedent() const {
    return this->isAntecedent_;
}

bool MessageState::getIsSent() const {
    return this->isSent_;
}

bool MessageState::getIsReceived() const {
    return this->isReceived_;
}

std::string MessageState::getNetworkId() const {
    return this->networkId_;
}

void MessageState::setIsAntecedent(bool value) {
    this->isAntecedent_ = value;
}

void MessageState::setIsReceived(bool value) {
    this->isReceived_ = value;
}

void MessageState::setNetworkId(const std::string& networkId) {
    this->networkId_ = networkId;
}

MessageInfo::MessageInfo(std::shared_ptr<MessageState> state, std::shared_ptr<MessageWidget> widget) :
    state_(state), widget_(widget) {}

std::shared_ptr<MessageState> MessageInfo::getState() const {
    return this->state_;
}

std::shared_ptr<MessageWidget> MessageInfo::getWidget() const {
    return this->widget_;
}

void MessageInfo::setWidget(std::shared_ptr<MessageWidget> widget) {
    this->widget_ = widget;
}
