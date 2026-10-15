#include "db_operator.hpp"

DBOperator::DBOperator() : log_(LOG_DB_INTERACTION), mutex_(), connection_(nullptr)
{
    connectToDB();

    if (!connection_)
        logTemplateError("{}, no DB connection", __FUNCTION__);
}

void DBOperator::connectToDB()
{
    // TODO make connection to db secure

    // get params
    DBConnectionPrameters parameters {};

    if (!parameters.isValid())
        return;

    parameters.log();

    // create connection
    auto info = parameters.getConnectionInfo();
    auto connection = PGconnPtr(PQconnectdb(info.data()), PQfinish);

    if (getConnectionStatus(connection))
        connection_ = connection;
}

PGresultPtr DBOperator::sendRequestAndReturnResult(const std::string &command)
{
    if (log_)
        logArgs(command);

    std::lock_guard<std::mutex> lock(mutex_);

    auto lambda = [](PGresult *result)
    {
        if (result)
            PQclear(result);
    };

    PGresultPtr result(PQexec(connection_.get(), command.data()), lambda);

    return result;
}

bool DBOperator::isReady()
{
    return (connection_) ? true : false;
}

bool getConnectionStatus(PGconnPtr connection)
{
    ConnStatusType status = PQstatus(connection.get());
    std::string mark = (status == 0) ? "OK" : "?";

    logTemplateInfo("DB connection status: {0} {1}", std::to_string(status), mark);

    if (status == ConnStatusType::CONNECTION_BAD) {
        logArgsError("DB connection status: CONNECTION_BAD");

        return false;
    }

    return true;
}
