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
printchar (void *out, uint8_t c)
{
  if (IsRAM ((uint32_t) out))
    {

      uint8_t **dst = out;
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

static uint32_t
prints (void *out, const uint8_t *string, uint32_t width, uint32_t pad)
{
  uint32_t pc = 0, padchar = ' ';

  if (width > 0)
    {
      uint32_t len = 0;
      const uint8_t *ptr;
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

/* the following should be enough for 64 bit uint32_t */
#define PRINT_BUF_LEN 32

static uint32_t
printi (void *out, int32_t i, uint32_t b, uint32_t sg, uint32_t width, uint32_t pad, uint32_t letbase)
{
  uint8_t print_buf[PRINT_BUF_LEN];
  uint8_t *s;
  uint32_t t, neg = 0, pc = 0;
  int32_t u = i;

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

  uint32_t ArrayStart;
  uint32_t ArraySz;
enum {EL_32, EL_16, EL_8};

uint32_t
xprint (void *out, bool term, uint32_t *varg)
{
  uint32_t width, pad;
  uint32_t pwidth;
  bool PrintArray = false;
  uint32_t ArrayStart = 0;
  uint8_t ArrayElSz = EL_32;
//  uint32_t ArraySz = 0;
  uint8_t ArraySep = 0;
  uint32_t pc = 0;
  uint8_t *format = (uint8_t*) (*varg++);
  uint8_t scr[2];

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
	  if(*format == '[')
	   {
	   ++format;
	   PrintArray = true; // signal that it's an array
	   if( *format == 's')
	    {
	    ArrayElSz = EL_16;
	    format++;
	    }
	   else if( *format == 'c')
	    {
	    ArrayElSz = EL_8;
	    format++;
	    }
//	   if( (*format >= '0') && (*format <= '9')) //Array definition in string  in format [x,y
//	    {
//	    for (; *format >= '0' && *format <= '9'; ++format)
//	     {
//	      ArrayStart *= 10;
//	      ArrayStart += *format - '0';
//	     }
//	    ArraySep = *format; // Next character is array separator
//	    ++format;
//	    for (; *format >= '0' && *format <= '9'; ++format)
//	     {
//	      ArraySz *= 10;
//	      ArraySz += *format - '0';
//	     }
//	    }
//	   else	//Array definition in parameters
//	    {
 	    ArrayStart = *varg++;
 	    ArraySz = *varg++;
	    ArraySep = *format++;
//	    }
	   }
	  else
	   {
	   PrintArray = false;
	   }
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
#ifdef PRINTF_FLOAT
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
#endif
	  if (*format == 's')
	    {
	      uint8_t *s = *((uint8_t**) varg++);
	      pc += prints (out, s ? s : (uint8_t *)"(null)", width, pad);
	      continue;
	    }
	  if ( (*format == 'd') || (*format == 'x') || (*format == 'X') || (*format == 'u'))
	    {
	      if( !PrintArray)
	       {
	        if( *format == 'd')
	         pc += printi (out, *varg++, 10, 1, width, pad, 'a');
	        else if(*format == 'x')
	         pc += printi (out, *varg++, 16, 0, width, pad, 'a');
	        else if(*format == 'X')
	         pc += printi (out, *varg++, 16, 0, width, pad, 'A');
	        else if(*format == 'u')
	         pc += printi (out, *varg++, 10, 0, width, pad, 'a');
	       }
	      else
	       {
 	       uint32_t i;
 	       for(i=ArrayStart; i<(ArrayStart+ArraySz); i++)
	        {
	        if( ArrayElSz == EL_32)
	         {
	         uint32_t *v = (uint32_t *)*varg;
	         if( i != ArrayStart)
	          printchar (out, ArraySep);
	         if( *format == 'd')
	          pc += printi (out, v[i], 10, 1, width, pad, 'a');
	         else if(*format == 'x')
	          pc += printi (out, v[i], 16, 0, width, pad, 'a');
	         else if(*format == 'X')
	          pc += printi (out, v[i], 16, 0, width, pad, 'A');
	         else if(*format == 'u')
	          pc += printi (out, v[i], 10, 0, width, pad, 'a');
	         }
	        else if( ArrayElSz == EL_16)
	         {
	         uint16_t *v = (uint16_t *)*varg;
	         if( i != ArrayStart)
	          printchar (out, ArraySep);
	         if( *format == 'd')
	          pc += printi (out, v[i], 10, 1, width, pad, 'a');
	         else if(*format == 'x')
	          pc += printi (out, v[i], 16, 0, width, pad, 'a');
	         else if(*format == 'X')
	          pc += printi (out, v[i], 16, 0, width, pad, 'A');
	         else if(*format == 'u')
	          pc += printi (out, v[i], 10, 0, width, pad, 'a');
	         }
	        else if( ArrayElSz == EL_8)
	         {
	         uint8_t *v = (uint8_t *)*varg;
	         if( i != ArrayStart)
	          printchar (out, ArraySep);
	         if( *format == 'd')
	          pc += printi (out, v[i], 10, 1, width, pad, 'a');
	         else if(*format == 'x')
	          pc += printi (out, v[i], 16, 0, width, pad, 'a');
	         else if(*format == 'X')
	          pc += printi (out, v[i], 16, 0, width, pad, 'A');
	         else if(*format == 'u')
	          pc += printi (out, v[i], 10, 0, width, pad, 'a');
	         }
	        }
	       varg++;
	       }
	      continue;
	    }
//	  if (*format == 'x')
//	    {
//	      pc += printi (out, *varg++, 16, 0, width, pad, 'a');
//	      continue;
//	    }
//	  if (*format == 'X')
//	    {
//	      pc += printi (out, *varg++, 16, 0, width, pad, 'A');
//	      continue;
//	    }
//	  if (*format == 'u')
//	    {
//	      pc += printi (out, *varg++, 10, 0, width, pad, 'a');
//	      continue;
//	    }
#ifdef PRINTF_FLOAT
	  if (*format == 'f')
	    {
	      const uint32_t p[] =
		{ 0, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000,
		    1000000000 };
	      float fNumber;
	      int32_t IntNumber;

	      // align for double
	      varg = (uint32_t*) (((uint32_t) varg + 4) & ~7);

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
	      /* uint8_t are converted to uint32_t then pushed on the stack */
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
      **(uint8_t**) out = 0;
    }
  return pc;
}

/* assuming sizeof(void *) == sizeof(uint32_t) */

uint32_t
exprintf (const uint8_t *format, ...)
{
  uint32_t *varg = (void*) (&format);
  return xprint ((void*) putchar, false, varg);
}

uint32_t
exuprintf (void *dst, const uint8_t *format, ...)
{
  uint32_t *varg = (uint32_t*) (&format);
  return xprint ((void*) dst, false, varg);
}

uint32_t
exsprintf (uint8_t *str, const uint8_t *fmt, ...)
{
  uint32_t *varg = (uint32_t*) (&fmt);
  return xprint ((void*) &str, true, varg);
}
