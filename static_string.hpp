#ifndef STATIC_STRING_H
#define STATIC_STRING_H

#include <array>
#include <string>
#include <string_view>
#include <algorithm>

namespace ss {

using size_type = size_t;

// Static string forward declaration
template<typename CharType, size_type Length>
class static_string;

// c string to std::array
template<typename CharType, size_type... i>
inline constexpr auto to_array(const CharType* str, std::integer_sequence<size_type, i...>)
{
    return std::array{ (str[i])... };
}

template<typename CharType, size_type N>
inline constexpr auto to_array(const CharType (&str)[N])
{
    return to_array(str, std::make_integer_sequence<size_type, N - 1> {});
}

// Concat arrays
template<typename CharType, size_type N1, size_type N2>
constexpr auto concat(std::array<CharType, N1> lhs, std::array<CharType, N2> rhs)
{
    std::array<CharType, N1 + N2> result {};
    size_type index = 0;

    for (auto& el : lhs) {
        result[index] = std::move(el);
        ++index;
    }
    for (auto& el : rhs) {
        result[index] = std::move(el);
        ++index;
    }

    return result;
}

// Concat two static strings
template<class StringL, class StringR>
constexpr auto concat(StringL const& lhs, StringR const& rhs)
{
    return static_string(concat(lhs.buffer(), rhs.buffer()));
}

// Operator+ (lvalue, lvalue)
template<class StringL, class StringR>
constexpr auto operator+(StringL const& lhs, StringR const& rhs)
{
    return concat(lhs, rhs);
}

// Operator+ (rvalue, rvalue)
template<class StringL, class StringR>
constexpr auto operator+(StringL&& lhs, StringR&& rhs)
{
    return concat(lhs, rhs);
}

// Operator+ (lvalue, rvalue)
template<class StringL, class StringR>
constexpr auto operator+(StringL const& lhs, StringR&& rhs)
{
    return concat(lhs, rhs);
}

// Operator+ (rvalue, lvalue)
template<class StringL, class StringR>
constexpr auto operator+(StringL&& lhs, StringR const& rhs)
{
    return concat(lhs, rhs);
}

// Static string
template<typename CharType, size_type Length>
class static_string
{
public:

    template<typename OtherCharType, size_type OtherLength>
    friend class static_string;

    using char_type = CharType;
    using buffer_type = std::array<CharType, Length>;

    static constexpr size_type npos = static_cast<size_type>(-1);
    static constexpr size_type length = std::size(buffer_type{});

    constexpr static_string() = default;

    constexpr /* explicit */ static_string(char_type const (&str)[Length+1])
        : buffer_(ss::to_array(str))
    {
    }

    constexpr explicit static_string(buffer_type&& data)
        : buffer_(std::forward<buffer_type>(data))
    {
    }

    constexpr static_string(static_string const&) = default;

    constexpr static_string(static_string&&) = default;

    static_string& operator=(static_string const&) = delete;

    static_string& operator=(static_string&&) = delete;


    constexpr auto size() const { return buffer_.size(); }

    constexpr auto const& buffer() const { return buffer_; }

    constexpr auto const* data() const { return buffer_.data(); }

    template<size_type idx>
    constexpr auto const& get() const
    {
        static_assert(idx < length);

        return buffer_.at(idx);
    }

    constexpr auto view() const
    {
        return std::string_view(buffer_.data(), buffer_.size());
    }

    template<char_type c>
    constexpr size_type find() const
    {
        for (size_type p = 0; p < size(); ++p) {
            if (buffer_[p] == c)
                return p;
        }
        return npos;
    }

    template<size_type N>
    constexpr size_type find(std::array<char_type, N> const& substr) const
    {
        static_assert(N > 0);

        if (substr.size() > length)
            return npos;

        for (size_type p = 0; p + substr.size() <= size(); ++p) {

            if (buffer_[p] == substr[0]) {
                // have position of the first character in substr
                bool match = true;
                for (size_type i = 0; /*p2 < size() &&*/ i < substr.size(); i++) {
                    if (buffer_[p + i] != substr[i]) {
                        match = false;
                        break;
                    }
                }
                // match
                if (match)
                    return p;
            }
        }

        return npos;
    }

    template<typename SubstringType>
    constexpr size_type find(SubstringType const& substr) const
    {
        return find(substr.buffer());
    }

    template<size_type N>
    constexpr size_type find(char_type const (&str)[N]) const
    {
        return find(to_array(str));
    }

