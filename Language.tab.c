/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "Language.y"

void yyerror (char *s);
int yylex();
#include <stdio.h>     
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "symTab.h"

extern char* yytext;
char buffer[2048] = {'\0'};
const char fileName[15] = "yaccOutput.txt";
static int idx=0;

typedef enum { Assign, Output } GetSizeMode;

// Symbol Table Functions
int checkIfExists(char* var) {
    for (int i=0 ; i<idx ; ++i)
        if (strcmp(SymTable[i].name, var)==0)
            return 1;
    return 0;
}

varType getType(char* var) {
    for (int i=0 ; i<idx ; ++i)
        if (strcmp(SymTable[i].name, var)==0)
            return SymTable[i].type;
    return None;
}

void addToSymbolTable(char* varName, varType typ) {
    char msg[32];
    char *token = strtok(varName, ",");
    while (token != NULL) {
        if (checkIfExists(token) == 1) {
            sprintf(msg, "%s already exists", varName);
            yyerror(msg);
        }
        SymTable[idx].name = malloc(strlen(token) + 1);
        strcpy(SymTable[idx].name, token);
        SymTable[idx].type = typ;
        printf("\t*Insert \"%s\" into symtab; typ:%d\n", token, typ);
        ++idx;
        token = strtok(NULL, ",");
    }
}

// Literal Helper Functions

char* copyStr(char* str) {
    char *newStr, *p;
    newStr = (char*)malloc(strlen(str) + 1);   
    strcpy(newStr, str);
    return newStr;
}

char* copyNumber(int num) {
    char msg[32];
    char* strNum = malloc(20 * sizeof(char));
    if (strNum == NULL) {
        sprintf(msg, "Memory allocation failed.");
        yyerror(msg);
    }
    
    snprintf(strNum, 20, "%d", num);
    printf("\t*Copy %s\n", strNum); 

    return strNum;
}

int isNumber(char* str) {
    for (int i = 0; i < strlen(str); i++)
        if (!isdigit(str[i]))
            return 0;
    return 1;
}

int isString(char *str) {
    if (str && str[0] == '"' && str[strlen(str) - 1] == '"') {
        return 1; 
    }
    return 0; 
}

int isSet(char* str) {
    if (str[0] == '[' && str[strlen(str) - 1] == ']')
        for (int i = 1; i < strlen(str) - 1; i++)
            if (!isdigit(str[i]) && str[i] != ',')
                return 0;
    return 1;
}

char* yaccStrndup(const char* str, size_t n) {
    char* result;
    size_t len = strlen(str);

    if (n < len) {
        len = n;
    }

    result = (char*)malloc(len + 1);
    if (!result) {
        return NULL;
    }

    result[len] = '\0';
    return (char*)memcpy(result, str, len);
}

void parseExpression(char* value, char** expr1, char** expr2, char** op) {
    *expr1 = value;
    *op = NULL;
    *expr2 = NULL;

    char* current = value;
    char* operatorPos = NULL;
    int parenDepth = 0;

    while (*current == ' ') current++;

    while (*current != '\0') {
        if (*current == '(') {
            parenDepth++;
        } else if (*current == ')') {
            parenDepth--;
        } else if (parenDepth == 0 && (*current == '+' || *current == '-' || *current == '*')) {
            operatorPos = current;
            break;
        }
        current++;
    }

    if (operatorPos != NULL) {
        *op = malloc(2); 
        (*op)[0] = *operatorPos;
        (*op)[1] = '\0'; 

        *operatorPos = '\0';
        *expr2 = operatorPos + 1; 

        while (**expr2 == ' ') (*expr2)++;
    }

    char* end = *expr1 + strlen(*expr1) - 1;
    while (end > *expr1 && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
}

void parseConditions(char* value, char** expr1, char** expr2, char** op) {
    *expr1 = NULL;
    *expr2 = NULL;
    *op = NULL;

    if (value[0] == '!') {
        *expr1 = strdup("!");
        *expr2 = strdup(value + 1);  
        *op = NULL;  
    } else {
        if (strstr(value, "==")) {
            *op = strdup("==");
        } else if (strstr(value, "<=")) {
            *op = strdup("<=");
        } else if (strstr(value, ">=")) {
            *op = strdup(">=");
        }
        else if (strchr(value, '>')) {
            *op = strdup(">");
        } else if (strchr(value, '<')) {
            *op = strdup("<");
        }

        if (*op != NULL) {
            *expr1 = yaccStrndup(value, strstr(value, *op) - value);  
            *expr2 = strdup(strstr(value, *op) + strlen(*op));   
        } else {
            *expr1 = strdup(value);
        }
    }

    if (*expr1 != NULL) {
        *expr1 = strtok(*expr1, " ");
    }
    if (*expr2 != NULL) {
        *expr2 = strtok(*expr2, " ");
    }
}

char* addToSet(char* list, char* str) {
    char *p;
    buffer[0] = '\0'; 
    strcpy(buffer, list);
    strcat(buffer, ",");

    if ((p = strchr(str, ']')) || (p = strchr(str, ','))) {
        *p = '\0';
    }
    strcat(buffer, str);
    size_t new_len = strlen(buffer) + 1;
    char *new_result = realloc(list, new_len);
    
    if (new_result == NULL) {
        yyerror("Memory allocation failed.");
        return list;
    }

    strcpy(new_result, buffer);
    printf("\t*NewStrList:%s\n", new_result);

    memset(buffer, '\0', sizeof(buffer));
    return new_result;
}

int checkIntegerOnlyOperations(char* str) {
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == '(' || str[i] == '/'|| str[i] == '|') {
            return 1;
        }
    }
    return 0;
}

int checkSetCollectionOnlyOperations(char* str) {
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == '!') {
            return 1;
        }
    }
    return 0;
}

int checkSetCollectionOperations(char* str) {
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == '*' || str[i] == '+' || str[i] == '-') {
            return 1;
        }
    }
    return 0;
}

int checkIntStrConditionsOnly(char* str) {
    for (int i = 0; i < strlen(str); i++) {
        if (strncmp(&str[i], ">=", 2) == 0 || strncmp(&str[i], "<=", 2) == 0 || str[i] == '>' || str[i] == '<' || strncmp(&str[i], "==", 2)) {
            return 1;
        }
    }
    return 0;
}

int checkColSetConditionsOnly(char* str) {
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == '!' || strncmp(&str[i], "==", 2) == 0) {
            return 1;
        }
    }
    return 0;
}

int checkConditions(char* str) {
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == '!' || strncmp(&str[i], "==", 2) == 0 || strncmp(&str[i], ">=", 2) == 0 || strncmp(&str[i], "<=", 2) == 0 || str[i] == '>' || str[i] == '<') {
            return 1;
        }
    }
    return 0;
}

char* generateGetSetSize(char* varName, char* expr, GetSizeMode mode) {
    char msg[32];
    int len;
    char* result = NULL;

    memset(buffer, '\0', sizeof(buffer));

    while (*expr && isspace((unsigned char)*expr)) expr++;
    char* end = expr + strlen(expr) - 1;
    while (end > expr && isspace((unsigned char)*end)) end--;
    *(end + 1) = 0;
    len = strlen(expr);

    if (expr[0] == '|' && expr[len - 1] == '|') {
        char* innerExpr = yaccStrndup(expr + 1, len - 2);
        if (getType(innerExpr) == Set) {
            if (mode == Assign) {
                snprintf(buffer, sizeof(buffer), "%s = %s.getSize();\n", varName, innerExpr);
            } else {
                snprintf(buffer, sizeof(buffer), "%s.getSize()\n", innerExpr);
            }
            result = malloc(strlen(buffer) + 1);
            if (result == NULL) {
                snprintf(msg, sizeof(msg), "Memory allocation error");
                yyerror(msg);
                return NULL;
            }
            strcpy(result, buffer);
        } else {
            snprintf(msg, sizeof(msg), "%s is not a set", innerExpr);
            yyerror(msg);
        }
        free(innerExpr); 
    }

    memset(buffer, '\0', sizeof(buffer));
    return result;
}


char* concatSentences(char* sentence1, char* sentence2) {
    memset(buffer, '\0', sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "%s\n%s", sentence1, sentence2);

    char* result = malloc(strlen(buffer) + 1);
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }
    strcpy(result, buffer);
    memset(buffer, '\0', sizeof(buffer));
    return result;
}

char* escapeQuotes(const char* str) {
    size_t bufIndex = 0;
    const char* p = str;
    memset(buffer, '\0', sizeof(buffer));

    while (*p && bufIndex < sizeof(buffer) - 2) {  
        if (*p == '"') {
            if (bufIndex >= sizeof(buffer) - 3) break; 
            buffer[bufIndex++] = '\\';
        }
        buffer[bufIndex++] = *p++;
    }
    buffer[bufIndex] = '\0';

    char* result = malloc(bufIndex + 1);
    if (result == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return NULL;
    }

    strcpy(result, buffer);
    memset(buffer, '\0', sizeof(buffer));
    return result;
}

