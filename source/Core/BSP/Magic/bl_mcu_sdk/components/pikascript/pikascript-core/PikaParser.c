/*
 * This file is part of the PikaScript project.
 * http://github.com/pikastech/pikascript
 *
 * MIT License
 *
 * Copyright (c) 2021 lyon 李昂 liang6516@outlook.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "PikaParser.h"
#include "BaseObj.h"
#include "PikaObj.h"
#include "dataQueue.h"
#include "dataQueueObj.h"
#include "dataStack.h"
#include "dataStrs.h"

char* strsPopTokenWithSkip_byStr(Args* buffs,
                                 char* stmts,
                                 char* str,
                                 char skipStart,
                                 char skipEnd) {
    int32_t str_size = strGetSize(str);
    int32_t size = strGetSize(stmts);
    if (0 == size) {
        return NULL;
    }
    char* strOut = args_getBuff(buffs, size);
    int32_t stmtEnd = 0;
    uint8_t isGetSign = 0;
    int32_t parentheseDeepth = 0;
    for (int32_t i = 0; i < size; i++) {
        if (skipStart == stmts[i]) {
            parentheseDeepth++;
        }
        if (skipEnd == stmts[i]) {
            parentheseDeepth--;
        }
        if (parentheseDeepth == 0) {
            if (0 == strncmp(stmts + i, str, str_size)) {
                stmtEnd = i;
                isGetSign = 1;
                break;
            }
        }
    }
    if (!isGetSign) {
        stmtEnd = size;
    }
    for (int32_t i = 0; i < stmtEnd; i++) {
        strOut[i] = stmts[i];
    }
    memmove(stmts, stmts + stmtEnd + str_size, size);
    strOut[stmtEnd] = 0;
    return strOut;
}

char* strsPopTokenWithSkip(Args* buffs,
                           char* stmts,
                           char sign,
                           char skipStart,
                           char skipEnd) {
    char str_buff[2] = {0};
    str_buff[0] = sign;
    return strsPopTokenWithSkip_byStr(buffs, stmts, str_buff, skipStart,
                                      skipEnd);
}

char* strsGetCleanCmd(Args* outBuffs, char* cmd) {
    int32_t size = strGetSize(cmd);
    Args* buffs = New_strBuff();
    char* tokens = Lexer_getTokens(buffs, cmd);
    uint16_t token_size = strCountSign(tokens, 0x1F) + 1;
    char* strOut = args_getBuff(outBuffs, size);
    int32_t iOut = 0;
    for (uint16_t i = 0; i < token_size; i++) {
        char* token = strsPopToken(buffs, tokens, 0x1F);
        for (uint16_t k = 0; k < strGetSize(token + 1); k++) {
            strOut[iOut] = token[k + 1];
            iOut++;
        }
    }
    /* add \0 */
    strOut[iOut] = 0;
    args_deinit(buffs);
    return strOut;
}

enum TokenType {
    TOKEN_strEnd = 0,
    TOKEN_symbol,
    TOKEN_keyword,
    TOKEN_operator,
    TOKEN_devider,
    TOKEN_literal,
};

enum StmtType {
    STMT_reference,
    STMT_string,
    STMT_number,
    STMT_method,
    STMT_operator,
    STMT_none,
};

char* strsDeleteBetween(Args* buffs, char* strIn, char begin, char end) {
    int32_t size = strGetSize(strIn);
    char* strOut = args_getBuff(buffs, size);
    uint8_t deepth = 0;
    uint32_t iOut = 0;
    for (int i = 0; i < size; i++) {
        if (end == strIn[i]) {
            deepth--;
        }
        if (0 == deepth) {
            strOut[iOut] = strIn[i];
            iOut++;
        }
        if (begin == strIn[i]) {
            deepth++;
        }
    }
    strOut[iOut] = 0;
    return strOut;
}

