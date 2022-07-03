namespace ss {

// Hash
template<typename CharType, size_type N>
static constexpr size_t to_hash(std::array<CharType, N> el)
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

// c string to std::array
template<typename CharType, size_type... i>
static constexpr auto to_array(const CharType* str, std::integer_sequence<size_type, i...>)
{
    return std::array{ (str[i])... };
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
template<class TypeL, class TypeR>
constexpr auto concat(static_string<TypeL> const& lhs, static_string<TypeR> const& rhs)
{
    return static_string(concat(lhs.buffer(), rhs.buffer()));
}

// Operator+ (lvalue, lvalue)
template<class TypeL, class TypeR>
constexpr auto operator+(static_string<TypeL> const& lhs, static_string<TypeR> const& rhs)
{
    return static_string(concat(lhs.buffer(), rhs.buffer()));
}

// Operator+ (rvalue, rvalue)
template<class TypeL, class TypeR>
constexpr auto operator+(static_string<TypeL>&& lhs, static_string<TypeR>&& rhs)
{
    return static_string(concat(lhs.buffer(), rhs.buffer()));
}

// Operator+ (lvalue, rvalue)
template<class TypeL, class TypeR>
constexpr auto operator+(static_string<TypeL> const& lhs, static_string<TypeR>&& rhs)
{
    return static_string(concat(lhs.buffer(), rhs.buffer()));
}

// Operator+ (rvalue, lvalue)
template<class TypeL, class TypeR>
constexpr auto operator+(static_string<TypeL>&& lhs, static_string<TypeR> const& rhs)
{
    return static_string(concat(lhs.buffer(), rhs.buffer()));
}

// Append
template<typename BufferType>
template<typename OtherBufferType>
constexpr auto static_string<BufferType>::append(static_string<OtherBufferType> const& oth) const
{
    return ss::concat(*this, oth);
}

// Prepend
template<typename BufferType>
template<typename OtherBufferType>
constexpr auto static_string<BufferType>::prepend(static_string<OtherBufferType> const& oth) const
{
    return ss::concat(oth, *this);
}

// Insert
template<typename BufferType>
template<size_type idx, typename OtherBufferType>
constexpr auto static_string<BufferType>::insert(static_string<OtherBufferType> const& oth) const
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

// Replace
template<typename BufferType>
template<size_type idx, typename OtherBufferType>
constexpr auto static_string<BufferType>::replace(static_string<OtherBufferType> const& oth) const
{
    static_assert(idx <= length);

    if constexpr (idx == 0) {
        return oth + substr<oth.length, length-oth.length>();
    }
    else if constexpr (idx+oth.length == length) {
        return substr<0, idx>() + oth;
    }
    else {
        return substr<0, idx>() + oth + substr<idx + oth.length, length - oth.length - idx>();
    }
}

// Erase
template<typename BufferType>
template<size_type idx, size_type N>
constexpr auto static_string<BufferType>::erase() const
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

// Printable to ostream
template<typename StringType, std::enable_if_t< std::is_base_of_v<ss::static_string_parameter, StringType>, bool > = false>
std::ostream& operator<<(std::ostream& os, StringType const& s)
{
    os << s.view();
    return os;
}

} // namespace ss
