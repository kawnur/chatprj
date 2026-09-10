#ifndef UTILS_WIDGETS_HPP
#define UTILS_WIDGETS_HPP

#include <memory>
#include <vector>

#include <QString>

void showErrorDialogAndLogError(QString &&message);

class ButtonInfo;

std::shared_ptr<std::vector<ButtonInfo>> getButtonInfoVector(const QString&);

#endif // UTILS_WIDGETS_HPP
