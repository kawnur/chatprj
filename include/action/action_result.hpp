#ifndef ACTION_RESULT_HPP
#define ACTION_RESULT_HPP

#include <string>

#include "logging.hpp"

class ActionResult
{
public:
    // TODO add container concept
    ActionResult(bool status, const std::string &definition);
    ActionResult(bool status, std::string &&definition);

    template<typename T>
    ActionResult(bool status, T &&definitions) : status_(status)
    {
        definitions_ = std::forward<T>(definitions);
        // coutVectorState(definitions_);
    }

    virtual ~ActionResult();

    bool status() const;
    std::string definition() const;

protected:
    bool status_;
    std::vector<std::string> definitions_;
};

template<typename T>
class ActionValueResult : public ActionResult
{
public:
    ActionValueResult(const T &value, bool status, const std::string &definition)
        : value_(value), ActionResult(status, definition) {}

    ~ActionValueResult() { logArgsInfo(Q_FUNC_INFO); }

    T value() const
    {
        return value_;
    }

private:
    T value_;
};

template<typename T>
class ActionSharedValueResult : public ActionResult
{
public:
    ActionSharedValueResult(std::shared_ptr<T> value, bool status, const std::string &definition)
        : value_(value), ActionResult(status, definition) {}

    ~ActionSharedValueResult() { logArgsInfo(Q_FUNC_INFO); }

    std::shared_ptr<T> value() const
    {
        return value_;
    }

private:
    std::shared_ptr<T> value_;
};

#endif // ACTION_RESULT_HPP
