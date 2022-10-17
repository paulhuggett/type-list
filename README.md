# type-list

A compile-time type list for C++17 and later.

## Templates

Name | Description
---- | -----------
`make<...T>` | Constructs a type list whose members are the template parameter pack T.
`size<TypeList>` | Returns the number of elements in the container
`contains<TypeList,Element>` | Checks if there is an element of type Element in the list.
`equal<TypeList1,TypeList2>` | Checks whether TypeList1 contains the same elements as TypeList2.
`transform<TypeList,UnaryOperation>` | Applies an operation to each of the members of a type list and yields a new list containing the the transformed members.
`foldl<TypeList,BinaryOperation,Initial>` | If the list if empty, the result is the initial value; else we recurse, making the new initial value the result of combining the old initial value with the first element.

