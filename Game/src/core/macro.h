#ifndef MACRO_H
#define MACRO_H

// TODO: Add support for Mac OS compilers?
#if defined(__clang__)
#define GENGINE_IS_CPP_CLANG
#elif defined(__GNUC__) || defined(__GNUG__)
#define GENGINE_IS_CPP_GCC
#elif defined(_MSC_VER)
#define GENGINE_IS_CPP_MSVC
#else
#define GENGINE_IS_CPP_OTHER_COMPILER
#endif

#if defined(_WIN32)
  #define GENGINE_IS_OS_WINDOWS
#elif defined(__linux__)
  #define GENGINE_IS_OS_LINUX
#elif defined(__APPLE__)
  #define GENGINE_IS_OS_MAC
#else
  #define GENGINE_IS_OS_OTHER
#endif

// TODO: Add support for Mac OS compilers?
#define DETAIL_STRINGIFY(a) #a
#if defined(GENGINE_IS_CPP_CLANG)
  #ifndef DISABLE_WARNING_BEGIN
    #define DISABLE_WARNING_BEGIN _Pragma("clang diagnostic push")
    #define DISABLE_WARNING(w) _Pragma(DETAIL_STRINGIFY(clang diagnostic ignored w))
    #define DISABLE_WARNING_END _Pragma("clang diagnostic pop")
  #endif // DISABLE_WARNING_BEGIN
#elif defined(GENGINE_IS_CPP_GCC)
  #ifndef DISABLE_WARNING_BEGIN
    #define DISABLE_WARNING_BEGIN _Pragma("GCC diagnostic push")
    #define DISABLE_WARNING(w) _Pragma(DETAIL_STRINGIFY(GCC diagnostic ignored w))
    #define DISABLE_WARNING_END _Pragma("GCC diagnostic pop")
  #endif // DISABLE_WARNING_BEGIN
#elif defined(GENGINE_IS_CPP_MSVC)
  #ifndef DISABLE_WARNING_BEGIN
    #define DISABLE_WARNING_BEGIN _Pragma("warning(push)")
    #define DISABLE_WARNING(w) _Pragma(DETAIL_STRINGIFY(warning(disable: w)))
    #define DISABLE_WARNING_END _Pragma("warning(pop)")
  #endif // DISABLE_WARNING_BEGIN
#endif

#endif // MACRO_H