static enum StmtType Lexer_matchStmtType(char* right) {
    Args* buffs = New_strBuff();
    enum StmtType stmtType = STMT_none;
    char* rightWithoutSubStmt = strsDeleteBetween(buffs, right, '(', ')');
    char* tokens = Lexer_getTokens(buffs, rightWithoutSubStmt);
    uint16_t token_size = strCountSign(tokens, 0x1F) + 1;
    uint8_t is_get_operator = 0;
    uint8_t is_get_method = 0;
    uint8_t is_get_string = 0;
    uint8_t is_get_number = 0;
    uint8_t is_get_symbol = 0;
    for (int i = 0; i < token_size; i++) {
        char* token = strsPopToken(buffs, tokens, 0x1F);
        enum TokenType token_type = token[0];
        /* collect type */
        if (token_type == TOKEN_operator) {
            is_get_operator = 1;
            continue;
        }
        if (token_type == TOKEN_devider) {
            is_get_method = 1;
            continue;
        }
        if (token_type == TOKEN_literal) {
            if (token[1] == '\'' || token[1] == '"') {
                is_get_string = 1;
                continue;
            }
            is_get_number = 1;
            continue;
        }
        if (token_type == TOKEN_symbol) {
            is_get_symbol = 1;
            continue;
        }
    }
    if (is_get_operator) {
        stmtType = STMT_operator;
        goto exit;
    }
    if (is_get_method) {
        stmtType = STMT_method;
        goto exit;
    }
    if (is_get_string) {
        stmtType = STMT_string;
        goto exit;
    }
    if (is_get_number) {
        stmtType = STMT_number;
        goto exit;
    }
    if (is_get_symbol) {
        stmtType = STMT_reference;
        goto exit;
    }
exit:
    args_deinit(buffs);
    return stmtType;
}

uint8_t Parser_checkIsDirect(char* str) {
    /* include '0' */
    uint32_t size = strGetSize(str) + 1;
    for (int i = 1; i + 1 < size; i++) {
        if ((str[i - 1] != '%') && (str[i - 1] != '!') && (str[i - 1] != '<') &&
            (str[i - 1] != '>') && (str[i - 1] != '=') && (str[i - 1] != '+') &&
            (str[i - 1] != '-') && (str[i - 1] != '*') && (str[i - 1] != '/') &&
            (str[i + 1] != '=') && (str[i] == '=')) {
            return 1;
        }
    }
    return 0;
}

char* Lexer_printTokens(Args* outBuffs, char* tokens) {
    /* init */
    Args* buffs = New_strBuff();
    char* printOut = strsCopy(buffs, "");

    /* process */
    uint16_t tokenSize = strCountSign(tokens, 0x1F) + 1;
    for (int i = 0; i < tokenSize; i++) {
        char* token = strsPopToken(buffs, tokens, 0x1F);
        if (token[0] == TOKEN_operator) {
            printOut = strsAppend(buffs, printOut, "{opt}");
            printOut = strsAppend(buffs, printOut, token + 1);
        }
        if (token[0] == TOKEN_devider) {
            printOut = strsAppend(buffs, printOut, "{dvd}");
            printOut = strsAppend(buffs, printOut, token + 1);
        }
        if (token[0] == TOKEN_symbol) {
            printOut = strsAppend(buffs, printOut, "{sym}");
            printOut = strsAppend(buffs, printOut, token + 1);
        }
        if (token[0] == TOKEN_literal) {
            printOut = strsAppend(buffs, printOut, "{lit}");
            printOut = strsAppend(buffs, printOut, token + 1);
        }
    }
    /* out put */
    printOut = strsCopy(outBuffs, printOut);
    args_deinit(buffs);
    return printOut;
}

Arg* Lexer_setToken(Arg* tokens_arg,
                    enum TokenType token_type,
                    char*
                    operator) {
    Args* buffs = New_strBuff();
    char token_type_buff[3] = {0};
    token_type_buff[0] = 0x1F;
    token_type_buff[1] = token_type;
    char* tokens = arg_getStr(tokens_arg);
    tokens = strsAppend(buffs, tokens, token_type_buff);
    tokens = strsAppend(buffs, tokens, operator);
    Arg* new_tokens_arg = arg_setStr(tokens_arg, "", tokens);
    arg_deinit(tokens_arg);
    args_deinit(buffs);
    return new_tokens_arg;
}

