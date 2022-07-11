#ifndef STATIC_STRING_HPP
#define STATIC_STRING_HPP

#include <array>
#include <string>
#include <string_view>
#include <algorithm>

namespace ss {

using size_type = size_t;

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

template<typename CharType, size_type N>
inline constexpr size_t to_hash(std::array<CharType, N> el);

template<typename CharType, size_type N1, size_type N2>
inline constexpr auto concat(std::array<CharType, N1> lhs, std::array<CharType, N2> rhs);

struct static_string_parameter {};

template<class BufferType>
class static_string : private static_string_parameter
{
public:

    using char_type = char;
    using buffer_type = BufferType;

    static constexpr size_type npos = static_cast<size_type>(-1);
    static constexpr size_type length = std::size(buffer_type{});

    constexpr static_string() = default;

    template<size_type N>
    constexpr explicit static_string(char const (&str)[N])
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
    constexpr size_type find(const char (&str)[N]) const
    {
        return find(to_array(str));
    }

    template<size_type pos, size_type N>
    constexpr auto substr() const
    {
        static_assert(pos < length);
        static_assert(pos + N <= length);

        auto const arr = to_array(&buffer_[pos], std::make_integer_sequence<size_t, N>{});
        using substr_type = decltype(arr);
        return static_string<substr_type>(std::forward<substr_type>(arr));
    }

    template<typename SubstringType>
    constexpr bool operator==(SubstringType const& substr) const
    {
        return size() == substr.size() && find(substr) == 0;
    }

    template<size_type N>
    constexpr bool operator==(char_type const (&substr)[N]) const
    {
        auto const arr = to_array(substr);
        using substr_type = decltype(arr);
        return operator==(static_string<substr_type>(std::forward<substr_type>(arr)));
    }

    template<typename SubstringType>
    constexpr bool operator!=(SubstringType const& substr) const
    {
        return !operator==(substr);
    }

    template<size_type N>
    constexpr bool operator!=(char_type const (&substr)[N]) const
    {
        auto const arr = to_array(substr);
        using substr_type = decltype(arr);
        return operator!=(static_string<substr_type>(std::forward<substr_type>(arr)));
    }

    template<typename OtherBufferType>
    constexpr auto append(static_string<OtherBufferType> const& oth) const;

    template<size_type N>
    constexpr auto append(char_type const (&str)[N]) const;

    template<typename OtherBufferType>
    constexpr auto prepend(static_string<OtherBufferType> const& oth) const;

    template<size_type N>
    constexpr auto prepend(char_type const (&str)[N]) const;

    template<size_type idx, typename OtherBufferType>
    constexpr auto insert(static_string<OtherBufferType> const& oth) const;

    template<size_type idx, size_type N>
    constexpr auto insert(char_type const (&str)[N]) const;

    template<size_type idx, typename OtherBufferType>
    constexpr auto replace(static_string<OtherBufferType> const& oth) const;

    template<size_type idx, size_type N>
    constexpr auto replace(char_type const (&str)[N]) const;

    template<size_type idx, size_type N>
    constexpr auto erase() const;


    //    template<typename T1, typename T2>
    //    friend constexpr auto operator+(static_string<T1> ls, static_string<T2> rs);

    // non constexpr functions
//    template<size_type idx, char_type c>
//    void set()
//    {
//        buffer_[idx] = c;
//        hash_ = to_hash(buffer_);
//    }

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

// Deduction guide for c style const char array
template<size_type N>
static_string(char const (&)[N]) -> static_string<std::array<char, N-1>>;

} // namespace ss

#include "static_string.ipp"

#endif