char* wrapWithQuotes(char* segment, varType type) {
    const char* contentToWrap;
    memset(buffer, '\0', sizeof(buffer));

    if (type == Collection) {
        contentToWrap = escapeQuotes(segment);
        if (contentToWrap == NULL) {
            return NULL;
        }
    } else {
        contentToWrap = segment;
    }
    snprintf(buffer, sizeof(buffer), "std::string(\"%s\")", contentToWrap);

    char* wrappedSegment = (char*)malloc(strlen(buffer) + 1);
    if (wrappedSegment == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        if (type == Collection) {
            free((char*)contentToWrap);
        }
        return NULL;
    }

    strcpy(wrappedSegment, buffer);
    if (type == Collection) {
        free((char*)contentToWrap);
    }

    memset(buffer, '\0', sizeof(buffer));
    return wrappedSegment;
}

// Variable Functions
char* concatWithDelimiter(char* str1, char* str2, char* delimiter) {
    char msg[64];
    memset(buffer, '\0', sizeof(buffer));
    if (strlen(str1) + strlen(str2) + strlen(delimiter) + 1 > sizeof(buffer)) {
        snprintf(msg, sizeof(msg), "%s - Buffer overflow risk", str1);
        yyerror(msg);
        return NULL;
    }
    snprintf(buffer, sizeof(buffer), "%s%s%s", str1, delimiter, str2);
    char* result = (char*)malloc(strlen(buffer) + 1);
    if (result == NULL) {
        snprintf(msg, sizeof(msg), "%s - Memory allocation failed", str1);
        yyerror(msg);
        return NULL;
    }
    strcpy(result, buffer);
    memset(buffer, '\0', sizeof(buffer));
    return result;
}

char* addVarToList(char* list, char* str) {
    char msg[64];
    if (checkIfExists(str) != 0) {
        snprintf(msg, sizeof(msg), "%s already exists", str);
        yyerror(msg);
        return list;
    } else {
        if (list == NULL) {
            size_t len = strlen(str);
            if (len + 1 > sizeof(buffer)) {
                yyerror("Buffer overflow risk");
                return list;
            }

            strcpy(buffer, str);
            char* newList = malloc(len + 1);
            if (newList == NULL) {
                yyerror("Memory allocation failed");
                return list;
            }
            strcpy(newList, buffer);
            memset(buffer, '\0', sizeof(buffer));
            return newList;
        }
        char* newList = concatWithDelimiter(list, str, ",");
        if (newList == NULL) {
            yyerror("Concatenation failed");
            return list;
        }
        printf("\t*NewVarList:%s\n", newList); 
        free(list);
        return newList;
    }
}

void saveVarsToSymbolTable(const char* str, char delimiter, varType type) {
    const char* itemStart = str;
    const char* itemEnd;

    while (*itemStart) {
        while (isspace((unsigned char)*itemStart)) itemStart++;
        itemEnd = itemStart;
        
        while (*itemEnd && *itemEnd != delimiter) itemEnd++;
        const char* end = itemEnd - 1;

        while (end > itemStart && isspace((unsigned char)*end)) end--;
        size_t itemLength = end - itemStart + 1;

        if (itemLength > 0) {
            char* item = (char*)malloc(itemLength + 1);
            if (item == NULL) {
                fprintf(stderr, "Memory allocation error\n");
                return;
            }
            strncpy(item, itemStart, itemLength);
            item[itemLength] = '\0';
            addToSymbolTable(item, type);
            free(item);
        }
        itemStart = itemEnd;
        if (*itemStart == delimiter) itemStart++;
    }
}

char* addStrToList(char* list, char* str) {
    memset(buffer, '\0', sizeof(buffer));
    if (list == NULL) {
        size_t len = strlen(str);
        if (len + 1 > sizeof(buffer)) {
            fprintf(stderr, "Buffer overflow risk\n");
            return list;
        }
        strcpy(buffer, str);
        char* newList = malloc(len + 1);
        if (newList == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            return list;
        }
        strcpy(newList, buffer);
        memset(buffer, '\0', sizeof(buffer));
        return newList;
    }
    char* newList = concatWithDelimiter(list, str, ",");
    if (newList == NULL) {
        fprintf(stderr, "Concatenation failed\n");
        return list;
    }
    printf("\t*NewStringList:%s\n", newList); 
    free(list);
    return newList;
}

// Integer Functions
void validateIntExpression(char* expr) {
    char msg[64];
    int len = strlen(expr);
    char* operator = NULL;
    int parentheses_depth = 0;

    while (*expr && isspace((unsigned char)*expr)) expr++;
    char* end = expr + strlen(expr) - 1;
    while (end > expr && isspace((unsigned char)*end)) end--;
    *(end + 1) = 0;
    len = strlen(expr);

    if (expr[0] == '(' && expr[len - 1] == ')') {
        char* innerExpr = yaccStrndup(expr + 1, len - 2);
        validateIntExpression(innerExpr);
        free(innerExpr);
        return;
    }

    for (int i = 0; i < len; i++) {
        if (expr[i] == '(') parentheses_depth++;
        else if (expr[i] == ')') parentheses_depth--;
        else if (parentheses_depth == 0 && strchr("+-*/", expr[i])) {
            operator = &expr[i];
            break;
        }
    }

    if (operator != NULL) {
        char operator_char = *operator;
        *operator = '\0'; 
        char* token1 = expr;
        char* token2 = operator + 1;

        if (operator_char == '/' && isNumber(token2) && atoi(token2) == 0) {
            sprintf(msg, "Error: division by zero");
            yyerror(msg);
        }

        validateIntExpression(token1);
        validateIntExpression(token2);

        *operator = operator_char;  
    } else {
        if (!isNumber(expr) && getType(expr) != Integer) {
            sprintf(msg, "%s is not an integer", expr);
            yyerror(msg);
        }
    }
}

char* generateIntDefine(char* varList) {
    char msg[64];
    char* result = NULL;
    memset(buffer, '\0', sizeof(buffer));
    size_t resultLen = strlen("int ") + strlen(varList) + 3;
    if (resultLen > sizeof(buffer)) {
        yyerror("Buffer overflow risk");
        return NULL;
    }

    char* varListCopy = strdup(varList);
    if (varListCopy == NULL) {
        yyerror("Memory allocation error");
        return NULL;
    }

    saveVarsToSymbolTable(varListCopy, ',', Integer);
    snprintf(buffer, sizeof(buffer), "int %s;\n", varList);
    result = malloc(strlen(buffer) + 1);
    if (result == NULL) {
        yyerror("Memory allocation error");
        free(varListCopy);
        return NULL;
    }

    strcpy(result, buffer);
    free(varListCopy);
    memset(buffer, '\0', sizeof(buffer));
    return result;
}

char* generateIntAssignment(char* varName, char* value) {
    if (value[0] == '|') {
        return generateGetSetSize(varName, value, Assign);
    }
    
    validateIntExpression(value);
    memset(buffer, '\0', sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "%s = %s;\n", varName, value);

    char* result = malloc(strlen(buffer) + 1);
    if (result == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }
    strcpy(result, buffer);
    memset(buffer, '\0', sizeof(buffer));
    return result;
}

// String Functions
char* generateStrDefine(char* varList) {
    memset(buffer, '\0', sizeof(buffer));
    size_t totalLength = strlen("std::string ") + strlen(varList) + 3; 

    if (totalLength > sizeof(buffer)) {
        fprintf(stderr, "Buffer overflow risk\n");
        return NULL;
    }

    char* varListCopy = strdup(varList);
    if (varListCopy == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }
    saveVarsToSymbolTable(varListCopy, ',', String);
    snprintf(buffer, sizeof(buffer), "std::string %s;\n", varList);
    char* result = malloc(strlen(buffer) + 1);
    if (result == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        free(varListCopy);
        exit(1);
    }
    strcpy(result, buffer);

    free(varListCopy);
    memset(buffer, '\0', sizeof(buffer));
    return result;
}

char* generateStrAssignment(char* varName, char* value) {
    memset(buffer, '\0', sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "%s = %s;\n", varName, value);

    char* result = malloc(strlen(buffer) + 1);
    if (result == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }

    strcpy(result, buffer);
    memset(buffer, '\0', sizeof(buffer));
    return result;
}

// Set functions
char* processSetWithBrackets(char* value) {
 char* result = (char*)malloc(strlen(value) * 2 + 1);
    if (!result) {
        fprintf(stderr, "Memory allocation error\n");
        return NULL;
    }
    result[0] = '\0'; 

    char* cursor = value; 
    char* start = NULL;
    char* end = NULL;

    while ((start = strchr(cursor, '[')) != NULL) {
        end = strchr(start, ']');
        if (!end) {
            fprintf(stderr, "Error: unmatched '['\n");
            free(result);
            return NULL;
        }

        strncat(result, cursor, start - cursor);
        size_t segmentLen = (end - start) + 1;
        char* segment = yaccStrndup(start, segmentLen);
        char* wrappedSegment = wrapWithQuotes(segment, Set);
        strcat(result, wrappedSegment);
        free(segment);
        free(wrappedSegment);
        cursor = end + 1;
    }

    strcat(result, cursor);
    printf("*Processed Value: %s\n", result);
    return result;
}

