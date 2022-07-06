#include "static_string.hpp"
#include <iostream>
#include <sstream>
#include <cassert>

class Log
{
    std::ostringstream os_;
public:
    ~Log()
    {
        auto s = os_.str();
        if (s.empty() || s.back() != '\n')
            s += '\n';

        std::cout.flush();
        std::cout << s;
    }

    template <typename T>
    Log& operator<< (const T& val) {
        os_  << val;
        return *this;
    }
};

int main()
{
    using namespace ss;

    constexpr static_string s1("hello ");
    static_assert(s1.size() == 6);
    static_assert(s1.get<0>() == 'h');
    static_assert(s1.find<'e'>() == 1);
    static_assert(s1.find<'l'>() == 2);
    static_assert(s1.find<'x'>() == s1.npos);
    static_assert(s1.find(static_string("hello")) == 0);
    static_assert(s1.find("hello") == 0);
    static_assert(s1.find("hello") == 0);
    static_assert(s1.find("hell") == 0);
    static_assert(s1.find("ell") == 1);
    static_assert(s1.find("ello") == 1);
    static_assert(s1.find("ellow") == s1.npos);
    static_assert(s1.find("elloww") == s1.npos);
    static_assert(s1.find("ello w") == s1.npos);

    static_assert(s1 == static_string("hello "));
    static_assert(s1 == "hello ");
    static_assert(s1 != static_string("yello "));
    static_assert(s1 != "yello ");

    static_assert(s1.substr<0,6>() == "hello ");
    static_assert(s1.substr<0,2>() == "he");
    static_assert(s1.substr<1,3>() == "ell");
    static_assert(s1.substr<2,4>() == "llo ");
    //s1.substr<2,10>(); // should not compile

    //static_assert(s1.prepend("abc ") == static_string("abc hello "));
    static_assert(s1.prepend(static_string("abc ")) == "abc hello ");
    static_assert(s1.prepend("abc ") == "abc hello ");

    static_assert(s1.append(static_string("abc")) == "hello abc");
    static_assert(s1.append("abc") == "hello abc");

    static_assert(s1.insert<0>(static_string("abc ")) == "abc hello ");
    static_assert(s1.insert<0>("abc ") == "abc hello ");
    static_assert(s1.insert<6>(static_string("abc ")) == "hello abc ");
    static_assert(s1.insert<6>("abc ") == "hello abc ");
    static_assert(s1.insert<4>(static_string(" ")) == "hell o ");
    static_assert(s1.insert<4>(" ") == static_string("hell o "));

    static_assert(s1.replace<0>(static_string("yi")) == "yillo ");
    static_assert(s1.replace<0>("yi") == "yillo ");
    static_assert(s1.replace<1>(static_string("ig")) == "higlo ");
    static_assert(s1.replace<1>("ig") == "higlo ");
    static_assert(s1.replace<4>(static_string("uu")) == "helluu");
    static_assert(s1.replace<4>("uu") == "helluu");
    //s1.replace<5>(static_string("uu")); // should not compile

    static_assert(s1.erase<2, 1>() == "helo ");
    static_assert(s1.erase<0, 1>() == "ello ");
    static_assert(s1.erase<5, 1>() == "hello");
    static_assert(s1.erase<4, 2>() == "hell");
    // s1.erase<4, 3>() // should not compile

    static_assert(s1.replace<0>("m").replace<5>("w") == "mellow");

    constexpr static_string copy = s1;
    static_assert(s1 == static_string("hello "));
    static_assert(copy.find("hello ") == 0);
    Log() << s1;

    constexpr static_string s2("world");
    static_assert(s2.size() == 5);
    static_assert(s2.get<0>() == 'w');
    Log() << s2;

    constexpr auto buff_merged = concat(s1.buffer(), s2.buffer());
    static_assert(std::get<0>(buff_merged) == 'h');
    static_assert(std::get<6>(buff_merged) == 'w');

    constexpr auto merged = concat(s1, s2);
    static_assert(merged.size() == 6 + 5);
    static_assert(merged.get<0>() == 'h');
    static_assert(merged.get<6>() == 'w');
    Log() << merged;

    constexpr auto merged2 = s1.append(s2);
    static_assert(merged2.size() == 6 + 5);
    static_assert(merged2.get<0>() == 'h');
    static_assert(merged2.get<6>() == 'w');
    Log() << merged2;

    constexpr auto merged3 = s1 + s2 + static_string(",") + static_string(" ") + s1 + s2 ;
    static_assert(merged3.size() == 6 + 5 + 1 + 1 + 6 + 5);
    static_assert(merged3.get<0>() == 'h');
    static_assert(merged3.get<6>() == 'w');
    static_assert(merged3.get<11>() == ',');
    static_assert(merged3.get<13>() == 'h');
    static_assert(merged3.get<19>() == 'w');
    static_assert(merged3.find("hello world, hello world") == 0);
    static_assert(merged3 == "hello world, hello world");
    static_assert(merged3 != "yello world, hello world");
    static_assert(merged3.hash() == 12301343714320165257u);
    Log() << merged3;
    Log() << merged3.hash();
    // TODO: hash function same as std strings:
    //Log() << std::hash<std::string>{}(merged3.view().data());
    //assert(merged3.hash() == std::hash<std::string>{}(merged3.view().data()));

    auto copy3 = merged3; // non constexpr
    assert(copy3.find("hello world, hello world") == 0); // only runtime check possible

    auto constexpr s4 = static_string("aaabaaaXaaab");
    static_assert(s4.find("aaabaaaXaaab") == 0);
    static_assert(s4.find("a") == 0);
    static_assert(s4.find("b") == 3);
    static_assert(s4.find("X") == 7);
    static_assert(s4.find("aaab") == 0);
    static_assert(s4.find("aaaX") == 4);
    static_assert(s4.find("aaXa") == 5);
    static_assert(s4.find("Xaaab") == 7);
    static_assert(s4.find("aaaQa") == s4.npos);
    static_assert(s4.find("aaaQ") == s4.npos);

    return 0;
}