#!/usr/bin/env python3

# Copyright (c) 2014 Ingo Ruhnke <grumbel@gmail.com>
#
# This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not
#    claim that you wrote the original software. If you use this software
#    in a product, an acknowledgment in the product documentation would be
#    appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.

import codecs

def parse(text):
    stack = [[]]
    state = 'list'
    i = 0
    line = 1
    column = 0
    while i < len(text):
        c = text[i]
        if c == '\n':
            line += 1
            column = 0
        else:
            column += 1

        if state == 'list':
            if c == '(':
                stack.append([])
            elif c == ')':
                stack[-2].append(stack.pop())
            elif c == "\"":
                state = 'string'
                atom = ""
            elif c == ";":
                state = 'comment'
            elif c.isalpha():
                state = 'symbol'
                atom = c
            elif c.isdigit():
                state = 'number'
                atom = c
            elif c.isspace():
                pass
            else:
                raise Exception("%d:%d: error: unexpected character: '%s'" % (line, column, c))

        elif state == 'comment':
            if c == '\n':
                state = 'list'
            else:
                pass

        elif state == 'string':
            if c == "\\":
                i += 1
                atom += text[i]
            elif c == "\"":
                stack[-1].append(atom)
                state = 'list'
            else:
                atom += c

        elif state == 'number':
            if not c.isdigit() or c != ".":
                stack[-1].append(int(atom))
                state = 'list'
                i -= 1
            else:
                atom += c

        elif state == 'symbol':
            if c.isspace() or c == '(' or c == ')':
                stack[-1].append(atom)
                state = 'list'
                i -= 1
            else:
                atom += c

        # print c, stack

        i += 1

    if len(stack) == 1:
        return stack[0]
    else:
        raise Exception("error: list not closed")

if __name__ == "__main__":
    print("parsing...")
    result = parse(r'(() ("bar" foo) ()) () bar ')
    print("1.", result)
    print("2.", parse(""";;comment
    ("Hello World" 5 1 123) ("Hello" 123 123 "foobar") ;; comment"""))
    print("3.", parse(r'(8(8)8)'))
    print("4.", parse(r''))
    print("5.", parse(r'  '))
    with codecs.open("white.stf", encoding='utf-8') as fin:
        print("6.", parse(fin.read()))

# EOF #
