/*
MIT License — Copyright (c) 2026 Cudev
See LICENSE file in the project root for full license text.
*/

#pragma once

#include <optional>
#include <string>
#include <type_traits>
#include <utility>

namespace Cudev {

/*
    @summary
    Describes the state of a result.
*/
enum class ResultStatus
{
    Success,
    Failure,
    Info,
    Warning
};

/*
    @summary
    Stores either a result value or an error.

    @param Value
    The stored value type.

    @param Error
    The stored error type.
*/
template <typename Value, typename Error = std::string>
class Result
{
public:
    /*
        @summary
        Creates a successful result.

        @param value
        The result value.

        @param message
        Optional status text.

        @returns
        A successful result containing value.
    */
    static Result success(Value value, std::string message = {})
    {
        return Result(ResultStatus::Success, std::move(value), std::nullopt, std::move(message));
    }

    /*
        @summary
        Creates a failed result.

        @param error
        The failure value.

        @param message
        Optional failure text.

        @returns
        A failed result containing error.
    */
    static Result failure(Error error, std::string message = {})
    {
        if constexpr (std::is_same_v<Error, std::string>)
        {
            if (message.empty())
            {
                message = error;
            }
        }

        return Result(ResultStatus::Failure, std::nullopt, std::move(error), std::move(message));
    }

    /*
        @summary
        Creates an informational result.

        @param value
        The result value.

        @param message
        Optional status text.

        @returns
        An informational result containing value.
    */
    static Result info(Value value, std::string message = {})
    {
        return Result(ResultStatus::Info, std::move(value), std::nullopt, std::move(message));
    }

    /*
        @summary
        Creates a warning result.

        @param value
        The result value.

        @param message
        Optional status text.

        @returns
        A warning result containing value.
    */
    static Result warning(Value value, std::string message = {})
    {
        return Result(ResultStatus::Warning, std::move(value), std::nullopt, std::move(message));
    }

    /*
        @summary
        Returns the result status.

        @returns
        The current result status.
    */
    constexpr ResultStatus status() const noexcept
    {
        return status_;
    }

    /*
        @summary
        Tests whether the result succeeded.

        @returns
        True when the result status is Success.
    */
    constexpr bool succeeded() const noexcept
    {
        return status_ == ResultStatus::Success;
    }

    /*
        @summary
        Tests whether the result failed.

        @returns
        True when the result status is Failure.
    */
    constexpr bool failed() const noexcept
    {
        return status_ == ResultStatus::Failure;
    }

    /*
        @summary
        Returns the stored value.

        @returns
        A reference to the stored value.
    */
    const Value& value() const &
    {
        return value_.value();
    }

    /*
        @summary
        Moves the stored value out of the result.

        @returns
        The stored value.
    */
    Value&& value() &&
    {
        return std::move(value_.value());
    }

    /*
        @summary
        Returns the status message.

        @returns
        The result message.
    */
    const std::string& message() const noexcept
    {
        return message_;
    }

    /*
        @summary
        Returns the stored error.

        @returns
        The error when the result contains one.
    */
    const std::optional<Error>& error() const noexcept
    {
        return error_;
    }

private:
    Result(ResultStatus status, std::optional<Value> value, std::optional<Error> error, std::string message)
        : status_(status), value_(std::move(value)), error_(std::move(error)), message_(std::move(message))
    {
    }

    ResultStatus status_;
    std::optional<Value> value_;
    std::optional<Error> error_;
    std::string message_;
};

template <>
class Result<void, std::string>
{
public:
    /*
        @summary
        Creates a successful result.

        @param message
        Optional status text.

        @returns
        A successful result.
    */
    static Result success(std::string message = {})
    {
        return Result(ResultStatus::Success, std::nullopt, std::move(message));
    }

    /*
        @summary
        Creates a failed result.

        @param error
        The failure text.

        @param message
        Optional status text.

        @returns
        A failed result.
    */
    static Result failure(std::string error, std::string message = {})
    {
        if (message.empty())
        {
            message = error;
        }

        return Result(ResultStatus::Failure, std::move(error), std::move(message));
    }

