#ifndef ACTION_RESULT_HPP
#define ACTION_RESULT_HPP

#include <string>

#include "alias.hpp"
#include "logging.hpp"

class ActionResult
{
public:
    // TODO add container concept
    template<class T>
    ActionResult(bool status, T &&value) : status_(status)
    {
        if constexpr (Same<T, std::string>)
            definitions_.push_back(value);
        else
            definitions_ = std::forward<T>(value);

        // coutVectorState(definitions_);
    }

    virtual ~ActionResult();

    bool status() const;
    std::string definition() const;

protected:
    bool status_;
    std::vector<std::string> definitions_;
};

template<class T>
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

template<class T>
class ActionSharedValueResult : public ActionResult
{
public:
    ActionSharedValueResult(TPtr<T> value, bool status, const std::string &definition)
        : value_(value), ActionResult(status, definition) {}

    ~ActionSharedValueResult() { logArgsInfo(Q_FUNC_INFO); }

    TPtr<T> value() const
    {
        return value_;
    }

private:
    TPtr<T> value_;
};

#endif // ACTION_RESULT_HPP
