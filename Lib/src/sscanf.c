// SSCANF fairly roughly as defined in K&R

int iprintf (const char *format, ...);

#include <ctype.h>

#include "Platform.h"
#include "sscanf.h"

#define LIT 0
#define FMTW 1
#define FMTT 2

#define T_NONE 0
#define T_CHAR 1
#define T_INT 2
#define T_LONG 3
#define T_STR 4
#define T_FORD 5	// float OR double
#define T_FLOAT 6
#define T_DOUBLE 7

int
sscanf (char *ip, char *ctl, void *args, ...)
{

  void **arg;			// Pointer to next arg
  unsigned int nargs = 0;		// Conversions completed
  unsigned int mod = LIT;

  unsigned int FieldWidth = 0;		// Width of field
  int Sign = 0;				// Positive number
  unsigned int base = 0;
  unsigned int Signed = 0;
  unsigned int Siz = 0;
  unsigned int Skip = 0;			// Suppression

  unsigned int c = 0;

  char Leader = 0;
  union
  {
    int i;
    long l;
    float f;
    double lf;
  } val;

  unsigned int i;

  arg = &args;

  while (*ctl)			// Run along the format string
    {

INPUT_SPACES:           	// Skip input string spaces
      c = *(ip++);
      if (!c)
	return nargs;               	// Input finished
      if (isspace (c))
	goto INPUT_SPACES;
      ip--;                     	// Put back first nonspace

      while (isspace (*ctl))	// skip format string spaces
	++ctl;
      if (!ctl)
	return nargs;             	// finished the format string

      switch (mod)
	{
	case LIT:
	  if (*ctl == '%')	// Start formatted convert?
	    {
	      base = 0;
	      FieldWidth = 0;
	      Sign = 1;
	      val.lf = 0;
	      Skip = 0;
	      Siz = T_NONE;
	      mod = FMTW;
	      ctl++;
	      continue;
	    }
	  if (*(ctl++) != *(ip++)) // Otherwise just a literal match
	    return nargs;		// No match, so finished
	  continue;

	case FMTW:
	  if (*ctl == '%')	// Literal %
	    {
	      if (*(ip++) != '%')
		return nargs;
	      ctl++;
	      mod = LIT;
	      continue;
	    }

	  FieldWidth = 0;		// Presume infinite width

	  if (!isdigit (*ctl))
	    {
	      if (*ctl == '.')	// DP so must be float or double
		{
		  ctl++;
		}
	      else
		mod = FMTT;		// Not a digit, so try type
	      continue;
	    }
	  else			// Numbers are field size
	    {
	      while (isdigit (*ctl))
		{
		  FieldWidth = FieldWidth * 10 + (*(ctl++) - '0');
		}
	      mod = FMTT;
	      continue;
	    }

	case FMTT:

	  if (*ctl == 'l')	// long
	    {
	      if (Siz == T_LONG)
		return nargs;		// Second l, so buggered up
	      Siz = T_LONG;
	      ctl++;
	      continue;
	    }
#ifdef printf__FLOAT
	  if (*ctl == 'f')	// float
	    {
	      if (Siz == T_LONG)
		Siz = T_DOUBLE;
	      else
		Siz = T_FLOAT;
	      goto CVT;
	    }
#endif
	  if (*ctl == 'd')	// Decimal signed
	    {
	      if (base)
		return nargs;	// Second conversion spec, so buggered up
	      if (!Siz)          	// Already long?
		Siz = T_INT;       	// No, so INT
	      base = 10;
	      Signed = 1;
	      goto CVT;
	    }

	  if (*ctl == 'u')	// Unsigned
	    {
	      if (base)
		return nargs;	// Second conversion spec, so buggered up
	      if (!Siz)          	// Already long?
		Siz = T_INT;       	// No, so INT
	      base = 10;
	      Signed = 0;
	      goto CVT;
	    }

	  if (*ctl == 'x')	// Hex
	    {
	      if (base)
		return nargs;	// Second conversion spec, so buggered up
	      if (!Siz)          	// Already long?
		Siz = T_INT;       	// No, so INT
	      base = 16;
	      Signed = 0;
	      goto CVT;
	    }

	  if (*ctl == 'o')	// Octal
	    {
	      if (base)
		return nargs;	// Second conversion spec, so buggered up
	      if (!Siz)          	// Already long?
		Siz = T_INT;       	// No, so INT
	      base = 8;
	      Signed = 0;
	      goto CVT;
	    }

	  if (*ctl == 'c')	// Characters
	    {
	      if (base || (Siz == T_LONG))
		return nargs;	// Second conversion spec, so buggered up
	      Siz = T_CHAR;
	      base = -1;
	      goto CVT;
	    }

	  if (*ctl == 's')	// String
	    {
	      if (base || (Siz == T_LONG))
		return nargs;	// Second conversion spec, so buggered up
	      Siz = T_STR;
	      base = -2;
	      goto CVT;
	    }

	  if (*ctl == '*')	// Suppress any of %*wwt %*t %ww*t
	    {
	      if (Skip || Siz)
		return nargs;		// already got skip, so buggered up
	      Skip = 1;
	      ctl++;
	      continue;
	    }

	}

// Formatted now, so ready to convert the input!!

CVT:

      switch (Siz)
	{
	case T_NONE:
	  break;

	case T_CHAR:
	  if (!FieldWidth)    // Not specified, default 1
	    FieldWidth = 1;
	  for (i = 0; i < FieldWidth; i++)
	    {
	      unsigned int c = *(ip++);
	      if (!c)            // Break on end of string
		break;
	      if (c == (unsigned int)EOF)      // finish on EOF (if possible)
		return nargs;
	      if (!Skip)         // Skip or assign and increment pointer
		*(char*) arg[nargs]++ = c;
	    }
	  break;

	case T_STR:
	  for (i = 0; (i < FieldWidth) || !FieldWidth; i++)
	    {
	      int c = *(ip++);
	      if (!c || isspace (c))  // Finish on space or null
		{
		  if (!Skip)            // Assign if not blocked
		    *(char*) arg[nargs] = 0;
		  break;
		}
	      if (c == EOF)          // Check for input end/ error
		return nargs;
	      if (!Skip)
		*(char*) arg[nargs]++ = c;
	    }
	  break;

	case T_INT:
	case T_LONG:
	  {
	    // Run along the field until you have enough characters
	    // If FieldWidth not defined, it is infinite
	    for (i = 0; (i < FieldWidth) || !FieldWidth; i++)
	      {
		c = *(ip++);
		if (c == (unsigned int)EOF)
		  return nargs;
		if ((base == 16) ? !isxdigit (c) : !isdigit (c))
		  {
		    if ((c == '-') && Signed)
		      {
			Sign = -1;
			i--;
			continue;
		      }
		    else
		      {
			if (base == 16)
			  {
			    if ((c == 'x') || (c == 'X'))
			      {
				if (!val.l && !Leader)
				  {
				    Leader = 1;
				    i -= 2;
				    continue;
				  }
				else
				  return nargs;
			      }
			  }
			else if (base == 8)
			  {
			    if ((c == 'o') || (c == 'O'))
			      {
				if (!val.l && !Leader)
				  {
				    Leader = 1;
				    i--;
				    continue;
				  }
				else
				  return nargs;
			      }
			  }
			ip--;
			break;
		      }
		  }
		else
		  {
		    if (base == 16)
		      {
			c = tolower (c);
			if (c > '9')
			  c = c - 'a' + 10;
			else
			  c -= '0';
		      }
		    else
		      c -= '0';
		    val.l = val.l * base + c;
		  }
	      }
	    if (!Skip)
	      {
		if (Siz == T_INT)
		  *(int*) arg[nargs] = val.i * Sign;
		else
		  *(long*) arg[nargs] = val.l * Sign;
	      }
	  }
	  break;

#ifdef printf__FLOAT
	case T_FLOAT:
	case T_DOUBLE:
	  {
	    double base = 10.0;
	    //iprintf( "Scanning float %s\r\n", ip);
	    // %f ignores field width
	    for (i = 0;;)
	      {
		c = *(ip++);
		if (isspace (c))
		  break;
		if (c == '.')	// Dec point or literal '.' delimiter
		  {
		    if (base != 10.0)
		      break;
		    base = 0.1;
		    continue;
		  }
		else if (c == EOF)
		  return nargs;
		else if ((c == '-'))// Minus sign if leading; delimiter if trailing
		  {
		    if (i)
		      break;
		    Sign = -1;
		    continue;
		  }
		else if (!isdigit (c))	// Finished the field
		  {
		    ip--;
		    break;
		  }

		c -= '0';
		if (base == 10.0)
		  val.lf = val.lf * 10.0 + c;
		else
		  {
		    val.lf = val.lf + base * c;
		    base *= 0.1;
		  }
	      }
	    if (!Skip)
	      {
		if (Siz == T_FLOAT)
		  *(float*) arg[nargs] = (float) val.lf * Sign;
		else
		  *(double*) arg[nargs] = val.lf * Sign;
	      }
	  }
	  break;
#endif
	}

      if (!Skip)
	nargs++;       // On to the next parameter
      ctl++;
      mod = LIT;     // Reset to literals
    }
  return nargs;

}