char* generateSetDefine(char* varName) {
    char msg[32];
    char* result = NULL;
    size_t resultSize = 0;
    size_t bufferSize = 256;

    result = malloc(bufferSize);
    if (result == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }
    result[0] = '\0'; 

    char* start = varName;
    char* end = varName;

    while (*end != '\0') {
        while (*end != ',' && *end != '\0') {
            end++;
        }

        char temp = *end;
        *end = '\0';

        while (*start == ' ') start++;

        char* tokenEnd = start + strlen(start) - 1;
        while (tokenEnd > start && *tokenEnd == ' ') tokenEnd--;
        *(tokenEnd + 1) = '\0';

        if (*start != '\0') {
            addToSymbolTable(start, Set);
            size_t newEntrySize = strlen(start) + 24;
            resultSize += newEntrySize;

            if (resultSize >= bufferSize) {
                bufferSize = resultSize + 1;
                result = realloc(result, bufferSize);
                if (result == NULL) {
                    fprintf(stderr, "Memory reallocation error\n");
                    exit(1);
                }
            }
            strcat(result, "SetLangVar<int> ");
            strcat(result, start);
            strcat(result, ";\n");
        }

        *end = temp;
        if (*end != '\0') {
            end++;
            start = end;
        }
    }

    return result;
}

int validateSetExpression(char* value) {
    char* expr1 = NULL;
    char* expr2 = NULL;
    char* op = NULL;
    parseExpression(value, &expr1, &expr2, &op);

    if (op == NULL) {
        if (getType(expr1) != Set && isNumber(expr1) == 1 && !(getType(expr1) == Integer) && isSet(expr1) == 0) {
            char msg[64];
            sprintf(msg, "%s is not a valid set", expr1);
            yyerror(msg);
        }
    } else {
        if (!validateSetExpression(expr1) || !validateSetExpression(expr2)) {
            return 0; 
        }

        if (!(getType(value) == Set) && (isNumber(value) == 0 && *op == '*')  && !(getType(expr1) == Integer) && isSet(value) == 0) {
            char msg[64];
            sprintf(msg, "%s is not a valid expression", value);
            yyerror(msg);
        }
    }
    return 1;
}

char* generateSetAssign(char* varName, char* value) {
    char msg[64];
    char* processedValue = NULL;

    printf("*Set Value: %s\n", value);
    if (strcmp(value, "[]") == 0) {
        return strdup("");
    }
    if (strchr(value, '[') != NULL && strchr(value, ']') != NULL) {
        processedValue = processSetWithBrackets(value);
        if (processedValue == NULL) {
            snprintf(msg, sizeof(msg), "Error processing value");
            yyerror(msg);
            return NULL;
        }
    } else {
        processedValue = value;
    }

    memset(buffer, '\0', sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "%s = %s;\n", varName, processedValue);
    char* result = malloc(strlen(buffer) + 1);
    if (result == NULL) {
        snprintf(msg, sizeof(msg), "Memory allocation error");
        yyerror(msg);
        if (processedValue != value) {
            free(processedValue);
        }
        return NULL;
    }
    strcpy(result, buffer);
    if (processedValue != value) {
        free(processedValue);
    }
    memset(buffer, '\0', sizeof(buffer));
    return result;
}

// Collection functions
int hasMatchingBracesCollection(const char *str) {
    const char *openBrace = strchr(str, '{');
    const char *closeBrace = strchr(str, '}');
    if (openBrace != NULL && closeBrace != NULL && openBrace < closeBrace) {
        return 1;
    }
    return 0;
}

char* processCollectionWithBrackets(char* value) {
    char* result = (char*)malloc(strlen(value) * 2 + 1); 
    if (!result) {
        fprintf(stderr, "Memory allocation error\n");
        return NULL;
    }
    result[0] = '\0';

    char* cursor = value; 
    char* start = NULL;
    char* end = NULL;

    while ((start = strchr(cursor, '{')) != NULL) {
        end = strchr(start, '}');
        if (!end) {
            fprintf(stderr, "Error: unmatched '{'\n");
            free(result);
            return NULL;
        }

        strncat(result, cursor, start - cursor);
        size_t segmentLen = (end - start) + 1;
        char* segment = yaccStrndup(start, segmentLen);
        char* wrappedSegment = wrapWithQuotes(segment, Collection);

        strcat(result, wrappedSegment);
        free(segment);
        free(wrappedSegment);
        cursor = end + 1;
    }

    strcat(result, cursor);
    printf("*Processed Value: %s\n", result);
    return result;
}

char* generateCollectionDefine(char* varName) {
    size_t resultSize = 0;
    memset(buffer, '\0', sizeof(buffer));

    char* start = varName;
    char* end = varName;

    while (*end != '\0') {
        while (*end != ',' && *end != '\0') {
            end++;
        }

        char temp = *end;
        *end = '\0';

        while (*start == ' ') start++;

        char* tokenEnd = start + strlen(start) - 1;
        while (tokenEnd > start && *tokenEnd == ' ') tokenEnd--;
        *(tokenEnd + 1) = '\0';

        if (*start != '\0') {
            addToSymbolTable(start, Collection);

            size_t newEntrySize = strlen(start) + 29;
            resultSize += newEntrySize;

            if (resultSize >= sizeof(buffer)) {
                fprintf(stderr, "Buffer overflow risk\n");
                return NULL;
            }

            strcat(buffer, "SetLangVar<string> ");
            strcat(buffer, start);
            strcat(buffer, ";\n");
        }

        *end = temp;
        if (*end != '\0') {
            end++;
            start = end;
        }
    }

    char* result = malloc(strlen(buffer) + 1);
    if (result == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return NULL;
    }

    strcpy(result, buffer);
    memset(buffer, '\0', sizeof(buffer));
    return result;
}

int validateCollectionExpression(char* value) {
    char* expr1 = NULL;
    char* expr2 = NULL;
    char* op = NULL;
    parseExpression(value, &expr1, &expr2, &op);

    if (op == NULL) {
        if (getType(expr1) != Collection && isString(expr1) == 0 &&  !(getType(expr1) == String) && hasMatchingBracesCollection(expr1) == 0) {
            char msg[64];
            sprintf(msg, "%s is not a valid collection", expr1);
            yyerror(msg);
        }
    } else {
        if (!validateCollectionExpression(expr1) || !validateCollectionExpression(expr2)) {
            return 0; 
        }

        if (!(getType(value) == Collection) && (isString(value) == 0 && *op == '*') && !(getType(value) == String) && hasMatchingBracesCollection(value) == 0) {
            char msg[64];
            sprintf(msg, "%s is not a valid expression", value);
            yyerror(msg);
        }
    }
    return 1;
}

char* generateCollectionAssign(char* varName, char* value) {
    char msg[64];
    char* processedValue = NULL;

    printf("*Collection Value: %s\n", value);

    if (strcmp(value, "{}") == 0) {
        return strdup("");
    }

    if (strchr(value, '{') != NULL && strchr(value, '}') != NULL) {
        processedValue = processCollectionWithBrackets(value);
        if (processedValue == NULL) {
            snprintf(msg, sizeof(msg), "Error processing value");
            yyerror(msg);
            return NULL;
        }
    } else {
        processedValue = value;
    }

    memset(buffer, '\0', sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "%s = %s;\n", varName, processedValue);
    char* result = malloc(strlen(buffer) + 1);
    if (result == NULL) {
        snprintf(msg, sizeof(msg), "Memory allocation error");
        yyerror(msg);
        if (processedValue != value) {
            free(processedValue);
        }
        return NULL;
    }
    strcpy(result, buffer);

    if (processedValue != value) {
        free(processedValue);
    }
    memset(buffer, '\0', sizeof(buffer));
    return result;
}


char* GenerateAssignment(char* varName, char* valueList) {
    memset(buffer, '\0', sizeof(buffer));

    if (checkIfExists(varName) == 0) {
        snprintf(buffer, sizeof(buffer), "%s not defined", varName);
        yyerror(buffer);
        return NULL;
    }

    if (getType(varName) == Integer) {
        if (checkSetCollectionOnlyOperations(valueList) == 1) {
            snprintf(buffer, sizeof(buffer), "operations not allowed on integer");
            yyerror(buffer);
            return NULL;
        } else {
            return generateIntAssignment(varName, valueList);
        }
    } else if (getType(varName) == Set) {
        if (checkIntegerOnlyOperations(valueList) == 1) {
            snprintf(buffer, sizeof(buffer), "operations not allowed on set");
            yyerror(buffer);
            return NULL;
        } else {
            return generateSetAssign(varName, valueList);
        }
    } else if (getType(varName) == Collection) {
        if (checkIntegerOnlyOperations(valueList) == 1) {
            snprintf(buffer, sizeof(buffer), "operations not allowed on collection");
            yyerror(buffer);
            return NULL;
        } else {
            return generateCollectionAssign(varName, valueList);
        }
    }
    memset(buffer, '\0', sizeof(buffer));
    return NULL;
}

