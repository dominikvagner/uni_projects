/**
 *  Project: Implementation of imperative language compiler IFJ21
 *  @file scanner.c
 *
 *  @brief Implementation of lexical analyzer.
 *
 *  @author <xvagne10> Dominik Vágner
 */

#include "scanner.h"
#include "error.h"
#include "tokenList.h"
#include <ctype.h>
#include <malloc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int get_token_list(DLList *list) {
  token_ptr new = NULL;

  while (new == NULL || new->type != T_EOF) {
    new = NULL;
    int error = get_single_token(&new);
    if (error) {
      return ERR_LEX;
    }
    if (new->type != T_OTHER &&new->type != T_EOL) {
      DLL_InsertLast(list, new);
    }
  }

  return 0;
}

int get_single_token(token_ptr *insert_into) {
  *insert_into = malloc(sizeof(struct token));

  if (*insert_into == NULL) {
    return ERR_INTERNAL;
  }

  (*insert_into)->data = malloc(sizeof(struct token_data));

  if ((*insert_into)->data == NULL) {
    return ERR_INTERNAL;
  }

  (*insert_into)->data->string = malloc(sizeof(string));
  (*insert_into)->data->integer = 0;
  (*insert_into)->data->number = 0.0;
  (*insert_into)->next = NULL;
  (*insert_into)->prev = NULL;

  if (lex_fsm(insert_into) == ERR_LEX)
    return ERR_LEX;

  if ((*insert_into)->type == T_ID)
    is_token_keyword(insert_into);

  return 0;
}

bool is_token_keyword(token_ptr *token) {
  string *potential_keyword = (*token)->data->string;
  if (!strCompareConstant(potential_keyword, "do"))
    (*token)->type = T_K_DO;
  else if (!strCompareConstant(potential_keyword, "else"))
    (*token)->type = T_K_ELSE;
  else if (!strCompareConstant(potential_keyword, "end"))
    (*token)->type = T_K_END;
  else if (!strCompareConstant(potential_keyword, "function"))
    (*token)->type = T_K_FUNCTION;
  else if (!strCompareConstant(potential_keyword, "global"))
    (*token)->type = T_K_GLOBAL;
  else if (!strCompareConstant(potential_keyword, "if"))
    (*token)->type = T_K_IF;
  else if (!strCompareConstant(potential_keyword, "integer"))
    (*token)->type = T_K_INTEGER;
  else if (!strCompareConstant(potential_keyword, "local"))
    (*token)->type = T_K_LOCAL;
  else if (!strCompareConstant(potential_keyword, "nil"))
    (*token)->type = T_K_NIL;
  else if (!strCompareConstant(potential_keyword, "number"))
    (*token)->type = T_K_NUMBER;
  else if (!strCompareConstant(potential_keyword, "require"))
    (*token)->type = T_K_REQUIRE;
  else if (!strCompareConstant(potential_keyword, "return"))
    (*token)->type = T_K_RETURN;
  else if (!strCompareConstant(potential_keyword, "string"))
    (*token)->type = T_K_STRING;
  else if (!strCompareConstant(potential_keyword, "then"))
    (*token)->type = T_K_THEN;
  else if (!strCompareConstant(potential_keyword, "while"))
    (*token)->type = T_K_WHILE;

  if ((*token)->type != T_ID) {
    strFree((*token)->data->string);
  }

  return false;
}

void print_token_list(DLList *list) {
  token_ptr index = NULL;
  DLL_First(list);
  DLL_GetFirst(list, &index);

  if (index == NULL) {
    fprintf(stdout, "Token list is empty.");
    return;
  }

  print_single_token(index);

  while (index != NULL) {
    print_single_token(index);
    if (index->next == NULL)
      break;
    else {
      DLL_Next(list);
      DLL_GetValue(list, &index);
    }
  }
}

