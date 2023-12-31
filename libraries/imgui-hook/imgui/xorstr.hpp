#pragma once
#include <string>
#include <array>
#include <utility>
#include <cstdarg>

namespace xorstr_impl {

#ifdef _MSC_VER
#define XORSTR_INLINE __forceinline
#else
#define XORSTR_INLINE inline
#endif

constexpr auto time = __TIME__;
constexpr auto seed = static_cast<int>(time[7]) +
                      static_cast<int>(time[6]) * 10 +
                      static_cast<int>(time[4]) * 60 +
                      static_cast<int>(time[3]) * 600 +
                      static_cast<int>(time[1]) * 3600 +
                      static_cast<int>(time[0]) * 36000;

template <int N>
struct random_generator {
private:
  static constexpr unsigned a = 16807;
  static constexpr unsigned m = 2147483647;
  static constexpr unsigned s = random_generator<N - 1>::value;
  static constexpr unsigned lo = a * (s & 0xFFFF);
  static constexpr unsigned hi = a * (s >> 16);
  static constexpr unsigned lo2 = lo + ((hi & 0x7FFF) << 16);
  static constexpr unsigned hi2 = hi >> 15;
  static constexpr unsigned lo3 = lo2 + hi;

public:
  static constexpr unsigned max = m;
  static constexpr unsigned value = lo3 > m ? lo3 - m : lo3;
};

template <>
struct random_generator<0> {
  static constexpr unsigned value = seed;
};

template <int N, int M>
struct random_int {
  static constexpr auto value = random_generator<N + 1>::value % M;
};

template <int N>
struct random_char {
  static const char value = static_cast<char>(1 + random_int<N, 0x7F - 1>::value);
};

template <size_t N, int K>
struct string {
private:
  const char key_;
  std::array<char, N + 1> encrypted_;

  constexpr char enc(char c) const {
    return c ^ key_;
  }

  char dec(char c) const {
    return c ^ key_;
  }

public:
  template <size_t... Is>
  constexpr XORSTR_INLINE string(const char* str, std::index_sequence<Is...>) :
    key_(random_char<K>::value), encrypted_{ { enc(str[Is])... } } {}

  XORSTR_INLINE decltype(auto) decrypt() {
    for (size_t i = 0; i < N; ++i) {
      encrypted_[i] = dec(encrypted_[i]);
    }
    encrypted_[N] = '\0';
    return encrypted_.data();
  }
};

#undef XORSTR_INLINE

}

#define xorstr(s) (xorstr_impl::string<sizeof(s) - 1, \
  __COUNTER__>(s, std::make_index_sequence<sizeof(s) - 1>()).decrypt())
