%{
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


%}

%union {
    char* str;
    int num;
}

%token t_STRING_CMD t_SET_CMD t_COLLECTION_CMD t_IF_CMD t_ELSE_CMD t_WHILE_CMD t_FOR_CMD
%token t_INTEGER_CMD t_OUTPUT_CMD t_INPUT_CMD
%token GREATER LESS EQUAL GREATER_EQUAL LESS_EQUAL NOT
%token <str> t_STRING t_ID 
%token <num> t_INTEGER
%token PLUS MINUS MULTIPLY DIVIDE

%type <str> VAR VAR_LIST INT_LIST SET STRING_LIST COLLECTION Prog
%type <str> DEFINES ASSIGNMENTS EXPRESSIONS BLOCK SENTENCE_LIST SENTENCE CONTROL IO  FOR_CONDITION LITERALS IF_CONDITIONS WHILE_CONDITIONS

%left EQUAL
%left AND OR
%left PLUS MINUS
%left MULTIPLY DIVIDE
%left '('

%right '='
%nonassoc t_ELSE_CMD

%start Prog
%debug

%%

Prog: Prog SENTENCE  {printToFile($2);}  
    | SENTENCE       {printToFile($1);}  
    ;

SENTENCE: DEFINES       { $$ = $1; }
        | ASSIGNMENTS   { $$ = $1; }
        | IO            { $$ = $1; }
        | CONTROL       { $$ = $1; printf("Control: %s\n", $$); }
        ;

CONTROL: t_IF_CMD '(' IF_CONDITIONS ')' BLOCK                           { $$ = generateIf($3, $5, NULL);  printf("IF: %s\n", $3); }
       | t_IF_CMD '(' IF_CONDITIONS ')' BLOCK t_ELSE_CMD BLOCK          { $$ = generateIf($3, $5, $7);}
       | t_WHILE_CMD '(' WHILE_CONDITIONS ')' BLOCK                     { $$ = generateWhile($3, $5); }
       | t_FOR_CMD '(' FOR_CONDITION')' BLOCK                           { $$ = generateFor($3, $5); }
       ;

BLOCK: '{' SENTENCE_LIST '}'                        { $$ = $2; }
        | SENTENCE                                 { $$ = $1; printf("BLOCK: %s\n", $$); }
     ;

SENTENCE_LIST: SENTENCE_LIST SENTENCE               { $$ = concatSentences($1, strdup($2)); }
             | SENTENCE                             { $$ = strdup($1); printf("SENTENCE: %s\n", $$); }
             ;

DEFINES: t_INTEGER_CMD VAR_LIST ';'                  { $$ = generateIntDefine($2); }
        | t_STRING_CMD VAR_LIST ';'                  { $$ = generateStrDefine($2); }
        | t_SET_CMD VAR_LIST ';'                     { $$ = generateSetDefine($2); }
        | t_COLLECTION_CMD VAR_LIST ';'              { $$ = generateCollectionDefine($2); }
        ;

ASSIGNMENTS: VAR '=' EXPRESSIONS ';'                 { $$ = GenerateAssignment($1, $3); }
            | VAR '=' t_STRING ';'                   { $$ = generateStrAssignment($1, $3); }
            ;

IO :      t_OUTPUT_CMD t_STRING ';'                 { $$ = generateOutput($2); }
        | t_OUTPUT_CMD t_STRING SET ';'             { $$ = generateVarOutput($2, $3); }
        | t_OUTPUT_CMD t_STRING COLLECTION ';'      { $$ = generateVarOutput($2, $3); }
        | t_OUTPUT_CMD t_STRING EXPRESSIONS ';'     { $$ = generateExpressionOutput($2, $3); }
        | t_INPUT_CMD t_STRING VAR ';'              { $$ = generateInput($2, $3); }
        ;