    /*
        @summary
        Creates an informational result.

        @param message
        The status text.

        @returns
        An informational result.
    */
    static Result info(std::string message)
    {
        return Result(ResultStatus::Info, std::nullopt, std::move(message));
    }

    /*
        @summary
        Creates a warning result.

        @param message
        The status text.

        @returns
        A warning result.
    */
    static Result warning(std::string message)
    {
        return Result(ResultStatus::Warning, std::nullopt, std::move(message));
    }

    /*
        @summary
        Returns the result status.

        @returns
        The current result status.
    */
    constexpr ResultStatus status() const noexcept
    {
        return status_;
    }

    /*
        @summary
        Tests whether the result succeeded.

        @returns
        True when the result status is Success.
    */
    constexpr bool succeeded() const noexcept
    {
        return status_ == ResultStatus::Success;
    }

    /*
        @summary
        Tests whether the result failed.

        @returns
        True when the result status is Failure.
    */
    constexpr bool failed() const noexcept
    {
        return status_ == ResultStatus::Failure;
    }

    /*
        @summary
        Returns the status message.

        @returns
        The result message.
    */
    const std::string& message() const noexcept
    {
        return message_;
    }

    /*
        @summary
        Returns the stored error text.

        @returns
        The error text when the result contains one.
    */
    const std::optional<std::string>& error() const noexcept
    {
        return error_;
    }

private:
    Result(ResultStatus status, std::optional<std::string> error, std::string message)
        : status_(status), error_(std::move(error)), message_(std::move(message))
    {
    }

    ResultStatus status_;
    std::optional<std::string> error_;
    std::string message_;
};

template <typename Error>
class Result<void, Error>
{
public:
    /*
        @summary
        Creates a successful result.

        @param message
        Optional status text.

        @returns
        A successful result.
    */
    static Result success(std::string message = {})
    {
        return Result(ResultStatus::Success, std::nullopt, std::move(message));
    }

    /*
        @summary
        Creates a failed result.

        @param error
        The failure value.

        @param message
        Optional status text.

        @returns
        A failed result.
    */
    static Result failure(Error error, std::string message = {})
    {
        return Result(ResultStatus::Failure, std::move(error), std::move(message));
    }

    /*
        @summary
        Creates an informational result.

        @param message
        The status text.

        @returns
        An informational result.
    */
    static Result info(std::string message)
    {
        return Result(ResultStatus::Info, std::nullopt, std::move(message));
    }

    /*
        @summary
        Creates a warning result.

        @param message
        The status text.

        @returns
        A warning result.
    */
    static Result warning(std::string message)
    {
        return Result(ResultStatus::Warning, std::nullopt, std::move(message));
    }

    /*
        @summary
        Returns the result status.

        @returns
        The current result status.
    */
    constexpr ResultStatus status() const noexcept
    {
        return status_;
    }

    /*
        @summary
        Tests whether the result succeeded.

        @returns
        True when the result status is Success.
    */
    constexpr bool succeeded() const noexcept
    {
        return status_ == ResultStatus::Success;
    }

    /*
        @summary
        Tests whether the result failed.

        @returns
        True when the result status is Failure.
    */
    constexpr bool failed() const noexcept
    {
        return status_ == ResultStatus::Failure;
    }

    /*
        @summary
        Returns the status message.

        @returns
        The result message.
    */
    const std::string& message() const noexcept
    {
        return message_;
    }

    /*
        @summary
        Returns the stored error.

        @returns
        The error when the result contains one.
    */
    const std::optional<Error>& error() const noexcept
    {
        return error_;
    }

private:
    Result(ResultStatus status, std::optional<Error> error, std::string message)
        : status_(status), error_(std::move(error)), message_(std::move(message))
    {
    }

    ResultStatus status_;
    std::optional<Error> error_;
    std::string message_;
};

}