// Input and Output Functions
char* generateOutput(char* str) {
    memset(buffer, '\0', sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "std::cout << %s << std::endl;\n", str);

    char* result = malloc(strlen(buffer) + 1);
    if (result == NULL) {
        snprintf(buffer, sizeof(buffer), "Memory allocation error");
        yyerror(buffer);
        return NULL;
    }

    strcpy(result, buffer);
    memset(buffer, '\0', sizeof(buffer));
    printf("\t*Output: %s\n", result);
    return result;
}

char* generateInput(char* prompt, char* var) {
    char msg[64];
    size_t usedSize = 0;

    varType type = getType(var);
    if (type == None) {
        snprintf(msg, sizeof(msg), "%s not defined.", var);
        yyerror(msg);
        return NULL;
    }

    memset(buffer, '\0', sizeof(buffer));
    usedSize += snprintf(buffer + usedSize, sizeof(buffer) - usedSize, "std::cout << %s;\n", prompt);

    if (type == Integer) {
        usedSize += snprintf(buffer + usedSize, sizeof(buffer) - usedSize, 
            "std::cin >> %s;\n"
            "std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\\n');\n", var);
    } else if (type == String) {
        usedSize += snprintf(buffer + usedSize, sizeof(buffer) - usedSize, 
            "std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\\n');\n"
            "std::getline(std::cin, %s);\n", var);
    } else if (type == Set || type == Collection) {
        usedSize += snprintf(buffer + usedSize, sizeof(buffer) - usedSize,
            "std::getline(std::cin, temp);\n"
            "%s.insertFromString(temp);\n"
            "std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\\n');\n"
            "temp.clear();\n", var);
    }

    char* result = malloc(strlen(buffer) + 1);
    if (result == NULL) {
        snprintf(msg, sizeof(msg), "Memory allocation error");
        yyerror(msg);
        return NULL;
    }

    strcpy(result, buffer);
    memset(buffer, '\0', sizeof(buffer));
    return result;
}

char* generateVarOutput(char* prompt, char* var) {
    char msg[64];

    if (getType(var) == None) {
        snprintf(msg, sizeof(msg), "%s not defined.", var);
        yyerror(msg);
        return NULL;
    }

    memset(buffer, '\0', sizeof(buffer));
    snprintf(buffer, sizeof(buffer), 
             "std::cout << %s << ' ';\n"
             "std::cout << %s << std::endl;\n", 
             prompt, var);

    char* result = malloc(strlen(buffer) + 1);
    if (result == NULL) {
        snprintf(msg, sizeof(msg), "Memory allocation error");
        yyerror(msg);
        return NULL;
    }

    strcpy(result, buffer);
    memset(buffer, '\0', sizeof(buffer));
    return result;
}

char* generateExpressionOutput(char* prompt, char* expr) {
    char msg[64];
    char* exprCopy = NULL;
    char* getSize = NULL;

    if (checkSetCollectionOperations(expr) == 0 && *expr == '|') {
        getSize = generateGetSetSize(NULL, expr, Output);
        if (getSize == NULL) {
            snprintf(msg, sizeof(msg), "Memory allocation error");
            yyerror(msg);
            return NULL;
        }
        printf("*getSize: %s", getSize);
    } else {
        exprCopy = strdup(expr);
        if (exprCopy == NULL) {
            snprintf(msg, sizeof(msg), "Memory allocation error");
            yyerror(msg);
            return NULL;
        }

        char* expr1 = NULL;
        char* expr2 = NULL;
        char* op = NULL;
        parseExpression(exprCopy, &expr1, &expr2, &op);
        printf("*generateExpressionOutput expr1: %s\n", expr1);
        printf("*generateExpressionOutput expr2: %s\n", expr2);
        printf("*generateExpressionOutput op: %s\n", op);

        if (getType(expr) == Integer) {
            validateIntExpression(exprCopy);
        } else if (getType(expr) == Set) {
            if (validateSetExpression(exprCopy) == 0) {
                snprintf(msg, sizeof(msg), "Invalid set expression in output");
                yyerror(msg);
                free(exprCopy);
                return NULL;
            }
        } else if (getType(expr) == Collection) {
            if (validateCollectionExpression(exprCopy) == 0) {
                snprintf(msg, sizeof(msg), "Invalid collection expression in output");
                yyerror(msg);
                free(exprCopy);
                return NULL;
            }
        }
    }

    memset(buffer, '\0', sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "std::cout << %s << ' ';\nstd::cout << %s << std::endl;\n",
             prompt, getSize ? getSize : expr);

    char* result = malloc(strlen(buffer) + 1);
    if (result == NULL) {
        snprintf(msg, sizeof(msg), "Memory allocation error");
        yyerror(msg);
        free(exprCopy);
        free(getSize);
        return NULL;
    }
    strcpy(result, buffer);

    free(exprCopy);
    free(getSize);
    memset(buffer, '\0', sizeof(buffer));
    return result;
}

// Conditions
int validateConditions(char* value) {
    char* expr1 = NULL;
    char* expr2 = NULL;
    char* op = NULL;
    char printCond[64];

    if (checkConditions(value) == 0) {
        if (getType(value) != Collection && getType(value) != Set) {
            char msg[64];
            sprintf(msg, "%s is not a valid set or collection", value);
            yyerror(msg);
        }
        return 1;
    }

    parseConditions(value, &expr1, &expr2, &op);

    if (op == NULL) {
        if (*expr1 == '!') {
            if (getType(expr2) != Collection && getType(expr2) != Set) {
                char msg[64];
                sprintf(msg, "%s is not a valid set or collection", expr2);
                yyerror(msg);
            }
        }
    } else {
        if((getType(expr1) == Integer || isNumber(expr1) == 0) && (getType(expr2) == Integer || isNumber(expr1) == 0) && checkIntStrConditionsOnly(value) == 0) {
            char msg[64];
            sprintf(msg, "Invalid condition on integer");
            yyerror(msg);
        }
        else if((getType(expr1) == String || isString(expr1) == 1) && (getType(expr2) == String || isString(expr1) == 1) && checkIntStrConditionsOnly(value) == 0) {
            char msg[64];
            sprintf(msg, "Invalid condition on string");
            yyerror(msg);
        } else if (getType(expr1) == Set && getType(expr2) == Set && checkColSetConditionsOnly(value) == 0) {
            char msg[64];
            sprintf(msg, "Invalid condition on set");
            yyerror(msg);
        } else if (getType(expr1) == Collection && getType(expr2) == Collection && checkColSetConditionsOnly(value) == 0) {
            char msg[64];
            sprintf(msg, "Invalid condition on set");
            yyerror(msg);
        }
    }
    return 0;
}

char* checkForCondition(char* item, char* var) {
    char msg[64];

    printf("*Item: %s\n", item);
    printf("*Var: %s\n", var);

    varType varType = getType(var);
    if (varType != Set && varType != Collection) {
        snprintf(msg, sizeof(msg), "%s is not a set or collection", var);
        yyerror(msg);
        return NULL;
    }

    if (varType == Integer || isNumber(item)) {
        validateIntExpression(item);
    } else if (varType == Set) {
        if (!validateSetExpression(item)) {
            snprintf(msg, sizeof(msg), "%s is not a valid set expression", item);
            yyerror(msg);
            return NULL;
        }
    } else if (varType == Collection) {
        if (!validateCollectionExpression(item)) {
            snprintf(msg, sizeof(msg), "%s is not a valid collection expression", item);
            yyerror(msg);
            return NULL;
        }
    }

    memset(buffer, '\0', sizeof(buffer));

    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer),
             "for (int i = 0; i < %s.getSize(); i++) {\n", var);

    if (varType == Collection) {
        if (checkIfExists(item) == 0) {
            addToSymbolTable(item, String);
            snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer),
                     "\tstd::string %s;\n", item);
        }
        snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer),
                 "if(%s.getSize() >= i)\n", var);
        snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer),
                 "\t%s = %s[i];\n", item, var);
    } else if (varType == Set) {
        if (checkIfExists(item) == 0) {
            addToSymbolTable(item, String);
            snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer),
                     "\tint %s;\n", item);
        }
        snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer),
                 "if(%s.getSize() >= i)\n", var);
        snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer),
                 "\t%s = %s[i];\n", item, var);
    }

    char* result = malloc(strlen(buffer) + 1);
    if (result == NULL) {
        snprintf(msg, sizeof(msg), "Memory allocation error");
        yyerror(msg);
        return NULL;
    }
    strcpy(result, buffer);

    memset(buffer, '\0', sizeof(buffer));
    printf("*checkForCondition: %s\n", result);
    return result;
}