void print_single_token(token_ptr token) {
  if (token != NULL) {
    const char *token_type_strings[] = {"T_OTHER",
                                        "T_EOL",
                                        "T_EOF",
                                        "T_SUB",
                                        "T_ADD",
                                        "T_MUL",
                                        "T_DIV",
                                        "T_IDIV",
                                        "T_STRLEN",
                                        "T_LEFT_PAR",
                                        "T_RIGHT_PAR",
                                        "T_DOUBLE_DOT",
                                        "T_ASSIGN",
                                        "T_EQL",
                                        "T_GT",
                                        "T_GTE",
                                        "T_LT",
                                        "T_LTE",
                                        "T_NEQL",
                                        "T_CONCAT",
                                        "T_COMMA",
                                        "T_ID",
                                        "T_INT",
                                        "T_DOUBLE",
                                        "T_STRING",
                                        "T_K_DO",
                                        "T_K_ELSE",
                                        "T_K_END",
                                        "T_K_FUNCTION",
                                        "T_K_GLOBAL",
                                        "T_K_IF",
                                        "T_K_INTEGER",
                                        "T_K_LOCAL",
                                        "T_K_NIL",
                                        "T_K_NUMBER",
                                        "T_K_REQUIRE",
                                        "T_K_RETURN",
                                        "T_K_STRING",
                                        "T_K_THEN",
                                        "T_K_WHILE",
                                        "",
                                        "T_P_EXPRESSION"};

    fprintf(
        stdout,
        "Next token:\n  Token Type = %s\n  Line number = %d\n  Column number = %d\n  Token data:\n\
    Token string = %s\n    Token integer = %d\n    Token float = %lf\n\n",
        token_type_strings[token->type], token->line_num, token->col_num,
        strGetString(token->data->string), token->data->integer,
        token->data->number);
  }
}

