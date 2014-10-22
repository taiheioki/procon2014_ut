#ifndef UTIL_STRING_HPP
#define UTIL_STRING_HPP

namespace util{

inline constexpr int toupper(int c){
    return c >= 'a' && c <='z' ? c-('z'-'Z') : c;
}

inline int strcmp(const char* lhs, const char *rhs){
    const char* p1 = lhs;
    const char* p2 = rhs;
    while(*p1 != '\0' && *p2 != '\0' && *p1 == *p2) {
        p1++;
        p2++;
    }
    return *p1 == '\0' && *p2 == '\0' ? 0 :
           *p1 == '\0' ? -1 :
           *p2 == '\0' ?  1 : *p1 - *p2;
}

inline int strcmp_insensitive(const char* lhs, const char *rhs){
    const char* p1 = lhs;
    const char* p2 = rhs;
    while(*p1 != '\0' && *p2 != '\0' && toupper(*p1) == toupper(*p2)) {
        p1++;
        p2++;
    }
    return *p1 == '\0' && *p2 == '\0' ? 0 :
           *p1 == '\0' ? -1 :
           *p2 == '\0' ?  1 : *p1 - *p2;
}

}
#endif // STRING_HPP