char* generateFor(char* forCond, char* block) {
    memset(buffer, '\0', sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "%s%s\n}\n", forCond, block);

    char* result = malloc(strlen(buffer) + 1);
    if (result == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return NULL;
    }
    strcpy(result, buffer);

    memset(buffer, '\0', sizeof(buffer));
    printf("*generateFor: %s\n", result);
    return result;
}

char* generateWhile(char* cond, char* block) {
    char* copyCond = strdup(cond);
    if (copyCond == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return NULL;
    }

    int isSetOrColNoOp = validateConditions(copyCond);
    memset(buffer, '\0', sizeof(buffer));

    if (isSetOrColNoOp) {
        snprintf(buffer, sizeof(buffer), "while (!!%s) {\n%s\n}\n", cond, block);
    } else {
        snprintf(buffer, sizeof(buffer), "while (%s) {\n%s\n}\n", cond, block);
    }

    char* result = malloc(strlen(buffer) + 1);
    if (result == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        free(copyCond);
        return NULL;
    }

    strcpy(result, buffer);
    free(copyCond);
    memset(buffer, '\0', sizeof(buffer));

    return result;
}

char* generateIf(char* condition, char* ifBlock, char* elseBlock) {    
    char copyCond[256] = {'\0'}; 
    strncpy(copyCond, condition, sizeof(copyCond) - 1);  
    copyCond[sizeof(copyCond) - 1] = '\0'; 
    
    int isSetOrColNoOp = validateConditions(copyCond);
    
    char condBuffer[256] = {'\0'};
    
    if (strstr(condition, "=={}") || strstr(condition, "==[]")) {
        snprintf(condBuffer, sizeof(condBuffer), "!(%.*s)", (int)(strstr(condition, "==") - condition), condition);
    } else if (strstr(condition, ">{}") || strstr(condition, ">[]") || strstr(condition, ">{}") || strstr(condition, ">={}")) {
        snprintf(condBuffer, sizeof(condBuffer), "%.*s > 0", (int)(strstr(condition, ">") - condition), condition);
    } else if (strstr(condition, "<{}") || strstr(condition, "<[]") || strstr(condition, "<={}")) {
        snprintf(condBuffer, sizeof(condBuffer), "%.*s < 0", (int)(strstr(condition, "<") - condition), condition);
    } else {
        snprintf(condBuffer, sizeof(condBuffer), "%s", condition);
    }

    memset(buffer, '\0', sizeof(buffer));
    if (isSetOrColNoOp) {
        snprintf(buffer, sizeof(buffer), "if (!!%s) {\n%s\n}\n", condBuffer, ifBlock);
    } else {
        snprintf(buffer, sizeof(buffer), "if (%s) {\n%s\n}\n", condBuffer, ifBlock);
    }

    if (elseBlock) {
        strcat(buffer, " else {\n");
        strcat(buffer, elseBlock);
        strcat(buffer, "\n}\n");
    }

    char* result = malloc(strlen(buffer) + 1);
    if (result == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return NULL;
    }
    strcpy(result, buffer);

    memset(buffer, '\0', sizeof(buffer));
    printf("*generateIf: %s\n", result);
    return result;
}


// Print To File
void printToFile(const char* content) {
    char msg[64];

    if (content == NULL) {
        sprintf(msg, "Content is null");
        yyerror(msg);
    }

    for (size_t i = 0; i < 1024; ++i) {
        if (content[i] == '\0') {
            printf("*\\0 found at index %zu\n", i);
            break;
        }
    }

    FILE *fp = fopen(fileName, "a");
    if (fp == NULL) {
        sprintf(msg, "Error opening file");
        yyerror(msg);
    }
        
    fprintf(fp, "%s", content);
    fclose(fp); 
    printf("*Printed to file: %s\n", content);
}



