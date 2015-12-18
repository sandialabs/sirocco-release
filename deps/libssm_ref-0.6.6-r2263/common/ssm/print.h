#ifndef SSM_PRINT_H
#define SSM_PRINT_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

// ssm_print:  adapted from msf_print, may change later
// Matthew Shane Farmer

//
// msf_print: some pretty printing stuff
// Matthew Shane Farmer

// string length and new string allocation

static inline size_t msf_vfstrlen(const char *fmt, va_list ap)
{
  int result = vsnprintf(NULL, 0, fmt, ap) + 1;
  return (size_t)result;
}


static inline const char *msf_malloc_vsprintf(const char *fmt, va_list ap)
{ 
  va_list apc;
  va_copy(apc, ap);
  size_t len = msf_vfstrlen(fmt,apc);
  char *result = malloc(len);
  vsnprintf(result, len, fmt, ap);
  return result;
}

static inline const char *msf_malloc_sprintf(const char *fmt, ...)
{
  const char *result;
  va_list ap;
  va_start(ap, fmt);
  result = msf_malloc_vsprintf(fmt, ap);
  va_end(ap);
  return result;
}
// experimental adaptable solution

static inline const char *msf_translate_colfmt(const char c)
{
  switch(c)
  {
    case 'L':
      return "%%-%d.%ds";
    case 'R':
      return "%%%d.%ds";
    case 'l':
      return "%%-%d.%ds ";
    case 'r':
    default:
      return "%%%d.%ds ";
  }
}

static inline void msf_puts(size_t cols, const char *colfmt, size_t *cw, const char **cstr)
{
  char fmt[cols][30];
  size_t lines[cols];
  size_t cols_done = 0;
  size_t output_len = 0;
  size_t max_col_len = 0;
  size_t i;
  for(i = 0; i < cols; i++)
  {
    lines[i] = ((strlen(cstr[i]) -1) / cw[i]) + 1;
    output_len += cw[i] + 1;
    if((cw[i] + 1) > max_col_len)
      max_col_len = cw[i] + 1;
    snprintf(fmt[i], 29, msf_translate_colfmt(colfmt[i]), cw[i], cw[i]);
  }
  char output[output_len];
  char col_output[max_col_len];
  while(cols_done < cols)
  {
    output[0] = 0;
    for(i = 0; i < cols; i++)
    {
      if(lines[i] > 0)
      {
        snprintf(col_output, max_col_len, fmt[i], cstr[i]); 
        cstr[i] += cw[i];
        lines[i]--;
        if(lines[i] == 0)
          cols_done++;
      }
      else
        snprintf(col_output, max_col_len, fmt[i], " ");
      
      strcat(output, col_output);
    }
    puts(output);
  }
}

static inline void msf_vprintf(const char *colfmt, va_list ap)
{
  size_t cols = strlen(colfmt);
  size_t cw[cols];
  const char *cstr[cols];
  const char *tmp;
  size_t i;
  for(i = 0; i < cols; i++)
  {
    cw[i] = (size_t)va_arg(ap, int);
    cstr[i] = va_arg(ap, const char *);
  }
  tmp = msf_malloc_vsprintf(cstr[cols-1], ap);
  cstr[cols-1] = tmp;

  msf_puts(cols, colfmt, cw, cstr);
  
  free((char *)tmp);
}

static inline void msf_printf(const char *colfmt, ...)
{
  va_list ap;
  va_start(ap, colfmt);
  msf_vprintf(colfmt, ap);
  va_end(ap);
}

#define ssm_print msf_print

#endif
