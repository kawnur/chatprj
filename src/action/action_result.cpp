#include "action_result.hpp"

ActionResult::ActionResult(bool status, const std::string &definition)
    : status_(status), definition_(definition) {}

ActionResult::~ActionResult() { logArgsInfo(Q_FUNC_INFO); }

bool ActionResult::status() const
{
    return status_;
}

std::string ActionResult::definition() const
{
    return definition_;
}