#line 1469 "Language.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "Language.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_t_STRING_CMD = 3,               /* t_STRING_CMD  */
  YYSYMBOL_t_SET_CMD = 4,                  /* t_SET_CMD  */
  YYSYMBOL_t_COLLECTION_CMD = 5,           /* t_COLLECTION_CMD  */
  YYSYMBOL_t_IF_CMD = 6,                   /* t_IF_CMD  */
  YYSYMBOL_t_ELSE_CMD = 7,                 /* t_ELSE_CMD  */
  YYSYMBOL_t_WHILE_CMD = 8,                /* t_WHILE_CMD  */
  YYSYMBOL_t_FOR_CMD = 9,                  /* t_FOR_CMD  */
  YYSYMBOL_t_INTEGER_CMD = 10,             /* t_INTEGER_CMD  */
  YYSYMBOL_t_OUTPUT_CMD = 11,              /* t_OUTPUT_CMD  */
  YYSYMBOL_t_INPUT_CMD = 12,               /* t_INPUT_CMD  */
  YYSYMBOL_GREATER = 13,                   /* GREATER  */
  YYSYMBOL_LESS = 14,                      /* LESS  */
  YYSYMBOL_EQUAL = 15,                     /* EQUAL  */
  YYSYMBOL_GREATER_EQUAL = 16,             /* GREATER_EQUAL  */
  YYSYMBOL_LESS_EQUAL = 17,                /* LESS_EQUAL  */
  YYSYMBOL_NOT = 18,                       /* NOT  */
  YYSYMBOL_t_STRING = 19,                  /* t_STRING  */
  YYSYMBOL_t_ID = 20,                      /* t_ID  */
  YYSYMBOL_t_INTEGER = 21,                 /* t_INTEGER  */
  YYSYMBOL_PLUS = 22,                      /* PLUS  */
  YYSYMBOL_MINUS = 23,                     /* MINUS  */
  YYSYMBOL_MULTIPLY = 24,                  /* MULTIPLY  */
  YYSYMBOL_DIVIDE = 25,                    /* DIVIDE  */
  YYSYMBOL_AND = 26,                       /* AND  */
  YYSYMBOL_OR = 27,                        /* OR  */
  YYSYMBOL_28_ = 28,                       /* '('  */
  YYSYMBOL_29_ = 29,                       /* '='  */
  YYSYMBOL_30_ = 30,                       /* ')'  */
  YYSYMBOL_31_ = 31,                       /* '{'  */
  YYSYMBOL_32_ = 32,                       /* '}'  */
  YYSYMBOL_33_ = 33,                       /* ';'  */
  YYSYMBOL_34_ = 34,                       /* ':'  */
  YYSYMBOL_35_ = 35,                       /* ','  */
  YYSYMBOL_36_ = 36,                       /* '['  */
  YYSYMBOL_37_ = 37,                       /* ']'  */
  YYSYMBOL_YYACCEPT = 38,                  /* $accept  */
  YYSYMBOL_Prog = 39,                      /* Prog  */
  YYSYMBOL_SENTENCE = 40,                  /* SENTENCE  */
  YYSYMBOL_CONTROL = 41,                   /* CONTROL  */
  YYSYMBOL_BLOCK = 42,                     /* BLOCK  */
  YYSYMBOL_SENTENCE_LIST = 43,             /* SENTENCE_LIST  */
  YYSYMBOL_DEFINES = 44,                   /* DEFINES  */
  YYSYMBOL_ASSIGNMENTS = 45,               /* ASSIGNMENTS  */
  YYSYMBOL_IO = 46,                        /* IO  */
  YYSYMBOL_IF_CONDITIONS = 47,             /* IF_CONDITIONS  */
  YYSYMBOL_WHILE_CONDITIONS = 48,          /* WHILE_CONDITIONS  */
  YYSYMBOL_FOR_CONDITION = 49,             /* FOR_CONDITION  */
  YYSYMBOL_EXPRESSIONS = 50,               /* EXPRESSIONS  */
  YYSYMBOL_LITERALS = 51,                  /* LITERALS  */
  YYSYMBOL_VAR_LIST = 52,                  /* VAR_LIST  */
  YYSYMBOL_VAR = 53,                       /* VAR  */
  YYSYMBOL_SET = 54,                       /* SET  */
  YYSYMBOL_COLLECTION = 55,                /* COLLECTION  */
  YYSYMBOL_INT_LIST = 56,                  /* INT_LIST  */
  YYSYMBOL_STRING_LIST = 57                /* STRING_LIST  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  28
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   253

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  38
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  20
/* YYNRULES -- Number of rules.  */
#define YYNRULES  65
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  134

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   282


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      28,    30,     2,     2,    35,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    34,    33,
       2,    29,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    36,     2,    37,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    31,     2,    32,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,  1428,  1428,  1429,  1432,  1433,  1434,  1435,  1438,  1439,
    1440,  1441,  1444,  1445,  1448,  1449,  1452,  1453,  1454,  1455,
    1458,  1459,  1462,  1463,  1464,  1465,  1466,  1469,  1470,  1471,
    1472,  1473,  1474,  1475,  1479,  1480,  1481,  1482,  1483,  1484,
    1485,  1488,  1492,  1493,  1494,  1495,  1496,  1497,  1498,  1499,
    1502,  1503,  1504,  1505,  1506,  1509,  1510,  1513,  1516,  1517,
    1520,  1521,  1524,  1525,  1528,  1529
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "t_STRING_CMD",
  "t_SET_CMD", "t_COLLECTION_CMD", "t_IF_CMD", "t_ELSE_CMD", "t_WHILE_CMD",
  "t_FOR_CMD", "t_INTEGER_CMD", "t_OUTPUT_CMD", "t_INPUT_CMD", "GREATER",
  "LESS", "EQUAL", "GREATER_EQUAL", "LESS_EQUAL", "NOT", "t_STRING",
  "t_ID", "t_INTEGER", "PLUS", "MINUS", "MULTIPLY", "DIVIDE", "AND", "OR",
  "'('", "'='", "')'", "'{'", "'}'", "';'", "':'", "','", "'['", "']'",
  "$accept", "Prog", "SENTENCE", "CONTROL", "BLOCK", "SENTENCE_LIST",
  "DEFINES", "ASSIGNMENTS", "IO", "IF_CONDITIONS", "WHILE_CONDITIONS",
  "FOR_CONDITION", "EXPRESSIONS", "LITERALS", "VAR_LIST", "VAR", "SET",
  "COLLECTION", "INT_LIST", "STRING_LIST", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-78)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-41)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     147,    -8,    -8,    -8,     6,    12,    19,    -8,    46,    48,
     -78,   115,   -78,   -78,   -78,   -78,   -78,    64,   -15,   -78,
      16,    17,   118,    10,    -8,    31,   141,    -8,   -78,   -78,
     159,   -78,    -8,   -78,   -78,    -8,   -78,   -78,    -8,   172,
      -6,   -12,    74,   196,   -78,    76,   -78,   -78,    -8,    77,
      61,    82,    83,    80,   -78,   -78,   201,   -78,    75,    84,
      95,    98,   206,   -78,   -78,    89,   218,   -78,   -78,     1,
     -78,   -78,     2,    91,   172,   172,   172,   172,   172,   172,
     172,   172,   172,   172,   -78,    91,   145,   145,   145,   145,
     145,    91,    -8,   -78,   -78,   -78,   -78,   -78,   -78,   -78,
     -78,   -78,   113,   119,   -78,   147,   -78,   127,   227,   227,
     227,   227,   227,    55,    55,   -78,   -78,    20,   -78,   -78,
     -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,    78,
      91,   -78,   -78,   -78
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      57,     0,     3,     7,     4,     5,     6,     0,     0,    56,
       0,     0,     0,     0,     0,     0,     0,     0,     1,     2,
       0,    17,     0,    18,    19,     0,    51,    50,     0,     0,
       0,     0,     0,     0,    49,    54,    52,    53,     0,     0,
       0,    54,     0,     0,    16,    22,     0,    54,    52,    53,
       0,    51,     0,    55,    32,     0,     0,    65,    60,     0,
      63,    59,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    39,     0,     0,     0,     0,     0,
       0,     0,     0,    25,    23,    24,    26,    21,    20,    48,
      47,    61,     0,     0,    58,     0,    13,     8,    27,    28,
      29,    30,    31,    42,    43,    44,    45,    46,    10,    34,
      35,    36,    37,    38,    11,    41,    64,    62,    15,     0,
       0,    12,    14,     9
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -78,   -78,     4,   -78,   -77,   -78,   -78,   -78,   -78,   -78,
     -78,   -78,   -20,   -18,    14,     0,   116,   117,   -78,   -78
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,    11,   106,    13,   107,   129,    14,    15,    16,    42,
      49,    52,    43,    44,    18,    57,    46,    47,    72,    69
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      17,    19,    19,    19,    12,    50,    56,    19,   118,    70,
      62,    17,    10,    67,   124,    29,    20,    21,    31,    66,
      32,    25,    45,    51,    53,    71,    68,    60,    48,    36,
      10,    37,    63,   101,    22,    64,   102,   103,    65,   104,
      23,    40,    79,    80,    81,    82,    41,    24,    84,    33,
      34,    32,    32,   133,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,    54,    26,    32,    27,   119,   120,
     121,   122,   123,    17,    86,    87,    88,    89,    90,    81,
      82,     1,     2,     3,     4,    17,     5,     6,     7,     8,
       9,    17,   125,    30,     1,     2,     3,     4,    10,     5,
       6,     7,     8,     9,    73,    17,   -33,    85,    94,   128,
     131,    10,   -40,    91,    92,    28,    99,    95,     1,     2,
       3,     4,   105,     5,     6,     7,     8,     9,    96,    17,
      17,    97,   126,   132,   130,    10,    35,    36,    10,    37,
     127,     0,    58,    59,     0,    38,    39,     0,     0,    40,
       1,     2,     3,     4,    41,     5,     6,     7,     8,     9,
      36,    10,    37,     0,    36,    10,    37,    10,    38,    39,
       0,     0,    40,     0,    55,     0,    40,    41,    61,    10,
      37,    41,     0,     0,     0,     0,    38,    39,     0,     0,
      40,    36,    10,    37,     0,    41,     0,     0,     0,    38,
      39,     0,     0,    40,     0,     0,     0,     0,    41,    74,
      75,    76,    77,    78,     0,     0,     0,     0,    79,    80,
      81,    82,    83,    79,    80,    81,    82,    83,    79,    80,
      81,    82,    83,     0,    93,     0,     0,     0,     0,    98,
      79,    80,    81,    82,    83,     0,     0,     0,   100,    79,
      80,    81,    82,    83
};