Arg* Lexer_setSymbel(Arg* tokens_arg,
                     char* stmt,
                     uint32_t i,
                     int* symbol_start_index) {
    Args* buffs = New_strBuff();
    /* nothing to add symbel */
    if (i == *symbol_start_index) {
        *symbol_start_index = -1;
        goto exit;
    }
    char* symbol_buff = args_getBuff(buffs, i - *symbol_start_index);
    memcpy(symbol_buff, stmt + *symbol_start_index, i - *symbol_start_index);
    /* literal */
    if ((symbol_buff[0] == '-') || (symbol_buff[0] == '\'') ||
        (symbol_buff[0] == '"') ||
        ((symbol_buff[0] >= '0') && (symbol_buff[0] <= '9'))) {
        tokens_arg = Lexer_setToken(tokens_arg, TOKEN_literal, symbol_buff);
    } else {
        /* symbol */
        tokens_arg = Lexer_setToken(tokens_arg, TOKEN_symbol, symbol_buff);
    }
    *symbol_start_index = -1;
exit:
    args_deinit(buffs);
    return tokens_arg;
}

/* tokens is devided by space */
/* a token is [TOKENTYPE|(CONTENT)] */
char* Lexer_getTokens(Args* outBuffs, char* stmt) {
    /* init */
    Arg* tokens_arg = New_arg(NULL);
    tokens_arg = arg_setStr(tokens_arg, "", "");
    uint32_t size = strGetSize(stmt);
    uint8_t c0 = 0;
    uint8_t c1 = 0;
    uint8_t c2 = 0;
    uint8_t c3 = 0;
    int symbol_start_index = -1;
    int is_in_string = 0;

    /* process */
    for (int i = 0; i < size; i++) {
        /* update char */
        c0 = stmt[i];
        if (i + 1 < size) {
            c1 = stmt[i + 1];
        }
        if (i + 2 < size) {
            c2 = stmt[i + 2];
        }
        if (i + 3 < size) {
            c3 = stmt[i + 3];
        }
        if (-1 == symbol_start_index) {
            symbol_start_index = i;
        }

        /* solve string */
        if (0 == is_in_string) {
            if ('\'' == c0) {
                /* in ' */
                is_in_string = 1;
                continue;
            }
            if ('"' == c0) {
                /* in "" */
                is_in_string = 2;
                continue;
            }
        }

        if (1 == is_in_string) {
            if ('\'' == c0) {
                is_in_string = 0;
                tokens_arg = Lexer_setSymbel(tokens_arg, stmt, i + 1,
                                             &symbol_start_index);
            }
            continue;
        }
        if (2 == is_in_string) {
            if ('"' == c0) {
                is_in_string = 0;
                tokens_arg = Lexer_setSymbel(tokens_arg, stmt, i + 1,
                                             &symbol_start_index);
            }
            continue;
        }

        /* match devider*/
        if (('(' == c0) || (')' == c0) || (',' == c0)) {
            tokens_arg =
                Lexer_setSymbel(tokens_arg, stmt, i, &symbol_start_index);
            char content[2] = {0};
            content[0] = c0;
            tokens_arg = Lexer_setToken(tokens_arg, TOKEN_devider, content);
            continue;
        }
        /* match operator */
        if (('>' == c0) || ('<' == c0) || ('*' == c0) || ('/' == c0) ||
            ('+' == c0) || ('-' == c0) || ('!' == c0) || ('=' == c0) ||
            ('%' == c0) || ('&' == c0) || ('|' == c0) || ('^' == c0) ||
            ('~' == c0)) {
            if (('*' == c0) || ('/' == c0)) {
                /*
                     //=, **=
                */
                if ((c0 == c1) && ('=' == c2)) {
                    char content[4] = {0};
                    content[0] = c0;
                    content[1] = c1;
                    content[2] = '=';
                    tokens_arg = Lexer_setSymbel(tokens_arg, stmt, i,
                                                 &symbol_start_index);
                    tokens_arg =
                        Lexer_setToken(tokens_arg, TOKEN_operator, content);
                    i = i + 2;
                    continue;
                }
            }
            /*
                >>, <<, **, //
            */
            if (('>' == c0) || ('<' == c0) || ('*' == c0) || ('/' == c0)) {
                if (c0 == c1) {
                    char content[3] = {0};
                    content[0] = c0;
                    content[1] = c1;
                    tokens_arg = Lexer_setSymbel(tokens_arg, stmt, i,
                                                 &symbol_start_index);
                    tokens_arg =
                        Lexer_setToken(tokens_arg, TOKEN_operator, content);
                    i = i + 1;
                    continue;
                }
            }
            /*
                >=, <=, *=, /=, +=, -=, !=, ==, %=
            */
            if (('>' == c0) || ('<' == c0) || ('*' == c0) || ('/' == c0) ||
                ('+' == c0) || ('-' == c0) || ('!' == c0) || ('=' == c0) ||
                ('%' == c0)) {
                if ('=' == c1) {
                    char content[3] = {0};
                    content[0] = c0;
                    content[1] = c1;
                    tokens_arg = Lexer_setSymbel(tokens_arg, stmt, i,
                                                 &symbol_start_index);
                    tokens_arg =
                        Lexer_setToken(tokens_arg, TOKEN_operator, content);
                    i = i + 1;
                    continue;
                }
            }
            /* single */
            char content[2] = {0};
            content[0] = c0;
            if ('-' != c0) {
                tokens_arg =
                    Lexer_setSymbel(tokens_arg, stmt, i, &symbol_start_index);
                tokens_arg =
                    Lexer_setToken(tokens_arg, TOKEN_operator, content);
                continue;
            }
            /* when c0 is '-' */
            if (!((c1 >= '0') && (c1 <= '9'))) {
                /* is a '-' */
                tokens_arg =
                    Lexer_setSymbel(tokens_arg, stmt, i, &symbol_start_index);
                tokens_arg =
                    Lexer_setToken(tokens_arg, TOKEN_operator, content);
                continue;
            }
            if (i != symbol_start_index) {
                /* is a '-' */
                tokens_arg =
                    Lexer_setSymbel(tokens_arg, stmt, i, &symbol_start_index);
                tokens_arg =
                    Lexer_setToken(tokens_arg, TOKEN_operator, content);
                continue;
            }
            /* is a symbel */
            continue;
        }
        /* not */
        if ('n' == c0) {
            if (('o' == c1) && ('t' == c2) && (' ' == c3)) {
                tokens_arg =
                    Lexer_setSymbel(tokens_arg, stmt, i, &symbol_start_index);
                tokens_arg =
                    Lexer_setToken(tokens_arg, TOKEN_operator, " not ");
                i = i + 3;
                continue;
            }
        }
        /* and */
        if ('a' == c0) {
            if (('n' == c1) && ('d' == c2) && (' ' == c3)) {
                tokens_arg =
                    Lexer_setSymbel(tokens_arg, stmt, i, &symbol_start_index);
                tokens_arg =
                    Lexer_setToken(tokens_arg, TOKEN_operator, " and ");
                i = i + 3;
                continue;
            }
        }
        /* or */
        if ('o' == c0) {
            if (('r' == c1) && (' ' == c2)) {
                tokens_arg =
                    Lexer_setSymbel(tokens_arg, stmt, i, &symbol_start_index);
                tokens_arg = Lexer_setToken(tokens_arg, TOKEN_operator, " or ");
                i = i + 2;
                continue;
            }
        }
        /* skip spaces */
        if (' ' == c0) {
            /* not get symbal */
            if (i == symbol_start_index) {
                symbol_start_index++;
            } else {
                /* already get symbal */
                tokens_arg =
                    Lexer_setSymbel(tokens_arg, stmt, i, &symbol_start_index);
            }
        }
        if (i == size - 1) {
            /* last check symbel */
            tokens_arg =
                Lexer_setSymbel(tokens_arg, stmt, size, &symbol_start_index);
        }
    }
    /* output */
    char* tokens = arg_getStr(tokens_arg);
    tokens = strsCopy(outBuffs, tokens);
    arg_deinit(tokens_arg);
    return tokens;
}

