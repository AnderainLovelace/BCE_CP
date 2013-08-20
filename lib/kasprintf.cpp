/*
 *	Cross-platform 'kasprintf' Function
 *
 *	Copyright (C) 2011-2012 XiaoJSoft Studio. All Rights Reserved.
 *	Copyright (C) Ji WenCong <whs_jwc@163.com>
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "../include/kasprintf.h"

/*
 *	kasprintf()
 *
 *	Print to allocated string.
 */

char * d_strdup (const char * s)
{
	int len = strlen(s);
	char * p = (char*)calloc(len+1,1);
	return strcpy(p,s);
}

typedef int int32_t;
typedef unsigned int uint32_t;
/* flags */
#define LEFT    0x01
#define PLUS    0x02
#define SPACE   0x04
#define SPECIAL 0x08
#define ZERO    0x10
#define SIGN    0x20 /* signed if set */
#define SMALL   0x40 /* 'abcdef' if set, 'ABCDEF' otherwise */

int32_t get_wide (const char **s) {
    int32_t res = 0;
    while (isdigit (**s)) res = 10*res + *((*s)++) - '0';
    return res;
}

#define LONG_STRSIZE_BASE_2 32

void number_to_string (long num, int32_t base, int32_t flags, int32_t wide, int32_t precision, char **s) {
    char sign;  /* sign printed : '+', '-', ' ', or 0 (no sign) */
    int32_t num_cpy = num;
    unsigned long ul_num = (unsigned long) num; /* for unsigned format */

    /* string representation of num (reversed) */
    char tmp[LONG_STRSIZE_BASE_2];
    int32_t i = 0; /* number of figures in tmp */

    const char *digits = "0123456789ABCDEF";
    if (flags & SMALL) digits = "0123456789abcdef";

    if ((base < 2) || (base > 16)) return;

    if ((flags & SIGN) && (num < 0)) { sign = '-'; num = -num; }
    else sign = (flags & PLUS) ? '+' : ((flags & SPACE) ? ' ' : 0);
    if (sign) wide--;

    if (flags & SPECIAL) {
        if ((base == 16) && (num != 0)) wide -= 2;  /* '0x' or '0X' */
        if (base == 8) { wide--; precision--; }     /* '0' */
    }
    
    if (num == 0) tmp[i++] = '0';
    /* signed format */
    if (flags & SIGN) {
        while (num != 0) {
            tmp[i++] = digits[num % base];
            num = num / base;
        } 
    }
    /* unsigned format */
    else {
        while (ul_num != 0) {
            tmp[i++] = digits[ul_num % base];
            ul_num = ul_num / base;
        } 
    }

    if (i > precision) precision = i;
    wide -= precision;

    /* wide = number of padding chars */
    /* precision = number of figures after the sign and the special chars */

    /* right justified and no zeropad : pad with spaces */
    if (!(flags & (LEFT + ZERO))) while (wide-- > 0) *((*s)++) = ' ';

    if (sign) *((*s)++) = sign;
    if ((flags & SPECIAL) && (num_cpy != 0)) {
        if (base == 8) *((*s)++) = '0';
        if (base == 16) {
            *((*s)++) = '0';
            if (flags & SMALL) *((*s)++) = 'x';
            else *((*s)++) = 'X';
        }
    }

    /* rigth justified and zeropad : pad with 0 */
    if (!(flags & LEFT)) while (wide-- > 0) *((*s)++) = '0';

    /* print num */
    while (i < precision--) *((*s)++) = '0';
    while (i-- > 0) *((*s)++) = tmp[i];

    /* left justfied : pad with spaces */
    while (wide-- > 0) *((*s)++) = ' ';
}

int32_t d_vsprintf (char *str, const char *format, va_list ap) {
    char c;
    char *start = str;
    int32_t flags;
    int32_t wide;
    int32_t precision;
    int32_t qualifier;
    char *s;
    int32_t i, len, base;

    while ((c = *format++) != 0) {
        if (c != '%') { *str++ = c; continue; }
        if (*format == '%') { *str++ = '%'; format++; continue; }

        /* get flags */
        flags = 0;
        while (1) {
            if (*format == '-') { flags |= LEFT;    format++; continue; }
            if (*format == '+') { flags |= PLUS;    format++; continue; }
            if (*format == ' ') { flags |= SPACE;   format++; continue; }
            if (*format == '#') { flags |= SPECIAL; format++; continue; }
            if (*format == '0') { flags |= ZERO   ; format++; continue; }
            break;
        }

        /* get wide */
        wide = -1;
        if (isdigit (*format)) wide = get_wide ((const char **)(&format));
        else if (*format == '*') { wide = va_arg (ap, int32_t); format++; }

        /* get precision */
        precision = -1;
        if (*format == '.') {
            format++;
            if (isdigit (*format))
                precision = get_wide ((const char **)(&format));
            else if (*format == '*') {
                precision = va_arg (ap, int32_t);
                format++;
            }
            else precision = 0;
        }

        /* get qualifier */
        qualifier = -1;
        if ((*format == 'h') || (*format == 'l')) qualifier = *format++;

        /* get format */
        switch (*format++) {
            case 'i':
            case 'd':
                flags |= SIGN;
                if (precision != -1) flags &= ~ZERO;
                switch (qualifier) {
                    case 'h':
                        number_to_string ((short) va_arg (ap, int32_t), 10, flags,
                            wide, precision, &str);
                        break;
                    case 'l':
                        number_to_string (va_arg (ap, long), 10, flags,
                            wide, precision, &str);
                        break;
                    default:
                        number_to_string (va_arg (ap, int32_t), 10, flags,
                            wide, precision, &str);
                        break;
                }
                break;

            case 'u':
                base = 10;
                goto num_to_str_without_sign;

            case 'o':
                base = 8;
                goto num_to_str_without_sign;

            case 'x':
                flags |= SMALL;
            case 'X':
                base = 16;

                num_to_str_without_sign:
                flags &= (~PLUS & ~SPACE);
                if (precision != -1) flags &= ~ZERO;
                switch (qualifier) {
                    case 'h':
                        number_to_string ((unsigned short) va_arg (ap, int32_t), \
                            base, flags, wide, precision, &str);
                        break;
                    case 'l':
                        number_to_string ((unsigned long) va_arg (ap, long), \
                            base, flags, wide, precision, &str);
                        break;
                    default:
                        number_to_string((uint32_t)va_arg (ap, int32_t), \
                         base, flags, wide, precision, &str);
                        break;
                }
                break;

            case 's':
                s = va_arg (ap, char *);
                len = strlen (s);
                if ((precision >= 0) && (len > precision)) len = precision;

                /* rigth justified : pad with spaces */
                if (!(flags & LEFT)) while (len < wide--) *str++ = ' ';
                for (i = 0; i < len; i++) *str++ = *s++;
                /* left justified : pad with spaces */
                while (len < wide--) *str++ = ' ';
                break;

            case 'c':
                /* rigth justified : pad with spaces */
                if (!(flags & LEFT)) while (1 < wide--) *str++ = ' ';
                *str++ = (unsigned char) va_arg (ap, int32_t);
                /* left justified : pad with spaces */
                while (1 < wide--) *str++ = ' ';
                break;

            default:
                return -1;
        }
    }
    *str = 0;
        
    return (int32_t)(str-start);
}



int kasprintf(char **buf,  const char *fmt, ...) {
	va_list ap;
	int len;
	char b[512];

	va_start(ap, fmt);
	len = d_vsprintf(b, fmt, ap);
	va_end(ap);

	*buf = d_strdup(b);

	return(len);
}

