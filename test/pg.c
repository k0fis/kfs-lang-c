
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include "rt/utils.h"

#define BUFFER_LEN 1024

char *input = "{{napicu}} pako je defo a je to -{{napicu}}- a nebo taky -{{napicu}}{{napicu}}- debil";

int main() {
  regex_t regex;
  int result;
  char msgbuf[BUFFER_LEN];
  regmatch_t match;

  result = regcomp(&regex, "{{[^ }]+}}", REG_EXTENDED);
  if (result) {
    KFS_ERROR("Could not compile regex");
  } else {
    KFS_INFO(input);
    result = regexec(&regex, input, 1, &match, 0);
    if (!result) {
      KFS_INFO("Match");
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
        output = realloc(output, outlen + nameLen);
        snprintf(output+outlen, nameLen, "%.*s", nameLen-1, cursor+match.rm_so+2);

        cursor += match.rm_eo;
        result = regexec(&regex, cursor, 1, &match, 0);
        if (result) {
          int outlen = strlen(output);
          int len = strlen(cursor);
          output = realloc(output, outlen + len + 1);
          strcat(output,  cursor);
          break;
        }
      }
      KFS_INFO(output);
      free(output);
    } else if (result == REG_NOMATCH) {
      KFS_INFO("No match");
    } else {
      regerror(result, &regex, msgbuf, sizeof(msgbuf));
      KFS_ERROR("Regex match failed"); KFS_ERROR(msgbuf);
    }
  }
  regfree(&regex);
}