uint8_t Lexer_isContain(char* tokens, char* operator) {
    Args* buffs = New_strBuff();
    char* tokens_buff = strsCopy(buffs, tokens);
    uint8_t res = 0;
    uint16_t token_size = strCountSign(tokens, 0x1F) + 1;
    for (int i = 0; i < token_size; i++) {
        char* token = strsPopToken(buffs, tokens_buff, 0x1F);
        if (TOKEN_operator == token[0]) {
            if (strEqu(token + 1, operator)) {
                res = 1;
                goto exit;
            }
        }
    }
exit:
    args_deinit(buffs);
    return res;
}

char* Lexer_getOperator(Args* outBuffs, char* stmt) {
    Args* buffs = New_strBuff();
    char* tokens = Lexer_getTokens(buffs, stmt);
    char* operator= NULL;
    const char operators[][6] = {
        "**", "~",   "*",  "/",  "%",  "//",  "+",     "-",     ">>",  "<<",
        "&",  "^",   "|",  "<",  "<=", ">",   ">=",    "!=",    "==",  "%=",
        "/=", "//=", "-=", "+=", "*=", "**=", " not ", " and ", " or "};
    for (int i = 0; i < sizeof(operators) / 6; i++) {
        if (Lexer_isContain(tokens, (char*)operators[i])) {
            operator= strsCopy(buffs, (char*)operators[i]);
        }
    }
    /* out put */
    operator= strsCopy(outBuffs, operator);
    args_deinit(buffs);
    return operator;
}

