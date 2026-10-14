#ifndef DB_OPERATOR_HPP
#define DB_OPERATOR_HPP

#include <memory>
#include <mutex>

#include "constants.hpp"
#include "db_interaction.hpp"
#include "logging.hpp"
#include "utils.hpp"

class DBOperator
{
public:
    DBOperator();
    ~DBOperator() = default;

    void connectToDB();

    template<typename... Ts>
    std::shared_ptr<DBReplyData> getDBData(DBRequestType type, Ts &&...args)
    {
        DBRequestData requestData { type };
        auto data = getReplyData(requestData, args...);

        if (!data) {
            auto entry = getStringByFormat("{0}, {1}", requestData.logMark(), DB_REPLY_NULL);
            // showErrorDialogAndLogError(entry);

            return nullptr;
        }

        if (data->isEmpty()) {
            auto entry = getStringByFormat("{0}, {1}", requestData.logMark(), DB_REPLY_EMPTY);
            // showWarningDialogAndLogWarning(entry);

            // return nullptr;
        }

        return data;
    }

    template<typename... Ts>
    std::shared_ptr<DBReplyData> getReplyData(DBRequestData &requestData, Ts &&...args)
    {
        if (!requestData.isValid()) {
            logArgsError("requestData is invalid");

            return nullptr;
        }

        auto command = requestData.buildCommand(args...);
        std::shared_ptr<PGresult> dbResult = sendRequestAndReturnResult(command);

        if (log_) {
            logArgs(logDelimiter);
            logArgs(requestData.logMark());
            logArgs("dbResult:", dbResult.get());
        }

        if (!dbResult) {
            // showErrorDialogAndLogError("Database request error, dbResult is nullptr");

            return nullptr;
        }

        auto replyData = std::make_shared<DBReplyData>(log_, requestData.replyKeys());

        if (replyData->getDataFromResult(dbResult, 0) == -1) {
            // showErrorDialogAndLogError("Error getting data from dbResult");

            return nullptr;
        }

        if (log_) {
            // logArgs("dbData->size():", dbData->size());
            replyData->log();
            logArgs(logDelimiter);
        }

        return replyData;
    }

    std::shared_ptr<PGresult> sendRequestAndReturnResult(const std::string &command);
    bool isReady();


private:
    bool log_;
    std::mutex mutex_;  // ???
    std::shared_ptr<PGconn> connection_;
};

bool getConnectionStatus(std::shared_ptr<PGconn> connection);

#endif // DB_OPERATOR_HPP
