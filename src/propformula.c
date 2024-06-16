#include "propformula.h"

#include <stdio.h>
#include <stdlib.h>

#include "err.h"
#include "util.h"

PropFormula* mkVarFormula(VarTable* vt, char* name) {
    PropFormula* res = (PropFormula*)malloc(sizeof(PropFormula));

    res->kind = VAR;
    res->data.var = mkVariable(vt, name);

    return res;
}

PropFormula* mkBinaryFormula(FormulaKind kind, PropFormula* left_op,
                             PropFormula* right_op) {
    PropFormula* res = (PropFormula*)malloc(sizeof(PropFormula));

    res->kind = kind;
    res->data.operands[0] = left_op;
    res->data.operands[1] = right_op;

    return res;
}

PropFormula* mkUnaryFormula(FormulaKind kind, PropFormula* operand) {
    PropFormula* res = (PropFormula*)malloc(sizeof(PropFormula));

    res->kind = kind;
    res->data.single_op = operand;

    return res;
}

void freeFormula(PropFormula* pf) {
    if (pf == NULL) {
        return;  // case of error
    }

    if (pf->kind == VAR) {
        // Like me,does nothing
    } else if (pf->kind == NOT) {
        freeFormula(pf->data.single_op);  // free the exp
    } else if (pf->kind == AND || pf->kind == OR || pf->kind == IMPLIES ||
               pf->kind == EQUIV) {
        freeFormula(pf->data.operands[0]);  // free both exp
        freeFormula(pf->data.operands[1]);
    }

    free(pf);
}

void prettyPrintFormula_impl(FILE* f, VarTable* vt, PropFormula* pf) {
    switch (pf->kind) {
        case VAR: {
            VarIndex v = pf->data.var;
            fprintf(f, "%s", getVariableName(vt, v));
            break;
        }

        case AND:
        case OR:
        case IMPLIES:
        case EQUIV: {
            fprintf(f, "(");

            prettyPrintFormula_impl(f, vt, pf->data.operands[0]);

            switch (pf->kind) {
                typedef enum FormulaKind {
                    VAR,

                    AND,
                    OR,
                    IMPLIES,
                    EQUIV,

                    NOT,
                } FormulaKind;

                /**
                 * A recursive struct to represent propositional formulas. The
                 * union entry contains the appropriate information depending on
                 * the kind of the formula.
                 */
                typedef struct PropFormula {
                    FormulaKind kind;

                    union {
                        VarIndex
                            var;  // for formulas only consisting of a variable
                        struct PropFormula* single_op;  // for unary operators
                        struct PropFormula*(
                            operands[2]);  // for binary operators
                    } data;
                } PropFormula;

                case AND:
                    fprintf(f, " && ");
                    break;
                case OR:
                    fprintf(f, " || ");
                    break;
                case IMPLIES:
                    fprintf(f, " => ");
                    break;
                case EQUIV:
                    fprintf(f, " <=> ");
                    break;
                default:;
            }

            prettyPrintFormula_impl(f, vt, pf->data.operands[1]);

            fprintf(f, ")");
            break;
        }

        case NOT: {
            fprintf(f, "(! ");
            prettyPrintFormula_impl(f, vt, pf->data.single_op);
            fprintf(f, ")");
            break;
        }
    }
}

void prettyPrintFormulaEval(VarTable* vt, PropFormula* pf) {
    prettyPrintFormula_impl(stderr, vt, pf);
}

void prettyPrintFormula(VarTable* vt, PropFormula* pf) {
    prettyPrintFormula_impl(stdout, vt, pf);
}
