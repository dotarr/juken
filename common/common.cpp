#include <stdarg.h>
#include <ctype.h>

#include "common.h"

char
aschar(unsigned char val)
{
    if ( isprint(val) && !iscntrl(val) )
        return (char) val;
    else
        return '.';
}

void
printdata(const byte data[], int count)
{
    int i = 0;
    while ( count > 0 )
    {
        typedef char hex_t[4];
        hex_t hex[8] = { "   ", "   ", "   ", "   ", 
                         "   ", "   ", "   ", "   "  };
        char c[8] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };

        int j = 0;
        unsigned char val;
        switch ( count )
        {
            default:
            case 8:
                val = data[i+j];
                ::sprintf(hex[j], "%02X ", val);
                c[j] = aschar(val);
                j++;
            case 7:
                val = data[i+j];
                ::sprintf(hex[j], "%02X ", val);
                c[j] = aschar(val);
                j++;
            case 6:
                val = data[i+j];
                ::sprintf(hex[j], "%02X ", val);
                c[j] = aschar(val);
                j++;
            case 5:
                val = data[i+j];
                ::sprintf(hex[j], "%02X ", val);
                c[j] = aschar(val);
                j++;
            case 4:
                val = data[i+j];
                ::sprintf(hex[j], "%02X ", val);
                c[j] = aschar(val);
                j++;
            case 3:
                val = data[i+j];
                ::sprintf(hex[j], "%02X ", val);
                c[j] = aschar(val);
                j++;
            case 2:
                val = data[i+j];
                ::sprintf(hex[j], "%02X ", val);
                c[j] = aschar(val);
                j++;
            case 1:
                val = data[i+j];
                ::sprintf(hex[j], "%02X ", val);
                c[j] = aschar(val);
                j++;
                break;
        }
        ::printf("\t");
        for (int k=0; k<8; k++) ::printf("%s", hex[k]);
        ::printf("\t");
        for (int k=0; k<8; k++) ::printf("%c", c[k]);
        ::printf("\n");

        i += j; 
        count -= j;
    }
}

const short err_str_len = 1024;
static char err_str[err_str_len];

const short tmp_str_len = 256;
static char tmp_str[tmp_str_len];

void
vthrow_errno(int err, const char* fmt, va_list ap)
{
    ::snprintf(tmp_str, tmp_str_len, "%d %s", err, strerror(err));

    ::vsnprintf(err_str, err_str_len, fmt, ap);
    ::strcat(err_str, tmp_str);

    va_end(ap);

    throw err_str;
}

void
ThrowIf(bool cond, const char* fmt, ...)
{
    if ( cond )
    {
        int err = errno;
        va_list ap;
        va_start(ap, fmt);
        vthrow_errno(err, fmt, ap);
    }
}

void
ThrowIfMinus1(int val, const char* fmt, ...)
{
    if ( val == -1 )
    {
        int err = errno;
        va_list ap;
        va_start(ap, fmt);
        vthrow_errno(err, fmt, ap);
    }
}
void
ThrowIfNeg(int val, const char* fmt, ...)
{
    if ( val < 0 )
    {
        int err = errno;
        va_list ap;
        va_start(ap, fmt);
        vthrow_errno(err, fmt, ap);
    }
}

void
ThrowIfNull(const void* p, const char* fmt, ...)
{
    if ( p == NULL )
    {
        int err = errno;
        va_list ap;
        va_start(ap, fmt);
        vthrow_errno(err, fmt, ap);
    }
}

void
Throw(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    ::vsnprintf(err_str, err_str_len, fmt, ap);
    va_end(ap);

    throw err_str;
}


