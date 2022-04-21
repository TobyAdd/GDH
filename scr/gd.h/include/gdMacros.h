#ifndef __GDMACROS_H__
#define __GDMACROS_H__

/** __STR_CAT__(str)
* Concatenate 2 tokens. Don't use this.
*/
#define __STR_CAT___(str1, str2) str1##str2
#define __STR_CAT__(str1, str2) __STR_CAT___(str1, str2)

/** PAD
* Add padding to a class / struct. For shifting classes /
* structs to be aligned, if too lazy to fully reverse.
*
* Based on line number, to be standard C / C++ compatible.
*/
#ifndef PAD
#define PAD(size) char __STR_CAT__(__, __STR_CAT__(pad, __LINE__))[size] = {};
#endif

/** GDH_ADD
* This class/enum/type does not exist
* in GD itself, however it has been
* added for better code readability
* & maintainability.
*
* Variadic arguments used due to
* enums otherwise not expanding
* correctly.
*/
#define GDH_ADD(...) __VA_ARGS__

/** GDH_DLL
* This class/enum/type does not exist
* in GD itself, however it has been
* added for better code readability
* & maintainability.
*
* Variadic arguments used due to
* enums otherwise not expanding
* correctly.
*/
#ifdef GDH_IS_DLL
    #ifdef _EXPORTING
        #define GDH_DLL __declspec(dllexport)
    #else
        #define GDH_DLL __declspec(dllimport)
    #endif
#else
    #define GDH_DLL
#endif

/** __WARN_DEBUG__
* Set this macro to 0 to disable warnings when compiling in debug.
*/
#define __WARN_DEBUG__ true

#endif 