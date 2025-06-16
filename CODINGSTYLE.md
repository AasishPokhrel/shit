# SuperTux Coding Standards

## Language

C++17 is the main langauge used for this project. GCC, Clang and MSVC are supported.

For better backward compatibilty with older compiler, namely gcc5,
some C++17 features are restricted:

* generic lambda functions are not allowed, e.g. `[](auto foo){}`
* tuple constructors have to be explicit, e.g. `std::tuple<int, int>{5, 6}`, not `{5, 6}`

## Repository Structure

Properly separate between generic engine code and game specific code whenever
feasible.

Third party libraries are not allowed in `src/`, they go to `external/`.

Third party libraries that are imported into `external/` as git
submodule have to be forked and included into the SuperTux
organisation on Github, not directly included from upstream.

## File Formating

Do not have spaces at the end of lines.

Files should always end with `/* EOF */` and a newline or a similar
marker approprimate for the given language. This marker can be left
out for fileformats that have an end tag, e.g. `</html>`.

Aim for one file per class, small helper classes in the same file are ok.

## Includes

The path in `#include` directives must not contain `..`.

All paths for includes from SuperTux must be relative to the `src/`
directory and use `#include "..."`.

Use the `#include <>` syntax for libraries in `external/`, use `cmake`
to set the include path properly.

The order of includes shall be as follows, each of those subgroups
shall be ordered alphabetically:

* include of header file when in a .cpp file
* include of the base class in a header file of a derived class
* system includes
* external includes
* local includes

Conditional includes should be indented.

```c++
#ifdef FOOBAR
#  include "foobar.hpp"
#endif
```

Include guards are of the form:

```c++
#ifndef HEADER_SUPERTUX_{PATH}_{FILE}_HPP
#define HEADER_SUPERTUX_{PATH}_{FILE}_HPP
```

`tools/fix_include_guards.sh` is a little script that will help to fix
include guards on file renames.

## Variables

Prefix member variable names with `m_`, global variables with `g_`, and static
variables with `s_`. DynamicScopeRefs are prefixed with `d_`.

## Classes

Mark all classes as `final` unless they are specifically designed
with polymorphism in mind.

Mark all functions that override a virtual function in a base class with `override`.

Write simple getters/setters inside a header file on a single line. Mark them as `inline`.

Properly separate data members and member functions. Do not mix them in the same
`public`/`protected`/`private` section.

List virtual functions before non-virtual functions.

The order of declarations in a class shall be as follows:

```c++
class Foo final
{
public:
protected:
private:
   // type declarations, needs to come first as later stuff might depend on them

public:
protected:
private:
   // static stuff

public:
protected:
private:
   // constructors
   // destructor

public:
protected:
private:
   // virtual member functions
   // non-virtual member functions

public:
protected:
private:
   // member variables with an m_ prefix

private:
  // non-copyable footer
  Foo(const Foo&) = delete;
  Foo& operator=(const Foo&) = delete;
};
```

## Pointers

Do not use raw pointers and `new`/`delete`, use
`std::unique_ptr<>`/`std::make_unique<>` instead.

Only use `std::smart_ptr<>` when sharing of data is required, prefer
`std::unique_ptr<>` when possible.

Pass and return values as value, `&` or `const&`, only use `*` when
the value is expected to be `nullptr`.

Do not pass values as `const std::unique_ptr<T>&` or `const
std::shared_ptr<T>&`, dereference the pointer and pass as `const&`.

To check for nullptr, use an `if` statements with initializer when possible:

```c++
if (auto* ptr = get_ptr()) {
  // code here
}
```

## auto

Don't use `auto` for basic types (`int`, `float`, `std::string`, ...)

Only use `auto` when the exact type is unnecessary to know (e.g.
iterators) or obvious from the context, (e.g. `auto foo =
Foo::create()`)

Capture pointers as `auto*`, not just `auto`.

Use `const auto&` for loops to avoid copies.

## Namespaces

Namespaces should be written in the form:

```c++
namespace my_namespace {
...
} // namespace my_namespace
```

With no newline before the `{`. Do not indent the content inside the
namespace. The namespace itself should be all lowercase.

## Compiler Warnings and Errors

Compile with a maximum warning level and with `-Werror`. This can be accomplished with:

```console
cmake .. -DCMAKE_BUILD_TYPE=Release  -DWARNINGS=ON -DWERROR=ON
```

This requires, among other things:

* use of `final` and `override` keywords

* use of `static_cast` and `reinterpret_cast`, not old style C casts

* all member variables have to be initialized in the constructor

* all `int`/`float` conversion has to be explicit

## Comments

Avoid comments unless they explain something important and
non-obvious. Document *why* the code does something, not *what* it
does.

Prefer to use good function and variable names to create
self-documenting code.

Use the `//` syntax for regular comments, even multiline, don't use `/* */`.

For Doxygen (code documentation), use the `/** */` syntax, don't use
`/**<` and other styles of comments.

For translator information, use the `// l10n:` syntax.

Don't do `*` prefix decorations in comments, keep things simple and
compact:

```c++
/*
 *  Don't do this
 */
```

Instead:

```c++
// Do this
```

or:

```c++
/** Also ok when it's a Doxygen comment */
```

## Spaces

Use a space after `if`/`while`/`switch`/`for`:

`for (int i = 0; i < len; ++i) ...`

`if (a > b) ...`

`while (a > b) ...`

`switch (myenum) ...`

But don't use a space after a function name:

`myfunc ()` // don't do this

`myfunc()` // do this

## Line breaks

Write simple getters/setters inside a header file on a single line:

```c++
Vector get_pos() const { return m_pos; }
```

Don't include more complex functions inside a header file, unless it's
necessary (template, performance).

Declare functions like:

```c++
ReturnType
ClassName::function_name()
{
...
}
```

We don't have any hard rules on linebreaks before `{` inside
functions, but generally prefer the no-linebreak version when doing
simple one-liners and the other one when doing more complex stuff:

```c++
if (foo) {
  one_line_function_call()
}
```

```c++
if (foo)
{
  long();
  complex();
  series();
  of();
  calls();
}
```

## Other Information

More general info on good practices can be found in [Google's C++
Style Guide](https://google.github.io/styleguide/cppguide.html) and in
the [C++ Core Guidelines](http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines),
note however that we do not strictly follow either of them and divert from
them in some points, so just take them as general guidelines, not hard rules.
