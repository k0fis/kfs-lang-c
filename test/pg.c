
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include "rt/utils.h"

#define BUFFER_LEN 1024

char * replace_system_props(regex_t regex, char *input) {
  int result;
  char msgbuf[BUFFER_LEN];
  regmatch_t match;
  result = regexec(&regex, input, 1, &match, 0);
  if (!result) {
    char *cursor = input;
    KFS_MALLOC_CHAR(output, 1);
    while (match.rm_so >= 0) {
      int len1 = (int)(match.rm_so)+1;
      if (len1 > 1) {
        int outlen = strlen(output);
        output = realloc(output, outlen + len1);
        snprintf(output+outlen, len1, "%.*s", len1-1, cursor);
      }
      int outlen = strlen(output);
      int nameLen = (int)(match.rm_eo - match.rm_so)-4 + 1;
      KFS_MALLOC_CHAR(name, nameLen);
      snprintf(name, nameLen, "%.*s", nameLen-1, cursor+match.rm_so+2);
      char *replace = getenv(name);
      KFS_DEBUG_ML("name: %s\nreplace: %s", name, replace);
      free(name);
      if (replace != NULL) {
        output = realloc(output, outlen + strlen(replace)+1);
        strcat(output, replace);
      } else {
        output = realloc(output, outlen + nameLen + 5);
        sprintf(output+outlen,"{{%.*s}}", nameLen-1, cursor+match.rm_so+2);
      }

      cursor += match.rm_eo;
      result = regexec(&regex, cursor, 1, &match, 0);
      if (result) {
        int len = strlen(cursor);
        if (len > 0) {
          int outlen = strlen(output);
          output = realloc(output, outlen + len + 1);
          strcat(output,  cursor);
        }
        break;
      }
    }
    return output;
  } else if (result == REG_NOMATCH) {
    KFS_DEBUG("No match: %s", input);
  } else {
    regerror(result, &regex, msgbuf, sizeof(msgbuf));
    KFS_ERROR("Regex match failed: %s", msgbuf);
  }
  return input;
}

int main() {
  regex_t regex;
  char *regextStr = "{{[^ }]+}}";
  int result = regcomp(&regex, regextStr, REG_EXTENDED);
  if (result) {
    KFS_ERROR("Could not compile regex: %s", regextStr);
  } else {
    setenv("napicu", "popici", 0);
    char * input = "{{napicu}} pako je defo a je to -{{napicu}}- a nebo taky -{{napicu2}}{{napicu}}- debil";
    char *out = replace_system_props(regex, input);
    KFS_DEBUG_ML("input: %s\noutput: %s", input, out);
    free(out);
    regfree(&regex);
  }
}