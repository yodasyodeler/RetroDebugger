#pragma once

#include <variant>

namespace Rdb {

template<typename NumericType>
concept Numeric = std::is_arithmetic_v<NumericType>;

template<Numeric... NumericType>
class NumericValueBase {
public:
    using NumericVariant = std::variant<NumericType...>;

    NumericValueBase() = delete;

    constexpr NumericValueBase(auto value) :
        m_value{ value } {}

    auto operator<=>(const NumericValueBase<NumericType...>& value) const {
        return Get<double>() <=> value.Get<double>();
    }
    bool operator==(const NumericValueBase<NumericType...>& value) const {
        return (*this <=> value) == 0;
    }

    bool IsDouble() const {
        return std::holds_alternative<double>(m_value);
    }

    template<Numeric... NumericType>
    bool IsInt() const {
        return std::holds_alternative<int>(m_value);
    }

    template<Numeric... NumericType>
    NumericValueBase<NumericType...> operator+(const NumericValueBase<NumericType...>& value) const {
        if (IsDouble() || value.IsDouble()) {
            return { Get<double>() + value.Get<double>() };
        }
        else {
            return { Get<int>() + value.Get<int>() };
        }
    }

    template<Numeric... NumericType>
    NumericValueBase<NumericType...> operator-(const NumericValueBase<NumericType...>& value) const {
        if (IsDouble() || value.IsDouble()) {
            return { Get<double>() - value.Get<double>() };
        }
        else {
            return { Get<int>() - value.Get<int>() };
        }
    }

    template<Numeric... NumericType>
    NumericValueBase<NumericType...> operator*(const NumericValueBase<NumericType...>& value) const {
        if (IsDouble() || value.IsDouble()) {
            return { Get<double>() * value.Get<double>() };
        }
        else {
            return { Get<int>() * value.Get<int>() };
        }
    }

    template<Numeric... NumericType>
    NumericValueBase<NumericType...> operator/(const NumericValueBase<NumericType...>& value) const {
        if (IsDouble() || value.IsDouble()) {
            return { Get<double>() / value.Get<double>() };
        }
        else {
            return { Get<int>() / value.Get<int>() };
        }
    }

    template<Numeric... NumericType>
    NumericValueBase<NumericType...> operator|(const NumericValueBase<NumericType...>& value) const {
        if (IsDouble() || value.IsDouble()) {
            return { Get<double>() | value.Get<double>() };
        }
        else {
            return { Get<int>() | value.Get<int>() };
        }
    }

    template<Numeric... NumericType>
    NumericValueBase<NumericType...> operator^(const NumericValueBase<NumericType...>& value) const {
        if (IsDouble() || value.IsDouble()) {
            return { Get<double>() ^ value.Get<double>() };
        }
        else {
            return { Get<int>() ^ value.Get<int>() };
        }
    }

    template<Numeric... NumericType>
    NumericValueBase<NumericType...> operator&(const NumericValueBase<NumericType...>& value) const {
        if (IsDouble() || value.IsDouble()) {
            return { Get<double>() & value.Get<double>() };
        }
        else {
            return { Get<int>() & value.Get<int>() };
        }
    }

    template<Numeric... NumericType>
    NumericValueBase<NumericType...> operator<<(const NumericValueBase<NumericType...>& value) const {
        if (IsDouble() || value.IsDouble()) {
            return { Get<double>() << value.Get<double>() };
        }
        else {
            return { Get<int>() << value.Get<int>() };
        }
    }

    template<Numeric... NumericType>
    NumericValueBase<NumericType...> operator>>(const NumericValueBase<NumericType...>& value) const {
        if (IsDouble() || value.IsDouble()) {
            return { Get<double>() >> value.Get<double>() };
        }
        else {
            return { Get<int>() >> value.Get<int>() };
        }
    }
    template<Numeric numericType>
    constexpr numericType Get() const {
        if (IsInt()) {
            return static_cast<numericType>(std::get<int>(m_value));
        }
        else if (IsDouble()) {
            return static_cast<numericType>(std::get<double>(m_value));
        }

        return std::get<numericType>(m_value);
    }

private:
    NumericVariant m_value;
};

}