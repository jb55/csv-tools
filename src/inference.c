#include "inference.h"
#include <ctype.h>

//enum field_type
//infer_type(const char * field, int len) {
//  int i = 0;
//  char c = 0;
//  int anydigits = 0;
//  for (i = 0; i < len; i++) {
//    c = field[i];
//    if (isdigit(c))
//      anydigits = 1;
//    if (isnumeric(c)) {
//     continue;
//    }
//    else return string_type;
//  }
//  if (!anydigits) return string_type;
//  return numeric_type;
//}

int
should_quote(const char * field, int len, char delim) {
  int i;
  char c;
  for (i = 0; i < len; i++) {
    c = field[i];
    if (c == delim) return 1;
  }
  return 0;
}
