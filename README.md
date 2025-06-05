# kfs-lang-c
simple script language in C for possible use in embedded systems.


## Using
 - flex
 - bison
 - cmake - my Makefile is not compatibile with IDE ;-)
 - clion ( Non-commercial use)
 - Header-only Linked List in C - https://github.com/embeddedartistry/c-linked-list

## Todo - plan to add:
### kfs-lang features
 - add to_json - value convert into json
 - save string/value into file
 - libcurl get, post

## Maybe future
 - threads 
   - https://www.geeksforgeeks.org/multithreading-in-c/ 
   - https://www.geeksforgeeks.org/mutex-lock-for-linux-thread-synchronization/
 - 

### project improvements
 - export into binary package
 - create scripts for homebrew
 - describe add expression

## Add command into kfs-lang
1. add token in <i>parser.y</i> : <pre>%token TOKEN_DUMP     "dump"</pre>
2. add rule in <i>parser.y</i> : <pre>| "dump" ";"  { $$ = expression_create_dump(); }</pre>
3. add rule in <i>lexer.l</i>: <pre>"dump"      { return TOKEN_DUMP; }</pre>
3. create way to define <b>expression</b> in <i>expression.h</i> and <i>expression.c</i> - add name into <b>enum tagOperationType</b>, create procedure for create expression with this new type (<i>expression_create_dump()</i>) <pre>Expression *expression_create_dump() {
   Expression *expression = expression_new(eDUMP);
   return expression;
}</pre>
4. add evaluate this expression in <i>kfs_lang_eval.c</i> procedure <i>kfs_lang_eval_value</i><pre>...
case eDUMP:
  str = kfs_lang_vars_to_string(kfsLangEnv, KLVTS_ALL_SPACES);
  printf("%s",str);
  free(str);
  return NULL;
...</pre>
5. add some test's