AST* AST_parseStmt(AST* ast, char* stmt) {
    Args* buffs = New_strBuff();
    char* assignment = strsGetFirstToken(buffs, stmt, '(');
    char* direct = NULL;
    char* method = NULL;
    char* ref = NULL;
    char* str = NULL;
    char* num = NULL;
    char* right = NULL;
    /* solve direct */
    uint8_t directExist = 0;
    if (Parser_checkIsDirect(assignment)) {
        directExist = 1;
    }
    if (directExist) {
        direct = strsGetFirstToken(buffs, assignment, '=');
        obj_setStr(ast, (char*)"direct", direct);
    }
    /* solve right stmt */
    if (directExist) {
        right = strsGetLastToken(buffs, stmt, '=');
    } else {
        right = stmt;
    }
    enum StmtType stmtType = Lexer_matchStmtType(right);
    /* solve operator stmt */
    if (STMT_operator == stmtType) {
        char* rightWithoutSubStmt = strsDeleteBetween(buffs, right, '(', ')');
        char* operator= Lexer_getOperator(buffs, rightWithoutSubStmt);
        obj_setStr(ast, (char*)"operator", operator);
        char* rightBuff = strsCopy(buffs, right);
        char* subStmt1 =
            strsPopTokenWithSkip_byStr(buffs, rightBuff, operator, '(', ')');
        char* subStmt2 = rightBuff;
        queueObj_pushObj(ast, (char*)"stmt");
        AST_parseStmt(queueObj_getCurrentObj(ast), subStmt1);
        queueObj_pushObj(ast, (char*)"stmt");
        AST_parseStmt(queueObj_getCurrentObj(ast), subStmt2);
        goto exit;
    }
    /* solve method stmt */
    if (STMT_method == stmtType) {
        method = strsGetFirstToken(buffs, right, '(');
        obj_setStr(ast, (char*)"method", method);
        char* subStmts = strsCut(buffs, stmt, '(', ')');
        while (1) {
            char* subStmt =
                strsPopTokenWithSkip(buffs, subStmts, ',', '(', ')');
            if (NULL == subStmt) {
                break;
            }
            queueObj_pushObj(ast, (char*)"stmt");
            AST_parseStmt(queueObj_getCurrentObj(ast), subStmt);
        }
        goto exit;
    }
    /* solve reference stmt */
    if (STMT_reference == stmtType) {
        ref = right;
        obj_setStr(ast, (char*)"ref", ref);
        goto exit;
    }
    /* solve str stmt */
    if (STMT_string == stmtType) {
        str = right;
        str = strsDeleteChar(buffs, str, '\'');
        str = strsDeleteChar(buffs, str, '\"');
        obj_setStr(ast, (char*)"string", str);
        goto exit;
    }
    /* solve number stmt */
    if (STMT_number == stmtType) {
        num = right;
        obj_setStr(ast, (char*)"num", num);
        goto exit;
    }
exit:
    args_deinit(buffs);
    return ast;
}

