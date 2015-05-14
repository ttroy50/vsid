#include "Logger.h"

int format_hexdump (const u_char *buffer, int size,
        char *obuf, int obuf_sz)
{
    u_char c;
    char textver[16 + 1];

    //int maxlen = (obuf_sz / 68) * 16;
    int maxlen = (obuf_sz / 69) * 16; // +1 for \t

    if (size > maxlen)
        size = maxlen;

    int i;

    for (i = 0; i < (size >> 4); i++)
    {
        int j;

        *obuf++ = '\t';
        for (j = 0 ; j < 16; j++)
        {
            c = (u_char) buffer[(i << 4) + j];
            ::sprintf (obuf, "%02X ", c);
            obuf += 3;
            if (j == 7)
            {
                ::sprintf (obuf, " ");
                obuf++;
            }
            textver[j] = (c < 0x20 || c > 0x7e) ? '.' : c;
        }

        textver[j] = 0;

        ::sprintf (obuf, "  %s\n", textver);

        while (*obuf != '\0')
            obuf++;
    }

    if (size % 16)
    {
        *obuf++ = '\t';
        for (i = 0 ; i < size % 16; i++)
        {
            c = (u_char) buffer[size - size % 16 + i];
            ::sprintf (obuf, "%02X ",c);
            obuf += 3;
            if (i == 7)
            {
                ::sprintf (obuf, " ");
                obuf++;
            }
            textver[i] = (c < 0x20 || c > 0x7e) ? '.' : c;
        }

        for (i = size % 16; i < 16; i++)
        {
            ::sprintf (obuf, "   ");
            obuf += 3;
            textver[i] = ' ';
        }

        textver[i] = 0;
        ::sprintf (obuf, "  %s\n", textver);
    }
    return size;
}