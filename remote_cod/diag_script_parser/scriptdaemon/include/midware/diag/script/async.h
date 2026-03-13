//
//
#ifndef MIDWARE_DIAG_SCRIPT_ASYNC_H
#define MIDWARE_DIAG_SCRIPT_ASYNC_H

#include <cassert>
#include <set>
#include <memory>
#include <stdexcept>
#include <functional>

#include <midware/diag/script/common.h>
#include <midware/diag/script/log.h>
#include <midware/diag/script/scopeguard.h>

namespace midware::diag::script {

// 单线程版本的 Promise/Future 实现，使用上需要配合 Poller 实现异步执行，不要在多线程环境中使用；
// Future::Then 只支持单次通知，不支持多联级。

class AsyncCancelled: public std::runtime_error {
public:
    AsyncCancelled()
        : std::runtime_error("Async operation is canceled") {
    }
};

class AsyncCompletion {
public:
    AsyncCompletion() = default;
    virtual ~AsyncCompletion() = default;

public:
    virtual void OnCompleted() = 0;
};

class AsyncCommon {
public:
    enum class State {
        kPending = 0,
        kReady,
        kExcepted,
        kCancelling,
        kCancelled
    };

public:
    AsyncCommon(State state)
        : state_(state) {
    }

public:
    State GetState() const {
        return state_;
    }

    bool IsPending() const {
        return (state_ == State::kPending);
    }

    bool IsReady() const {
        return (state_ == State::kReady);
    }

    bool IsExcepted() const {
        return (state_ == State::kExcepted);
    }

    bool IsCancelling() const {
        return (state_ == State::kCancelling);
    }

    bool IsCancelled() const {
        return (state_ == State::kCancelled);
    }

    bool IsCompleted() const {
        return (state_ == State::kReady || state_ == State::kExcepted || state_ == State::kCancelled);
    }

    void Cancel() {
        if (state_ == State::kPending) {
            SetState(State::kCancelling);
        }
    }

    void InterruptPoint() {
        if (state_ == State::kCancelling) {
            SetState(State::kCancelled);
            throw AsyncCancelled();
        }
    }

    void SetException(std::exception_ptr e) {
        if (state_ == State::kPending) {
            exception_ = e;
            SetState(State::kExcepted);
        }
    }

    void RegisterCompleteCallback(AsyncCompletion* completion) {
        assert(completion != nullptr);
        completions_.emplace(completion);
        CheckCompletion();
    }

    void UnregisgerCompleteCallback(AsyncCompletion* completion) {
        assert(completion != nullptr);
        auto it = completions_.find(completion);
        if (it != completions_.end()) {
            completions_.erase(it);
        }
    }

protected:
    void SetState(State state) noexcept {
        if (state_ != state) {
            state_ = state;
            CheckCompletion();
        }
    }

    void CheckCompletion() {
        if (IsCompleted()) {
            auto completions = std::move(completions_);
            for (auto& completion: completions) {
                assert(completion != nullptr);
                try {
                    completion->OnCompleted();
                } catch (...) {
                }
            }
        }
    }

    void ThrowException() const {
        assert(state_ == State::kExcepted && exception_);
        std::rethrow_exception(exception_);
    }

private:
    State state_;
    std::exception_ptr exception_;
    std::set<AsyncCompletion*> completions_;
};

template <typename T>
class AsyncData final: public AsyncCommon {
public:
    AsyncData()
        : AsyncCommon(State::kPending) {
    }

    AsyncData(const T& result)
        : AsyncCommon(State::kReady)
        , value_(result) {
    }

    AsyncData(T&& result)
        : AsyncCommon(State::kReady)
        , value_(std::move(result)) {
    }

    AsyncData(const AsyncData&) = delete;
    AsyncData(AsyncData&&) = delete;

public:
    AsyncData& operator=(const AsyncData&) = delete;
    AsyncData& operator=(AsyncData&&) = delete;

