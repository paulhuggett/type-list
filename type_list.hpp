/// \file type_list.hpp
/// \brief Implements type_list: a compile-time collection of types.
//
// Copyright 2022 Paul Bowen-Huggett
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef TYPE_LIST_HPP
#define TYPE_LIST_HPP

#include <type_traits>

#if __cplusplus >= 202002L
#define TYPE_LIST_CXX20REQUIRES(x) requires x
#else
#define TYPE_LIST_CXX20REQUIRES(x)
#endif  // __cplusplus >= 202002L

namespace type_list {

template <typename... Types>
struct type_list;
template <>
struct type_list<> {};

#if __cplusplus >= 202002L
// concepts
// ~~~~~~~~
/// An element in a type list must contain member types names 'first' and
/// 'rest'. The end of the list is given by the type_list<> specialization.
template <typename T>
concept is_type_list = requires {
  typename T::first;
  typename T::rest;
}
|| std::is_same_v<T, type_list<>>;

template <typename UnaryOperation, typename TypeList>
concept is_unary_operation = requires (UnaryOperation&&) {
  typename UnaryOperation::template type<typename TypeList::first>;
}
|| std::is_same_v<TypeList, type_list<>>;

template <typename BinaryOperation, typename TypeList, typename InitialElement>
concept is_binary_operation = requires (BinaryOperation&&, InitialElement&&) {
  typename BinaryOperation::template type<InitialElement,
                                          typename TypeList::first>;
}
|| std::is_same_v<TypeList, type_list<>>;
#endif  // __cplusplus >= 202002L

// type list
// ~~~~~~~~~
/// An instance of type_list represents an element in a list. It is somewhat
/// like a cons cell in Lisp: it has two slots, and each slot holds a type.
///
/// A list is formed when a series of type_list instances are chained together,
/// so that each cell refers to the next one. There is one type_list instance
/// for each list member. The 'first' member holds a member type and the 'rest'
/// field is used to chain together type_list instances. The end of the list is
/// represented by a type_list specialization which takes no arguments and
/// contains no members.
template <typename First, typename Rest>
struct type_list<First, Rest> {
  using first = First;
  using rest = Rest;
};

// make
// ~~~~
/// Constructs a type_list from a template parameter pack.
template <typename... Types>
struct make;
template <>
struct make<> {
  using type = type_list<>;
};
template <typename T, typename... Ts>
struct make<T, Ts...> {
  using type = type_list<T, typename make<Ts...>::type>;
};
template <typename... Types>
using make_t = typename make<Types...>::type;

// size
// ~~~~
/// Yields the number of elements in the list.
template <typename TypeList>
TYPE_LIST_CXX20REQUIRES (is_type_list<TypeList>)
struct size
    : std::integral_constant<size_t,
                             1U + size<typename TypeList::rest>::value> {};
template <>
struct size<type_list<>> : std::integral_constant<size_t, 0U> {};

template <typename TypeList>
inline constexpr size_t size_v = size<TypeList>::value;

// contains
// ~~~~~~~~
/// Yields true if the type list contains a type matching Element and false
/// otherwise.
template <typename TypeList, typename Element>
TYPE_LIST_CXX20REQUIRES (is_type_list<TypeList>)
struct contains
    : std::bool_constant<std::is_same_v<Element, typename TypeList::first> ||
                         contains<typename TypeList::rest, Element>::value> {};
template <typename Element>
struct contains<type_list<>, Element> : std::bool_constant<false> {};

template <typename TypeList, typename Element>
inline constexpr bool contains_v = contains<TypeList, Element>::value;

// equal
// ~~~~~
template <typename TypeList1, typename TypeList2>
TYPE_LIST_CXX20REQUIRES (is_type_list<TypeList1>&& is_type_list<TypeList2>)
struct equal
    : std::bool_constant<
          std::is_same_v<typename TypeList1::first,
                         typename TypeList2::first> &&
          equal<typename TypeList1::rest, typename TypeList2::rest>::value> {};
template <typename TypeList1>
struct equal<TypeList1, type_list<>> : std::false_type {};
template <typename TypeList2>
struct equal<type_list<>, TypeList2> : std::false_type {};
template <>
struct equal<type_list<>, type_list<>> : std::true_type {};

template <typename TypeList1, typename TypeList2>
inline constexpr bool equal_v = equal<TypeList1, TypeList2>::value;

// transform
// ~~~~~~~~~
/// Applies UnaryOperation to each of the members of a type list and yields a
/// new list containing the the transformed members.
template <typename TypeList, typename UnaryOperation>
TYPE_LIST_CXX20REQUIRES ((is_type_list<TypeList> &&
                          is_unary_operation<UnaryOperation, TypeList>))
struct transform {
  using type = type_list<
      typename UnaryOperation::template type<typename TypeList::first>::type,
      typename transform<typename TypeList::rest, UnaryOperation>::type>;
};
template <typename Operation>
struct transform<type_list<>, Operation> {
  using type = type_list<>;
};
template <typename TypeList, typename Operation>
using transform_t = typename transform<TypeList, Operation>::type;

// fold left
// ~~~~~~~~~
/// If the list if empty, the result is the initial value; else we recurse,
/// making the new initial value the result of combining the old initial value
/// with the first element.
template <typename TypeList, typename BinaryOperation, typename Initial>
TYPE_LIST_CXX20REQUIRES (
    (is_type_list<TypeList> &&
     is_binary_operation<BinaryOperation, TypeList, Initial>))
struct foldl {
  using type = typename foldl<
      typename TypeList::rest, BinaryOperation,
      typename BinaryOperation::template type<typename TypeList::first,
                                              typename Initial::type>>::type;
};
template <typename BinaryOperation, typename InitialValue>
struct foldl<type_list<>, BinaryOperation, InitialValue> {
  using type = InitialValue;
};

}  // end namespace type_list

#endif  // TYPE_LIST_HPP
