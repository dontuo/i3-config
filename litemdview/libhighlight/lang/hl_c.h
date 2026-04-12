/*
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  Author: g0tsu
 *  Email:  g0tsu at dnmx.0rg
 */

const int HL_C_EXPR_SIZ = 14;
const char *HL_C_EXPR[] = {
  "goto",
  "break",
  "return",
  "continue",
  "asm",
  "case",
  "default",
  "if",
  "else",
  "switch",
  "while",
  "for",
  "do",
  "sizeof",
  "typeof",
};

const int HL_C_TYPE_SIZ = 50;
const char *HL_C_TYPE[] = {
  "int",
  "long",
  "short",
  "char",
  "void",
  "signed",
  "unsigned",
  "static",
  "float",
  "double",
  "size_t",
  "wchar_t",
  "time_t",
  "va_list",
  "jmp_buf",
  "FILE",
  "DIR",
  "wctype_t",
  "const",
  "bool",
  "complex",
  "struct",
  "union",
  "int8_t",
  "int16_t",
  "int32_t",
  "int64_t",
  "uint8_t",
  "uint16_t",
  "uint32_t",
  "uint64_t",
  "int_least8_t",
  "int_least16_t",
  "int_least32_t",
  "int_least64_t",
  "uint_least8_t",
  "uint_least16_t",
  "uint_least32_t",
  "uint_least64_t",
  "int_fast8_t",
  "int_fast16_t",
  "int_fast32_t",
  "int_fast64_t",
  "uint_fast8_t",
  "uint_fast16_t",
  "uint_fast32_t",
  "uint_fast64_t",
  "intptr_t",
  "uintptr_t",
  "intmax_t",
  "uintmax_t"
};

const int HL_C_DECL_SIZ = 14;
const char *HL_C_DECL[] = {
  "define",
  "typedef",
  "inline",
  "auto",
  "extern",
  "inline",
  "register",
  "volatile",
  "include",
  "endif",
  "ifdef",
  "ifndef",
  "else",
  "enum"
};

