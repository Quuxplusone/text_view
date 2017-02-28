[![Travis Build Status](https://travis-ci.org/tahonermann/text_view.svg?branch=master)](https://travis-ci.org/tahonermann/text_view)
Travis CI (Linux:gcc)

# [Text_view]
A [C++ Concepts][ISO/IEC 19217:2015]
based character encoding and code point enumeration library.

This project is the reference implementation for proposal [P0244] for the C++
standard.

This port of [Text_view] requires a C++17 conforming compiler that implements
[ISO/IEC technical specification 19217:2015, **C++ Extensions for concepts**
][ISO/IEC 19217:2015].  A port of [Text_view] that builds with C++11 conforming
compilers is available at [Text_view for range-v3][Text_view-range-v3].

For discussion of this project, please post and/or subscribe to the
`text_view@googlegroups.com` group hosted at
https://groups.google.com/d/forum/text_view

- [Overview](#overview)
- [Current features and limitations](#current-features-and-limitations)
- [Requirements](#requirements)
- [Build and installation](#build-and-installation)
  - [Building and installing gcc]
    (#building-and-installing-gcc)
  - [Building and installing cmcstl2]
    (#building-and-installing-cmcstl2)
  - [Building and installing Text_view]
    (#building-and-installing-text_view)
- [Usage](#usage)
  - [Header &lt;experimental/text_view&gt; synopsis]
    (#header-experimentaltext_view-synopsis)
  - [Concepts](#concepts)
  - [Type traits](#type-traits)
  - [Character sets](#character-sets)
  - [Character set identification](#character-set-identification)
  - [Character set information](#character-set-information)
  - [Characters](#characters)
  - [Encodings](#encodings)
  - [Text iterators](#text-iterators)
  - [Text view](#text-view)
  - [Exceptions](#exceptions)
- [Supported Encodings](#supported-encodings)
- [Terminology](#terminology)
  - [Code Unit](#code-unit)
  - [Code Point](#code-point)
  - [Character Set](#character-set)
  - [Character](#character)
  - [Encoding](#encoding)
- [References](#references)

# Overview
[C++11][ISO/IEC 14882:2011] added support for new character types ([N2249]) and
[Unicode] string literals ([N2442]), but neither [C++11][ISO/IEC 14882:2011],
nor more recent standards have provided means of efficiently and conveniently
enumerating [code points](#code-point) in [Unicode] or legacy encodings.  While
it is possible to implement such enumeration using interfaces provided in the
standard `<codecvt>` library, doing to is awkward, requires that text be
provided as pointers to contiguous memory, and inefficent due to virtual
function call overhead (__examples and data required to back up these
assertions__).

[Text_view] provides iterator and range based interfaces for encoding and
decoding strings in a variety of [character encodings](#encoding).  The
interface is intended to support all modern and legacy
[character encodings](#encoding), though this library does not yet provide
implementations for legacy [encodings](#encoding).

An example usage follows.  Note that `\u00F8` (LATIN SMALL LETTER O WITH STROKE)
is encoded as UTF-8 using two [code units](#code-unit) (`\xC3\xB8`), but
iterator based enumeration sees just the single [code point](#code-point).

```C++
using CT = utf8_encoding::character_type;
auto tv = make_text_view<utf8_encoding>(u8"J\u00F8erg is my friend");
auto it = tv.begin();
assert(*it++ == CT{0x004A}); // 'J'
assert(*it++ == CT{0x00F8}); // 'ø'
assert(*it++ == CT{0x0065}); // 'e'
```

The iterators and ranges that [Text_view] provides are compatible with the
non-modifying sequence utilities provided by the standard C++ `<algorithm>`
library.  This enables use of standard algorithms to search encoded text.

```C++
it = std::find(tv.begin(), tv.end(), CT{0x00F8});
assert(it != tv.end());
```

The iterators provided by [Text_view] also provide access to the underlying
[code unit](#code-unit) sequence.

```C++
auto base_it = it.base_range().begin();
assert(*base_it++ == '\xC3');
assert(*base_it++ == '\xB8');
assert(base_it == it.base_range().end());
```

[Text_view] ranges satisfy the requirements for use in
[C++11][ISO/IEC 14882:2011] range-based for statements with the removed
same type restriction for the begin and end expressions provided by [P0184R0]
as adopted for C++17.

```C++
for (const auto &ch : tv) {
  ...
}
```

# Current features and limitations
[Text_view] provides interfaces for the following:
- Encoding and decoding of text for the [encodings](#encoding) listed in
  [supported encodings](#supported-encodings).
- Encoding text using [C++11][ISO/IEC 14882:2011] compliant output iterators.
- Decoding text using input, forward, bidirectional, and random access
  iterators that are compliant with standard iterator requirements as specified
  in the [ranges proposal][N4560].
- Constructing view adapters for encoded text stored in arrays, containers,
  or std::basic_string, or referenced by another range or view.  These view
  adapters meet the requirements for views in the [ranges proposal][N4560].

[Text_view] does **not** currently provide interfaces for the following:
- Transcoding of code points from one [character set](#character-set) to
  another.
- Iterators for grapheme clusters or other boundary conditions.
- Collation.
- Localization.
- Internationalization.
- [Unicode] code point properties.
- [Unicode] normalization.

# Requirements
[Text_view] requires a C++ compiler that implements [ISO/IEC technical
specification 19217:2015, **C++ Extensions for concepts**][ISO/IEC 19217:2015]
As of 2016-08-26, this specification is only supported by [gcc] release
6.2.0 or later.  Additionally, [Text_view] depends on the [cmcstl2]
implementation of the [ranges proposal][N4560] for concept definitions.

# Build and installation
This section provides instructions for building [Text_view] and suitable
versions of its dependencies.

## Building and installing [gcc]
[Text_view] requires [gcc] version 6.2.0 or later.  The following commands
can be used to perform a suitable build of the current in-development release
of [gcc] on Linux if an installation of [gcc] 6.2.0 or later is not available.
If you have an installation of [gcc] 6.2.0 or later available, then there is
no need to build [gcc] yourself.

```sh
$ svn co svn://gcc.gnu.org/svn/gcc/trunk gcc-trunk-src
$ curl -O ftp://ftp.gnu.org/gnu/gmp/gmp-5.1.1.tar.bz2
$ curl -O ftp://ftp.gnu.org/gnu/mpfr/mpfr-3.1.2.tar.bz2
$ curl -O ftp://ftp.gnu.org/gnu/mpc/mpc-1.0.1.tar.gz
$ cd gcc-trunk-src
$ svn update -r 234230  # Optional command to select a known good gcc version
$ bzip2 -d -c ../gmp-5.1.1.tar.bz2 | tar -xvf -
$ mv gmp-5.1.1 gmp
$ bzip2 -d -c ../mpfr-3.1.2.tar.bz2 | tar -xvf -
$ mv mpfr-3.1.2 mpfr
$ tar -zxvf ../mpc-1.0.1.tar.gz
$ mv mpc-1.0.1 mpc
$ cd ..
$ mkdir gcc-trunk-build
$ cd gcc-trunk-build
$ LIBRARY_PATH=/usr/lib/$(gcc -print-multiarch); export LIBRARY_PATH
$ CPATH=/usr/include/$(gcc -print-multiarch); export CPATH
$ ../gcc-trunk-src/configure \
  CC=gcc \
  CXX=g++ \
  --prefix $(pwd)/../gcc-trunk-install \
  --disable-multilib \
  --disable-bootstrap \
  --enable-languages=c,c++
$ make -j 4
$ make install
$ cd ..
```

When complete, the new [gcc] build will be present in the `gcc-trunk-install`
directory.

## Building and installing [cmcstl2]
[Text_view] only depends on headers provided by [cmcstl2] and no build or
installation is required.  [Text_view] is known to build successfully with
[cmcstl2] git revision `b704dadecd0ae2fc11289e35b37d234d9c8eb644`.  The
following commands can be used to checkout a known good revision.

```sh
$ git clone https://github.com/CaseyCarter/cmcstl2.git cmcstl2
$ cd cmcstl2
$ git checkout b704dadecd0ae2fc11289e35b37d234d9c8eb644
```

## Building and installing [Text_view]
[Text_view] is currently a header-only library, so no build or installation is
currently required to use it.  It is unlikely that [Text_view] will remain a
header-only library in the future, so at some point, a build and installation
step will be required.

[Text_view] has a [CMake] based build system sufficient to build and run its
tests, to validate example code, and to perform a minimal installation following
established operating system conventions.  By default, files will be installed
under `/usr/local` on UNIX and UNIX-like systems, and under `C:\Program Files`
on Windows.  The installation location can be changed by invoking `cmake` with
a `-DCMAKE_INSTALL_PREFIX=<path>` option.  On UNIX and UNIX-like systems, header
files will be installed in the `include` directory of the installation
destination, and other files will be installed under `share/text_view`.  On
Windows, header files be installed in the `text_view\include` directory of the
installation destination, and other files will be installed under `text_view`.

Unless [cmcstl2] is installed to a common location, it will be necessary to
inform the build where it is installed.  This is typically done by setting the
`CMCSTL2_INSTALL_PATH` environment variable.  As of this writing, [cmcstl2]
does not provide an installation option, so `CMCSTL2_INSTALL_PATH` should
specify the location where the [cmcstl2] source resides (the directory that
contains the [cmcstl2] `include` directory).

The following commands suffice to build and run tests and examples, and perform
an installation.  If the build succeeds, built test and example programs will
be present in the `test` and `examples` subdirectories of the build directory
(the built test and example programs are not installed), and header files,
example code, cmake package configuration modules, and other miscellaneous files
will be present in the installation directory.

```sh
$ vi setenv.sh  # Update GCC_INSTALL_PATH and CMCSTL2_INSTALL_PATH.
$ . ./setenv.sh
$ mkdir build
$ cd build
$ cmake .. [-DCMAKE_INSTALL_PREFIX=/path/to/install/to]
$ cmake --build . --target install
$ ctest
```

`check` and `check-install` [CMake] targets are also available for automating
build and test.  The `check` target performs a build without installation and
then runs the tests.  The `check-install` target performs a build, runs tests,
installs to a location within the build directory, and then performs tests
(verifying that example code builds) on the installation.

The installation includes a [CMake] based build system for building the example
code.  To build all of the examples, run `cmake` specifying the `examples`
directory of the installation as the source directory.  Alternatively, each
example can be built independently by specifying its source directory as the
source directory in a `cmake` invocation.  If the installation was to a
non-default installation location (`-DCMAKE_INSTALL_PREFIX` was specified),
then it may be necessary to set `CMAKE_PREFIX_PATH` to the [Text_view]
installation location (the location `CMAKE_INSTALL_PREFIX` was set to) or
`text_view_DIR` to the directory containing the installed
`text_view-config.cmake` file, so that the [Text_view] package configuration
file is found.  See the [CMake] documentation for more details.

The following commands suffice to build all of the installed examples.

```sh
$ cd /path/to/installation/text_view/examples
$ mkdir build
$ cd build
$ cmake .. [-DCMAKE_PREFIX_PATH=/path/to/installation]
$ cmake --build .
$ ctest
```

# Usage
[Text_view] is currently a header-only library.  To use it in your own code,
add include paths for the `text_view/include` and [cmcstl2] installation
locations, and include the `text_view` header file:

```C++
#include <experimental/text_view>
```

[Text_view] installations include a [CMake] package configuration file suitable
for use in [CMake] based projects.  To use it, specify `text_view` as the
`<package>` argument to `find_package` in your [CMake] file.  The package
configuration module sets the `text_view_COMPILE_OPTIONS`,
`text_view_DEFINITIONS`, and `text_view_INCLUDE_DIRS` variables to suitable
values for use with the [CMake] `target_compile_options`,
`target_compile_definitions`, and `target_include_directories` commands.  If
[Text_view] was installed to a non-default installation location
(`-DCMAKE_INSTALL_PREFIX` was specified), then it may be necessary to set
`CMAKE_PREFIX_PATH` to the [Text_view] installation location (the location
`CMAKE_INSTALL_PREFIX` was set to) or `text_view_DIR` to the directory
containing the installed `text_view-config.cmake` file, so that the
[Text_view] package configuration file is found.  See the [CMake] documentation
for more details.  The `CMakeLists.txt` files provided with the installed
examples exemplify a minimal [CMake] based build system for a downstream
consumer of [Text_view].

All interfaces intended for public use are declared in the
`std::experimental::text` namespace.  The `text` namespace is an inline
namespace, so all entities are available from the `std::experimental` namespace
itself.

The interface descriptions in the sections that follow use the concept names
from the [ranges proposal][N4560], are intended to be used as specification,
and should be considered authoritative.  Any differences in behavior as
defined by these definitions as compared to the [Text_view] implementation are
unintentional and should be considered indicatative of a defect in either the
specification or the implementation.

## Header &lt;experimental/text_view&gt; synopsis

```C++
namespace std {
namespace experimental {
inline namespace text {

// concepts:
template<typename T> concept bool CodeUnit();
template<typename T> concept bool CodePoint();
template<typename T> concept bool CharacterSet();
template<typename T> concept bool Character();
template<typename T> concept bool CodeUnitIterator();
template<typename T, typename V> concept bool CodeUnitOutputIterator();
template<typename T> concept bool TextEncodingState();
template<typename T> concept bool TextEncodingStateTransition();
template<typename T> concept bool TextEncoding();
template<typename T, typename I> concept bool TextEncoder();
template<typename T, typename I> concept bool TextDecoder();
template<typename T, typename I> concept bool TextForwardDecoder();
template<typename T, typename I> concept bool TextBidirectionalDecoder();
template<typename T, typename I> concept bool TextRandomAccessDecoder();
template<typename T> concept bool TextIterator();
template<typename T, typename I> concept bool TextSentinel();
template<typename T> concept bool TextOutputIterator();
template<typename T> concept bool TextInputIterator();
template<typename T> concept bool TextForwardIterator();
template<typename T> concept bool TextBidirectionalIterator();
template<typename T> concept bool TextRandomAccessIterator();
template<typename T> concept bool TextView();
template<typename T> concept bool TextInputView();
template<typename T> concept bool TextForwardView();
template<typename T> concept bool TextBidirectionalView();
template<typename T> concept bool TextRandomAccessView();

// character sets:
class any_character_set;
class basic_execution_character_set;
class basic_execution_wide_character_set;
class unicode_character_set;

// implementation defined character set type aliases:
using execution_character_set = /* implementation-defined */ ;
using execution_wide_character_set = /* implementation-defined */ ;
using universal_character_set = /* implementation-defined */ ;

// character set identification:
class character_set_id;

template<CharacterSet CST>
  inline character_set_id get_character_set_id();

// character set information:
class character_set_info;

template<CharacterSet CST>
  inline const character_set_info& get_character_set_info();
const character_set_info& get_character_set_info(character_set_id id);

// character set and encoding traits:
template<typename T>
  using code_unit_type_t = /* implementation-defined */ ;
template<typename T>
  using code_point_type_t = /* implementation-defined */ ;
template<typename T>
  using character_set_type_t = /* implementation-defined */ ;
template<typename T>
  using character_type_t = /* implementation-defined */ ;
template<typename T>
  using encoding_type_t = /* implementation-defined */ ;
template<typename T>
  using default_encoding_type_t = /* implementation-defined */ ;

// characters:
template<CharacterSet CST> class character;
template <> class character<any_character_set>;

template<CharacterSet CST>
  bool operator==(const character<any_character_set> &lhs,
                  const character<CST> &rhs);
template<CharacterSet CST>
  bool operator==(const character<CST> &lhs,
                  const character<any_character_set> &rhs);
template<CharacterSet CST>
  bool operator!=(const character<any_character_set> &lhs,
                  const character<CST> &rhs);
template<CharacterSet CST>
  bool operator!=(const character<CST> &lhs,
                  const character<any_character_set> &rhs);

// encoding state and transition types:
class trivial_encoding_state;
class trivial_encoding_state_transition;
class utf8bom_encoding_state;
class utf8bom_encoding_state_transition;
class utf16bom_encoding_state;
class utf16bom_encoding_state_transition;
class utf32bom_encoding_state;
class utf32bom_encoding_state_transition;

// encodings:
class basic_execution_character_encoding;
class basic_execution_wide_character_encoding;
#if defined(__STDC_ISO_10646__)
class iso_10646_wide_character_encoding;
#endif // __STDC_ISO_10646__
class utf8_encoding;
class utf8bom_encoding;
class utf16_encoding;
class utf16be_encoding;
class utf16le_encoding;
class utf16bom_encoding;
class utf32_encoding;
class utf32be_encoding;
class utf32le_encoding;
class utf32bom_encoding;

// implementation defined encoding type aliases:
using execution_character_encoding = /* implementation-defined */ ;
using execution_wide_character_encoding = /* implementation-defined */ ;
using char8_character_encoding = /* implementation-defined */ ;
using char16_character_encoding = /* implementation-defined */ ;
using char32_character_encoding = /* implementation-defined */ ;

// itext_iterator:
template<TextEncoding ET, ranges::View VT>
  requires TextDecoder<ET, ranges::iterator_t<std::add_const_t<VT>>>()
  class itext_iterator;

// itext_sentinel:
template<TextEncoding ET, ranges::View VT>
  class itext_sentinel;

// otext_iterator:
template<TextEncoding ET, CodeUnitOutputIterator<code_unit_type_t<ET>> CUIT>
  class otext_iterator;

// otext_iterator factory functions:
template<TextEncoding ET, CodeUnitOutputIterator<code_unit_type_t<ET>> IT>
  auto make_otext_iterator(typename ET::state_type state, IT out)
  -> otext_iterator<ET, IT>;
template<TextEncoding ET, CodeUnitOutputIterator<code_unit_type_t<ET>> IT>
  auto make_otext_iterator(IT out)
  -> otext_iterator<ET, IT>;

// basic_text_view:
template<TextEncoding ET, ranges::View VT>
  class basic_text_view;

// basic_text_view type aliases:
using text_view = basic_text_view<execution_character_encoding,
                                  /* implementation-defined */ >;
using wtext_view = basic_text_view<execution_wide_character_encoding,
                                   /* implementation-defined */ >;
using u8text_view = basic_text_view<char8_character_encoding,
                                    /* implementation-defined */ >;
using u16text_view = basic_text_view<char16_character_encoding,
                                     /* implementation-defined */ >;
using u32text_view = basic_text_view<char32_character_encoding,
                                     /* implementation-defined */ >;

// basic_text_view factory functions:
template<TextEncoding ET, ranges::InputIterator IT, ranges::Sentinel<IT> ST>
  auto make_text_view(typename ET::state_type state, IT first, ST last)
  -> basic_text_view<ET, /* implementation-defined */ >;
template<ranges::InputIterator IT, ranges::Sentinel<IT> ST>
  requires requires () {
    typename default_encoding_type_t<ranges::value_type_t<IT>>;
  }
  auto make_text_view(typename default_encoding_type_t<ranges::value_type_t<IT>>::state_type state,
                      IT first,
                      ST last)
  -> basic_text_view<default_encoding_type_t<ranges::value_type_t<IT>>, /* implementation-defined */ >;
template<TextEncoding ET, ranges::InputIterator IT, ranges::Sentinel<IT> ST>
  auto make_text_view(IT first, ST last)
  -> basic_text_view<ET, /* implementation-defined */ >;
template<ranges::InputIterator IT, ranges::Sentinel<IT> ST>
  requires requires () {
    typename default_encoding_type_t<ranges::value_type_t<IT>>;
  }
  auto make_text_view(IT first, ST last)
  -> basic_text_view<default_encoding_type_t<ranges::value_type_t<IT>>, /* implementation-defined */ >;
template<TextEncoding ET, ranges::ForwardIterator IT>
  auto make_text_view(typename ET::state_type state,
                      IT first,
                      typename std::make_unsigned<ranges::difference_type_t<IT>>::type n)
  -> basic_text_view<ET, /* implementation-defined */ >;
template<ranges::ForwardIterator IT>
  requires requires () {
    typename default_encoding_type_t<ranges::value_type_t<IT>>;
  }
  auto make_text_view(typename typename default_encoding_type_t<ranges::value_type_t<IT>>::state_type state,
                      IT first,
                      typename std::make_unsigned<ranges::difference_type_t<IT>>::type n)
  -> basic_text_view<default_encoding_type_t<ranges::value_type_t<IT>>, /* implementation-defined */ >;
template<TextEncoding ET, ranges::ForwardIterator IT>
  auto make_text_view(IT first,
                      typename std::make_unsigned<ranges::difference_type_t<IT>>::type n)
  -> basic_text_view<ET, /* implementation-defined */ >;
template<ranges::ForwardIterator IT>
  requires requires () {
    typename default_encoding_type_t<ranges::value_type_t<IT>>;
  }
  auto make_text_view(IT first,
                      typename std::make_unsigned<ranges::difference_type_t<IT>>::type n)
  -> basic_text_view<default_encoding_type_t<ranges::value_type_t<IT>>, /* implementation-defined */ >;
template<TextEncoding ET, ranges::InputRange Iterable>
  auto make_text_view(typename ET::state_type state,
                      const Iterable &iterable)
  -> basic_text_view<ET, /* implementation-defined */ >;
template<ranges::InputRange Iterable>
  requires requires () {
    typename default_encoding_type_t<ranges::value_type_t<ranges::iterator_t<Iterable>>>;
  }
  auto make_text_view(typename default_encoding_type_t<ranges::value_type_t<ranges::iterator_t<Iterable>>>::state_type state,
                      const Iterable &iterable)
  -> basic_text_view<default_encoding_type_t<ranges::value_type_t<ranges::iterator_t<Iterable>>>, /* implementation-defined */ >;
template<TextEncoding ET, ranges::InputRange Iterable>
  auto make_text_view(const Iterable &iterable)
  -> basic_text_view<ET, /* implementation-defined */ >;
template<ranges::InputRange Iterable>
  requires requires () {
    typename default_encoding_type_t<ranges::value_type_t<ranges::iterator_t<Iterable>>>;
  }
  auto make_text_view(const Iterable &iterable)
  -> basic_text_view<default_encoding_type_t<ranges::value_type_t<ranges::iterator_t<Iterable>>>, /* implementation-defined */ >;
template<TextInputIterator TIT, TextSentinel<TIT> TST>
  auto make_text_view(TIT first, TST last)
  -> basic_text_view<ET, /* implementation-defined */ >;
template<TextView TVT>
  TVT make_text_view(TVT tv);

// exception classes:
class text_runtime_error;
class text_encode_error;
class text_decode_error;
class text_encode_overflow_error;
class text_decode_underflow_error;

} // inline namespace text
} // namespace experimental
} // namespace std
```

## Concepts

- [Concept CodeUnit](#concept-codeunit)
- [Concept CodePoint](#concept-codepoint)
- [Concept CharacterSet](#concept-characterset)
- [Concept Character](#concept-character)
- [Concept CodeUnitIterator](#concept-codeunititerator)
- [Concept CodeUnitOutputIterator](#concept-codeunitoutputiterator)
- [Concept TextEncodingState](#concept-textencodingstate)
- [Concept TextEncodingStateTransition]
  (#concept-textencodingstatetransition)
- [Concept TextEncoding](#concept-textencoding)
- [Concept TextEncoder](#concept-textencoder)
- [Concept TextDecoder](#concept-textdecoder)
- [Concept TextForwardDecoder](#concept-textforwarddecoder)
- [Concept TextBidirectionalDecoder](#concept-textbidirectionaldecoder)
- [Concept TextRandomAccessDecoder](#concept-textrandomaccessdecoder)
- [Concept TextIterator](#concept-textiterator)
- [Concept TextSentinel](#concept-textsentinel)
- [Concept TextOutputIterator](#concept-textoutputiterator)
- [Concept TextInputIterator](#concept-textinputiterator)
- [Concept TextForwardIterator](#concept-textforwarditerator)
- [Concept TextBidirectionalIterator](#concept-textbidirectionaliterator)
- [Concept TextRandomAccessIterator](#concept-textrandomaccessiterator)
- [Concept TextView](#concept-textview)
- [Concept TextInputView](#concept-textinputview)
- [Concept TextForwardView](#concept-textforwardview)
- [Concept TextBidirectionalView](#concept-textbidirectionalview)
- [Concept TextRandomAccessView](#concept-textrandomaccessview)

### Concept CodeUnit
The `CodeUnit` concept specifies requirements for a type usable as the
[code unit](#code-unit) type of a string type.

```C++
template<typename T> concept bool CodeUnit() {
  return /* implementation-defined */ ;
}
```

`CodeUnit<T>()` is satisfied if and only if
`std::is_integral<T>::value` is true and at least one of
`std::is_unsigned<T>::value` is true,
`std::is_same<std::remove_cv_t<T>, char>::value` is true, or
`std::is_same<std::remove_cv_t<T>, wchar_t>::value` is true.

### Concept CodePoint
The `CodePoint` concept specifies requirements for a type usable as the
[code point](#code-point) type of a [character set](#character-set) type.

```C++
template<typename T> concept bool CodePoint() {
  return /* implementation-defined */ ;
}
```

`CodePoint<T>()` is satisfied if and only if
`std::is_integral<T>::value` is true and at least one of
`std::is_unsigned<T>::value` is true,
`std::is_same<std::remove_cv_t<T>, char>::value` is true, or
`std::is_same<std::remove_cv_t<T>, wchar_t>::value` is true.

### Concept CharacterSet
The `CharacterSet` concept specifies requirements for a type that describes
a [character set](#character-set).  Such a type has a member typedef-name
declaration for a type that satisfies `CodePoint` and a static member function
that returns a name for the [character set](#character-set).

```C++
template<typename T> concept bool CharacterSet() {
  return CodePoint<code_point_type_t<T>>()
      && requires () {
           { T::get_name() } noexcept -> const char *;
         };
}
```

### Concept Character
The `Character` concept specifies requirements for a type that describes a
[character](#character) as defined by an associated
[character set](#character-set).  Non-static member functions provide access to
the [code point](#code-point) value of the described [character](#character).
Types that satisfy `Character` are regular and copyable.

```C++
template<typename T> concept bool Character() {
  return ranges::Regular<T>()
      && CharacterSet<character_set_type_t<T>>()
      && requires (T t,
                   const T ct,
                   code_point_type_t<character_set_type_t<T>> cp)
         {
           t.set_code_point(cp);
           { ct.get_code_point() } noexcept
               -> code_point_type_t<character_set_type_t<T>>;
           { ct.get_character_set_id() }
               -> character_set_id;
         };
}
```

### Concept CodeUnitIterator
The `CodeUnitIterator` concept specifies requirements of an iterator that
has a value type that satisfies `CodeUnit`.

```C++
template<typename T> concept bool CodeUnitIterator() {
  return ranges::Iterator<T>()
      && CodeUnit<ranges::value_type_t<T>>();
}
```

### Concept CodeUnitOutputIterator
The `CodeUnitOutputIterator` concept specifies requirements of an output
iterator that can be assigned from a type that satisfies `CodeUnit`.

```C++
template<typename T, typename V> concept bool CodeUnitOutputIterator() {
  return ranges::OutputIterator<T, V>()
      && CodeUnit<V>();
}
```

### Concept TextEncodingState
The `TextEncodingState` concept specifies requirements of types that hold
[encoding](#encoding) state.  Such types are semiregular.

```C++
template<typename T> concept bool TextEncodingState() {
  return ranges::Semiregular<T>();
}
```

### Concept TextEncodingStateTransition
The `TextEncodingStateTransition` concept specifies requirements of types
that hold [encoding](#encoding) state transitions.  Such types are
semiregular.

```C++
template<typename T> concept bool TextEncodingStateTransition() {
  return ranges::Semiregular<T>();
}
```

### Concept TextEncoding
The `TextEncoding` concept specifies requirements of types that define an
[encoding](#encoding).  Such types define member types that identify the
[code unit](#code-unit), [character](#character), encoding state, and encoding
state transition types, a static member function that returns an initial
encoding state object that defines the encoding state at the beginning of a
sequence of encoded characters, and static data members that specify the
minimum and maximum number of [code units](#code-units) used to encode any
single character.

```C++
template<typename T> concept bool TextEncoding() {
  return requires () {
           { T::min_code_units } noexcept -> int;
           { T::max_code_units } noexcept -> int;
         }
      && TextEncodingState<typename T::state_type>()
      && TextEncodingStateTransition<typename T::state_transition_type>()
      && CodeUnit<code_unit_type_t<T>>()
      && Character<character_type_t<T>>()
      && requires () {
           { T::initial_state() } noexcept
               -> const typename T::state_type&;
         };
}
```

### Concept TextEncoder
The `TextEncoder` concept specifies requirements of types that are used to
encode [characters](#character) using a particular [code unit](#code-unit)
iterator that satisfies `OutputIterator`.  Such a type satisifies
`TextEncoding` and defines static member functions used to encode state
transitions and [characters](#character).

```C++
template<typename T, typename I> concept bool TextEncoder() {
  return TextEncoding<T>()
      && ranges::OutputIterator<CUIT, code_unit_type_t<T>>()
      && requires (
           typename T::state_type &state,
           CUIT &out,
           typename T::state_transition_type stt,
           int &encoded_code_units)
         {
           T::encode_state_transition(state, out, stt, encoded_code_units);
         }
      && requires (
           typename T::state_type &state,
           CUIT &out,
           character_type_t<T> c,
           int &encoded_code_units)
         {
           T::encode(state, out, c, encoded_code_units);
         };
}
```

### Concept TextDecoder
The `TextDecoder` concept specifies requirements of types that are used to
decode [characters](#character) using a particular [code unit](#code-unit)
iterator that satisifies `InputIterator`.  Such a type satisfies
`TextEncoding` and defines a static member function used to decode state
transitions and [characters](#character).

```C++
template<typename T, typename I> concept bool TextDecoder() {
  return TextEncoding<T>()
      && ranges::InputIterator<CUIT>()
      && ranges::ConvertibleTo<ranges::value_type_t<CUIT>,
                               code_unit_type_t<T>>()
      && requires (
           typename T::state_type &state,
           CUIT &in_next,
           CUIT in_end,
           character_type_t<T> &c,
           int &decoded_code_units)
         {
           { T::decode(state, in_next, in_end, c, decoded_code_units) } -> bool;
         };
}
```

### Concept TextForwardDecoder
The `TextForwardDecoder` concept specifies requirements of types that are
used to decode [characters](#character) using a particular
[code unit](#code-unit) iterator that satisifies `ForwardIterator`.  Such a
type satisfies `TextDecoder`.

```C++
template<typename T, typename I> concept bool TextForwardDecoder() {
  return TextDecoder<T, CUIT>()
      && ranges::ForwardIterator<CUIT>();
}
```

### Concept TextBidirectionalDecoder
The `TextBidirectionalDecoder` concept specifies requirements of types that
are used to decode [characters](#character) using a particular
[code unit](#code-unit) iterator that satisifies `BidirectionalIterator`.  Such
a type satisfies `TextForwardDecoder` and defines a static member function
used to decode state transitions and [characters](#character) in the reverse
order of their encoding.

```C++
template<typename T, typename I> concept bool TextBidirectionalDecoder() {
  return TextForwardDecoder<T, CUIT>()
      && ranges::BidirectionalIterator<CUIT>()
      && requires (
           typename T::state_type &state,
           CUIT &in_next,
           CUIT in_end,
           character_type_t<T> &c,
           int &decoded_code_units)
         {
           { T::rdecode(state, in_next, in_end, c, decoded_code_units) } -> bool;
         };
}
```

### Concept TextRandomAccessDecoder
The `TextRandomAccessDecoder` concept specifies requirements of types that
are used to decode [characters](#character) using a particular
[code unit](#code-unit) iterator that satisifies `RandomAccessIterator`.  Such a
type satisfies `TextBidirectionalDecoder`, requires that the minimum and
maximum number of [code units](#code-unit) used to encode any character have
the same value, and that the encoding state be an empty type.

```C++
template<typename T, typename I> concept bool TextRandomAccessDecoder() {
  return TextBidirectionalDecoder<T, CUIT>()
      && ranges::RandomAccessIterator<CUIT>()
      && T::min_code_units == T::max_code_units
      && std::is_empty<typename T::state_type>::value;
}
```

### Concept TextIterator
The `TextIterator` concept specifies requirements of iterator types that are
used to encode and decode [characters](#character) as an [encoded](#encoding)
sequence of [code units](#code-unit).  [Encoding](#encoding) state is held in
each iterator instance and is made accessible via non-static member functions.

```C++
template<typename T> concept bool TextIterator() {
  return ranges::Iterator<T>()
      && TextEncoding<encoding_type_t<T>>()
      && TextEncodingState<typename T::state_type>()
      && requires (const T ct) {
           { ct.state() } noexcept
               -> const typename encoding_type_t<T>::state_type&;
         };
}
```

### Concept TextSentinel
The `TextSentinel` concept specifies requirements of types that are used to
mark the end of a range of encoded [characters](#character).  A type T that
satisfies `TextIterator` also satisfies `TextSentinel<T>` there by enabling
`TextIterator` types to be used as sentinels.

```C++
template<typename T, typename I> concept bool TextSentinel() {
  return ranges::Sentinel<T, I>()
      && TextIterator<I>();
}
```

### Concept TextOutputIterator
The `TextOutputIterator` concept refines `TextIterator` with a requirement that
the type also satisfy `ranges::OutputIterator` for the character type of the
associated encoding.

```C++
template<typename T> concept bool TextOutputIterator() {
  return TextIterator<I>();
      && ranges::OutputIterator<T, character_type_t<encoding_type_t<T>>>();
}
```

### Concept TextInputIterator
The `TextInputIterator` concept refines `TextIterator` with a requirement that
the type also satisfy `ranges::InputIterator` and that the iterator value type
satisfy `Character`.


```C++
template<typename T> concept bool TextInputIterator() {
  return TextIterator<T>()
      && ranges::InputIterator<T>()
      && Character<ranges::value_type_t<T>>();
}
```

### Concept TextForwardIterator
The `TextForwardIterator` concept refines `TextInputIterator` with a requirement
that the type also satisfy `ranges::ForwardIterator`.

```C++
template<typename T> concept bool TextForwardIterator() {
  return TextInputIterator<T>()
      && ranges::ForwardIterator<T>();
}
```

### Concept TextBidirectionalIterator
The `TextBidirectionalIterator` concept refines `TextForwardIterator` with a
requirement that the type also satisfy `ranges::BidirectionalIterator`.

```C++
template<typename T> concept bool TextBidirectionalIterator() {
  return TextForwardIterator<T>()
      && ranges::BidirectionalIterator<T>();
}
```

### Concept TextRandomAccessIterator
The `TextRandomAccessIterator` concept refines `TextBidirectionalIterator` with
a requirement that the type also satisfy `ranges::RandomAccessIterator`.

```C++
template<typename T> concept bool TextRandomAccessIterator() {
  return TextBidirectionalIterator<T>()
      && ranges::RandomAccessIterator<T>();
}
```

### Concept TextView
The `TextView` concept specifies requirements of types that provide view access
to an underlying [code unit](#code-unit) range.  Such types satisfy
`ranges::View`, provide iterators that satisfy `TextIterator`, define member
types that identify the [encoding](#encoding), encoding state, and underlying
[code unit](#code-unit) range and iterator types.  Non-static member functions
are provided to access the underlying [code unit](#code-unit) range and initial
[encoding](#encoding) state.

Types that satisfy `TextView` do not own the underlying [code unit](#code-unit)
range and are copyable in constant time.  The lifetime of the underlying range
must exceed the lifetime of referencing `TextView` objects.

```C++
template<typename T> concept bool TextView() {
  return ranges::View<T>()
      R& TextIterator<ranges::iterator_t<T>>()
      && TextEncoding<encoding_type_t<T>>()
      && ranges::View<typename T::view_type>()
      && TextEncodingState<typename T::state_type>()
      && CodeUnitIterator<code_unit_iterator_t<T>>()
      R& requires (T t, const T ct) {
           { t.base() } noexcept
               -> typename T::view_type&;
           { ct.base() } noexcept
               -> const typename T::view_type&;
           { ct.initial_state() } noexcept
               -> const typename T::state_type&;
         };
}
```

### Concept TextInputView
The `TextInputView` concept refines `TextView` with a requirement that the
view's iterator type also satisfy `TextInputIterator`.

```C++
template<typename T> concept bool TextInputView() {
  return TextView<T>()
      && TextInputIterator<ranges::iterator_t<T>>();
}
```

### Concept TextForwardView
The `TextForwardView` concept refines `TextInputView` with a requirement that
the view's iterator type also satisfy `TextForwardIterator`.

```C++
template<typename T> concept bool TextForwardView() {
  return TextInputView<T>()
      && TextForwardIterator<ranges::iterator_t<T>>();
}
```

### Concept TextBidirectionalView
The `TextBidirectionalView` concept refines `TextForwardView` with a requirement
that the view's iterator type also satisfy `TextBidirectionalIterator`.

```C++
template<typename T> concept bool TextBidirectionalView() {
  return TextForwardView<T>()
      && TextBidirectionalIterator<ranges::iterator_t<T>>();
}
```

### Concept TextRandomAccessView
The `TextRandomAccessView` concept refines `TextBidirectionalView` with a
requirement that the view's iterator type also satisfy
`TextRandomAccessIterator`.

```C++
template<typename T> concept bool TextRandomAccessView() {
  return TextBidirectionalView<T>()
      && TextRandomAccessIterator<ranges::iterator_t<T>>();
}
```

## Type traits

- [code_unit_type_t](#code_unit_type_t)
- [code_point_type_t](#code_point_type_t)
- [character_set_type_t](#character_set_type_t)
- [character_type_t](#character_type_t)
- [encoding_type_t](#encoding_type_t)
- [default_encoding_type_t](#default_encoding_type_t)

### code_unit_type_t

The `code_unit_type_t` type alias template provides convenient means for
selecting the associated [code unit](#code-unit) type of some other type,
such as an [encoding](#encoding) type that satisfies `TextEncoding`.  The
aliased type is the same as `typename T::code_unit_type`.

```C++
template<typename T>
  using code_unit_type_t = /* implementation-defined */ ;
```

### code_point_type_t

The `code_point_type_t` type alias template provides convenient means for
selecting the associated [code point](#code-point) type of some other type,
such as a type that satisfies `CharacterSet` or `Character`.  The aliased
type is the same as `typename T::code_point_type`.

```C++
template<typename T>
  using code_point_type_t = /* implementation-defined */ ;
```

### character_set_type_t

The `character_set_type_t` type alias template provides convenient means for
selecting the associated [character set](#character-set) type of some other
type, such as a type that satisfies `Character`.  The aliased type is the same
as `typename T::character_set_type`.

```C++
template<typename T>
  using character_set_type_t = /* implementation-defined */ ;
```

### character_type_t

The `character_type_t` type alias template provides convenient means for
selecting the associated [character](#character) type of some other type, such
as a type that satisfies `TextEncoding`.  The aliased type is the same as
`typename T::character_type`.

```C++
template<typename T>
  using character_type_t = /* implementation-defined */ ;
```

### encoding_type_t

The `encoding_type_t` type alias template provides convenient means for
selecting the associated [encoding](#encoding) type of some other type, such
as a type that satisfies `TextIterator` or `TextView`.  The aliased type is the
same as `typename T::encoding_type`.

```C++
template<typename T>
  using encoding_type_t = /* implementation-defined */ ;
```

### default_encoding_type_t

The `default_encoding_type_t` type alias template resolves to the default
[encoding](#encoding) type, if any, for a given type, such as a type that
satisfies `CodeUnit`.  Specializations are provided for the following
cv-unqualified and reference removed fundamental types.  Otherwise, the alias
will attempt to resolve against a `default_encoding_type` member type.

When `std::remove_cv_t<std::remove_reference_t<T>>` is ... | the default encoding is ...
---------------------------------------------------------- | ---------------------------
`char` | `execution_character_encoding`
`wchar_t` | `execution_wide_character_encoding`
`char16_t` | `char16_character_encoding`
`char32_t` | `char32_character_encoding`

```C++
template<typename T>
  using default_encoding_type_t = /* implementation-defined */ ;
```

## Character sets

- [Class any_character_set](#class-any_character_set)
- [Class basic_execution_character_set]
  (#class-basic_execution_character_set)
- [Class basic_execution_wide_character_set]
  (#class-basic_execution_wide_character_set)
- [Class unicode_character_set](#class-unicode_character_set)
- [Character set type aliases](#character-set-type-aliases)

### Class any_character_set

The `any_character_set` class provides a generic [character set](#character-set)
type used when a specific [character set](#character-set) type is unknown or
when the ability to switch between specific [character sets](#character-set)
is required.  This class satisfies the `CharacterSet` concept and has an
implementation defined `code_point_type` that is able to represent
[code point](#code-point) values from all of the implementation provided
[character set](#character-set) types.

```C++
class any_character_set {
public:
  using code_point_type = /* implementation-defined */;

  static const char* get_name() noexcept {
    return "any_character_set";
  }
};
```

### Class basic_execution_character_set

The `basic_execution_character_set` class represents the basic execution
character set specified in `[lex.charset]p3` of the [C++11][ISO/IEC 14882:2011]
standard.  This class satisfies the `CharacterSet` concept and has a
`code_point_type` member type that aliases `char`.

```C++
class basic_execution_character_set {
public:
  using code_point_type = char;

  static const char* get_name() noexcept {
    return "basic_execution_character_set";
  }
};
```

### Class basic_execution_wide_character_set

The `basic_execution_wide_character_set` class represents the basic execution
wide character set specified in `[lex.charset]p3` of the
[C++11][ISO/IEC 14882:2011] standard.  This class satisfies the `CharacterSet`
concept and has a `code_point_type` member type that aliases `wchar_t`.

```C++
class basic_execution_wide_character_set {
public:
  using code_point_type = wchar_t;

  static const char* get_name() noexcept {
    return "basic_execution_wide_character_set";
  }
};
```

### Class unicode_character_set

The `unicode_character_set` class represents the [Unicode]
[character sets](#character-set).  This class satisfies the `CharacterSet`
concept and has a `code_point_type` member type that aliases `char32_t`.

```C++
class unicode_character_set {
public:
  using code_point_type = char32_t;

  static const char* get_name() noexcept {
    return "unicode_character_set";
  }
};
```

### Character set type aliases

The `execution_character_set`,
`execution_wide_character_set`, and
`universal_character_set` type aliases reflect the implementation
defined execution, wide execution, and universal
[character sets](#character-set) specified in `[lex.charset]p2-3` of the C++
standard.

The [character set](#character-set) aliased by `execution_character_set` must be
a superset of the `basic_execution_character_set`
[character set](#character-set).  This alias refers to the
[character set](#character-set) that the compiler assumes during
translation; the [character set](#character-set) that the compiler uses when
translating [characters](#character) specified by universal-character-name
designators in ordinary string literals, not the locale sensitive run-time
execution [character set](#character-set).

The [character set](#character-set) aliased by `execution_wide_character_set`
must be a superset of the `basic_execution_wide_character_set`
[character set](#character-set).  This alias refers to the
[character set](#character-set)  that the compiler assumes during
translation; the [character set](#character-set) that the compiler uses when
translating [characters](#character) specified by universal-character-name
designators in wide string literals, not the locale sensitive run-time
execution wide [character set](#character-set).

The [character set](#character-set) aliased by `universal_character_set` must
be a superset of the `unicode_character_set` [character set](#character-set).

```C++
using execution_character_set = /* implementation-defined */ ;
using execution_wide_character_set = /* implementation-defined */ ;
using universal_character_set = /* implementation-defined */ ;
```

## Character set identification

- [Class character_set_id](#class-character_set_id)
- [get_character_set_id](#get_character_set_id)

### Class character_set_id

The `character_set_id` class provides unique, opaque values used to identify
[character sets](#character-set) at run-time.  Values of this type are produced
by `get_character_set_id()` and can be passed to `get_character_set_info()` to
obtain [character set](#character-set) information.  Values of this type are
copy constructible, copy assignable, equality comparable, and strictly totally
ordered.

```C++
class character_set_id {
public:
  character_set_id() = delete;

  friend bool operator==(character_set_id lhs, character_set_id rhs) noexcept;
  friend bool operator!=(character_set_id lhs, character_set_id rhs) noexcept;

  friend bool operator<(character_set_id lhs, character_set_id rhs) noexcept;
  friend bool operator>(character_set_id lhs, character_set_id rhs) noexcept;
  friend bool operator<=(character_set_id lhs, character_set_id rhs) noexcept;
  friend bool operator>=(character_set_id lhs, character_set_id rhs) noexcept;
};
```

### get_character_set_id

`get_character_set_id()` returns a unique, opaque value for the
[character set](#character-set) type specified by the template parameter.

```C++
template<CharacterSet CST>
  inline character_set_id get_character_set_id();
```

## Character set information

- [Class character_set_info](#class-character_set_info)
- [get_character_set_info](#get_character_set_info)

### Class character_set_info

The `character_set_info` class stores information about a
[character set](#character-set).  Values of this type are produced by the
`get_character_set_info()` functions based on a [character set](#character-set)
type or ID.

```C++
class character_set_info {
public:
  character_set_info() = delete;

  character_set_id get_id() const noexcept;

  const char* get_name() const noexcept;

private:
  character_set_id id; // exposition only
};
```

### get_character_set_info

The `get_character_set_info()` functions return a reference to a
`character_set_info` object based on a [character set](#character-set) type or
ID.

```C++
const character_set_info& get_character_set_info(character_set_id id);

template<CharacterSet CST>
  inline const character_set_info& get_character_set_info();
```

## Characters

- [Class template character](#class-template-character)

### Class template character

Objects of `character` class template specialization type define a
[character](#character) via the association of a [code point](#code-point)
value and a [character set](#character-set).  The specialization provided for
the `any_character_set` type is used to maintain a dynamic
[character set](#character-set) association while specializations for other
[character sets](#character-set) specify a static association.  These types
satisfy the `Character` concept and are default constructible, copy
constructible, copy assignable, and equality comparable.  Member functions
provide access to the [code point](#code-point) and
[character set](#character-set) ID values for the represented
[character](#character).  Default constructed objects represent a null
[character](#character) using a zero initialized [code point](#code-point)
value.

Objects with different [character set](#character-set) type are not equality
comparable with the exception that objects with a static
[character set](#character-set) type of `any_character_set` are comparable with
objects with any static [character set](#character-set) type.  In this case,
objects compare equally if and only if their [character set](#character-set)
ID and [code point](#code-point) values match.  Equality comparison between
objects with different static [character set](#character-set) type is not
implemented to avoid potentially costly unintended implicit transcoding between
[character sets](#character-set).

```C++
template<CharacterSet CST>
class character {
public:
  using character_set_type = CST;
  using code_point_type = code_point_type_t<character_set_type>;

  character() = default;
  explicit character(code_point_type code_point) noexcept;

  friend bool operator==(const character &lhs,
                         const character &rhs) noexcept;
  friend bool operator!=(const character &lhs,
                         const character &rhs) noexcept;

  void set_code_point(code_point_type code_point);
  code_point_type get_code_point() const noexcept;

  static character_set_id get_character_set_id();

private:
  code_point_type code_point; // exposition only
};

template<>
class character<any_character_set> {
public:
  using character_set_type = any_character_set;
  using code_point_type = code_point_type_t<character_set_type>;

  character() = default;
  explicit character(code_point_type code_point) noexcept;
  character(character_set_id cs_id, code_point_type code_point) noexcept;

  friend bool operator==(const character &lhs,
                         const character &rhs) noexcept;
  friend bool operator!=(const character &lhs,
                         const character &rhs) noexcept;

  void set_code_point(code_point_type code_point);
  code_point_type get_code_point() const noexcept;

  void set_character_set_id(character_set_id new_cs_id) noexcept;
  character_set_id get_character_set_id() const noexcept;

private:
  character_set_id cs_id;     // exposition only
  code_point_type code_point; // exposition only
};

template<CharacterSet CST>
  bool operator==(const character<any_character_set> &lhs,
                  const character<CST> &rhs);
template<CharacterSet CST>
  bool operator==(const character<CST> &lhs,
                  const character<any_character_set> &rhs);
template<CharacterSet CST>
  bool operator!=(const character<any_character_set> &lhs,
                  const character<CST> &rhs);
template<CharacterSet CST>
  bool operator!=(const character<CST> &lhs,
                  const character<any_character_set> &rhs);
```

## Encodings

- [class trivial_encoding_state](#class-trivial_encoding_state)
- [class trivial_encoding_state_transition]
  (#class-trivial_encoding_state_transition)
- [Class basic_execution_character_encoding]
  (#class-basic_execution_character_encoding)
- [Class basic_execution_wide_character_encoding]
  (#class-basic_execution_wide_character_encoding)
- [Class iso_10646_wide_character_encoding]
  (#class-iso_10646_wide_character_encoding)
- [Class utf8_encoding](#class-utf8_encoding)
- [Class utf8bom_encoding](#class-utf8bom_encoding)
- [Class utf16_encoding](#class-utf16_encoding)
- [Class utf16be_encoding](#class-utf16be_encoding)
- [Class utf16le_encoding](#class-utf16le_encoding)
- [Class utf16bom_encoding](#class-utf16bom_encoding)
- [Class utf32_encoding](#class-utf32_encoding)
- [Class utf32be_encoding](#class-utf32be_encoding)
- [Class utf32le_encoding](#class-utf32le_encoding)
- [Class utf32bom_encoding](#class-utf32bom_encoding)
- [Encoding type aliases](#encoding-type-aliases)

### Class trivial_encoding_state

The `trivial_encoding_state` class is an empty class used by stateless
[encodings](#encoding) to implement the parts of the generic
[encoding](#encoding) interfaces necessary to support stateful
[encodings](#encoding).

```C++
class trivial_encoding_state {};
```

### Class trivial_encoding_state_transition

The `trivial_encoding_state_transition` class is an empty class used by
stateless [encodings](#encoding) to implement the parts of the generic
[encoding](#encoding) interfaces necessary to support stateful
[encodings](#encoding) that support non-[code-point](#code-point)
encoding [code unit](#code-unit) sequences.

```C++
class trivial_encoding_state_transition {};
```

### Class basic_execution_character_encoding

The `basic_execution_character_encoding` class implements support for the
[encoding](#encoding) used for ordinary string literals limited to support
for the basic execution character set as defined in `[lex.charset]p3` of
the C++ standard.

This [encoding](#encoding) is trivial, stateless, fixed width, supports
random access decoding, and has a [code unit](#code-unit) of type `char`.

```C++
class basic_execution_character_encoding {
public:
  using state_type = trivial_encoding_state;
  using state_transition_type = trivial_encoding_state_transition;
  using character_type = character<basic_execution_character_set>;
  using code_unit_type = char;

  static constexpr int min_code_units = 1;
  static constexpr int max_code_units = 1;

  static const state_type& initial_state() noexcept;

  template<CodeUnitOutputIterator<code_unit_type> CUIT>
    static void encode_state_transition(state_type &state,
                                        CUIT &out,
                                        const state_transition_type &stt,
                                        int &encoded_code_units)

  template<CodeUnitOutputIterator<code_unit_type> CUIT>
    static void encode(state_type &state,
                       CUIT &out,
                       character_type c,
                       int &encoded_code_units)

  template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::Convertible<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool decode(state_type &state,
                       CUIT &in_next,
                       CUST in_end,
                       character_type &c,
                       int &decoded_code_units)

  template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::Convertible<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool rdecode(state_type &state,
                        CUIT &in_next,
                        CUST in_end,
                        character_type &c,
                        int &decoded_code_units)
};
```

### Class basic_execution_wide_character_encoding

The `basic_execution_wide_character_encoding` class implements support for the
[encoding](#encoding) used for wide string literals limited to support for the
basic execution wide-character set as defined in `[lex.charset]p3` of
the C++ standard.

This [encoding](#encoding) is trivial, stateless, fixed width, supports
random access decoding, and has a [code unit](#code-unit) of type `wchar_t`.

```C++
class basic_execution_wide_character_encoding {
public:
  using state_type = trivial_encoding_state;
  using state_transition_type = trivial_encoding_state_transition;
  using character_type = character<basic_execution_wide_character_set>;
  using code_unit_type = wchar_t;

  static constexpr int min_code_units = 1;
  static constexpr int max_code_units = 1;

  static const state_type& initial_state() noexcept;

  template<CodeUnitOutputIterator<code_unit_type> CUIT>
    static void encode_state_transition(state_type &state,
                                        CUIT &out,
                                        const state_transition_type &stt,
                                        int &encoded_code_units)

  template<CodeUnitOutputIterator<code_unit_type> CUIT>
    static void encode(state_type &state,
                       CUIT &out,
                       character_type c,
                       int &encoded_code_units)

  template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::Convertible<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool decode(state_type &state,
                       CUIT &in_next,
                       CUST in_end,
                       character_type &c,
                       int &decoded_code_units)

  template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::Convertible<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool rdecode(state_type &state,
                        CUIT &in_next,
                        CUST in_end,
                        character_type &c,
                        int &decoded_code_units)
};
```

### Class iso_10646_wide_character_encoding

The `iso_10646_wide_character_encoding` class is only defined
when the `__STDC_ISO_10646__` macro is defined.

The `iso_10646_wide_character_encoding` class implements support for the
[encoding](#encoding) used for wide string literals when that
[encoding](#encoding) uses the [Unicode] [character set](#character-set) and
`wchar_t` is large enough to store the [code point](#code-point) values of
all [characters](#character) defined by the version of the [Unicode] standard
indicated by the value of the `__STDC_ISO_10646__` macro as specified in
`[cpp.predefined]p2` of the C++ standard.

This [encoding](#encoding) is trivial, stateless, fixed width, supports random
access decoding, and has a [code unit](#code-unit) of type `wchar_t`.

```C++
#if defined(__STDC_ISO_10646__)
class iso_10646_wide_character_encoding {
public:
  using state_type = trivial_encoding_state;
  using state_transition_type = trivial_encoding_state_transition;
  using character_type = character<unicode_character_set>;
  using code_unit_type = wchar_t;

  static constexpr int min_code_units = 1;
  static constexpr int max_code_units = 1;

  static const state_type& initial_state() noexcept;

  template<CodeUnitOutputIterator<code_unit_type> CUIT>
    static void encode_state_transition(state_type &state,
                                        CUIT &out,
                                        const state_transition_type &stt,
                                        int &encoded_code_units)

  template<CodeUnitOutputIterator<code_unit_type> CUIT>
    static void encode(state_type &state,
                       CUIT &out,
                       character_type c,
                       int &encoded_code_units)

  template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::Convertible<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool decode(state_type &state,
                       CUIT &in_next,
                       CUST in_end,
                       character_type &c,
                       int &decoded_code_units)

  template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::Convertible<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool rdecode(state_type &state,
                        CUIT &in_next,
                        CUST in_end,
                        character_type &c,
                        int &decoded_code_units)
};
#endif // __STDC_ISO_10646__
```

### Class utf8_encoding

The `utf8_encoding` class implements support for the [Unicode] UTF-8
[encoding](#encoding).

This [encoding](#encoding) is stateless, variable width, supports bidirectional
decoding, and has a [code unit](#code-unit) of type `char`.

```C++
class utf8_encoding {
public:
  using state_type = trivial_encoding_state;
  using state_transition_type = trivial_encoding_state_transition;
  using character_type = character<unicode_character_set>;
  using code_unit_type = char;

  static constexpr int min_code_units = 1;
  static constexpr int max_code_units = 4;

  static const state_type& initial_state() noexcept;

  template<CodeUnitOutputIterator<std::make_unsigned_t<code_unit_type>> CUIT>
    static void encode_state_transition(state_type &state,
                                        CUIT &out,
                                        const state_transition_type &stt,
                                        int &encoded_code_units)

  template<CodeUnitOutputIterator<std::make_unsigned_t<code_unit_type>> CUIT>
    static void encode(state_type &state,
                       CUIT &out,
                       character_type c,
                       int &encoded_code_units)

  template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::Convertible<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool decode(state_type &state,
                       CUIT &in_next,
                       CUST in_end,
                       character_type &c,
                       int &decoded_code_units)

  template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::Convertible<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool rdecode(state_type &state,
                        CUIT &in_next,
                        CUST in_end,
                        character_type &c,
                        int &decoded_code_units)
};
```

### Class utf8bom_encoding

The `utf8bom_encoding` class implements support for the [Unicode] UTF-8
[encoding](#encoding) with a byte order mark (BOM).

This [encoding](#encoding) is stateful, variable width, supports bidirectional
decoding, and has a [code unit](#code-unit) of type `char`.

This [encoding](#encoding) defines a state transition class that enables
forcing or suppressing the encoding of a BOM, or influencing whether a decoded
BOM [code unit](#code-unit) sequence represents a BOM or a
[code point](#code-point).

```C++
class utf8bom_encoding_state {
  /* implementation-defined */
};

class utf8bom_encoding_state_transition {
public:
  static utf8bom_encoding_state_transition to_initial_state() noexcept;
  static utf8bom_encoding_state_transition to_bom_written_state() noexcept;
  static utf8bom_encoding_state_transition to_assume_bom_written_state() noexcept;
};

class utf8bom_encoding {
public:
  using state_type = utf8bom_encoding_state;
  using state_transition_type = utf8bom_encoding_state_transition;
  using character_type = character<unicode_character_set>;
  using code_unit_type = char;

  static constexpr int min_code_units = 1;
  static constexpr int max_code_units = 4;

  static const state_type& initial_state() noexcept;

  template<CodeUnitOutputIterator<std::make_unsigned_t<code_unit_type>> CUIT>
    static void encode_state_transition(state_type &state,
                                        CUIT &out,
                                        const state_transition_type &stt,
                                        int &encoded_code_units)

  template<CodeUnitOutputIterator<std::make_unsigned_t<code_unit_type>> CUIT>
    static void encode(state_type &state,
                       CUIT &out,
                       character_type c,
                       int &encoded_code_units)

  template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::Convertible<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool decode(state_type &state,
                       CUIT &in_next,
                       CUST in_end,
                       character_type &c,
                       int &decoded_code_units)

  template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::Convertible<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool rdecode(state_type &state,
                        CUIT &in_next,
                        CUST in_end,
                        character_type &c,
                        int &decoded_code_units)
};
```

### Class utf16_encoding

The `utf16_encoding` class implements support for the [Unicode] UTF-16
[encoding](#encoding).

This [encoding](#encoding) is stateless, variable width, supports bidirectional
decoding, and has a [code unit](#code-unit) of type `char16_t`.

```C++
class utf16_encoding {
public:
  using state_type = trivial_encoding_state;
  using state_transition_type = trivial_encoding_state_transition;
  using character_type = character<unicode_character_set>;
  using code_unit_type = char16_t;

  static constexpr int min_code_units = 1;
  static constexpr int max_code_units = 2;

  static const state_type& initial_state() noexcept;

  template<CodeUnitOutputIterator<code_unit_type> CUIT>
    static void encode_state_transition(state_type &state,
                                        CUIT &out,
                                        const state_transition_type &stt,
                                        int &encoded_code_units)

  template<CodeUnitOutputIterator<code_unit_type> CUIT>
    static void encode(state_type &state,
                       CUIT &out,
                       character_type c,
                       int &encoded_code_units)

  template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::Convertible<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool decode(state_type &state,
                       CUIT &in_next,
                       CUST in_end,
                       character_type &c,
                       int &decoded_code_units)

  template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::Convertible<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool rdecode(state_type &state,
                        CUIT &in_next,
                        CUST in_end,
                        character_type &c,
                        int &decoded_code_units)
};
```

### Class utf16be_encoding

The `utf16be_encoding` class implements support for the [Unicode] UTF-16
big-endian [encoding](#encoding).

This [encoding](#encoding) is stateless, variable width, supports bidirectional
decoding, and has a [code unit](#code-unit) of type `char`.

```C++
class utf16be_encoding {
public:
  using state_type = trivial_encoding_state;
  using state_transition_type = trivial_encoding_state_transition;
  using character_type = character<unicode_character_set>;
  using code_unit_type = char;

  static constexpr int min_code_units = 2;
  static constexpr int max_code_units = 4;

  static const state_type& initial_state() noexcept;

  template<CodeUnitOutputIterator<code_unit_type> CUIT>
    static void encode_state_transition(state_type &state,
                                        CUIT &out,
                                        const state_transition_type &stt,
                                        int &encoded_code_units)

  template<CodeUnitOutputIterator<code_unit_type> CUIT>
    static void encode(state_type &state,
                       CUIT &out,
                       character_type c,
                       int &encoded_code_units)

  template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::Convertible<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool decode(state_type &state,
                       CUIT &in_next,
                       CUST in_end,
                       character_type &c,
                       int &decoded_code_units)

  template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::Convertible<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool rdecode(state_type &state,
                        CUIT &in_next,
                        CUST in_end,
                        character_type &c,
                        int &decoded_code_units)
};
```

### Class utf16le_encoding

The `utf16le_encoding` class implements support for the [Unicode] UTF-16
little-endian [encoding](#encoding).

This [encoding](#encoding) is stateless, variable width, supports bidirectional
decoding, and has a [code unit](#code-unit) of type `char`.

```C++
class utf16le_encoding {
public:
  using state_type = trivial_encoding_state;
  using state_transition_type = trivial_encoding_state_transition;
  using character_type = character<unicode_character_set>;
  using code_unit_type = char;

  static constexpr int min_code_units = 2;
  static constexpr int max_code_units = 4;

  static const state_type& initial_state() noexcept;

  template<CodeUnitOutputIterator<code_unit_type> CUIT>
    static void encode_state_transition(state_type &state,
                                        CUIT &out,
                                        const state_transition_type &stt,
                                        int &encoded_code_units)

  template<CodeUnitOutputIterator<code_unit_type> CUIT>
    static void encode(state_type &state,
                       CUIT &out,
                       character_type c,
                       int &encoded_code_units)

  template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::Convertible<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool decode(state_type &state,
                       CUIT &in_next,
                       CUST in_end,
                       character_type &c,
                       int &decoded_code_units)

  template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::Convertible<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool rdecode(state_type &state,
                        CUIT &in_next,
                        CUST in_end,
                        character_type &c,
                        int &decoded_code_units)
};
```

### Class utf16bom_encoding

The `utf16bom_encoding_state` class implements support for the [Unicode] UTF-16
[encoding](#encoding) with a byte order mark (BOM).

This [encoding](#encoding) is stateful, variable width, supports bidirectional
decoding, and has a [code unit](#code-unit) of type `char`.

This [encoding](#encoding) defines a state transition class that enables
forcing or suppressing the encoding of a BOM, or influencing whether a decoded
BOM [code unit](#code-unit) sequence represents a BOM or a
[code point](#code-point).

```C++
class utf16bom_encoding_state {
  /* implementation-defined */
};

class utf16bom_encoding_state_transition {
public:
  static utf16bom_encoding_state_transition to_initial_state() noexcept;
  static utf16bom_encoding_state_transition to_bom_written_state() noexcept;
  static utf16bom_encoding_state_transition to_be_bom_written_state() noexcept;
  static utf16bom_encoding_state_transition to_le_bom_written_state() noexcept;
  static utf16bom_encoding_state_transition to_assume_bom_written_state() noexcept;
  static utf16bom_encoding_state_transition to_assume_be_bom_written_state() noexcept;
  static utf16bom_encoding_state_transition to_assume_le_bom_written_state() noexcept;
};

class utf16bom_encoding {
public:
  using state_type = utf16bom_encoding_state;
  using state_transition_type = utf16bom_encoding_state_transition;
  using character_type = character<unicode_character_set>;
  using code_unit_type = char;

  static constexpr int min_code_units = 2;
  static constexpr int max_code_units = 4;

  static const state_type& initial_state() noexcept;

  template<CodeUnitOutputIterator<code_unit_type> CUIT>
    static void encode_state_transition(state_type &state,
                                        CUIT &out,
                                        const state_transition_type &stt,
                                        int &encoded_code_units)

  template<CodeUnitOutputIterator<code_unit_type> CUIT>
    static void encode(state_type &state,
                       CUIT &out,
                       character_type c,
                       int &encoded_code_units)

  template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::Convertible<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool decode(state_type &state,
                       CUIT &in_next,
                       CUST in_end,
                       character_type &c,
                       int &decoded_code_units)

  template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::Convertible<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool rdecode(state_type &state,
                        CUIT &in_next,
                        CUST in_end,
                        character_type &c,
                        int &decoded_code_units)
};
```

### Class utf32_encoding

The `utf32_encoding` class implements support for the [Unicode] UTF-32
[encoding](#encoding).

This [encoding](#encoding) is trivial, stateless, fixed width, supports
random access decoding, and has a [code unit](#code-unit) of type `char32_t`.

```C++
class utf32_encoding {
public:
  using state_type = trivial_encoding_state;
  using state_transition_type = trivial_encoding_state_transition;
  using character_type = character<unicode_character_set>;
  using code_unit_type = char32_t;

  static constexpr int min_code_units = 1;
  static constexpr int max_code_units = 1;

  static const state_type& initial_state() noexcept;

  template<CodeUnitOutputIterator<code_unit_type> CUIT>
    static void encode_state_transition(state_type &state,
                                        CUIT &out,
                                        const state_transition_type &stt,
                                        int &encoded_code_units)

  template<CodeUnitOutputIterator<code_unit_type> CUIT>
    static void encode(state_type &state,
                       CUIT &out,
                       character_type c,
                       int &encoded_code_units)

  template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::Convertible<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool decode(state_type &state,
                       CUIT &in_next,
                       CUST in_end,
                       character_type &c,
                       int &decoded_code_units)

  template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::Convertible<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool rdecode(state_type &state,
                        CUIT &in_next,
                        CUST in_end,
                        character_type &c,
                        int &decoded_code_units)
};
```

### Class utf32be_encoding

The `utf32be_encoding` class implements support for the [Unicode] UTF-32
big-endian [encoding](#encoding).

This [encoding](#encoding) is stateless, fixed width, supports random access
decoding, and has a [code unit](#code-unit) of type `char`.

```C++
class utf32be_encoding {
public:
  using state_type = trivial_encoding_state;
  using state_transition_type = trivial_encoding_state_transition;
  using character_type = character<unicode_character_set>;
  using code_unit_type = char;

  static constexpr int min_code_units = 4;
  static constexpr int max_code_units = 4;

  static const state_type& initial_state() noexcept;

  template<CodeUnitOutputIterator<code_unit_type> CUIT>
    static void encode_state_transition(state_type &state,
                                        CUIT &out,
                                        const state_transition_type &stt,
                                        int &encoded_code_units)

  template<CodeUnitOutputIterator<code_unit_type> CUIT>
    static void encode(state_type &state,
                       CUIT &out,
                       character_type c,
                       int &encoded_code_units)

  template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::Convertible<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool decode(state_type &state,
                       CUIT &in_next,
                       CUST in_end,
                       character_type &c,
                       int &decoded_code_units)

  template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::Convertible<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool rdecode(state_type &state,
                        CUIT &in_next,
                        CUST in_end,
                        character_type &c,
                        int &decoded_code_units)
};
```

### Class utf32le_encoding

The `utf32le_encoding` class implements support for the [Unicode] UTF-32
little-endian [encoding](#encoding).

This [encoding](#encoding) is stateless, fixed width, supports random access
decoding, and has a [code unit](#code-unit) of type `char`.

```C++
class utf32le_encoding {
public:
  using state_type = trivial_encoding_state;
  using state_transition_type = trivial_encoding_state_transition;
  using character_type = character<unicode_character_set>;
  using code_unit_type = char;

  static constexpr int min_code_units = 4;
  static constexpr int max_code_units = 4;

  static const state_type& initial_state() noexcept;

  template<CodeUnitOutputIterator<code_unit_type> CUIT>
    static void encode_state_transition(state_type &state,
                                        CUIT &out,
                                        const state_transition_type &stt,
                                        int &encoded_code_units)

  template<CodeUnitOutputIterator<code_unit_type> CUIT>
    static void encode(state_type &state,
                       CUIT &out,
                       character_type c,
                       int &encoded_code_units)

  template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::Convertible<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool decode(state_type &state,
                       CUIT &in_next,
                       CUST in_end,
                       character_type &c,
                       int &decoded_code_units)

  template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::Convertible<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool rdecode(state_type &state,
                        CUIT &in_next,
                        CUST in_end,
                        character_type &c,
                        int &decoded_code_units)
};
```

### Class utf32bom_encoding

The `utf32bom_encoding` class implements support for the [Unicode] UTF-32
[encoding](#encoding) with a byte order mark (BOM).

This [encoding](#encoding) is stateful, variable width, supports bidirectional
decoding, and has a [code unit](#code-unit) of type `char`.

This [encoding](#encoding) defines a state transition class that enables
forcing or suppressing the encoding of a BOM, or influencing whether a decoded
BOM [code unit](#code-unit) sequence represents a BOM or a
[code point](#code-point).

```C++
class utf32bom_encoding_state {
  /* implementation-defined */
};

class utf32bom_encoding_state_transition {
public:
  static utf32bom_encoding_state_transition to_initial_state() noexcept;
  static utf32bom_encoding_state_transition to_bom_written_state() noexcept;
  static utf32bom_encoding_state_transition to_be_bom_written_state() noexcept;
  static utf32bom_encoding_state_transition to_le_bom_written_state() noexcept;
  static utf32bom_encoding_state_transition to_assume_bom_written_state() noexcept;
  static utf32bom_encoding_state_transition to_assume_be_bom_written_state() noexcept;
  static utf32bom_encoding_state_transition to_assume_le_bom_written_state() noexcept;
};

class utf32bom_encoding {
public:
  using state_type = utf32bom_encoding_state;
  using state_transition_type = utf32bom_encoding_state_transition;
  using character_type = character<unicode_character_set>;
  using code_unit_type = char;

  static constexpr int min_code_units = 4;
  static constexpr int max_code_units = 4;

  static const state_type& initial_state() noexcept;

  template<CodeUnitOutputIterator<code_unit_type> CUIT>
    static void encode_state_transition(state_type &state,
                                        CUIT &out,
                                        const state_transition_type &stt,
                                        int &encoded_code_units)

  template<CodeUnitOutputIterator<code_unit_type> CUIT>
    static void encode(state_type &state,
                       CUIT &out,
                       character_type c,
                       int &encoded_code_units)

  template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::Convertible<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool decode(state_type &state,
                       CUIT &in_next,
                       CUST in_end,
                       character_type &c,
                       int &decoded_code_units)

  template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::Convertible<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool rdecode(state_type &state,
                        CUIT &in_next,
                        CUST in_end,
                        character_type &c,
                        int &decoded_code_units)
};
```

### Encoding type aliases

The `execution_character_encoding`,
`execution_wide_character_encoding`,
`char8_character_encoding`,
`char16_character_encoding`, and
`char32_character_encoding` type aliases reflect the implementation
defined [encodings](#encoding) used for execution, wide execution, UTF-8,
char16_t, and char32_t string literals.

Each of these [encodings](#encoding) carries a compatibility requirement with
another [encoding](#encoding).  Decode compatibility is satisfied when the
following criteria is met.

1. Text encoded by the compatibility [encoding](#encoding) can be decoded
   by the aliased [encoding](#encoding).
2. Text encoded by the aliased [encoding](#encoding) can be decoded by the
   compatibility [encoding](#encoding) when encoded [characters](#character)
   are restricted to members of the [character set](#character-set) of the
   compatibility [encoding](#encoding).

These compatibility requirements allow implementation freedom to use
[encodings](#encoding) that provide features beyond the minimum requirements
imposed on the compatibility [encodings](#encoding) by the standard.  For
example, the [encoding](#encoding) aliased by `execution_character_encoding`
is allowed to support [characters](#character) that are not members of the
[character set](#character-set) of the `basic_execution_character_encoding`

The [encoding](#encoding) aliased by `execution_character_encoding` must be
decode compatible with the `basic_execution_character_encoding`
[encoding](#encoding).

The [encoding](#encoding) aliased by `execution_wide_character_encoding` must
be decode compatible with the
`basic_execution_wide_character_encoding` [encoding](#encoding).

The [encoding](#encoding) aliased by `char8_character_encoding` must be
decode compatible with the `utf8_encoding` [encoding](#encoding).

The [encoding](#encoding) aliased by `char16_character_encoding` must be
decode compatible with the `utf16_encoding` [encoding](#encoding).

The [encoding](#encoding) aliased by `char32_character_encoding` must be
decode compatible with the `utf32_encoding` [encoding](#encoding).

```C++
using execution_character_encoding = /* implementation-defined */ ;
using execution_wide_character_encoding = /* implementation-defined */ ;
using char8_character_encoding = /* implementation-defined */ ;
using char16_character_encoding = /* implementation-defined */ ;
using char32_character_encoding = /* implementation-defined */ ;
```

## Text iterators

- [Class template itext_iterator](#class-template-itext_iterator)
- [Class template itext_sentinel](#class-template-itext_sentinel)
- [Class template otext_iterator](#class-template-otext_iterator)
- [make_otext_iterator](#make_otext_iterator)

### Class template itext_iterator

Objects of `itext_iterator` class template specialization type provide a
standard iterator interface for enumerating the [characters](#character) encoded
by the associated [encoding](#encoding) `ET` in the [code unit](#code-unit)
sequence exposed by the associated view.  These types satisfy the
`TextInputIterator` concept and are default constructible, copy and move
constructible, copy and move assignable, and equality comparable.

These types also conditionally satisfy `ranges::ForwardIterator`,
`ranges::BidirectionalIterator`, and `ranges::RandomAccessIterator` depending
on traits of the associated [encoding](#encoding) `ET` and view `VT` as
described in the following table.

When `ET` and `ranges::iterator_t<VT>` satisfy ... | then `itext_iterator<ET, VT>` satisfies ...  | and `itext_iterator<ET, VT>::iterator_category` is ...
-------------------------------------------------- | -------------------------------------------- | ------------------------------------------------------
`TextDecoder` | `ranges::InputIterator` | `std::input_iterator_tag`
`TextForwardDecoder` | `ranges::ForwardIterator` | `std::forward_iterator_tag`
`TextBidirectionalDecoder` | `ranges::BidirectionalIterator` | `std::bidirectional_iterator_tag`
`TextRandomAccessDecoder` | `ranges::RandomAccessIterator` | `std::random_access_iterator_tag`

Member functions provide access to the stored [encoding](#encoding) state, the
underlying [code unit](#code-unit) iterator, and, when `ranges::ForwardIterator`
is satisified, the underlying [code unit](#code-unit) range for the current
[character](#character).  The underlying [code unit](#code-unit) range is
returned with an implementation defined type that satisfies `ranges::View`.
The `is_ok` member function returns true if the iterator is dereferenceable as
a result of having successfully decoded a [code point](#code-point) (This
predicate is used to distinguish between an input iterator that just
successfully decoded the last [code point](#code-point) in the
[code unit](#code-unit) stream as compared to one that was advanced after having
done so; in both cases, the underlying [code unit](#code-unit) input iterator
will compare equal to the end of the stream iterator).

`itext_iterator` is a [proxy iterator][P0022R1] since the value type is computed
and a meaningful reference type cannot be provided for the dereference and
subscript operators.  The `reference` member type is an alias of an
implementation defined proxy reference type, must satisfy `Character`, and must
support implicit conversion to `const value_type&`.

Note: Implementation of a reference proxy would be simplified if the
[operator dot proposal][P0252R0] is adopted.

```C++
template<TextEncoding ET, ranges::View VT>
  requires TextDecoder<
             ET,
             ranges::iterator_t<std::add_const_t<VT>>>()
class itext_iterator {
public:
  using encoding_type = ET;
  using view_type = VT;
  using state_type = typename encoding_type::state_type;

  using iterator = ranges::iterator_t<std::add_const_t<view_type>>;
  using iterator_category = /* implementation-defined */;
  using value_type = character_type_t<encoding_type>;
  using reference = value_type;
  using pointer = std::add_const_t<value_type>*;
  using difference_type = ranges::difference_type_t<iterator>;

  itext_iterator();

  itext_iterator(state_type state,
                 const view_type *view,
                 iterator first);

  reference operator*() const noexcept;
  pointer operator->() const noexcept;

  friend bool operator==(const itext_iterator &l, const itext_iterator &r);
  friend bool operator!=(const itext_iterator &l, const itext_iterator &r);

  friend bool operator<(const itext_iterator &l, const itext_iterator &r)
    requires TextRandomAccessDecoder<encoding_type, iterator>();
  friend bool operator>(const itext_iterator &l, const itext_iterator &r)
    requires TextRandomAccessDecoder<encoding_type, iterator>();
  friend bool operator<=(const itext_iterator &l, const itext_iterator &r)
    requires TextRandomAccessDecoder<encoding_type, iterator>();
  friend bool operator>=(const itext_iterator &l, const itext_iterator &r)
    requires TextRandomAccessDecoder<encoding_type, iterator>();

  itext_iterator& operator++();
  itext_iterator& operator++()
    requires TextForwardDecoder<encoding_type, iterator>();
  itext_iterator operator++(int);

  itext_iterator& operator--()
    requires TextBidirectionalDecoder<encoding_type, iterator>();
  itext_iterator operator--(int)
    requires TextBidirectionalDecoder<encoding_type, iterator>();

  itext_iterator& operator+=(difference_type n)
    requires TextRandomAccessDecoder<encoding_type, iterator>();
  itext_iterator& operator-=(difference_type n)
    requires TextRandomAccessDecoder<encoding_type, iterator>();

  friend itext_iterator operator+(itext_iterator l, difference_type n)
    requires TextRandomAccessDecoder<encoding_type, iterator>();
  friend itext_iterator operator+(difference_type n, itext_iterator r)
    requires TextRandomAccessDecoder<encoding_type, iterator>();

  friend itext_iterator operator-(itext_iterator l, difference_type n)
    requires TextRandomAccessDecoder<encoding_type, iterator>();
  friend difference_type operator-(const itext_iterator &l,
                                   const itext_iterator &r)
    requires TextRandomAccessDecoder<encoding_type, iterator>();

  reference operator[](difference_type n) const
    requires TextRandomAccessDecoder<encoding_type, iterator>();

  const state_type& state() const noexcept;

  const iterator& base() const noexcept;

  /* implementation-defined */ base_range() const noexcept
    requires TextDecoder<encoding_type, iterator>()
          && ranges::ForwardIterator<iterator>();

  bool is_ok() const noexcept;

private:
  state_type base_state;  // exposition only
  iterator base_iterator; // exposition only
  bool ok;                // exposition only
};
```

### Class template itext_sentinel

Objects of `itext_sentinel` class template specialization type denote the end
of a range of text as delimited by a sentinel object for the underlying
[code unit](#code-unit) sequence.  These types satisfy the `TextSentinel`
concept and are default constructible, copy and move constructible, copy and
move assignable, and equality comparable.  All objects of the same
`itext_sentinel` type compare equally.  Member functions provide access to the
sentinel for the underlying [code unit](#code-unit) sequence.

Objects of these types are equality comparable to `itext_iterator` objects that
have matching [encoding](#encoding) and view types.

```C++
template<TextEncoding ET, ranges::View VT>
class itext_sentinel {
public:
  using view_type = VT;
  using sentinel = ranges::sentinel_t<std::add_const_t<view_type>>;

  itext_sentinel() = default;

  itext_sentinel(sentinel s);

  friend bool operator==(const itext_iterator<ET, VT> &ti,
                         const itext_sentinel &ts);
  friend bool operator!=(const itext_iterator<ET, VT> &ti,
                         const itext_sentinel &ts);
  friend bool operator==(const itext_sentinel &ts,
                         const itext_iterator<ET, VT> &ti);
  friend bool operator!=(const itext_sentinel &ts,
                         const itext_iterator<ET, VT> &ti);

  const sentinel& base() const noexcept;

private:
  sentinel base_sentinel; // exposition only
};
```

### Class template otext_iterator

Objects of `otext_iterator` class template specialization type provide a
standard iterator interface for encoding [characters](#character) in the form
implemented by the associated [encoding](#encoding) `ET`.  These types satisfy
the `TextOutputIterator` concept and are default constructible, copy and move
constructible, and copy and move assignable.

Member functions provide access to the stored encoding state and the underlying
[code unit](#code-unit) output iterator.

```C++
template<TextEncoding ET, CodeUnitOutputIterator<code_unit_type_t<ET>> CUIT>
class otext_iterator {
public:
  using encoding_type = ET;
  using state_type = typename ET::state_type;
  using state_transition_type = typename ET::state_transition_type;

  using iterator = CUIT;
  using iterator_category = std::output_iterator_tag;
  using value_type = character_type_t<encoding_type>;
  using reference = value_type&;
  using pointer = value_type*;
  using difference_type = ranges::difference_type_t<iterator>;

  otext_iterator();

  otext_iterator(state_type state, iterator current);

  otext_iterator& operator*() const noexcept;

  otext_iterator& operator++() noexcept;
  otext_iterator& operator++(int) noexcept;

  otext_iterator& operator=(const state_transition_type &stt);
  otext_iterator& operator=(const character_type_t<encoding_type> &value);

  const state_type& state() const noexcept;

  const iterator& base() const noexcept;

private:
  state_type base_state;  // exposition only
  iterator base_iterator; // exposition only
};
```

### make_otext_iterator

The `make_otext_iterator` functions enable convenient construction of
`otext_iterator` objects via type deduction of the underlying
[code unit](#code-unit) output iterator type.  Overloads are provided to enable
construction with an explicit [encoding](#encoding) state or the implicit
[encoding](#encoding) dependent initial state.

```C++
template<TextEncoding ET, CodeUnitOutputIterator<code_unit_type_t<ET>> IT>
  auto make_otext_iterator(typename ET::state_type state, IT out)
  -> otext_iterator<ET, IT>;
template<TextEncoding ET, CodeUnitOutputIterator<code_unit_type_t<ET>> IT>
  auto make_otext_iterator(IT out)
  -> otext_iterator<ET, IT>;
```

## Text view

- [Class template basic_text_view](#class-template-basic_text_view)
- [Text view type aliases](#text-view-type-aliases)
- [make_text_view](#make_text_view)

### Class template basic_text_view

Objects of `basic_text_view` class template specialization type provide a view
of an underlying [code unit](#code-unit) sequence as a sequence of
[characters](#character).  These types satisfy the `TextView` concept and are
default constructible, copy and move constructible, and copy and move
assignable.  Member functions provide access to the underlying
[code unit](#code-unit) sequence and the initial [encoding](#encoding)
state for the range.

Constructors are provided to construct objects of these types from objects of
the underlying [code unit](#code-unit) view type and from iterator and sentinel
pairs, iterator and difference pairs, and range or `std::basic_string` types for
which an object of the underlying [code unit](#code-unit) view type can be
constructed.  For each of these, overloads are provided to construct the view
with an explicit [encoding](#encoding) state or with an implicit initial
[encoding](#encoding) state provided by the [encoding](#encoding) `ET`.

The end of the view is represented with a sentinel type when the end of the
underlying [code unit](#code-unit) view is represented with a sentinel type or
when the [encoding](#encoding) `ET` is a stateful [encoding](#encoding);
otherwise, the end of the view is represented with an iterator of the same type
as used for the beginning of the view.

```C++
template<TextEncoding ET, ranges::View VT>
class basic_text_view : public ranges::view_base {
public:
  using encoding_type = ET;
  using view_type = VT;
  using state_type = typename ET::state_type;
  using code_unit_iterator = ranges::iterator_t<std::add_const_t<view_type>>;
  using code_unit_sentinel = ranges::sentinel_t<std::add_const_t<view_type>>;
  using iterator = itext_iterator<ET, VT>;
  using sentinel = itext_sentinel<ET, VT>;

  basic_text_view();

  basic_text_view(state_type state,
                  view_type view);

  basic_text_view(view_type view);

  basic_text_view(state_type state,
                  code_unit_iterator first,
                  code_unit_sentinel last)
    requires ranges::Constructible<view_type,
                                   code_unit_iterator&&,
                                   code_unit_sentinel&&>();

  basic_text_view(code_unit_iterator first,
                  code_unit_sentinel last)
    requires ranges::Constructible<view_type,
                                   code_unit_iterator&&,
                                   code_unit_sentinel&&>();

  basic_text_view(state_type state,
                  code_unit_iterator first,
                  ranges::difference_type_t<code_unit_iterator> n)
    requires ranges::Constructible<view_type,
                                   code_unit_iterator,
                                   code_unit_iterator>();

  basic_text_view(code_unit_iterator first,
                  ranges::difference_type_t<code_unit_iterator> n)
    requires ranges::Constructible<view_type,
                                   code_unit_iterator,
                                   code_unit_iterator>();

  template<typename charT, typename traits, typename Allocator>
    basic_text_view(state_type state,
                    const basic_string<charT, traits, Allocator> &str)
    requires ranges::Constructible<code_unit_iterator, const charT *>()
          && ranges::ConvertibleTo<ranges::difference_type_t<code_unit_iterator>,
                                   typename basic_string<charT, traits, Allocator>::size_type>()
          && ranges::Constructible<view_type,
                                   code_unit_iterator,
                                   code_unit_sentinel>();

  template<typename charT, typename traits, typename Allocator>
    basic_text_view(const basic_string<charT, traits, Allocator> &str)
    requires ranges::Constructible<code_unit_iterator, const charT *>()
          && ranges::ConvertibleTo<ranges::difference_type_t<code_unit_iterator>,
                                   typename basic_string<charT, traits, Allocator>::size_type>()
          && ranges::Constructible<view_type,
                                   code_unit_iterator,
                                   code_unit_sentinel>();

  template<ranges::InputRange Iterable>
    basic_text_view(state_type state,
                    const Iterable &iterable)
    requires ranges::Constructible<code_unit_iterator,
                                   ranges::iterator_t<const Iterable>>()
          && ranges::Constructible<view_type,
                                   code_unit_iterator,
                                   code_unit_sentinel>();

  template<ranges::InputRange Iterable>
    basic_text_view(const Iterable &iterable)
    requires ranges::Constructible<code_unit_iterator,
                                   ranges::iterator_t<const Iterable>>()
          && ranges::Constructible<view_type,
                                   code_unit_iterator,
                                   code_unit_sentinel>();

  basic_text_view(iterator first, iterator last)
    requires ranges::Constructible<code_unit_iterator,
                                   decltype(std::declval<iterator>().base())>()
          && ranges::Constructible<view_type,
                                   code_unit_iterator,
                                   code_unit_iterator>();

  basic_text_view(iterator first, sentinel last)
    requires ranges::Constructible<code_unit_iterator,
                                   decltype(std::declval<iterator>().base())>()
          && ranges::Constructible<view_type,
                                   code_unit_iterator,
                                   code_unit_sentinel>();

  const view_type& base() const noexcept;
  view_type& base() noexcept;

  const state_type& initial_state() const noexcept;

  iterator begin() const;
  iterator end() const
    requires std::is_empty<state_type>::value
          && ranges::Iterator<code_unit_sentinel>();
  sentinel end() const
    requires !std::is_empty<state_type>::value
          || !ranges::Iterator<code_unit_sentinel>();

private:
  state_type base_state; // exposition only
  view_type base_view;   // exposition only
};

```

### Text view type aliases

The `text_view`, `wtext_view`, `u8text_view`, `u16text_view` and
`u32text_view` type aliases reference an implementation defined specialization
of `basic_text_view` for all five of the [encodings](#encoding) the standard
states must be provided.

The implementation defined view type used for the underlying
[code unit](#code-unit) view type must satisfy `ranges::View` and provide
iterators of pointer to the underlying [code unit](#code-unit) type to
contiguous storage.  The intent in providing these type aliases is to minimize
instantiations of the `basic_text_view` and `itext_iterator` class templates by
encouraging use of common view types with underlying [code unit](#code-unit)
views that reference contiguous storage, such as views into objects with a type
instantiated from `std::basic_string`.

It is permissible for the `text_view` and `u8text_view` type aliases to
reference the same type.  This will be the case when the execution character
[encoding](#encoding) is UTF-8.  Attempts to overload functions based on
`text_view` and `u8text_view` will result in multiple function
definition errors on such implementations.

```C++
using text_view = basic_text_view<
          execution_character_encoding,
          /* implementation-defined */ >;
using wtext_view = basic_text_view<
          execution_wide_character_encoding,
          /* implementation-defined */ >;
using u8text_view = basic_text_view<
          char8_character_encoding,
          /* implementation-defined */ >;
using u16text_view = basic_text_view<
          char16_character_encoding,
          /* implementation-defined */ >;
using u32text_view = basic_text_view<
          char32_character_encoding,
          /* implementation-defined */ >;
```

### make_text_view

The `make_text_view` functions enable convenient construction of
`basic_text_view` objects via implicit selection of a view type for the
underlying [code unit](#code-unit) sequence.

When provided iterators or ranges for contiguous storage, these functions return
a `basic_text_view` specialization type that uses the same implementation
defined view type as for the `basic_text_view` type
aliases as discussed in [Text view type aliases](#text-view-type-aliases).

Overloads are provided to construct `basic_text_view` objects from
iterator and sentinel pairs, iterator and difference pairs, and range or
`std::basic_string` objects.  For each of these overloads, additional 
overloads are provided to construct the view with an explicit
[encoding](#encoding) state or with an implicit initial [encoding](#encoding)
state provided by the [encoding](#encoding) `ET`.  Each of these overloads
requires that the [encoding](#encoding) type be explicitly specified.

Additional overloads are provided to construct the view from iterator and
sentinel pairs that satisfy `TextInputIterator` and objects of a type that
satisfies `TextView`.  For these overloads, the [encoding](#encoding) type is
deduced and the [encoding](#encoding) state is implicitly copied from the
arguments.

If `make_text_view` is invoked with an rvalue range, then the lifetime of the
returned object and all copies of it must end with the full-expression that the
`make_text_view` invocation is within.  Otherwise, the returned object or its
copies will hold iterators into a destructed object resulting in undefined
behavior.

```C++
template<TextEncoding ET, ranges::InputIterator IT, ranges::Sentinel<IT> ST>
  auto make_text_view(typename ET::state_type state,
                      IT first, ST last)
  -> basic_text_view<ET, /* implementation-defined */ >;

template<ranges::InputIterator IT, ranges::Sentinel<IT> ST>
  requires requires () {
    typename default_encoding_type_t<ranges::value_type_t<IT>>;
  }
  auto make_text_view(typename default_encoding_type_t<ranges::value_type_t<IT>>::state_type state,
                      IT first,
                      ST last)
  -> basic_text_view<default_encoding_type_t<ranges::value_type_t<IT>>, /* implementation-defined */ >;

template<TextEncoding ET, ranges::InputIterator IT, ranges::Sentinel<IT> ST>
  auto make_text_view(IT first, ST last)
  -> basic_text_view<ET, /* implementation-defined */ >;

template<ranges::InputIterator IT, ranges::Sentinel<IT> ST>
  requires requires () {
    typename default_encoding_type_t<ranges::value_type_t<IT>>;
  }
  auto make_text_view(IT first,
                      ST last)
  -> basic_text_view<default_encoding_type_t<ranges::value_type_t<IT>>, /* implementation-defined */ >;

template<TextEncoding ET, ranges::ForwardIterator IT>
  auto make_text_view(typename ET::state_type state,
                      IT first,
                      ranges::difference_type_t<IT> n)
  -> basic_text_view<ET, /* implementation-defined */ >;

template<ranges::ForwardIterator IT>
  requires requires () {
    typename default_encoding_type_t<ranges::value_type_t<IT>>;
  }
  auto make_text_view(typename default_encoding_type_t<ranges::value_type_t<IT>>::state_type state,
                      IT first,
                      ranges::difference_type_t<IT> n)
  -> basic_text_view<default_encoding_type_t<ranges::value_type_t<IT>>, /* implementation-defined */ >;

template<TextEncoding ET, ranges::ForwardIterator IT>
  auto make_text_view(IT first,
                      ranges::difference_type_t<IT> n)
  -> basic_text_view<ET, /* implementation-defined */ >;

template<ranges::ForwardIterator IT>
  requires requires () {
    typename default_encoding_type_t<ranges::value_type_t<IT>>;
  }
  auto make_text_view(IT first,
                      ranges::difference_type_t<IT> n)
  -> basic_text_view<default_encoding_type_t<ranges::value_type_t<IT>>, /* implementation-defined */ >;

template<TextEncoding ET, ranges::InputRange Iterable>
  auto make_text_view(typename ET::state_type state,
                      const Iterable &iterable)
  -> basic_text_view<ET, /* implementation-defined */ >;

template<ranges::InputRange Iterable>
  requires requires () {
    typename default_encoding_type_t<ranges::value_type_t<ranges::iterator_t<Iterable>>>;
  }
  auto make_text_view(typename default_encoding_type_t<ranges::value_type_t<ranges::iterator_t<RT>>>::state_type state,
                      const RT &range)
  -> basic_text_view<default_encoding_type_t<ranges::value_type_t<ranges::iterator_t<Iterable>>>, /* implementation-defined */ >;

template<TextEncoding ET, ranges::InputRange Iterable>
  auto make_text_view(const Iterable &iterable)
  -> basic_text_view<ET, /* implementation-defined */ >;

template<ranges::InputRange Iterable>
  requires requires () {
    typename default_encoding_type_t<ranges::value_type_t<ranges::iterator_t<Iterable>>>;
  }
  auto make_text_view(
    const RT &range)
  -> basic_text_view<default_encoding_type_t<ranges::value_type_t<ranges::iterator_t<Iterable>>>, /* implementation-defined */ >;

template<TextInputIterator TIT, TextSentinel<TIT> TST>
  auto make_text_view(TIT first, TST last)
  -> basic_text_view<encoding_type_t<TIT>, /* implementation-defined */ >;

template<TextView TVT>
  TVT make_text_view(TVT tv);
```

## Exceptions

- [Class text_runtime_error](#class-text_runtime_error)
- [Class text_encode_error](#class-text_encode_error)
- [Class text_decode_error](#class-text_decode_error)
- [Class text_encode_overflow_error](#class-text_encode_overflow_error)
- [Class text_decode_underflow_error](#class-text_decode_underflow_error)

### Class text_runtime_error

The `text_runtime_error` class defines the base class for the types of objects
thrown as exceptions to report errors detected during text processing.

```C++
class text_runtime_error : public std::runtime_error
{
public:
  using std::runtime_error::runtime_error;
};
```

### Class text_encode_error

The `text_encode_error` class defines the types of objects thrown as exceptions
to report errors detected during encoding of a [character](#character).  Objects
of this type are generally thrown in response to an attempt to encode a
[character](#character) with an invalid [code point](#code-point) value, or to
encode an invalid state transition.

```C++
class text_encode_error : public text_runtime_error
{
public:
  using text_runtime_error::text_runtime_error;
};
```

### Class text_decode_error

The `text_decode_error` class defines the types of objects thrown as exceptions
to report errors detected during decoding of a [code unit](#code-unit) sequence.
Objects of this type are generally thrown in response to an attempt to decode
an ill-formed [code unit](#code-unit) sequence, a [code unit](#code-unit)
sequence that specifies an invalid [code point](#code-point) value, or a
[code unit](#code-unit) sequence that specifies an invalid state transition.

```C++
class text_decode_error : public text_runtime_error
{
public:
  using text_runtime_error::text_runtime_error;
};
```

### Class text_encode_overflow_error

The `text_encode_overflow_error` class defines the types of objects thrown as
exceptions to report overflow detected during encoding of a
[character](#character).

```C++
class text_encode_overflow_error : public text_runtime_error
{
public:
  using text_runtime_error::text_runtime_error;
};
```

### Class text_decode_underflow_error

The `text_decode_underflow_error` class defines the types of objects thrown as
exceptions to report undeflow detected during decoding of a
[code unit](#code-unit) sequence.

```C++
class text_decode_underflow_error : public text_runtime_error
{
public:
  using text_runtime_error::text_runtime_error;
};
```

# Supported Encodings
As of 2015-12-31, supported [encodings](#encoding) include:

Encoding name | Description | Encoding type
------------- | ----------- | -------------
execution_character_encoding | Type alias for the encoding of ordinary string literals (C++11 2.3p3) | implementation defined
execution_wide_character_encoding | Type alias for the encoding of wide string literals (C++11 2.3p3) | implementation defined
char8_character_encoding | Type alias for the encoding of UTF-8 string literals (C++11 2.14.5p7) | implementation defined
char16_character_encoding | Type alias for the encoding of char16_t string literals (C++11 2.14.5p9) | implementation defined
char32_character_encoding | Type alias for the encoding of char32_t string literals (C++11 2.14.5p10) | implementation defined
basic_execution_character_encoding | An encoding that meets the minimum requirements of C++11 2.3p3 | trivial
basic_execution_wide_character_encoding | An encoding that meets the minimum requirements of C++11 2.3p3 | trivial
iso_10646_wide_character_encoding | An ISO 10646 encoding.  Only defined if __STDC_ISO_10646__ is defined | trivial
utf8_encoding | [Unicode] UTF-8 | stateless, variable width
utf8bom_encoding | [Unicode] UTF-8 with a byte order mark | stateful, variable width
utf16_encoding | [Unicode] UTF-16, native endian | stateless, variable width
utf16be_encoding | [Unicode] UTF-16, big endian | stateless, variable width
utf16le_encoding | [Unicode] UTF-16, little endian | stateless, variable width
utf16bom_encoding | [Unicode] UTF-16 with a byte order mark | stateful, variable width
utf32_encoding | [Unicode] UTF-32, native endian | trivial
utf32be_encoding | [Unicode] UTF-32, big endian | stateless, fixed width
utf32le_encoding | [Unicode] UTF-32, little endian | stateless, fixed width
utf32bom_encoding | [Unicode] UTF-32 with a byte order mark | stateful, variable width

# Terminology
The terminology used in this document and in the [Text_view] library has been
chosen to be consistent with industry standards and, in particular, the
[Unicode standard].  Any inconsistencies in the use of this terminology and that
in the [Unicode standard] is unintentional.  The terms described in this
document and used within the [Text_view] library comprise a subset of the
terminology used within the [Unicode standard]; only those terms necessary to
specify functionality exhibited by this library are included here.  Those who
would like to learn more about general text processing terminology in computer
systems are encouraged to read chatper 2, "General Structure" of the
[Unicode standard].

## Code Unit
A single, indivisible, integral element of an encoded sequence of characters.  A
sequence of one or more code units specifies a code point or encoding state
transition as defined by a character encoding.  A code unit does not, by itself,
identify any particular character or code point; the meaning ascribed to a
particular code unit value is derived from a character encoding definition.

The [C++11][ISO/IEC 14882:2011] `char`, `wchar_t`, `char16_t`, and `char32_t`
types are most commonly used as code unit types.

The string literal `u8"J\u00F8erg"` contains 7 code units and 6 code unit
sequences; "\u00F8" is encoded using two code units and string literals contain
a trailing NUL code unit.

The string literal `"J\u00F8erg"` contains an implementation defined number of
code units.  The standard does not specify the encoding of ordinary and wide
string literals, so the number of code units encoded by "\u00F8" depends on the
implementation defined encoding used for ordinary string literals.

## Code Point
An integral value denoting an abstract character as defined by a character set.
A code point does not, by itself, identify any particular character; the
meaning ascribed to a particular code point value is derived from a character
set definition.

The [C++11][ISO/IEC 14882:2011] `char`, `wchar_t`, `char16_t`, and `char32_t`
types are most commonly used as code point types.

The string literal `u8"J\u00F8erg"` describes a sequence of 6 code point values;
string literals implicitly specify a trailing NUL code point.

The string literal `"J\u00F8erg"` describes a sequence of an implementation
defined number of code point values.  The standard does not specify the encoding
of ordinary and wide string literals, so the number of code points encoded by
"\u00F8" depends on the implementation defined encoding used for ordinary string
literals.  Implementations are permitted to translate a single code point in the
source or Unicode character sets to multiple code points in the execution
encoding.

## Character Set
A mapping of code point values to abstract characters.  A character set need not
provide a mapping for every possible code point value representable by the code
point type.

[C++11][ISO/IEC 14882:2011] does not specify the use of any particular character
set or encoding for ordinary and wide character and string literals, though it
does place some restrictions on them.  Unicode character and string literals are
governed by the [Unicode] standard.

Common character sets include [ASCII], [Unicode][Unicode code charts], and
[Windows code page 1252].

## Character
An element of written language, for example, a letter, number, or symbol.  A
character is identified by the combination of a character set and a code point
value.

## Encoding
A method of representing a sequence of characters as a sequence of code unit
sequences.

An encoding may be stateless or stateful.  In stateless encodings, characters
may be encoded or decoded starting from the beginning of any code unit sequence.
In stateful encodings, it may be necessary to record certain affects of
previously encoded characters in order to correctly encode additional
characters, or to decode preceding code unit sequences in order to correctly
decode following code unit sequences.

An encoding may be fixed width or variable width.  In fixed width encodings,
all characters are encoded using a single code unit sequence and all code unit
sequences have the same length.  In variable width encodings, different
characters may require multiple code unit sequences, or code unit sequences of
varying length.

An encoding may support bidirectional or random access decoding of code unit
sequences.  In bidirectional encodings, characters may be decoded by traversing
code unit sequences in reverse order.  Such encodings must support a method to
identify the start of a preceding code unit sequence.  In random access
encodings, characters may be decoded from any code unit sequence within the
sequence of code unit sequences, in constant time, without having to decode any
other code unit sequence.  Random access encodings are necessarily stateless
and fixed length.  An encoding that is neither bidirectional, nor random
access, may only be decoded by traversing code unit sequences in forward order.

An encoding may support encoding characters from multiple character sets.  Such
an encoding is either stateful and defines code unit sequences that switch the
active character set, or defines code unit sequences that implicitly identify
a character set, or both.

A trivial encoding is one in which all encoded characters correspond to a single
character set and where each code unit encodes exactly one character using the
same value as the code point for that character.  Such an encoding is stateless,
fixed width, and supports random access decoding.

Common encodings include the [Unicode] UTF-8, UTF-16, and UTF-32 encodings, the
ISO/IEC 8859 series of encodings including [ISO/IEC 8859-1], and many trivial
encodings such as [Windows code page 1252].

# References
- [Text_view]  
  https://github.com/tahonermann/text_view
- [Text_view-range-v3]
  https://github.com/tahonermann/text_view-range-v3
- [cmcstl2]
  https://github.com/CaseyCarter/cmcstl2
- [ISO/IEC technical specification 19217:2015, C++ Extensions for concepts][ISO/IEC 19217:2015]  
  http://www.iso.org/iso/home/store/catalogue_tc/catalogue_detail.htm?csnumber=64031
- [P0252R0: Operator Dot Wording][P0252R0]
  http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0252r0.pdf
- [P0184R0: Generalizing the Range-Based For Loop][P0184R0]  
  http://open-std.org/JTC1/SC22/WG21/docs/papers/2016/p0184r0.html
- [P0022R1: Proxy Iterators for the Ranges Extensions][P0022R1]  
  http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0022r1.html
- [N4560: Working Draft, C++ Extensions for Ranges][N4560]  
  http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4560.pdf
- [N2442: Raw and Unicode String Literals; Unified Proposal (Rev. 2)][N2442]  
  http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2442.htm
- [N2249: New Character Types in C++][N2249]  
  http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2249.html
- [Unicode]  
  http://unicode.org
- [CMake]  
  https://cmake.org

[Text_view]:
https://github.com/tahonermann/text_view
(Text_view library)
[Text_view-range-v3]:
https://github.com/tahonermann/text_view-range-v3
(Text_view for range-v3)
[cmcstl2]:
https://github.com/CaseyCarter/cmcstl2
(An implementation of C++ Extensions for Ranges)
[Unicode]:
http://unicode.org
(The Unicode Consortium)
[Unicode standard]:
http://www.unicode.org/standard/standard.html
(The Unicode Standard)
[Unicode code charts]:
http://www.unicode.org/charts
(Unicode Character Code Charts)
[ASCII]:
http://webstore.ansi.org/RecordDetail.aspx?sku=INCITS+4-1986[R2012]
(INCITS 4-1986[R2012])
[Windows code page 1252]:
https://msdn.microsoft.com/en-us/library/cc195054.aspx
(Code Page 1252 Windows Latin 1)
[ISO/IEC 8859-1]:
http://webstore.ansi.org/RecordDetail.aspx?sku=ISO%2fIEC+8859-1%3a1998
(ISO/IEC 8859-1:1998 Information technology - 8-bit single-byte coded graphic character sets - Part 1: Latin alphabet No. 1)
[ISO/IEC 14882:2011]:
http://www.iso.org/iso/home/store/catalogue_ics/catalogue_detail_ics.htm?csnumber=50372
(ISO/IEC 14882:2011 Information technology -- Programming languages -- C++)
[ISO/IEC 19217:2015]:
http://www.iso.org/iso/home/store/catalogue_tc/catalogue_detail.htm?csnumber=64031
(ISO/IEC technical specification 19217:2015, C++ Extensions for concepts)
[N2249]:
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2249.html
(N2249: New Character Types in C++)
[N2442]:
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2442.htm
(N2442: Raw and Unicode String Literals; Unified Proposal (Rev. 2))
[N4560]:
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4560.pdf
(N4560: Working Draft, C++ Extensions for Ranges)
[P0022R1]:
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0022r1.html
(P0022R1: Proxy Iterators for the Ranges Extensions)
[P0184R0]:
http://open-std.org/JTC1/SC22/WG21/docs/papers/2016/p0184r0.html
(P0184R0: Generalizing the Range-Based For Loop)
[P0244]:
http://wg21.link/p0244
(P0244: Text_view: A C++ concepts and range based character encoding and code point enumeration library)
[P0252R0]:
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0252r0.pdf
(P0252R0: Operator Dot Wording)
[gcc]:
https://gcc.gnu.org
(GCC, the GNU Compiler Collection)
[cmake]:
https://cmake.org
(The CMake build system)