static int32_t Parser_getPyLineBlockDeepth(char* line) {
    uint32_t size = strGetSize(line);
    for (int i = 0; i < size; i++) {
        if (line[i] != ' ') {
            uint32_t spaceNum = i;
            if (0 == spaceNum % 4) {
                return spaceNum / 4;
            }
            /* space Num is not 4N, error*/
            return -1;
        }
    }
    return 0;
}

AST* AST_parseLine(char* line, Stack* blockStack) {
    AST* ast = New_queueObj();
    Args* buffs = New_strBuff();
    line = strsDeleteChar(buffs, line, '\r');
    uint8_t blockDeepth = Parser_getPyLineBlockDeepth(line);
    uint8_t blockDeepthLast = blockDeepth;
    if (NULL != blockStack) {
        blockDeepthLast = args_getInt(blockStack, "top");
        for (int i = 0; i < blockDeepthLast - blockDeepth; i++) {
            QueueObj* exitBlock = obj_getObj(ast, "exitBlock", 0);
            if (NULL == exitBlock) {
                obj_newObj(ast, "exitBlock", "", New_TinyObj);
                exitBlock = obj_getObj(ast, "exitBlock", 0);
                queueObj_init(exitBlock);
            }
            char buff[10] = {0};
            char* blockType = stack_popStr(blockStack, buff);
            queueObj_pushStr(exitBlock, blockType);
        }
    }
    obj_setInt(ast, "blockDeepth", blockDeepth);
    char* lineStart = line + blockDeepth * 4;
    char* stmt = lineStart;
    if (0 == strncmp(lineStart, (char*)"while ", 6)) {
        stmt = strsCut(buffs, lineStart, ' ', ':');
        obj_setStr(ast, "block", "while");
        if (NULL != blockStack) {
            stack_pushStr(blockStack, "while");
        }
    }
    if (0 == strncmp(lineStart, (char*)"if ", 3)) {
        stmt = strsCut(buffs, lineStart, ' ', ':');
        obj_setStr(ast, "block", "if");
        if (NULL != blockStack) {
            stack_pushStr(blockStack, "if");
        }
    }
    if (strEqu(lineStart, (char*)"return")) {
        obj_setStr(ast, "return", "");
        stmt = "";
    }
    if (0 == strncmp(lineStart, (char*)"return ", 7)) {
        char* lineBuff = strsCopy(buffs, lineStart);
        strsPopToken(buffs, lineBuff, ' ');
        stmt = lineBuff;
        obj_setStr(ast, "return", "");
    }
    if (0 == strncmp(lineStart, (char*)"def ", 4)) {
        stmt = "";
        char* declear = strsCut(buffs, lineStart, ' ', ':');
        declear = strsGetCleanCmd(buffs, declear);
        obj_setStr(ast, "block", "def");
        obj_setStr(ast, "declear", declear);
        if (NULL != blockStack) {
            stack_pushStr(blockStack, "def");
        }
    }
    stmt = strsGetCleanCmd(buffs, stmt);
    ast = AST_parseStmt(ast, stmt);
    goto exit;
exit:
    args_deinit(buffs);
    return ast;
}

