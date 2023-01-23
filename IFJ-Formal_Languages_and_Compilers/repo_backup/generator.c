/**
 *  @file generator.c
 *
 *  @brief Implementation of IFJCode21 generator for IFJ21 compiler.
 *
 *  @author <xvagne10> Dominik Vágner
 */

#include "generator.h"
#include "error.h"
#include "scanner.h"
#include "str.h"
#include "tokenList.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void generate_code_from_list(DLList *list) {
  token_ptr index = NULL;
  DLL_First(list);
  DLL_GetFirst(list, &index);

  if (index == NULL)
    return;

  string *base_code;
  base_code = malloc(sizeof(string));
  strInit(base_code);
  strAppendStr(base_code, ".IFJcode21\nJUMP $$main\n\n");
  generate_built_in_code(base_code);
  printf("%s", strGetString(base_code));

  string *main;
  main = malloc(sizeof(string));
  strInit(main);
  strAppendStr(main, "LABEL $$main\nCREATEFRAME\nPUSHFRAME\n");

  generate_code_from_token(&index, main);

  while (index != NULL) {
    generate_code_from_token(&index, main);
    if (index->next == NULL)
      break;
    else {
      DLL_Next(list);
      DLL_GetValue(list, &index);
    }
  }

  printf("%s", strGetString(main));

  return;
}

