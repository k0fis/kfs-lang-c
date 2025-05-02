
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include "rt/utils.h"

char *replace_system_props(regex_t regex, char *input);

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