    T Get() const {
        if (IsReady()) {
            return std::move(value_);
        }
        if (IsExcepted()) {
            ThrowException();
        }
        if (IsCancelled()) {
            throw AsyncCancelled();
        }
        throw std::runtime_error("Async result is not ready");
    }

    void Set(const T& result) {
        if (IsCancelling()) {
            SetState(State::kCancelled);
        }
        if (IsPending()) {
            value_ = result;
            SetState(State::kReady);
        }
    }

    void Set(T&& result) {
        if (IsCancelling()) {
            SetState(State::kCancelled);
        }
        if (IsPending()) {
            value_ = std::move(result);
            SetState(State::kReady);
        }
    }

private:
    T value_;
};

template <>
class AsyncData<void> final: public AsyncCommon {
public:
    AsyncData()
        : AsyncCommon(State::kPending) {
    }

    AsyncData(const AsyncData&) = delete;
    AsyncData(AsyncData&&) = delete;

public:
    AsyncData& operator=(const AsyncData&) = delete;
    AsyncData& operator=(AsyncData&&) = delete;

    void Get() const {
        if (IsReady()) {
            return;
        }
        if (IsExcepted()) {
            ThrowException();
        }
        if (IsCancelled()) {
            throw AsyncCancelled();
        }
        throw std::runtime_error("Async result is not ready");
    }

    void Set() {
        if (IsCancelling()) {
            SetState(State::kCancelled);
        }
        if (IsPending()) {
            SetState(State::kReady);
        }
    }
};

template <typename T>
class Future final: private AsyncCompletion {
public:
    using Continuation = std::function<void (Future<T>& future)>;

public:
    Future() = default;
    Future(const Future&) = delete;

    Future(Future&& other)
        : data_(std::move(other.data_))
        , continuation_(std::move(other.continuation_)) {
        if (data_ && continuation_) {
            data_->RegisterCompleteCallback(this);
            data_->UnregisgerCompleteCallback(&other);
        }
    }

    Future(std::shared_ptr<AsyncData<T>> data)
        : data_(data) {
    }

    ~Future() {
        if (data_) {
            data_->UnregisgerCompleteCallback(this);
        }
    }

public:
    Future& operator=(const Future&) = delete;

    Future& operator=(Future&& other) {
        if (data_ && continuation_) {
            data_->UnregisgerCompleteCallback(this);
        }
        data_ = std::move(other.data_);
        continuation_ = std::move(other.continuation_);
        if (data_ && continuation_) {
            data_->RegisterCompleteCallback(this);
            data_->UnregisgerCompleteCallback(&other);
        }
        return *this;
    }

    bool IsValid() const noexcept {
        return bool(data_);
    }

    operator bool() const noexcept {
        return IsValid();
    }

    bool IsPending() const noexcept {
        return (data_ && data_->IsPending());
    }

    bool IsReady() const noexcept {
        return (data_ && data_->IsReady());
    }

    T Get() {
        assert(data_);
        auto data = std::move(data_);
        if (continuation_) {
            continuation_ = nullptr;
            data->UnregisgerCompleteCallback(this);
        }
        return data->Get();
    }

    void Cancel() {
        assert(data_);
        data_->Cancel();
    }

    Future<T> Then(Continuation&& continuation) && {
        assert(data_);
        assert(continuation);
        continuation_ = std::move(continuation);
        if (continuation_) {
            data_->RegisterCompleteCallback(this);
        } else {
            data_->UnregisgerCompleteCallback(this);
        }
        return std::move(*this);
    }

private:
    void OnCompleted() override {
        if (continuation_) {
            continuation_(*this);
            continuation_ = nullptr;
        }
    }

private:
    std::shared_ptr<AsyncData<T>> data_;
    Continuation continuation_;
};

template <>
class Future<void> final: private AsyncCompletion {
public:
    using Continuation = std::function<void (Future<void>& future)>;

public:
    Future() = default;
    Future(const Future&) = delete;

