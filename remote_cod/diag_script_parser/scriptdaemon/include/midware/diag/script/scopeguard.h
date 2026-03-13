//
//
#ifndef __SCOPEGUARD_H__
#define __SCOPEGUARD_H__

#include <utility>

namespace midware {
namespace diag {
namespace script {

template <typename FN>
class ScopeGuard final {
public:
    explicit ScopeGuard(FN&& fn) noexcept
        : fn_(std::move(fn)) {
    }

    explicit ScopeGuard(const FN& fn) noexcept
        : fn_(fn) {
    }

    ScopeGuard(const ScopeGuard&) = delete;

    ScopeGuard(ScopeGuard &&other) noexcept
        : fn_(std::move(other.fn_))
        , dismissed_(other.dismissed_) {
    }

    ~ScopeGuard() noexcept {
        if (!dismissed_) {
            try{
            fn_();
            }catch(...){}
        }
    }

    ScopeGuard& operator=(const ScopeGuard&) = delete;

    ScopeGuard& operator=(ScopeGuard&& other) noexcept {
        if (this != &other) {
            fn_ = std::move(other.fn_);
            dismissed_ = other.dismissed_;
            other.dismissed_ = true;
        }
        return (*this);
    }

 public:
    void Dismiss() noexcept {
        dismissed_ = true;
    }

private:
    FN fn_;
    bool dismissed_ = false;
};

template <typename FN>
ScopeGuard<typename std::decay<FN>::type> MakeScopeGuard(FN&& fn) noexcept {
    return ScopeGuard<typename std::decay<FN>::type>(std::forward<FN>(fn));
}

} // namespace midware
} // namespace diag
} // namespace script

#endif  // __SCOPEGUARD_H__
