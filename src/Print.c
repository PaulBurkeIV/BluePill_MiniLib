#include <stdint.h>
#include <stdbool.h>
#include "Platform.h"
#include "Globals.h"

bool
IsRAM (uint32_t a)
{
  if (a < RAM_START)
    return false;
  if (a <= RAM_END)
    return true;
  return false;
}

/******************Printing*************/

static void
printchar (void *out, char c)
{
  if (IsRAM ((uint32_t) out))
    {

      char **dst = out;
      **dst = c;
      (*dst)++;
    }

  else
    {
      putchar (c);
    }
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

static int
prints (void *out, const char *string, int width, int pad)
{
  int pc = 0, padchar = ' ';

  if (width > 0)
    {
      int len = 0;
      const char *ptr;
      for (ptr = string; *ptr; ++ptr)
	++len;
      if (len >= width)
	width = 0;
      else
	width -= len;
      if (pad & PAD_ZERO)
	padchar = '0';
    }
  if (!(pad & PAD_RIGHT))
    {
      for (; width > 0; --width)
	{
	  printchar (out, padchar);
	  ++pc;
	}
    }
  for (; *string; ++string)
    {
      printchar (out, *string);
      ++pc;
    }
  for (; width > 0; --width)
    {
      printchar (out, padchar);
      ++pc;
    }

  return pc;
}

/* the following should be enough for 64 bit int */
#define PRINT_BUF_LEN 32

static int
printi (void *out, int i, int b, int sg, int width, int pad, int letbase)
{
  char print_buf[PRINT_BUF_LEN];
  char *s;
  int t, neg = 0, pc = 0;
  unsigned int u = i;

  if (i == 0)
    {
      print_buf[0] = '0';
      print_buf[1] = '\0';
      return prints (out, print_buf, width, pad);
    }

  if (sg && b == 10 && i < 0)
    {
      neg = 1;
      u = -i;
    }

  s = print_buf + PRINT_BUF_LEN - 1;
  *s = '\0';

  while (u)
    {
      t = u % b;
      if (t >= 10)
	t += letbase - '0' - 10;
      *--s = t + '0';
      u /= b;
    }

  if (neg)
    {
      if (width && (pad & PAD_ZERO))
	{
	  printchar (out, '-');
	  ++pc;
	  --width;
	}
      else
	{
	  *--s = '-';
	}
    }

  return pc + prints (out, s, width, pad);
}

int
xprint (void *out, bool term, int *varg)
{
  int width, pad;
  int pwidth;

  int pc = 0;
  char *format = (char*) (*varg++);
  char scr[2];

  for (; *format != 0; ++format)
    {
      if (*format == '%')
	{
	  ++format;
	  pwidth = width = pad = 0;
	  if (*format == '\0')
	    break;
	  if (*format == '%')
	    goto out;
	  if (*format == '-')
	    {
	      ++format;
	      pad = PAD_RIGHT;
	    }
	  while (*format == '0')
	    {
	      ++format;
	      pad |= PAD_ZERO;
	    }
	  for (; *format >= '0' && *format <= '9'; ++format)
	    {
	      width *= 10;
	      width += *format - '0';
	    }
	  // do prec
	  if (*format == '.')
	    {
	      format++;
	      while (*format == '0')
		{
		  ++format;
//					pad |= PAD_ZERO;
		}
	      for (; *format >= '0' && *format <= '9'; ++format)
		{
		  pwidth *= 10;
		  pwidth += *format - '0';
		}
	    }
	  if (*format == 's')
	    {
	      char *s = *((char**) varg++);
	      pc += prints (out, s ? s : "(null)", width, pad);
	      continue;
	    }
	  if (*format == 'd')
	    {
	      pc += printi (out, *varg++, 10, 1, width, pad, 'a');
	      continue;
	    }
	  if (*format == 'x')
	    {
	      pc += printi (out, *varg++, 16, 0, width, pad, 'a');
	      continue;
	    }
	  if (*format == 'X')
	    {
	      pc += printi (out, *varg++, 16, 0, width, pad, 'A');
	      continue;
	    }
	  if (*format == 'u')
	    {
	      pc += printi (out, *varg++, 10, 0, width, pad, 'a');
	      continue;
	    }
#ifdef PRINTF_FLOAT
	  if (*format == 'f')
	    {
	      const uint32_t p[] =
		{ 0, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000,
		    1000000000 };
	      float fNumber;
	      int32_t IntNumber;

	      // align for double
	      varg = (int*) (((int) varg + 4) & ~7);

	      fNumber = *((double*) varg);
	      varg += 2;	// inc passed double
	      IntNumber = (int32_t) fNumber;

	      if ((pwidth == 0) || (pwidth > 8))
		{
		  pwidth = 8;
		}
	      if (width > (pwidth + 1))
		{
		  width -= (pwidth + 1);
		}
	      printi (out, IntNumber, 10, 1, width, pad, 'a');
	      printchar (out, '.');
	      if (fNumber < 0)
		{
		  fNumber = -fNumber;
		  IntNumber = -IntNumber;
		}
	      IntNumber = (fNumber - IntNumber) * p[pwidth];
	      printi (out, IntNumber, 10, 0, pwidth, PAD_ZERO, 'a');
	      pc++;
	      continue;
	    }
#endif
	  if (*format == 'c')
	    {
	      /* char are converted to int then pushed on the stack */
	      scr[0] = *varg++;
	      scr[1] = '\0';
	      pc += prints (out, scr, width, pad);
	      continue;
	    }
	}
      else
	{
out:      printchar (out, *format);
	  ++pc;
	}
    }
  if (term)
    {
      **(char**) out = 0;
    }
  return pc;
}

/* assuming sizeof(void *) == sizeof(int) */

int
mprintf (const char *format, ...)
{
  int *varg = (void*) (&format);
  return xprint ((void*) putchar, false, varg);
}

int
uprintf (void *dst, const char *format, ...)
{
  int *varg = (int*) (&format);
  return xprint ((void*) dst, false, varg);
}

int
sprintf (char *str, const char *fmt, ...)
{
  int *varg = (int*) (&fmt);
  return xprint ((void*) &str, true, varg);
}
