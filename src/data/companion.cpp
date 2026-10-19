#include "companion.hpp"

Companion::Companion(int id, const std::string &name) : id_(id), name_(name) {}

int Companion::id() const
{
    return id_;
}

std::string Companion::name() const
{
    return name_;
}

void Companion::setName(const std::string &name)
{
    name_ = name;
}