char* Parser_LineToAsm(Args* buffs, char* line, Stack* blockStack) {
    AST* ast = AST_parseLine(line, blockStack);
    char* pikaAsm = AST_toPikaAsm(ast, buffs);
    AST_deinit(ast);
    return pikaAsm;
}

static Arg* ASM_saveSingleAsm(Args* buffs,
                              Arg* pikaAsmBuff,
                              char* singleAsm,
                              uint8_t isToFlash) {
    if (isToFlash) {
        uint8_t saveErr = __platformSavePikaAsm(singleAsm);
        if (0 == saveErr) {
            if (NULL != pikaAsmBuff) {
                arg_deinit(pikaAsmBuff);
            }
            return NULL;
        }
    }

    char* pikaAsm = arg_getStr(pikaAsmBuff);
    pikaAsm = strsAppend(buffs, pikaAsm, singleAsm);
    arg_deinit(pikaAsmBuff);
    pikaAsmBuff = arg_setStr(NULL, "", pikaAsm);
    return pikaAsmBuff;
}

static char* ASM_getOutAsm(Args* outBuffs,
                           Arg* pikaAsmBuff,
                           uint8_t isToFlash) {
    if (isToFlash) {
        return __platformLoadPikaAsm();
    }
    return strsCopy(outBuffs, arg_getStr(pikaAsmBuff));
}

char* Parser_multiLineToAsm(Args* outBuffs, char* multiLine) {
    Stack* blockStack = New_Stack();
    Arg* pikaAsmBuff = arg_setStr(NULL, "", "");
    uint32_t lineOffset = 0;
    uint32_t multiLineSize = strGetSize(multiLine);
    uint8_t isToFlash = __platformAsmIsToFlash(multiLine);
    while (1) {
        Args* singleRunBuffs = New_strBuff();
        char* line =
            strsGetFirstToken(singleRunBuffs, multiLine + lineOffset, '\n');
        uint32_t lineSize = strGetSize(line);
        lineOffset = lineOffset + lineSize + 1;
        char* singleAsm = Parser_LineToAsm(singleRunBuffs, line, blockStack);
        pikaAsmBuff = ASM_saveSingleAsm(singleRunBuffs, pikaAsmBuff, singleAsm,
                                        isToFlash);
        args_deinit(singleRunBuffs);
        if (lineOffset >= multiLineSize) {
            break;
        }
    }
    if (isToFlash) {
        __platformSavePikaAsmEOF();
    }
    char* outAsm = ASM_getOutAsm(outBuffs, pikaAsmBuff, isToFlash);
    if (NULL != pikaAsmBuff) {
        arg_deinit(pikaAsmBuff);
    }
    stack_deinit(blockStack);
    return outAsm;
}

char* AST_appandPikaAsm(AST* ast, AST* subAst, Args* buffs, char* pikaAsm) {
    uint32_t deepth = obj_getInt(ast, "deepth");
    while (1) {
        QueueObj* subStmt = queueObj_popObj(subAst);
        if (NULL == subStmt) {
            break;
        }
        obj_setInt(ast, "deepth", deepth + 1);
        pikaAsm = AST_appandPikaAsm(ast, subStmt, buffs, pikaAsm);
    }
    char* method = obj_getStr(subAst, "method");
    char* operator= obj_getStr(subAst, "operator");
    char* ref = obj_getStr(subAst, "ref");
    char* direct = obj_getStr(subAst, "direct");
    char* str = obj_getStr(subAst, "string");
    char* num = obj_getStr(subAst, "num");
    if (NULL != ref) {
        char buff[32] = {0};
        sprintf(buff, "%d REF %s\n", deepth, ref);
        pikaAsm = strsAppend(buffs, pikaAsm, buff);
    }
    if (NULL != operator) {
        char buff[32] = {0};
        sprintf(buff, "%d OPT %s\n", deepth, operator);
        pikaAsm = strsAppend(buffs, pikaAsm, buff);
    }
    if (NULL != method) {
        char buff[32] = {0};
        sprintf(buff, "%d RUN %s\n", deepth, method);
        pikaAsm = strsAppend(buffs, pikaAsm, buff);
    }
    if (NULL != str) {
        char buff[32] = {0};
        sprintf(buff, "%d STR %s\n", deepth, str);
        pikaAsm = strsAppend(buffs, pikaAsm, buff);
    }
    if (NULL != num) {
        char buff[32] = {0};
        sprintf(buff, "%d NUM %s\n", deepth, num);
        pikaAsm = strsAppend(buffs, pikaAsm, buff);
    }

    if (NULL != direct) {
        char buff[32] = {0};
        sprintf(buff, "%d OUT %s\n", deepth, direct);
        pikaAsm = strsAppend(buffs, pikaAsm, buff);
    }
    obj_setInt(ast, "deepth", deepth - 1);
    return pikaAsm;
}