    Future(Future&& other)
        : data_(std::move(other.data_))
        , continuation_(std::move(other.continuation_)) {
        if (data_ && continuation_) {
            data_->RegisterCompleteCallback(this);
            data_->UnregisgerCompleteCallback(&other);
        }
    }

    Future(std::shared_ptr<AsyncData<void>> data)
        : data_(data) {
    }

    ~Future() {
        if (data_) {
            data_->UnregisgerCompleteCallback(this);
        }
    }

public:
    Future& operator=(const Future&) = delete;

    Future& operator=(Future&& other) {
        if (data_ && continuation_) {
            data_->UnregisgerCompleteCallback(this);
        }
        data_ = std::move(other.data_);
        continuation_ = std::move(other.continuation_);
        if (data_ && continuation_) {
            data_->RegisterCompleteCallback(this);
            data_->UnregisgerCompleteCallback(&other);
        }
        return *this;
    }

    bool IsValid() const noexcept {
        return bool(data_);
    }

    operator bool() const noexcept {
        return IsValid();
    }

    bool IsPending() const noexcept {
        return (data_ && data_->IsPending());
    }

    bool IsReady() const noexcept {
        return (data_ && data_->IsReady());
    }

    void Get() {
        assert(data_);
        auto data = std::move(data_);
        if (continuation_) {
            continuation_ = nullptr;
            data->UnregisgerCompleteCallback(this);
        }
        data->Get();
    }

    void Cancel() {
        assert(data_);
        data_->Cancel();
    }

    Future<void> Then(Continuation&& continuation) && {
        assert(data_);
        assert(continuation);
        continuation_ = std::move(continuation);
        if (continuation_) {
            data_->RegisterCompleteCallback(this);
        } else {
            data_->UnregisgerCompleteCallback(this);
        }
        return std::move(*this);
    }

private:
    void OnCompleted() override {
        if (continuation_) {
            continuation_(*this);
        }
    }

private:
    std::shared_ptr<AsyncData<void>> data_;
    Continuation continuation_;
};

template <typename T>
class Promise final {
public:
    Promise()
        : data_(std::make_shared<AsyncData<T>>()) {
    }

    Promise(const Promise&) = delete;
    Promise(Promise&&) = default;

    Promise(const T& result)
        : data_(std::make_shared<AsyncData<T>>(result)) {
    }

    Promise(T&& result)
        : data_(std::make_shared<AsyncData<T>>(std::move(result))) {
    }

public:
    Promise& operator=(const Promise&) = delete;
    Promise& operator=(Promise&&) = default;

    bool IsPending() const noexcept {
        return data_->IsPending();
    }

    bool IsCancelling() const noexcept {
        return data_->IsCancelling();
    }

    void InterruptPoint() {
        data_->InterruptPoint();
    }

    void Set(const T& result) {
        data_->Set(result);
    }

    void Set(T&& result) {
        data_->Set(std::move(result));
    }

    void Set(std::exception_ptr e) {
        data_->SetException(e);
    }

    Future<T> GetFuture() {
        assert(data_.unique());
        return Future<T>(data_);
    }

private:
    std::shared_ptr<AsyncData<T>> data_;
};

template <>
class Promise<void> final {
public:
    Promise()
        : data_(std::make_shared<AsyncData<void>>()) {
    }

    Promise(const Promise&) = delete;
    Promise(Promise&&) = default;

public:
    Promise& operator=(const Promise&) = delete;
    Promise& operator=(Promise&&) = default;

    bool IsPending() const noexcept {
        return data_->IsPending();
    }

    bool IsCancelling() const noexcept {
        return data_->IsCancelling();
    }

    void InterruptPoint() {
        data_->InterruptPoint();
    }

    void Set() {
        data_->Set();
    }

    void Set(std::exception_ptr e) {
        data_->SetException(e);
    }

    Future<void> GetFuture() {
        assert(data_.unique());
        return Future<void>(data_);
    }

private:
    std::shared_ptr<AsyncData<void>> data_;
};

} // namespace midware::diag::script

#endif // MIDWARE_DIAG_SCRIPT_ASYNC_H
