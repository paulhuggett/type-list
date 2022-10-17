#include <algorithm>
#include <iostream>

#include "type_list.hpp"

template <typename TypeList>
TYPE_LIST_CXX20REQUIRES (type_list::is_type_list<TypeList>)
struct type_characteristics {
private:
  struct type_size {
    template <typename T>
    using type = std::integral_constant<size_t, sizeof (T)>;
  };
  struct type_align {
    template <typename T>
    using type = std::integral_constant<size_t, alignof (T)>;
  };

  struct max_value {
    template <typename Integral1, typename Integral2>
    using type =
        std::integral_constant<typename Integral1::value_type,
                               std::max (Integral1::value, Integral2::value)>;
  };

  using sizes_list = type_list::transform_t<TypeList, type_size>;
  using alignments_type = type_list::transform_t<TypeList, type_align>;

public:
  using largest =
      typename type_list::foldl<sizes_list, max_value,
                                std::integral_constant<size_t, 0>>::type;
  using most_aligned =
      typename type_list::foldl<alignments_type, max_value,
                                std::integral_constant<size_t, 0>>::type;
};

void show_type_characteristics () {
  using types = type_list::make_t<char, long long, int, unsigned>;
  std::cout << "size=" << type_characteristics<types>::largest::value
            << ", align=" << type_characteristics<types>::most_aligned::value
            << '\n';
}

struct add_one {
  template <typename Integral>
  using type = std::integral_constant<typename Integral::value_type,
                                      Integral::value + 1>;
};

int main () {
  using one = std::integral_constant<unsigned, 1>;
  using two = std::integral_constant<unsigned, 2>;
  using three = std::integral_constant<unsigned, 3>;
  using four = std::integral_constant<unsigned, 4>;

  using numbers = type_list::make_t<one, two, three>;
  static_assert (type_list::size_v<numbers> == 3);

  static_assert (type_list::contains_v<numbers, one>, "The list must contain 'one'");
  static_assert (type_list::contains_v<numbers, two>, "The list must contain 'two'");
  static_assert (type_list::contains_v<numbers, three>, "The list must contain 'three'");
  static_assert (!type_list::contains_v<numbers, four>, "The list must not contain 'four'");
  static_assert (!type_list::contains_v<numbers, char>, "The list must not contain 'char  '");

  using plus_one = type_list::transform_t<numbers, add_one>;
  static_assert (!type_list::contains_v<plus_one, one>);
  static_assert (type_list::contains_v<plus_one, two>);
  static_assert (type_list::contains_v<plus_one, three>);
  static_assert (type_list::contains_v<plus_one, four>);

  static_assert (type_list::equal_v<numbers, numbers>);
  static_assert (
      type_list::equal_v<numbers, type_list::make_t<one, two, three>>);
  static_assert (!type_list::equal_v<numbers, type_list::make_t<one, two>>,
                 "lists should not be equal: the second has one fewer member");
  static_assert (
      !type_list::equal_v<numbers, type_list::make_t<one, two, three, four>>,
      "lists should not be equal: the second has one extra member");
  static_assert (!type_list::equal_v<numbers, plus_one>);

  std::cout << "length of types=" << type_list::size_v<numbers> << '\n';
  std::cout << numbers::first::value << ' ' << numbers::rest::first::value
            << ' ' << numbers::rest::rest::first::value << '\n';
  std::cout << "length of plusone=" << type_list::size_v<plus_one> << '\n';
  std::cout << plus_one::first::value << ' ' << plus_one::rest::first::value
            << ' ' << plus_one::rest::rest::first::value << '\n';

  show_type_characteristics ();
}