void generate_code_from_token(token_ptr *token, string *main) {
  static int def_function = 0;
  static int def_params = 0;
  static int def_retvals = 0;
  static int def_var = 0;
  static int assign = 0;
  static int reassign = 0;
  static int func_body = 0;
  static int func_call = 0;
  static int to_be_called_func = 0;
  static int main_body_call = 0;
  static int return_func = 0;
  static char *called_func;
  static char *var_name;
  static char *var_type;

  static int curr_param_num = 0;
  static int curr_func_param_num = 0;
  static int curr_retval_num = 0;

  static int curr_func_write = 0;

  static int require_read = 0;

  if (!require_read) {
    if ((*token)->type == T_STRING &&
        !strCompareConstant((*token)->data->string, "ifj21")) {
      require_read = 1;
      return;
    } else {
      return;
    }
  }

  token_type t_type = (*token)->type;

  switch (t_type) {
  case T_K_FUNCTION:
    def_function = 1;
    break;

  case T_ID:
    if (def_function && !def_params && !func_body) {
      printf("LABEL %s\nPUSHFRAME\nCREATEFRAME\n",
             strGetString((*token)->data->string));
      printf("DEFVAR TF@%%%%express\n");
      printf("DEFVAR TF@%%%%tmpparam1\n");
      printf("DEFVAR TF@%%%%tmpparam2\n");
      printf("DEFVAR TF@%%%%tmpret1\n");
    } else if (def_params) {
      curr_param_num++;
      printf("DEFVAR TF@%c%s\nMOVE TF@%c%s LF@%c%d\n", '%',
             strGetString((*token)->data->string), '%',
             strGetString((*token)->data->string), '%', curr_param_num);
    } else if ((reassign || assign) && (*token)->next->type == T_LEFT_PAR &&
               !func_call) {
      return_func = 1;
      func_call = 1;
      called_func = strGetString((*token)->data->string);
      break;
    } else if (func_body && !def_var && !func_call &&
               (*token)->next->type != T_ASSIGN) {
      if ((*token)->next->type == T_LEFT_PAR) {
        func_call = 1;
        called_func = strGetString((*token)->data->string);
      }
    } else if (func_body && def_var && !func_call) {
      var_name = strGetString((*token)->data->string);
      // printf("DEFVAR TF@\037%s\n", var_name);
    } else if (func_body && func_call && curr_func_write) {
      printf("WRITE TF@%c%s\n", '%', strGetString((*token)->data->string));
    } else if (func_body && func_call && !to_be_called_func) {
      to_be_called_func = 1;
      curr_func_param_num++;
      printf("DEFVAR TF@%c%d\nMOVE TF@%c%d TF@%c%s\n", '%', curr_func_param_num,
             '%', curr_func_param_num, '%',
             strGetString((*token)->data->string));
      if ((*token)->next->type == T_RIGHT_PAR) {
        to_be_called_func = 0;
        curr_func_param_num = 0;
      }
    } else if (func_body && func_call && to_be_called_func) {
      curr_func_param_num++;
      printf("DEFVAR TF@%c%d\nMOVE TF@%c%d TF@%c%s\n", '%', curr_func_param_num,
             '%', curr_func_param_num, '%',
             strGetString((*token)->data->string));
      if ((*token)->next->type == T_RIGHT_PAR) {
        to_be_called_func = 0;
        curr_func_param_num = 0;
        printf("CREATEFRAME\nCALL %s\n", called_func);
      }
    } else if (func_body && (*token)->next->type == T_ASSIGN) {
      var_name = strGetString((*token)->data->string);
      reassign = 1;
    } else if (!func_body) {
      if ((*token)->next->type == T_LEFT_PAR &&
          !strcmp(strGetString((*token)->data->string), "write")) {
        func_call = 1;
        curr_func_write = 1;
        main_body_call = 1;
        called_func = strGetString((*token)->data->string);
        strAppendStr(main, "DEFVAR TF@%%express\n");
        strAppendStr(main, "DEFVAR TF@%%tmpparam1\n");
        strAppendStr(main, "DEFVAR TF@%%tmpparam2\n");
        strAppendStr(main, "DEFVAR TF@%%tmpret1\n");
        break;
      } else {
        strAppendStr(main, "CREATEFRAME\nCALL ");
        strAppendStr(main, strGetString((*token)->data->string));
        strAppendChar(main, '\n');
      }
    }
    break;

  case T_LEFT_PAR:
    if (def_function && !func_body) {
      def_params = 1;
      break;
    }
    if (func_call) {
      if (!strcmp(called_func, "write"))
        curr_func_write = 1;
    }
    break;

  case T_RIGHT_PAR:
    if (def_params) {
      def_params = 0;
      curr_param_num = 0;
      if (def_function && (*token)->next->type == T_DOUBLE_DOT) {
        def_retvals = 1;
      } else if (def_function) {
        func_body = 1;
      }
      break;
    } else if (func_call) {
      func_call = 0;
      if (curr_func_write) {
        curr_func_write = 0;
      } else if (curr_func_param_num == 0) {
        printf("CALL %s\n", called_func);
        if (return_func) {
          return_func = 0;
          printf("MOVE TF@%c%s TF@%cfuncret\n", '%', var_name, '%');
        }
      }
    } else if (main_body_call) {
      main_body_call = 0;
    }
    break;

  case T_P_EXPRESSION:
    if (strGetLength((*token)->data->string) == 0) {
      break;
    }

    char *postfix_exp = strGetString((*token)->data->string);
    const char delim[2] = " ";
    char *tk;
    char *buffer = malloc(sizeof(char) * 2048);
    char *buffer_start = buffer;

    tk = strtok(postfix_exp, delim);

    while (tk != NULL) {
      // printf("TK = %s\n", tk);
      if (tk[0] == '\'' && tk[strlen(tk) - 1] == '\'') {
        buffer += sprintf(buffer, "PUSHS ");
        if (!strcmp(tk, "'\\092\\010'")) {
          buffer += sprintf(buffer, "string@\\010");
        } else {
          tk++;
          if (tk[0] == '\\' && tk[1] == '0' && tk[2] == '9' && tk[3] == '2' &&
              tk[4] == '\\' && tk[5] == '0' && tk[6] == '1' && tk[7] == '0') {
            tk = tk + 4;
          }
          tk[strlen(tk) - 1] = 0;
          buffer += sprintf(buffer, "string@%s", tk);
        }
        buffer += sprintf(buffer, "\n");
      } else if (atoi(tk) == 0 && tk[0] == '0') {
        buffer += sprintf(buffer, "PUSHS int@0\n");
      } else if (atoi(tk) != 0 && strchr(tk, '.') == NULL) {
        buffer += sprintf(buffer, "PUSHS int@%d\n", atoi(tk));
      } else if (atof(tk) != 0) {
        buffer += sprintf(buffer, "PUSHS float@%a\n", atof(tk));
      } else if (!strcmp(tk, "-")) {
        buffer += sprintf(buffer, "SUBS\n");
      } else if (!strcmp(tk, "+")) {
        buffer += sprintf(buffer, "ADDS\n");
      } else if (!strcmp(tk, "*")) {
        buffer += sprintf(buffer, "MULS\n");
      } else if (!strcmp(tk, "/")) {
        buffer += sprintf(buffer, "DIVS\n");
      } else if (!strcmp(tk, "//")) {
        buffer += sprintf(buffer, "IDIVS\n");
      } else if (!strcmp(tk, "<")) {
        buffer += sprintf(buffer, "LTS\n");
      } else if (!strcmp(tk, ">")) {
        buffer += sprintf(buffer, "GTS\n");
      } else if (!strcmp(tk, "==")) {
        buffer += sprintf(buffer, "EQS\n");
      } else if (!strcmp(tk, "<=")) {
        buffer += sprintf(buffer, "POPS TF@%stmpparam2\n", "%%");
        buffer += sprintf(buffer, "POPS TF@%stmpparam1\n", "%%");
        buffer += sprintf(buffer, "PUSHS TF@%stmpparam1\n", "%%");
        buffer += sprintf(buffer, "PUSHS TF@%stmpparam2\n", "%%");
        buffer += sprintf(buffer, "LTS\n");
        buffer += sprintf(buffer, "POPS TF@%stmpret1\n", "%%");
        buffer += sprintf(buffer, "PUSHS TF@%stmpparam1\n", "%%");
        buffer += sprintf(buffer, "PUSHS TF@%stmpparam2\n", "%%");
        buffer += sprintf(buffer, "EQS\n");
        buffer += sprintf(buffer, "PUSHS TF@%stmpret1\n", "%%");
        buffer += sprintf(buffer, "ORS\n");
      } else if (!strcmp(tk, ">=")) {
        buffer += sprintf(buffer, "POPS TF@%stmpparam2\n", "%%");
        buffer += sprintf(buffer, "POPS TF@%stmpparam1\n", "%%");
        buffer += sprintf(buffer, "PUSHS TF@%stmpparam1\n", "%%");
        buffer += sprintf(buffer, "PUSHS TF@%stmpparam2\n", "%%");
        buffer += sprintf(buffer, "GTS\n");
        buffer += sprintf(buffer, "POPS TF@%stmpret1\n", "%%");
        buffer += sprintf(buffer, "PUSHS TF@%stmpparam1\n", "%%");
        buffer += sprintf(buffer, "PUSHS TF@%stmpparam2\n", "%%");
        buffer += sprintf(buffer, "EQS\n");
        buffer += sprintf(buffer, "PUSHS TF@%stmpret1\n", "%%");
        buffer += sprintf(buffer, "ORS\n");
      } else if (!strcmp(tk, "~=")) {
        buffer += sprintf(buffer, "EQS\n");
        buffer += sprintf(buffer, "NOTS\n");
      } else if (!strcmp(tk, "..")) {
        buffer += sprintf(buffer, "POPS TF@%stmpparam2\n", "%%");
        buffer += sprintf(buffer, "POPS TF@%stmpparam1\n", "%%");
        buffer += sprintf(buffer,
                          "CONCAT TF@%stmpret1 TF@%stmpparam1 TF@%stmpparam2\n",
                          "%%", "%%", "%%");
        buffer += sprintf(buffer, "PUSHS TF@%stmpret1\n", "%%");
      } else if (!strcmp(tk, "#")) {
        buffer += sprintf(buffer, "POPS TF@%stmpparam1\n", "%%");
        buffer +=
            sprintf(buffer, "STRLEN TF@%stmpret1 TF@%stmpparam1\n", "%%", "%%");
        buffer += sprintf(buffer, "PUSHS TF@%stmpret1\n", "%%");
      } else {
        buffer += sprintf(buffer, "PUSHS TF@%c%s\n", '%', tk);
      }
      tk = strtok(NULL, delim);
    }
    buffer += sprintf(buffer, "POPS TF@%sexpress\n", "%%");
    buffer += sprintf(buffer, "CLEARS\n");

    if (assign) {
      buffer += sprintf(buffer, "DEFVAR TF@%c%s\n", '%', var_name);
      buffer +=
          sprintf(buffer, "MOVE TF@%c%s TF@%sexpress\n", '%', var_name, "%%");
      assign = 0;
    } else if (reassign) {
      buffer +=
          sprintf(buffer, "MOVE TF@%c%s TF@%sexpress\n", '%', var_name, "%%");
      reassign = 0;
    } else if (curr_func_write) {
      buffer += sprintf(buffer, "WRITE TF@%sexpress\n", "%%");
    }
    if (main_body_call)
      strAppendStr(main, buffer_start);
    else
      printf("%s", buffer_start);

    break;

  case T_K_LOCAL:
    if (func_body) {
      def_var = 1;
    }
    break;

  case T_K_END:
    if (def_function && func_body) {
      def_function = 0;
      func_body = 0;
      printf("POPFRAME\nRETURN\n\n");
    }
    break;

  case T_K_INTEGER:
  case T_K_NUMBER:
  case T_K_STRING:
    if (def_retvals) {
      curr_retval_num++;
      printf("DEFVAR LF\037retval%d\nMOVE LF@\037retval%d nil@nil\n",
             curr_retval_num, curr_retval_num);

      if ((*token)->next->type != T_COMMA) {
        def_retvals = 0;
        func_body = 1;
      }
    } else if (def_var) {
      if (t_type == T_K_INTEGER)
        var_type = "int";
      else if (t_type == T_K_NUMBER)
        var_type = "float";
      else if (t_type == T_K_STRING)
        var_type = "string";

      def_var = 0;
      if ((*token)->next->type == T_ASSIGN) {
        assign = 1;
      }
    }
    break;

  default:
    break;
  }

  return;
}

void generate_built_in_code(string *code) {
  strAppendStr(code, "\
# PREDEFINED FUNCTIONS START\n\
LABEL $$check_if_int\n\
TYPE TF@$type_check_result TF@$type_check\n\
JUMPIFNEQ $$type_error TF@$type_check_result string@int\n\
RETURN\n\
\n\
LABEL $$type_error\n\
WRITE string@\\010\\032Incompatible\\032type\\032error.\n\
EXIT int@4\n\
\n\
LABEL readi\n\
PUSHFRAME\n\
DEFVAR LF@%funcret\n\
READ LF@%funcret int\n\
POPFRAME\n\
RETURN\n\
\n\
LABEL readn\n\
PUSHFRAME\n\
DEFVAR LF@%funcret\n\
READ LF@%funcret float\n\
POPFRAME\n\
RETURN\n\
\n\
LABEL reads\n\
PUSHFRAME\n\
DEFVAR LF@%funcret\n\
READ LF@%funcret string\n\
POPFRAME\n\
RETURN\n\
\n\
# PREDEFINED FUNCTIONE END\n\
\n");
}