static char* ASM_addBlockDeepth(AST* ast,
                                Args* buffs,
                                char* pikaAsm,
                                uint8_t deepthOffset) {
    pikaAsm = strsAppend(buffs, pikaAsm, (char*)"B");
    char buff[11];
    pikaAsm = strsAppend(
        buffs, pikaAsm,
        fast_itoa(buff, obj_getInt(ast, "blockDeepth") + deepthOffset));
    pikaAsm = strsAppend(buffs, pikaAsm, (char*)"\n");
    return pikaAsm;
}

char* AST_toPikaAsm(AST* ast, Args* buffs) {
    Args* runBuffs = New_strBuff();
    char* pikaAsm = strsCopy(runBuffs, "");
    QueueObj* exitBlock = obj_getObj(ast, "exitBlock", 0);
    if (NULL != exitBlock) {
        while (1) {
            uint8_t blockTypeNum = obj_getInt(exitBlock, "top") -
                                   obj_getInt(exitBlock, "bottom") - 1;
            char* blockType = queueObj_popStr(exitBlock);
            if (NULL == blockType) {
                break;
            }
            /* goto the while start when exit while block */
            if (strEqu(blockType, "while")) {
                pikaAsm = ASM_addBlockDeepth(ast, buffs, pikaAsm, blockTypeNum);
                pikaAsm = strsAppend(buffs, pikaAsm, (char*)"0 JMP -1\n");
            }
            /* return when exit method */
            if (strEqu(blockType, "def")) {
                pikaAsm = strsAppend(buffs, pikaAsm, (char*)"0 RET\n");
            }
        }
    }
    pikaAsm = ASM_addBlockDeepth(ast, buffs, pikaAsm, 0);
    obj_setInt(ast, "deepth", 0);

    /* parse ast to asm main process */
    pikaAsm = AST_appandPikaAsm(ast, ast, runBuffs, pikaAsm);

    if (strEqu(obj_getStr(ast, "block"), "while")) {
        pikaAsm = strsAppend(runBuffs, pikaAsm, "0 JEZ 2\n");
    }
    if (strEqu(obj_getStr(ast, "block"), "if")) {
        pikaAsm = strsAppend(runBuffs, pikaAsm, "0 JEZ 1\n");
    }
    if (strEqu(obj_getStr(ast, "block"), "def")) {
        pikaAsm = strsAppend(runBuffs, pikaAsm, "0 DEF ");
        pikaAsm = strsAppend(runBuffs, pikaAsm, obj_getStr(ast, "declear"));
        pikaAsm = strsAppend(runBuffs, pikaAsm, "\n");
        pikaAsm = strsAppend(runBuffs, pikaAsm, "0 JMP 1\n");
    }
    if (obj_isArgExist(ast, "return")) {
        pikaAsm = strsAppend(runBuffs, pikaAsm, "0 RET\n");
    }
    pikaAsm = strsCopy(buffs, pikaAsm);
    args_deinit(runBuffs);
    return pikaAsm;
}
int32_t AST_deinit(AST* ast) {
    return obj_deinit(ast);
}
