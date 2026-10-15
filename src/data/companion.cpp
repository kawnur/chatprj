#include "companion.hpp"

#include "data.hpp"
#include "manager.hpp"

Companion::Companion(int id, const std::string &name) : id_(id), name_(name) {}

int Companion::id() const
{
    return id_;
}

std::string Companion::name() const
{
    return name_;
}

void Companion::updateData(CompanionDataPtr data)
{
    name_ = data->name();
    getManager()->updateSocketInfo(id_, data);
}