    template<size_type pos, size_type N>
    constexpr auto substr() const
    {
        static_assert(pos < length);
        static_assert(pos + N <= length);

        auto arr = to_array(&buffer_[pos], std::make_integer_sequence<size_t, N>{});
        return static_string<char_type, N>(std::forward<decltype(arr)>(arr));
    }

    template<typename SubstringType>
    constexpr bool operator==(SubstringType const& substr) const
    {
        return size() == substr.size() && find(substr) == 0;
    }

    template<size_type N>
    constexpr bool operator==(char_type const (&substr)[N]) const
    {
        auto arr = to_array(substr);
        return operator==(static_string<char_type, arr.size()>(std::forward<decltype(arr)>(arr)));
    }

    template<typename SubstringType>
    constexpr bool operator!=(SubstringType const& substr) const
    {
        return !operator==(substr);
    }

    template<size_type N>
    constexpr bool operator!=(char_type const (&substr)[N]) const
    {
        return !operator==(substr);
    }

    template<typename OtherStringType>
    constexpr auto append(OtherStringType const& oth) const
    {
        return ss::concat(*this, oth);
    }

    template<size_type N>
    constexpr auto append(char_type const (&str)[N]) const
    {
        auto arr = to_array(str);
        return append(static_string<char_type, arr.size()>(std::forward<decltype(arr)>(arr)));
    }

    template<typename OtherStringType>
    constexpr auto prepend(OtherStringType const& oth) const
    {
        return ss::concat(oth, *this);
    }

    template<size_type N>
    constexpr auto prepend(char_type const (&str)[N]) const
    {
        auto arr = to_array(str);
        return prepend(static_string<char_type, arr.size()>(std::forward<decltype(arr)>(arr)));
    }

    template<size_type idx, typename OtherStringType>
    constexpr auto insert(OtherStringType const& oth) const
    {
        static_assert(idx <= length);

        if constexpr (idx == 0) {
            return oth.template append(*this);
        }
        else if constexpr (idx == length) {
            return *this + oth;
        }
        else {
            return substr<0, idx>() + oth + substr<idx, length - idx>();
        }
    }

    template<size_type idx, size_type N>
    constexpr auto insert(char_type const (&str)[N]) const
    {
        auto arr = to_array(str);
        return insert<idx>(static_string<char_type, arr.size()>(std::forward<decltype(arr)>(arr)));
    }

    template<size_type idx, typename OtherStringType>
    constexpr auto replace(OtherStringType const& oth) const
    {
        static_assert(idx <= length);

        constexpr auto oth_length = OtherStringType::length;

        if constexpr (idx == 0u) {
            return oth + substr<oth_length, length - oth_length>();
        }
        else if constexpr (idx + oth_length == length) {
            return substr<0u, idx>() + oth;
        }
        else {
            return substr<0u, idx>() + oth + substr<idx + oth_length, length - oth_length - idx>();
        }
    }

    template<size_type idx, size_type N>
    constexpr auto replace(char_type const (&str)[N]) const
    {
        auto arr = to_array(str);
        return replace<idx>(static_string<char_type, arr.size()>(std::forward<decltype(arr)>(arr)));
    }

    template<size_type idx, size_type N>
    constexpr auto erase() const
    {
        static_assert(N > 0);
        static_assert(idx + N <= length);

        if constexpr (idx == 0) {
            return substr<N, length - N>();
        }
        else if constexpr (idx + N == length) {
            return substr<0, idx>();
        }
        else {
            return substr<0, idx>() + substr<idx + 1, length - idx - 1>();;
        }
    }

    // Printable
    template<typename Stream>
    friend Stream& operator<<(Stream& os, static_string const& s)
    {
        os << s.view();
        return os;
    }

private:
    buffer_type buffer_;
};

// Deduction guide
template<typename CharType, size_type N, class ContainerType = std::array<CharType, N>>
static_string(CharType const (&)[N]) -> static_string<CharType, N-1>;

// Hash
template<typename CharType, size_type N>
constexpr size_t hash(std::array<CharType, N> el)
{
    // TODO: replace, this is a toy hash
    size_t res = 5381;
    for (size_t i = 0; i < N; ++i) {
        res = res * 33 ^ static_cast<size_t>(el[i]);
    }
    //size_t res = static_cast<size_t>(0xc70f6907UL);
    //for (size_t i = 0; i < N; ++i) {
    //    res += (res * 31 + static_cast<size_t>(el[i]));
    //}
    return res;
}

// Hash
template<typename StringType>
constexpr size_t hash(StringType const& str)
{
    return hash(str.buffer());
}

} // namespace ss

#endif