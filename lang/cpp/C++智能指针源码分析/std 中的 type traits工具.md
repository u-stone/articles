*分析的源码对应的是 `Xcode 12.0.1` 中 `llvm` 关于 `libc++` 的实现，对应 `clang` 版本 `Apple clang version 12.0.0 (clang-1200.0.32.2)`。`llvm` 源码[地址](https://github.com/llvm/llvm-project)，及 `llvm` [下载地址汇总](https://releases.llvm.org/download.html)。`libc++` 的[文档](https://releases.llvm.org/16.0.0/projects/libcxx/docs/index.html)*

[toc]

# @All

在文件 `/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1/type_traits` 的头部有下面这样的定义：

```C++
namespace std
{

    // helper class:
    template <class T, T v> struct integral_constant;
    typedef integral_constant<bool, true>  true_type;   // C++11
    typedef integral_constant<bool, false> false_type;  // C++11

    template <bool B>                                   // C++14
    using bool_constant = integral_constant<bool, B>;   // C++14
    typedef bool_constant<true> true_type;              // C++14
    typedef bool_constant<false> false_type;            // C++14

    // helper traits
    template <bool, class T = void> struct enable_if;
    template <bool, class T, class F> struct conditional;

    // Primary classification traits:
    template <class T> struct is_void;
    template <class T> struct is_null_pointer;  // C++14
    template <class T> struct is_integral;
    template <class T> struct is_floating_point;
    template <class T> struct is_array;
    template <class T> struct is_pointer;
    template <class T> struct is_lvalue_reference;
    template <class T> struct is_rvalue_reference;
    template <class T> struct is_member_object_pointer;
    template <class T> struct is_member_function_pointer;
    template <class T> struct is_enum;
    template <class T> struct is_union;
    template <class T> struct is_class;
    template <class T> struct is_function;

    // Secondary classification traits:
    template <class T> struct is_reference;
    template <class T> struct is_arithmetic;
    template <class T> struct is_fundamental;
    template <class T> struct is_member_pointer;
    template <class T> struct is_scalar;
    template <class T> struct is_object;
    template <class T> struct is_compound;

    // Const-volatile properties and transformations:
    template <class T> struct is_const;
    template <class T> struct is_volatile;
    template <class T> struct remove_const;
    template <class T> struct remove_volatile;
    template <class T> struct remove_cv;
    template <class T> struct add_const;
    template <class T> struct add_volatile;
    template <class T> struct add_cv;

    // Reference transformations:
    template <class T> struct remove_reference;
    template <class T> struct add_lvalue_reference;
    template <class T> struct add_rvalue_reference;

    // Pointer transformations:
    template <class T> struct remove_pointer;
    template <class T> struct add_pointer;

    template<class T> struct type_identity;                     // C++20
    template<class T>
      using type_identity_t = typename type_identity<T>::type;  // C++20

    // Integral properties:
    template <class T> struct is_signed;
    template <class T> struct is_unsigned;
    template <class T> struct make_signed;
    template <class T> struct make_unsigned;

    // Array properties and transformations:
    template <class T> struct rank;
    template <class T, unsigned I = 0> struct extent;
    template <class T> struct remove_extent;
    template <class T> struct remove_all_extents;

    template <class T> struct is_bounded_array;                 // C++20
    template <class T> struct is_unbounded_array;               // C++20

    // Member introspection:
    template <class T> struct is_pod;
    template <class T> struct is_trivial;
    template <class T> struct is_trivially_copyable;
    template <class T> struct is_standard_layout;
    template <class T> struct is_literal_type;
    template <class T> struct is_empty;
    template <class T> struct is_polymorphic;
    template <class T> struct is_abstract;
    template <class T> struct is_final; // C++14
    template <class T> struct is_aggregate; // C++17

    template <class T, class... Args> struct is_constructible;
    template <class T>                struct is_default_constructible;
    template <class T>                struct is_copy_constructible;
    template <class T>                struct is_move_constructible;
    template <class T, class U>       struct is_assignable;
    template <class T>                struct is_copy_assignable;
    template <class T>                struct is_move_assignable;
    template <class T, class U>       struct is_swappable_with;       // C++17
    template <class T>                struct is_swappable;            // C++17
    template <class T>                struct is_destructible;

    template <class T, class... Args> struct is_trivially_constructible;
    template <class T>                struct is_trivially_default_constructible;
    template <class T>                struct is_trivially_copy_constructible;
    template <class T>                struct is_trivially_move_constructible;
    template <class T, class U>       struct is_trivially_assignable;
    template <class T>                struct is_trivially_copy_assignable;
    template <class T>                struct is_trivially_move_assignable;
    template <class T>                struct is_trivially_destructible;

    template <class T, class... Args> struct is_nothrow_constructible;
    template <class T>                struct is_nothrow_default_constructible;
    template <class T>                struct is_nothrow_copy_constructible;
    template <class T>                struct is_nothrow_move_constructible;
    template <class T, class U>       struct is_nothrow_assignable;
    template <class T>                struct is_nothrow_copy_assignable;
    template <class T>                struct is_nothrow_move_assignable;
    template <class T, class U>       struct is_nothrow_swappable_with; // C++17
    template <class T>                struct is_nothrow_swappable;      // C++17
    template <class T>                struct is_nothrow_destructible;

    template <class T> struct has_virtual_destructor;

    template<class T> struct has_unique_object_representations;         // C++17

    // Relationships between types:
    template <class T, class U> struct is_same;
    template <class Base, class Derived> struct is_base_of;

    template <class From, class To> struct is_convertible;
    template <typename From, typename To> struct is_nothrow_convertible;                  // C++20
    template <typename From, typename To> inline constexpr bool is_nothrow_convertible_v; // C++20

    template <class Fn, class... ArgTypes> struct is_invocable;
    template <class R, class Fn, class... ArgTypes> struct is_invocable_r;

    template <class Fn, class... ArgTypes> struct is_nothrow_invocable;
    template <class R, class Fn, class... ArgTypes> struct is_nothrow_invocable_r;

    // Alignment properties and transformations:
    template <class T> struct alignment_of;
    template <size_t Len, size_t Align = most_stringent_alignment_requirement>
        struct aligned_storage;
    template <size_t Len, class... Types> struct aligned_union;
    template <class T> struct remove_cvref; // C++20

    template <class T> struct decay;
    template <class... T> struct common_type;
    template <class T> struct underlying_type;
    template <class> class result_of; // undefined
    template <class Fn, class... ArgTypes> class result_of<Fn(ArgTypes...)>;
    template <class Fn, class... ArgTypes> struct invoke_result;  // C++17

    // const-volatile modifications:
    template <class T>
      using remove_const_t    = typename remove_const<T>::type;  // C++14
    template <class T>
      using remove_volatile_t = typename remove_volatile<T>::type;  // C++14
    template <class T>
      using remove_cv_t       = typename remove_cv<T>::type;  // C++14
    template <class T>
      using add_const_t       = typename add_const<T>::type;  // C++14
    template <class T>
      using add_volatile_t    = typename add_volatile<T>::type;  // C++14
    template <class T>
      using add_cv_t          = typename add_cv<T>::type;  // C++14

    // reference modifications:
    template <class T>
      using remove_reference_t     = typename remove_reference<T>::type;  // C++14
    template <class T>
      using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;  // C++14
    template <class T>
      using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;  // C++14

    // sign modifications:
    template <class T>
      using make_signed_t   = typename make_signed<T>::type;  // C++14
    template <class T>
      using make_unsigned_t = typename make_unsigned<T>::type;  // C++14

    // array modifications:
    template <class T>
      using remove_extent_t      = typename remove_extent<T>::type;  // C++14
    template <class T>
      using remove_all_extents_t = typename remove_all_extents<T>::type;  // C++14

    template <class T>
      inline constexpr bool is_bounded_array_v
        = is_bounded_array<T>::value;                                     // C++20
      inline constexpr bool is_unbounded_array_v
        = is_unbounded_array<T>::value;                                   // C++20

    // pointer modifications:
    template <class T>
      using remove_pointer_t = typename remove_pointer<T>::type;  // C++14
    template <class T>
      using add_pointer_t    = typename add_pointer<T>::type;  // C++14

    // other transformations:
    template <size_t Len, std::size_t Align=default-alignment>
      using aligned_storage_t = typename aligned_storage<Len,Align>::type;  // C++14
    template <std::size_t Len, class... Types>
      using aligned_union_t   = typename aligned_union<Len,Types...>::type;  // C++14
    template <class T>
      using remove_cvref_t    = typename remove_cvref<T>::type;  // C++20
    template <class T>
      using decay_t           = typename decay<T>::type;  // C++14
    template <bool b, class T=void>
      using enable_if_t       = typename enable_if<b,T>::type;  // C++14
    template <bool b, class T, class F>
      using conditional_t     = typename conditional<b,T,F>::type;  // C++14
    template <class... T>
      using common_type_t     = typename common_type<T...>::type;  // C++14
    template <class T>
      using underlying_type_t = typename underlying_type<T>::type;  // C++14
    template <class T>
      using result_of_t       = typename result_of<T>::type;  // C++14
    template <class Fn, class... ArgTypes>
      using invoke_result_t   = typename invoke_result<Fn, ArgTypes...>::type;  // C++17

    template <class...>
      using void_t = void;   // C++17

      // See C++14 20.10.4.1, primary type categories
      template <class T> inline constexpr bool is_void_v
        = is_void<T>::value;                                             // C++17
      template <class T> inline constexpr bool is_null_pointer_v
        = is_null_pointer<T>::value;                                     // C++17
      template <class T> inline constexpr bool is_integral_v
        = is_integral<T>::value;                                         // C++17
      template <class T> inline constexpr bool is_floating_point_v
        = is_floating_point<T>::value;                                   // C++17
      template <class T> inline constexpr bool is_array_v
        = is_array<T>::value;                                            // C++17
      template <class T> inline constexpr bool is_pointer_v
        = is_pointer<T>::value;                                          // C++17
      template <class T> inline constexpr bool is_lvalue_reference_v
        = is_lvalue_reference<T>::value;                                 // C++17
      template <class T> inline constexpr bool is_rvalue_reference_v
        = is_rvalue_reference<T>::value;                                 // C++17
      template <class T> inline constexpr bool is_member_object_pointer_v
        = is_member_object_pointer<T>::value;                            // C++17
      template <class T> inline constexpr bool is_member_function_pointer_v
        = is_member_function_pointer<T>::value;                          // C++17
      template <class T> inline constexpr bool is_enum_v
        = is_enum<T>::value;                                             // C++17
      template <class T> inline constexpr bool is_union_v
        = is_union<T>::value;                                            // C++17
      template <class T> inline constexpr bool is_class_v
        = is_class<T>::value;                                            // C++17
      template <class T> inline constexpr bool is_function_v
        = is_function<T>::value;                                         // C++17

      // See C++14 20.10.4.2, composite type categories
      template <class T> inline constexpr bool is_reference_v
        = is_reference<T>::value;                                        // C++17
      template <class T> inline constexpr bool is_arithmetic_v
        = is_arithmetic<T>::value;                                       // C++17
      template <class T> inline constexpr bool is_fundamental_v
        = is_fundamental<T>::value;                                      // C++17
      template <class T> inline constexpr bool is_object_v
        = is_object<T>::value;                                           // C++17
      template <class T> inline constexpr bool is_scalar_v
        = is_scalar<T>::value;                                           // C++17
      template <class T> inline constexpr bool is_compound_v
        = is_compound<T>::value;                                         // C++17
      template <class T> inline constexpr bool is_member_pointer_v
        = is_member_pointer<T>::value;                                   // C++17

      // See C++14 20.10.4.3, type properties
      template <class T> inline constexpr bool is_const_v
        = is_const<T>::value;                                            // C++17
      template <class T> inline constexpr bool is_volatile_v
        = is_volatile<T>::value;                                         // C++17
      template <class T> inline constexpr bool is_trivial_v
        = is_trivial<T>::value;                                          // C++17
      template <class T> inline constexpr bool is_trivially_copyable_v
        = is_trivially_copyable<T>::value;                               // C++17
      template <class T> inline constexpr bool is_standard_layout_v
        = is_standard_layout<T>::value;                                  // C++17
      template <class T> inline constexpr bool is_pod_v
        = is_pod<T>::value;                                              // C++17
      template <class T> inline constexpr bool is_literal_type_v
        = is_literal_type<T>::value;                                     // C++17
      template <class T> inline constexpr bool is_empty_v
        = is_empty<T>::value;                                            // C++17
      template <class T> inline constexpr bool is_polymorphic_v
        = is_polymorphic<T>::value;                                      // C++17
      template <class T> inline constexpr bool is_abstract_v
        = is_abstract<T>::value;                                         // C++17
      template <class T> inline constexpr bool is_final_v
        = is_final<T>::value;                                            // C++17
      template <class T> inline constexpr bool is_aggregate_v
        = is_aggregate<T>::value;                                        // C++17
      template <class T> inline constexpr bool is_signed_v
        = is_signed<T>::value;                                           // C++17
      template <class T> inline constexpr bool is_unsigned_v
        = is_unsigned<T>::value;                                         // C++17
      template <class T, class... Args> inline constexpr bool is_constructible_v
        = is_constructible<T, Args...>::value;                           // C++17
      template <class T> inline constexpr bool is_default_constructible_v
        = is_default_constructible<T>::value;                            // C++17
      template <class T> inline constexpr bool is_copy_constructible_v
        = is_copy_constructible<T>::value;                               // C++17
      template <class T> inline constexpr bool is_move_constructible_v
        = is_move_constructible<T>::value;                               // C++17
      template <class T, class U> inline constexpr bool is_assignable_v
        = is_assignable<T, U>::value;                                    // C++17
      template <class T> inline constexpr bool is_copy_assignable_v
        = is_copy_assignable<T>::value;                                  // C++17
      template <class T> inline constexpr bool is_move_assignable_v
        = is_move_assignable<T>::value;                                  // C++17
      template <class T, class U> inline constexpr bool is_swappable_with_v
        = is_swappable_with<T, U>::value;                                // C++17
      template <class T> inline constexpr bool is_swappable_v
        = is_swappable<T>::value;                                        // C++17
      template <class T> inline constexpr bool is_destructible_v
        = is_destructible<T>::value;                                     // C++17
      template <class T, class... Args> inline constexpr bool is_trivially_constructible_v
        = is_trivially_constructible<T, Args...>::value;                 // C++17
      template <class T> inline constexpr bool is_trivially_default_constructible_v
        = is_trivially_default_constructible<T>::value;                  // C++17
      template <class T> inline constexpr bool is_trivially_copy_constructible_v
        = is_trivially_copy_constructible<T>::value;                     // C++17
      template <class T> inline constexpr bool is_trivially_move_constructible_v
        = is_trivially_move_constructible<T>::value;                     // C++17
      template <class T, class U> inline constexpr bool is_trivially_assignable_v
        = is_trivially_assignable<T, U>::value;                          // C++17
      template <class T> inline constexpr bool is_trivially_copy_assignable_v
        = is_trivially_copy_assignable<T>::value;                        // C++17
      template <class T> inline constexpr bool is_trivially_move_assignable_v
        = is_trivially_move_assignable<T>::value;                        // C++17
      template <class T> inline constexpr bool is_trivially_destructible_v
        = is_trivially_destructible<T>::value;                           // C++17
      template <class T, class... Args> inline constexpr bool is_nothrow_constructible_v
        = is_nothrow_constructible<T, Args...>::value;                   // C++17
      template <class T> inline constexpr bool is_nothrow_default_constructible_v
        = is_nothrow_default_constructible<T>::value;                    // C++17
      template <class T> inline constexpr bool is_nothrow_copy_constructible_v
        = is_nothrow_copy_constructible<T>::value;                       // C++17
      template <class T> inline constexpr bool is_nothrow_move_constructible_v
        = is_nothrow_move_constructible<T>::value;                       // C++17
      template <class T, class U> inline constexpr bool is_nothrow_assignable_v
        = is_nothrow_assignable<T, U>::value;                            // C++17
      template <class T> inline constexpr bool is_nothrow_copy_assignable_v
        = is_nothrow_copy_assignable<T>::value;                          // C++17
      template <class T> inline constexpr bool is_nothrow_move_assignable_v
        = is_nothrow_move_assignable<T>::value;                          // C++17
      template <class T, class U> inline constexpr bool is_nothrow_swappable_with_v
        = is_nothrow_swappable_with<T, U>::value;                       // C++17
      template <class T> inline constexpr bool is_nothrow_swappable_v
        = is_nothrow_swappable<T>::value;                               // C++17
      template <class T> inline constexpr bool is_nothrow_destructible_v
        = is_nothrow_destructible<T>::value;                             // C++17
      template <class T> inline constexpr bool has_virtual_destructor_v
        = has_virtual_destructor<T>::value;                              // C++17
      template<class T> inline constexpr bool has_unique_object_representations_v // C++17
        = has_unique_object_representations<T>::value;

      // See C++14 20.10.5, type property queries
      template <class T> inline constexpr size_t alignment_of_v
        = alignment_of<T>::value;                                        // C++17
      template <class T> inline constexpr size_t rank_v
        = rank<T>::value;                                                // C++17
      template <class T, unsigned I = 0> inline constexpr size_t extent_v
        = extent<T, I>::value;                                           // C++17

      // See C++14 20.10.6, type relations
      template <class T, class U> inline constexpr bool is_same_v
        = is_same<T, U>::value;                                          // C++17
      template <class Base, class Derived> inline constexpr bool is_base_of_v
        = is_base_of<Base, Derived>::value;                              // C++17
      template <class From, class To> inline constexpr bool is_convertible_v
        = is_convertible<From, To>::value;                               // C++17
      template <class Fn, class... ArgTypes> inline constexpr bool is_invocable_v
        = is_invocable<Fn, ArgTypes...>::value;                          // C++17
      template <class R, class Fn, class... ArgTypes> inline constexpr bool is_invocable_r_v
        = is_invocable_r<R, Fn, ArgTypes...>::value;                     // C++17
      template <class Fn, class... ArgTypes> inline constexpr bool is_nothrow_invocable_v
        = is_nothrow_invocable<Fn, ArgTypes...>::value;                  // C++17
      template <class R, class Fn, class... ArgTypes> inline constexpr bool is_nothrow_invocable_r_v
        = is_nothrow_invocable_r<R, Fn, ArgTypes...>::value;             // C++17

      // [meta.logical], logical operator traits:
      template<class... B> struct conjunction;                           // C++17
      template<class... B>
        inline constexpr bool conjunction_v = conjunction<B...>::value;  // C++17
      template<class... B> struct disjunction;                           // C++17
      template<class... B>
        inline constexpr bool disjunction_v = disjunction<B...>::value;  // C++17
      template<class B> struct negation;                                 // C++17
      template<class B>
        inline constexpr bool negation_v = negation<B>::value;           // C++17

}
```

# enable_if

```C++
template <bool, class _Tp = void> struct _LIBCPP_TEMPLATE_VIS enable_if {};
template <class _Tp> struct _LIBCPP_TEMPLATE_VIS enable_if<true, _Tp> {typedef _Tp type;};

#if _LIBCPP_STD_VER > 11
template <bool _Bp, class _Tp = void> using enable_if_t = typename enable_if<_Bp, _Tp>::type;
#endif
```