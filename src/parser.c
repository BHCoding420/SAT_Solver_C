#include "parser.h"

#include <string.h>

#include "ctype.h"
#include "err.h"
#include "lexer.h"
#include "list.h"
#include "propformula.h"
#include "string.h"
#include "util.h"

/**
 * Assigns symbols to strings.
 *
 * Aborts the program with an error message if an invalid input is detected.
 *
 * @param str  a string to translate
 * @return     the resulting symbol
 */

int checkProperVar(const char* str) {
    if (!isalpha(str[0])) {
        return 0;
    }
    int i = 1;
    while (str[i] != '\0') {
        if (!isalnum(str[i])) {
            return 0;
        }
        i++;
    }
    return 1;
}

FormulaKind toKind(const char* str) {
    // ...

    if (strcmp(str, "!") == 0) {
        return NOT;
    } else if (strcmp(str, "&&") == 0) {
        return AND;
    } else if (strcmp(str, "||") == 0) {
        return OR;
    } else if (strcmp(str, "=>") == 0) {
        return IMPLIES;
    } else if (strcmp(str, "<=>") == 0) {
        return EQUIV;
    } else if (checkProperVar(str) == 1) {
        return VAR;
    } else {
        err("not proper kind");
        return NOT;
    }
}

PropFormula* parseFormula(FILE* input, struct VarTable* vt) {
    char* token;
    token = nextToken(input);  // store first word in token

    if (token == NULL) {
        err("file ma fiho shi");
    }

    List stack = mkList();
    PropFormula*
        last_ele_Unary;  // will hiold every var we pop in case we recieve !
    PropFormula* e1;     // will hiold the last 2 expresions we pop in case we
                         // recieve binary op
    PropFormula* e2;

    while (token != NULL) {
        FormulaKind kind = toKind(token);
        // printf("%s \n", token);
        //  check if we obtained a var
        if (kind == VAR) {
            PropFormula* e = mkVarFormula(
                vt, token);  // create a Var formula from token and add it to
                             // stack,then proceed to next value
            // prettyPrintFormula(vt, e);
            push(&stack, e);
            // free(token);

            token = nextToken(input);

            // continue;
        } else if (kind == NOT) {
            free(token);
            last_ele_Unary = peek(&stack);  // obtain last element in stack
            if (last_ele_Unary == NULL) {
                err("iblis");  // in order to negate,we should have a variable
                               // expression,otherwise there is an error
            }
            pop(&stack);
            PropFormula* e = mkUnaryFormula(
                NOT,
                last_ele_Unary);  // create a Var formula from token and add it
                                  // to stack,then proceed to next value

            push(&stack, e);
            // freeFormula(last_ele_Unary);
            // free(token);
            token = nextToken(input);
            // continue;

        } else {
            // pop 2 last elements to combine in binary operator,the elements
            // should be var
            free(token);
            e1 = peek(&stack);
            if (e1 == NULL) {
                err("kes em l manyake bi ayre e1");
            }
            pop(&stack);

            e2 = peek(&stack);
            if (e2 == NULL) {
                err("kes em l manyake bi ayre e2");
            }

            pop(&stack);
            PropFormula* e = mkBinaryFormula(
                kind, e2, e1);  // make binary formula from last 2 elemets
            push(&stack, e);
            // freeFormula(e1);

            // freeFormula(e2);

            token = nextToken(input);

            // continue;
        }
    }
    // if everything is correct and no additional conetnt available,then we
    // should have only one complete formula left
    PropFormula* main_exp;
    main_exp = peek(&stack);

    pop(&stack);

    if (!isEmpty(&stack)) {
        err("error");
    }

    return main_exp;
}
