#include "action_result.hpp"

ActionResult::ActionResult(bool status, const std::string &definition)
    : status_(status), definitions_()
{
    definitions_.push_back(definition);
}

ActionResult::ActionResult(bool status, std::string &&definition)
    : status_(status), definitions_()
{
    definitions_.push_back(definition);
}

ActionResult::~ActionResult() { logArgsInfo(Q_FUNC_INFO); }

bool ActionResult::status() const
{
    return status_;
}

std::string ActionResult::definition() const
{
    if (definitions_.empty())
        return ""s;
    else if (definitions_.size() == 1)
        return definitions_.at(0);
    else
        return buildTextAsUnorderedList(definitions_);
}
