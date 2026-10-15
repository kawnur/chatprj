#ifndef COMPANION_HPP
#define COMPANION_HPP

#include <memory>
#include <string>

#include "alias.hpp"

class CompanionData;

class Companion : public std::enable_shared_from_this<Companion>
{
public:
    Companion(int id, const std::string &name);
    ~Companion() = default;

    friend bool operator<(const Companion &object1, const Companion &object2)
    {
        return object1.id() < object2.id();
    }

    int id() const;
    std::string name() const;

    void updateData(CompanionDataPtr data);

private:
    int id_;  // TODO change type
    std::string name_;
};

#endif // COMPANION_HPP