int lex_fsm(token_ptr *token) {
  lexfsm_state pstate = S_START;
  lexfsm_state nstate;

  bool fsm_activator = true;
  static unsigned int line_num = 0;
  static unsigned int col_num = 1;

  static uint8_t escape_code = 0;

  string *read_input = malloc(sizeof(string));
  strInit(read_input);

  (*token)->type = T_OTHER;
  (*token)->line_num = line_num;
  (*token)->col_num = col_num;

  static char current = '\n';

  while (fsm_activator) {
    nstate = S_NULL;

    switch (pstate) {
    case S_START:
      if (current == '-')
        nstate = S_SUB;
      else if (current == '+')
        nstate = S_ADD;
      else if (current == '*')
        nstate = S_MUL;
      else if (current == '/')
        nstate = S_DIV;
      else if (current == '#')
        nstate = S_STRLEN;
      else if (current == '(')
        nstate = S_LEFT_PAR;
      else if (current == ')')
        nstate = S_RIGHT_PAR;
      else if (current == ':')
        nstate = S_DOUBLE_DOT;
      else if (current == '=')
        nstate = S_ASSIGN;
      else if (current == '>')
        nstate = S_GT;
      else if (current == '.')
        nstate = S_DOT0;
      else if (current == '<')
        nstate = S_LT;
      else if (current == ',')
        nstate = S_COMMA;
      else if (current == '_' || isalpha(current))
        nstate = S_ID;
      else if (current == '~')
        nstate = S_TILDA;
      else if (current == '0')
        nstate = S_INT0;
      else if (isdigit(current))
        nstate = S_INT;
      else if (current == '\"')
        nstate = S_STR_START;
      else if (current == '\n')
        nstate = S_EOL;
      else if (current == EOF)
        nstate = S_EOF;
      else if (isspace(current))
        nstate = S_SPACE;
      else
        nstate = S_ERR;

    case S_SPACE:
      if (current != '\n' && isspace(current))
        nstate = S_SPACE;
      break;

    case S_SUB:
      if (current == '-')
        nstate = S_COMMENT0;
      else
        (*token)->type = T_SUB;
      break;

    case S_COMMENT0:
      if (current == '\n') {
        ungetc(current, stdin);
        nstate = S_START;
      } else if (current == EOF) {
        nstate = S_ERR;
      } else if (current == '[')
        nstate = S_COMMENT1;
      else
        nstate = S_COMMENT0;
      break;

    case S_COMMENT1:
      if (current == '\n') {
        ungetc(current, stdin);
        nstate = S_START;
      } else if (current == EOF) {
        nstate = S_ERR;
      } else if (current == '[')
        nstate = S_COMMENT2;
      else
        nstate = S_COMMENT1;
      break;

    case S_COMMENT2:
      if (current == EOF) {
        nstate = S_ERR;
      } else if (current == ']')
        nstate = S_COMMENT3;
      else
        nstate = S_COMMENT2;
      break;

    case S_COMMENT3:
      if (current == EOF) {
        nstate = S_ERR;
      } else if (current == ']')
        nstate = S_START;
      else
        nstate = S_COMMENT2;
      break;

    case S_ADD:
      (*token)->type = T_ADD;
      break;

    case S_MUL:
      (*token)->type = T_MUL;
      break;

    case S_DIV:
      if (current == '/')
        nstate = S_IDIV;
      else
        (*token)->type = T_DIV;
      break;

    case S_IDIV:
      (*token)->type = T_IDIV;
      break;

    case S_STRLEN:
      (*token)->type = T_STRLEN;
      break;

    case S_LEFT_PAR:
      (*token)->type = T_LEFT_PAR;
      break;

    case S_RIGHT_PAR:
      (*token)->type = T_RIGHT_PAR;
      break;

    case S_DOUBLE_DOT:
      (*token)->type = T_DOUBLE_DOT;
      break;

    case S_ASSIGN:
      if (current == '=')
        nstate = S_EQL;
      else
        (*token)->type = T_ASSIGN;
      break;

    case S_EQL:
      (*token)->type = T_EQL;
      break;

    case S_GT:
      if (current == '=')
        nstate = S_GTE;
      else
        (*token)->type = T_GT;
      break;

    case S_GTE:
      (*token)->type = T_GTE;
      break;

    case S_LT:
      if (current == '=')
        nstate = S_LTE;
      else
        (*token)->type = T_LT;
      break;

    case S_LTE:
      (*token)->type = T_LTE;
      break;

    case S_COMMA:
      (*token)->type = T_COMMA;
      break;

    case S_ID:
      if (current == '_' || isalnum(current)) {
        strAppendChar(read_input, current);
        nstate = S_ID;
      } else {
        (*token)->data->string = read_input;
        (*token)->type = T_ID;
      }
      break;

    case S_INT:
      if (isdigit(current))
        nstate = S_INT;
      else if (current == '.')
        nstate = S_FP_DOT;
      else if (current == 'e' || current == 'E')
        nstate = S_EXP0;
      else {
        sscanf(strGetString(read_input), "%i", &(*token)->data->integer);
        (*token)->type = T_INT;
        break;
      }
      strAppendChar(read_input, current);
      break;

    case S_INT0:
      if (isdigit(current))
        nstate = S_INT0;
      else if (current == '.')
        nstate = S_FP_DOT;
      else if (current == 'e' || current == 'E')
        nstate = S_EXP0;
      else if (current == 'x' || current == 'X')
        nstate = S_HEX0;
      else {
        sscanf(strGetString(read_input), "%i", &(*token)->data->integer);
        (*token)->type = T_INT;
        break;
      }
      strAppendChar(read_input, current);
      break;

    case S_HEX0:
      if (isxdigit(current))
        nstate = S_HEX1;
      else {
        nstate = S_ERR;
        break;
      }
      strAppendChar(read_input, current);
      break;

    case S_HEX1:
      if (isxdigit(current))
        nstate = S_HEX1;
      else {
        sscanf(strGetString(read_input), "%d", &(*token)->data->integer);
        (*token)->type = T_INT;
        break;
      }
      strAppendChar(read_input, current);
      break;

    case S_FP_DOT:
      if (isdigit(current))
        nstate = S_NUMBER;
      else {
        nstate = S_ERR;
      }
      strAppendChar(read_input, current);
      break;

    case S_NUMBER:
      if (isdigit(current))
        nstate = S_NUMBER;
      else if (current == 'e' || current == 'E')
        nstate = S_EXP0;
      else {
        sscanf(strGetString(read_input), "%lf", &(*token)->data->number);
        (*token)->type = T_DOUBLE;
        break;
      }
      strAppendChar(read_input, current);
      break;

    case S_EXP0:
      if (current == '+' || current == '-')
        nstate = S_EXP1;
      else if (isdigit(current))
        nstate = S_EXP2;
      else {
        nstate = S_ERR;
        break;
      }
      strAppendChar(read_input, current);
      break;

    case S_EXP1:
      if (isdigit(current))
        nstate = S_EXP2;
      else {
        nstate = S_ERR;
        break;
      }
      strAppendChar(read_input, current);
      break;

    case S_EXP2:
      if (isdigit(current))
        nstate = S_EXP2;
      else {
        sscanf(strGetString(read_input), "%lf", &(*token)->data->number);
        (*token)->type = T_DOUBLE;
        break;
      }
      strAppendChar(read_input, current);
      break;

    case S_EOL:
      line_num++;
      col_num = 1;
      (*token)->type = T_EOL;
      break;

    case S_EOF:
      (*token)->type = T_EOF;
      break;

    case S_DOT0:
      if (current == '.')
        nstate = S_DOT1;
      else
        nstate = S_ERR;
      break;

    case S_DOT1:
      (*token)->type = T_CONCAT;
      break;

    case S_TILDA:
      if (current == '=')
        nstate = S_NEQL;
      else
        nstate = S_ERR;
      break;

    case S_NEQL:
      (*token)->type = T_NEQL;
      break;

    case S_STR_START:
      if (current > 31 && current != '\\' && current != '\"') {
        nstate = S_STR_START;
        strAppendChar(read_input, current);
      } else if (current == '\"')
        nstate = S_STR_FIN;
      else if (current == '\\') {
        nstate = S_STR_T1;
        escape_code = 0;
      }
      break;

    case S_STR_T1:
      if (current == '\\') {
        strAppendChar(read_input, '\\');
        nstate = S_STR_START;
      } else if (current == '\"') {
        strAppendChar(read_input, '\"');
        nstate = S_STR_START;
      } else if (current == 'n') {
        strAppendChar(read_input, '\n');
        nstate = S_STR_START;
      } else if (current == 't') {
        strAppendChar(read_input, '\t');
        nstate = S_STR_START;
      } else if (current == '0') {
        nstate = S_STR_T2;
      } else if (current == '1') {
        nstate = S_STR_T5;
        escape_code += 100;
      } else if (current == '2') {
        nstate = S_STR_T6;
        escape_code += 200;
      }
      break;

    case S_STR_T2:
      if (current == '0')
        nstate = S_STR_T3;
      else if (isdigit(current)) {
        nstate = S_STR_T4;
        escape_code += ((current - '0') * 10);
      } else {
        nstate = S_ERR;
      }
      break;

    case S_STR_T3:
      if (isdigit(current) && current != '0') {
        escape_code += (current - '0');
        strAppendChar(read_input, escape_code);
        nstate = S_STR_START;
      } else
        nstate = S_ERR;
      break;

    case S_STR_T4:
      if (isdigit(current)) {
        escape_code += (current - '0');
        strAppendChar(read_input, escape_code);
        nstate = S_STR_START;
      } else {
        nstate = S_ERR;
      }
      break;

    case S_STR_T5:
      if (isdigit(current)) {
        escape_code += ((current - '0') * 10);
        nstate = S_STR_T4;
      } else {
        nstate = S_ERR;
      }
      break;

    case S_STR_T6:
      if (current >= '0' && current <= '4') {
        escape_code += ((current - '0') * 10);
        nstate = S_STR_T7;
      } else if (current == '5') {
        nstate = S_STR_T8;
      } else {
        nstate = S_ERR;
      }
      break;

    case S_STR_T7:
      if (isdigit(current)) {
        escape_code += (current - '0');
        strAppendChar(read_input, escape_code);
        nstate = S_STR_START;
      } else {
        nstate = S_ERR;
      }
      break;

    case S_STR_T8:
      if (current >= '0' && current <= '5') {
        escape_code += (current - '0');
        strAppendChar(read_input, escape_code);
        nstate = S_STR_START;
      } else {
        nstate = S_ERR;
      }
      break;

    case S_STR_FIN:
      (*token)->data->string = read_input;
      (*token)->type = T_STRING;
      break;

    default:
      break;
    }

    if ((*token)->type != T_OTHER || nstate == S_NULL) {
      break;
    } else if (nstate == S_ERR) {
      if (pstate != S_START)
        break;
      fsm_activator = false;
    }

    if (strGetLength(read_input) == 0) {
      switch (nstate) {
      case S_ID:
      case S_INT:
      case S_INT0:
      case S_HEX0:
      case S_HEX1:
      case S_FP_DOT:
      case S_NUMBER:
      case S_EXP0:
      case S_EXP1:
      case S_EXP2:
      case S_STR_T1:
      case S_STR_T2:
      case S_STR_T3:
      case S_STR_T4:
      case S_STR_T5:
      case S_STR_T6:
      case S_STR_T7:
      case S_STR_T8:
      case S_STR_FIN:
        strAppendChar(read_input, current);
      default:
        break;
      }
    } else if (pstate == S_START) {
      strClear(read_input);
    }

    if (current != EOF) {
      col_num++;
      current = getc(stdin);
    }

    pstate = nstate;
  }

  return nstate == S_ERR ? ERR_LEX : 0;
}