IF_CONDITIONS:    EXPRESSIONS GREATER EXPRESSIONS            { $$ = concatWithDelimiter($1, $3, ">"); }
                | EXPRESSIONS LESS EXPRESSIONS              { $$ = concatWithDelimiter($1, $3, "<"); }
                | EXPRESSIONS EQUAL EXPRESSIONS             { $$ = concatWithDelimiter($1, $3, "==");  printf("EQUAL: %s\n", $$);}
                | EXPRESSIONS GREATER_EQUAL EXPRESSIONS     { $$ = concatWithDelimiter($1, $3, ">="); }
                | EXPRESSIONS LESS_EQUAL EXPRESSIONS        { $$ = concatWithDelimiter($1, $3, "<="); }
                | NOT VAR                                   { $$ = concatWithDelimiter("!", $2, ""); }
                | VAR                                       { $$ = strdup($1); }
                ;


WHILE_CONDITIONS: LITERALS GREATER LITERALS            { $$ = concatWithDelimiter($1, $3, ">"); }
                | LITERALS LESS LITERALS              { $$ = concatWithDelimiter($1, $3, "<"); }
                | LITERALS EQUAL LITERALS             { $$ = concatWithDelimiter($1, $3, "=="); }
                | LITERALS GREATER_EQUAL LITERALS     { $$ = concatWithDelimiter($1, $3, ">="); }
                | LITERALS LESS_EQUAL LITERALS        { $$ = concatWithDelimiter($1, $3, "<="); }
                | NOT VAR                             { $$ = concatWithDelimiter("!", $2, ""); }
                | VAR                                 { $$ = strdup($1); }
                ;

FOR_CONDITION: VAR ':' VAR                      { $$ = checkForCondition($1, $3); }
             ;


EXPRESSIONS : EXPRESSIONS PLUS EXPRESSIONS         { $$ = concatWithDelimiter($1, $3, "+"); }
            | EXPRESSIONS MINUS EXPRESSIONS        { $$ = concatWithDelimiter($1, $3, "-"); }
            | EXPRESSIONS MULTIPLY EXPRESSIONS     { $$ = concatWithDelimiter($1, $3, "*"); }
            | EXPRESSIONS DIVIDE EXPRESSIONS       { $$ = concatWithDelimiter($1, $3, "/"); }
            | EXPRESSIONS AND EXPRESSIONS          { $$ = concatWithDelimiter($1, $3, "*"); printf("AND: %s\n", $$); }
            | '(' EXPRESSIONS ')'                  { $$ = concatWithDelimiter("(", $2, ""); $$ = concatWithDelimiter($$, ")", ""); }
            | OR VAR OR                            { $$ = concatWithDelimiter("|", $2, ""); $$ = concatWithDelimiter($$, "|", ""); }
            | LITERALS                             { $$ = strdup($1); }  
            ;

LITERALS:    t_INTEGER                                { $$ = copyNumber($1); }
            | t_STRING                                 { $$ = copyStr($1); }
            | SET                                       { $$ = strdup($1); }
            | COLLECTION                                { $$ = strdup($1); }
            | VAR                                       { $$ = strdup($1); }  
            ;

VAR_LIST : VAR_LIST ',' VAR                         { $$ = addVarToList($1, $3); }
         | VAR                                      { $$ = strdup($1); }  
         ;

VAR : t_ID                                          { $$ = copyStr($1); }
    ;

SET : '[' INT_LIST ']'                              { $$ = concatWithDelimiter("[", $2, ""); $$ = concatWithDelimiter($$, "]", ""); }
    | '[' ']'                                       { $$ = strdup("[]"); }
    ;

COLLECTION : '{' '}'                                 { $$ = strdup("{}"); }
           | '{' STRING_LIST '}'                     { $$ = concatWithDelimiter("{", $2, ""); $$ = concatWithDelimiter($$, "}", ""); }
           ;

INT_LIST : INT_LIST ',' t_INTEGER                    { $$ = addToSet($1, copyNumber($3)); }
         | t_INTEGER                                 { $$ = copyNumber($1); }
         ;

STRING_LIST : STRING_LIST ',' t_STRING                { $$ = addStrToList($1, $3); }
            | t_STRING                                { $$ = copyStr($1); }
            ;

%%
