// #ifndef ACTION_WRAPPER_HPP
// #define ACTION_WRAPPER_HPP

// #include <memory>

// #include "action.hpp"
// #include "logging.hpp"
// #include "widgets_dialog.hpp"

// class Action;
// class CompanionData;
// class Dialog;

// class ActionWrapperBase
// {
// public:
//     ActionWrapperBase() = default;
//     virtual ~ActionWrapperBase() { logArgsInfo(Q_FUNC_INFO); }

//     virtual void act() {}
//     virtual void failed() {}
//     virtual void method1() {}
//     virtual std::shared_ptr<Action> getAction() { return nullptr; }
//     virtual std::shared_ptr<CompanionData> getCompanionData() { return nullptr; }
// };

// template<typename T, typename...Ts>
// class ActionWrapper
//     : public ActionWrapperBase, public std::enable_shared_from_this<ActionWrapper<T, Ts...>>
// {
// public:
//     ActionWrapper(Ts &&...params)
//     {
//         auto action = std::make_shared<T>(params...);
//         dialog_ = action->buildDialog();

//         auto cast = std::dynamic_pointer_cast<Action>(action);

//         if (!cast)
//             logArgsError("action cast error");

//         action_ = cast;
//     }

//     ~ActionWrapper() { logArgsInfo(Q_FUNC_INFO); }

//     std::shared_ptr<Action> getAction() { return action_; }
//     std::shared_ptr<Dialog> getDialog() { return dialog_; }

//     void set()
//     {
//         auto cast = std::dynamic_pointer_cast<ActionWrapperBase>(this->shared_from_this());

//         if (!cast)
//             logArgsError("action cast error");

//         action_->setWrapper(cast);

//         dialog_->setWrapper(cast);
//         dialog_->set();
//         dialog_->show();
//     }

//     std::shared_ptr<CompanionData> getCompanionData() override
//     {
//         return dialog_->getCompanionData();
//     }

//     void showDialog();

//     void act() override
//     {
//         action_->act();
//         getManager()->performCompanionAction(this->shared_from_this());
//     }

//     void failed() override { action_->failed(); }

//     void method1() override
//     {
//         action_->method1();
//         dialog_->method1();
//     }

// private:
//     std::shared_ptr<Action> action_;
//     std::shared_ptr<Dialog> dialog_;
// };

// #endif // ACTION_WRAPPER_HPP