static const yytype_int16 yycheck[] =
{
       0,     1,     2,     3,     0,    23,    26,     7,    85,    21,
      30,    11,    20,    19,    91,    11,     2,     3,    33,    39,
      35,     7,    22,    23,    24,    37,    32,    27,    18,    19,
      20,    21,    32,    32,    28,    35,    35,    35,    38,    37,
      28,    31,    22,    23,    24,    25,    36,    28,    48,    33,
      33,    35,    35,   130,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    33,    19,    35,    19,    86,    87,
      88,    89,    90,    73,    13,    14,    15,    16,    17,    24,
      25,     3,     4,     5,     6,    85,     8,     9,    10,    11,
      12,    91,    92,    29,     3,     4,     5,     6,    20,     8,
       9,    10,    11,    12,    30,   105,    30,    30,    33,   105,
      32,    20,    30,    30,    34,     0,    27,    33,     3,     4,
       5,     6,    31,     8,     9,    10,    11,    12,    33,   129,
     130,    33,    19,   129,     7,    20,    18,    19,    20,    21,
      21,    -1,    26,    26,    -1,    27,    28,    -1,    -1,    31,
       3,     4,     5,     6,    36,     8,     9,    10,    11,    12,
      19,    20,    21,    -1,    19,    20,    21,    20,    27,    28,
      -1,    -1,    31,    -1,    33,    -1,    31,    36,    19,    20,
      21,    36,    -1,    -1,    -1,    -1,    27,    28,    -1,    -1,
      31,    19,    20,    21,    -1,    36,    -1,    -1,    -1,    27,
      28,    -1,    -1,    31,    -1,    -1,    -1,    -1,    36,    13,
      14,    15,    16,    17,    -1,    -1,    -1,    -1,    22,    23,
      24,    25,    26,    22,    23,    24,    25,    26,    22,    23,
      24,    25,    26,    -1,    33,    -1,    -1,    -1,    -1,    33,
      22,    23,    24,    25,    26,    -1,    -1,    -1,    30,    22,
      23,    24,    25,    26
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,     5,     6,     8,     9,    10,    11,    12,
      20,    39,    40,    41,    44,    45,    46,    53,    52,    53,
      52,    52,    28,    28,    28,    52,    19,    19,     0,    40,
      29,    33,    35,    33,    33,    18,    19,    21,    27,    28,
      31,    36,    47,    50,    51,    53,    54,    55,    18,    48,
      51,    53,    49,    53,    33,    33,    50,    53,    54,    55,
      53,    19,    50,    53,    53,    53,    50,    19,    32,    57,
      21,    37,    56,    30,    13,    14,    15,    16,    17,    22,
      23,    24,    25,    26,    53,    30,    13,    14,    15,    16,
      17,    30,    34,    33,    33,    33,    33,    33,    33,    27,
      30,    32,    35,    35,    37,    31,    40,    42,    50,    50,
      50,    50,    50,    50,    50,    50,    50,    50,    42,    51,
      51,    51,    51,    51,    42,    53,    19,    21,    40,    43,
       7,    32,    40,    42
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    38,    39,    39,    40,    40,    40,    40,    41,    41,
      41,    41,    42,    42,    43,    43,    44,    44,    44,    44,
      45,    45,    46,    46,    46,    46,    46,    47,    47,    47,
      47,    47,    47,    47,    48,    48,    48,    48,    48,    48,
      48,    49,    50,    50,    50,    50,    50,    50,    50,    50,
      51,    51,    51,    51,    51,    52,    52,    53,    54,    54,
      55,    55,    56,    56,    57,    57
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     5,     7,
       5,     5,     3,     1,     2,     1,     3,     3,     3,     3,
       4,     4,     3,     4,     4,     4,     4,     3,     3,     3,
       3,     3,     2,     1,     3,     3,     3,     3,     3,     2,
       1,     3,     3,     3,     3,     3,     3,     3,     3,     1,
       1,     1,     1,     1,     1,     3,     1,     1,     3,     2,
       2,     3,     3,     1,     3,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* Prog: Prog SENTENCE  */
#line 1428 "Language.y"
                     {printToFile((yyvsp[0].str));}
#line 2619 "Language.tab.c"
    break;

  case 3: /* Prog: SENTENCE  */
#line 1429 "Language.y"
                     {printToFile((yyvsp[0].str));}
#line 2625 "Language.tab.c"
    break;

  case 4: /* SENTENCE: DEFINES  */
#line 1432 "Language.y"
                        { (yyval.str) = (yyvsp[0].str); }
#line 2631 "Language.tab.c"
    break;

  case 5: /* SENTENCE: ASSIGNMENTS  */
#line 1433 "Language.y"
                        { (yyval.str) = (yyvsp[0].str); }
#line 2637 "Language.tab.c"
    break;

  case 6: /* SENTENCE: IO  */
#line 1434 "Language.y"
                        { (yyval.str) = (yyvsp[0].str); }
#line 2643 "Language.tab.c"
    break;

  case 7: /* SENTENCE: CONTROL  */
#line 1435 "Language.y"
                        { (yyval.str) = (yyvsp[0].str); printf("Control: %s\n", (yyval.str)); }
#line 2649 "Language.tab.c"
    break;

  case 8: /* CONTROL: t_IF_CMD '(' IF_CONDITIONS ')' BLOCK  */
#line 1438 "Language.y"
                                                                        { (yyval.str) = generateIf((yyvsp[-2].str), (yyvsp[0].str), NULL);  printf("IF: %s\n", (yyvsp[-2].str)); }
#line 2655 "Language.tab.c"
    break;

  case 9: /* CONTROL: t_IF_CMD '(' IF_CONDITIONS ')' BLOCK t_ELSE_CMD BLOCK  */
#line 1439 "Language.y"
                                                                        { (yyval.str) = generateIf((yyvsp[-4].str), (yyvsp[-2].str), (yyvsp[0].str));}
#line 2661 "Language.tab.c"
    break;

  case 10: /* CONTROL: t_WHILE_CMD '(' WHILE_CONDITIONS ')' BLOCK  */
#line 1440 "Language.y"
                                                                        { (yyval.str) = generateWhile((yyvsp[-2].str), (yyvsp[0].str)); }
#line 2667 "Language.tab.c"
    break;

  case 11: /* CONTROL: t_FOR_CMD '(' FOR_CONDITION ')' BLOCK  */
#line 1441 "Language.y"
                                                                        { (yyval.str) = generateFor((yyvsp[-2].str), (yyvsp[0].str)); }
#line 2673 "Language.tab.c"
    break;

  case 12: /* BLOCK: '{' SENTENCE_LIST '}'  */
#line 1444 "Language.y"
                                                    { (yyval.str) = (yyvsp[-1].str); }
#line 2679 "Language.tab.c"
    break;

  case 13: /* BLOCK: SENTENCE  */
#line 1445 "Language.y"
                                                   { (yyval.str) = (yyvsp[0].str); printf("BLOCK: %s\n", (yyval.str)); }
#line 2685 "Language.tab.c"
    break;

  case 14: /* SENTENCE_LIST: SENTENCE_LIST SENTENCE  */
#line 1448 "Language.y"
                                                    { (yyval.str) = concatSentences((yyvsp[-1].str), strdup((yyvsp[0].str))); }
#line 2691 "Language.tab.c"
    break;

  case 15: /* SENTENCE_LIST: SENTENCE  */
#line 1449 "Language.y"
                                                    { (yyval.str) = strdup((yyvsp[0].str)); printf("SENTENCE: %s\n", (yyval.str)); }
#line 2697 "Language.tab.c"
    break;

  case 16: /* DEFINES: t_INTEGER_CMD VAR_LIST ';'  */
#line 1452 "Language.y"
                                                     { (yyval.str) = generateIntDefine((yyvsp[-1].str)); }
#line 2703 "Language.tab.c"
    break;

  case 17: /* DEFINES: t_STRING_CMD VAR_LIST ';'  */
#line 1453 "Language.y"
                                                     { (yyval.str) = generateStrDefine((yyvsp[-1].str)); }
#line 2709 "Language.tab.c"
    break;

  case 18: /* DEFINES: t_SET_CMD VAR_LIST ';'  */
#line 1454 "Language.y"
                                                     { (yyval.str) = generateSetDefine((yyvsp[-1].str)); }
#line 2715 "Language.tab.c"
    break;

  case 19: /* DEFINES: t_COLLECTION_CMD VAR_LIST ';'  */
#line 1455 "Language.y"
                                                     { (yyval.str) = generateCollectionDefine((yyvsp[-1].str)); }
#line 2721 "Language.tab.c"
    break;

  case 20: /* ASSIGNMENTS: VAR '=' EXPRESSIONS ';'  */
#line 1458 "Language.y"
                                                     { (yyval.str) = GenerateAssignment((yyvsp[-3].str), (yyvsp[-1].str)); }
#line 2727 "Language.tab.c"
    break;

  case 21: /* ASSIGNMENTS: VAR '=' t_STRING ';'  */
#line 1459 "Language.y"
                                                     { (yyval.str) = generateStrAssignment((yyvsp[-3].str), (yyvsp[-1].str)); }
#line 2733 "Language.tab.c"
    break;

  case 22: /* IO: t_OUTPUT_CMD t_STRING ';'  */
#line 1462 "Language.y"
                                                    { (yyval.str) = generateOutput((yyvsp[-1].str)); }
#line 2739 "Language.tab.c"
    break;

  case 23: /* IO: t_OUTPUT_CMD t_STRING SET ';'  */
#line 1463 "Language.y"
                                                    { (yyval.str) = generateVarOutput((yyvsp[-2].str), (yyvsp[-1].str)); }
#line 2745 "Language.tab.c"
    break;

  case 24: /* IO: t_OUTPUT_CMD t_STRING COLLECTION ';'  */
#line 1464 "Language.y"
                                                    { (yyval.str) = generateVarOutput((yyvsp[-2].str), (yyvsp[-1].str)); }
#line 2751 "Language.tab.c"
    break;

  case 25: /* IO: t_OUTPUT_CMD t_STRING EXPRESSIONS ';'  */
#line 1465 "Language.y"
                                                    { (yyval.str) = generateExpressionOutput((yyvsp[-2].str), (yyvsp[-1].str)); }
#line 2757 "Language.tab.c"
    break;

  case 26: /* IO: t_INPUT_CMD t_STRING VAR ';'  */
#line 1466 "Language.y"
                                                    { (yyval.str) = generateInput((yyvsp[-2].str), (yyvsp[-1].str)); }
#line 2763 "Language.tab.c"
    break;

  case 27: /* IF_CONDITIONS: EXPRESSIONS GREATER EXPRESSIONS  */
#line 1469 "Language.y"
                                                             { (yyval.str) = concatWithDelimiter((yyvsp[-2].str), (yyvsp[0].str), ">"); }
#line 2769 "Language.tab.c"
    break;

  case 28: /* IF_CONDITIONS: EXPRESSIONS LESS EXPRESSIONS  */
#line 1470 "Language.y"
                                                            { (yyval.str) = concatWithDelimiter((yyvsp[-2].str), (yyvsp[0].str), "<"); }
#line 2775 "Language.tab.c"
    break;

  case 29: /* IF_CONDITIONS: EXPRESSIONS EQUAL EXPRESSIONS  */
#line 1471 "Language.y"
                                                            { (yyval.str) = concatWithDelimiter((yyvsp[-2].str), (yyvsp[0].str), "==");  printf("EQUAL: %s\n", (yyval.str));}
#line 2781 "Language.tab.c"
    break;

  case 30: /* IF_CONDITIONS: EXPRESSIONS GREATER_EQUAL EXPRESSIONS  */
#line 1472 "Language.y"
                                                            { (yyval.str) = concatWithDelimiter((yyvsp[-2].str), (yyvsp[0].str), ">="); }
#line 2787 "Language.tab.c"
    break;

  case 31: /* IF_CONDITIONS: EXPRESSIONS LESS_EQUAL EXPRESSIONS  */
#line 1473 "Language.y"
                                                            { (yyval.str) = concatWithDelimiter((yyvsp[-2].str), (yyvsp[0].str), "<="); }
#line 2793 "Language.tab.c"
    break;

  case 32: /* IF_CONDITIONS: NOT VAR  */
#line 1474 "Language.y"
                                                            { (yyval.str) = concatWithDelimiter("!", (yyvsp[0].str), ""); }
#line 2799 "Language.tab.c"
    break;

  case 33: /* IF_CONDITIONS: VAR  */
#line 1475 "Language.y"
                                                            { (yyval.str) = strdup((yyvsp[0].str)); }
#line 2805 "Language.tab.c"
    break;

  case 34: /* WHILE_CONDITIONS: LITERALS GREATER LITERALS  */
#line 1479 "Language.y"
                                                       { (yyval.str) = concatWithDelimiter((yyvsp[-2].str), (yyvsp[0].str), ">"); }
#line 2811 "Language.tab.c"
    break;

  case 35: /* WHILE_CONDITIONS: LITERALS LESS LITERALS  */
#line 1480 "Language.y"
                                                      { (yyval.str) = concatWithDelimiter((yyvsp[-2].str), (yyvsp[0].str), "<"); }
#line 2817 "Language.tab.c"
    break;

  case 36: /* WHILE_CONDITIONS: LITERALS EQUAL LITERALS  */
#line 1481 "Language.y"
                                                      { (yyval.str) = concatWithDelimiter((yyvsp[-2].str), (yyvsp[0].str), "=="); }
#line 2823 "Language.tab.c"
    break;

  case 37: /* WHILE_CONDITIONS: LITERALS GREATER_EQUAL LITERALS  */
#line 1482 "Language.y"
                                                      { (yyval.str) = concatWithDelimiter((yyvsp[-2].str), (yyvsp[0].str), ">="); }
#line 2829 "Language.tab.c"
    break;

  case 38: /* WHILE_CONDITIONS: LITERALS LESS_EQUAL LITERALS  */
#line 1483 "Language.y"
                                                      { (yyval.str) = concatWithDelimiter((yyvsp[-2].str), (yyvsp[0].str), "<="); }
#line 2835 "Language.tab.c"
    break;

  case 39: /* WHILE_CONDITIONS: NOT VAR  */
#line 1484 "Language.y"
                                                      { (yyval.str) = concatWithDelimiter("!", (yyvsp[0].str), ""); }
#line 2841 "Language.tab.c"
    break;

  case 40: /* WHILE_CONDITIONS: VAR  */
#line 1485 "Language.y"
                                                      { (yyval.str) = strdup((yyvsp[0].str)); }
#line 2847 "Language.tab.c"
    break;

  case 41: /* FOR_CONDITION: VAR ':' VAR  */
#line 1488 "Language.y"
                                                { (yyval.str) = checkForCondition((yyvsp[-2].str), (yyvsp[0].str)); }
#line 2853 "Language.tab.c"
    break;

  case 42: /* EXPRESSIONS: EXPRESSIONS PLUS EXPRESSIONS  */
#line 1492 "Language.y"
                                                   { (yyval.str) = concatWithDelimiter((yyvsp[-2].str), (yyvsp[0].str), "+"); }
#line 2859 "Language.tab.c"
    break;

  case 43: /* EXPRESSIONS: EXPRESSIONS MINUS EXPRESSIONS  */
#line 1493 "Language.y"
                                                   { (yyval.str) = concatWithDelimiter((yyvsp[-2].str), (yyvsp[0].str), "-"); }
#line 2865 "Language.tab.c"
    break;

  case 44: /* EXPRESSIONS: EXPRESSIONS MULTIPLY EXPRESSIONS  */
#line 1494 "Language.y"
                                                   { (yyval.str) = concatWithDelimiter((yyvsp[-2].str), (yyvsp[0].str), "*"); }
#line 2871 "Language.tab.c"
    break;

  case 45: /* EXPRESSIONS: EXPRESSIONS DIVIDE EXPRESSIONS  */
#line 1495 "Language.y"
                                                   { (yyval.str) = concatWithDelimiter((yyvsp[-2].str), (yyvsp[0].str), "/"); }
#line 2877 "Language.tab.c"
    break;

  case 46: /* EXPRESSIONS: EXPRESSIONS AND EXPRESSIONS  */
#line 1496 "Language.y"
                                                   { (yyval.str) = concatWithDelimiter((yyvsp[-2].str), (yyvsp[0].str), "*"); printf("AND: %s\n", (yyval.str)); }
#line 2883 "Language.tab.c"
    break;

  case 47: /* EXPRESSIONS: '(' EXPRESSIONS ')'  */
#line 1497 "Language.y"
                                                   { (yyval.str) = concatWithDelimiter("(", (yyvsp[-1].str), ""); (yyval.str) = concatWithDelimiter((yyval.str), ")", ""); }
#line 2889 "Language.tab.c"
    break;

  case 48: /* EXPRESSIONS: OR VAR OR  */
#line 1498 "Language.y"
                                                   { (yyval.str) = concatWithDelimiter("|", (yyvsp[-1].str), ""); (yyval.str) = concatWithDelimiter((yyval.str), "|", ""); }
#line 2895 "Language.tab.c"
    break;

  case 49: /* EXPRESSIONS: LITERALS  */
#line 1499 "Language.y"
                                                   { (yyval.str) = strdup((yyvsp[0].str)); }
#line 2901 "Language.tab.c"
    break;

  case 50: /* LITERALS: t_INTEGER  */
#line 1502 "Language.y"
                                                      { (yyval.str) = copyNumber((yyvsp[0].num)); }
#line 2907 "Language.tab.c"
    break;

  case 51: /* LITERALS: t_STRING  */
#line 1503 "Language.y"
                                                       { (yyval.str) = copyStr((yyvsp[0].str)); }
#line 2913 "Language.tab.c"
    break;

  case 52: /* LITERALS: SET  */
#line 1504 "Language.y"
                                                        { (yyval.str) = strdup((yyvsp[0].str)); }
#line 2919 "Language.tab.c"
    break;

  case 53: /* LITERALS: COLLECTION  */
#line 1505 "Language.y"
                                                        { (yyval.str) = strdup((yyvsp[0].str)); }
#line 2925 "Language.tab.c"
    break;

  case 54: /* LITERALS: VAR  */
#line 1506 "Language.y"
                                                        { (yyval.str) = strdup((yyvsp[0].str)); }
#line 2931 "Language.tab.c"
    break;

  case 55: /* VAR_LIST: VAR_LIST ',' VAR  */
#line 1509 "Language.y"
                                                    { (yyval.str) = addVarToList((yyvsp[-2].str), (yyvsp[0].str)); }
#line 2937 "Language.tab.c"
    break;

  case 56: /* VAR_LIST: VAR  */
#line 1510 "Language.y"
                                                    { (yyval.str) = strdup((yyvsp[0].str)); }
#line 2943 "Language.tab.c"
    break;

  case 57: /* VAR: t_ID  */
#line 1513 "Language.y"
                                                    { (yyval.str) = copyStr((yyvsp[0].str)); }
#line 2949 "Language.tab.c"
    break;

  case 58: /* SET: '[' INT_LIST ']'  */
#line 1516 "Language.y"
                                                    { (yyval.str) = concatWithDelimiter("[", (yyvsp[-1].str), ""); (yyval.str) = concatWithDelimiter((yyval.str), "]", ""); }
#line 2955 "Language.tab.c"
    break;

  case 59: /* SET: '[' ']'  */
#line 1517 "Language.y"
                                                    { (yyval.str) = strdup("[]"); }
#line 2961 "Language.tab.c"
    break;

  case 60: /* COLLECTION: '{' '}'  */
#line 1520 "Language.y"
                                                     { (yyval.str) = strdup("{}"); }
#line 2967 "Language.tab.c"
    break;

  case 61: /* COLLECTION: '{' STRING_LIST '}'  */
#line 1521 "Language.y"
                                                     { (yyval.str) = concatWithDelimiter("{", (yyvsp[-1].str), ""); (yyval.str) = concatWithDelimiter((yyval.str), "}", ""); }
#line 2973 "Language.tab.c"
    break;

  case 62: /* INT_LIST: INT_LIST ',' t_INTEGER  */
#line 1524 "Language.y"
                                                     { (yyval.str) = addToSet((yyvsp[-2].str), copyNumber((yyvsp[0].num))); }
#line 2979 "Language.tab.c"
    break;

  case 63: /* INT_LIST: t_INTEGER  */
#line 1525 "Language.y"
                                                     { (yyval.str) = copyNumber((yyvsp[0].num)); }
#line 2985 "Language.tab.c"
    break;

  case 64: /* STRING_LIST: STRING_LIST ',' t_STRING  */
#line 1528 "Language.y"
                                                      { (yyval.str) = addStrToList((yyvsp[-2].str), (yyvsp[0].str)); }
#line 2991 "Language.tab.c"
    break;

  case 65: /* STRING_LIST: t_STRING  */
#line 1529 "Language.y"
                                                      { (yyval.str) = copyStr((yyvsp[0].str)); }
#line 2997 "Language.tab.c"
    break;


#line 3001 "Language.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 1532 "Language.y"

