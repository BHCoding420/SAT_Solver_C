#include "tseitin.h"

#include <stdio.h>

#include "err.h"
#include "propformula.h"
#include "util.h"
#include "variables.h"

/**
 * Inserts a clause with one literal into the CNF.
 *
 * @param vt   the underlying variable table
 * @param cnf  a formula
 * @param a    a literal
 */
void addUnaryClause(VarTable* vt, CNF* cnf, Literal a) {
    Clause* clause = mkTernaryClause(vt, a, 0, 0);
    addClauseToCNF(cnf, clause);
}

/**
 * Inserts a clause with two literals into the CNF.
 *
 * @param vt   the underlying variable table
 * @param cnf  a formula
 * @param a    the first literal
 * @param b    the second literal
 */
void addBinaryClause(VarTable* vt, CNF* cnf, Literal a, Literal b) {
    Clause* clause = mkTernaryClause(vt, a, b, 0);
    addClauseToCNF(cnf, clause);
}

/**
 * Inserts a clause with three literals into the CNF.
 *
 * @param vt   the underlying variable table
 * @param cnf  a formula
 * @param a    the first literal
 * @param b    the second literal
 * @param c    the third literal
 */
void addTernaryClause(VarTable* vt, CNF* cnf, Literal a, Literal b, Literal c) {
    Clause* clause = mkTernaryClause(vt, a, b, c);
    addClauseToCNF(cnf, clause);
}

/**
 * Adds clauses for a propositional formula to a CNF.
 *
 * For a propositional formula pf, clauses that are added that are equivalent to
 *
 *     x <=> pf
 *
 * where x is usually a fresh variable. This variable is also returned.
 *
 * @param vt   the underlying variable table
 * @param cnf  a formula
 * @param pf   a propositional formula
 * @return     the variable x, as described above
 */
VarIndex addClauses(VarTable* vt, CNF* cnf, const PropFormula* pf) {
    switch (pf->kind) {
        case VAR:
            return pf->data.var;

        case NOT: {
            VarIndex a = addClauses(
                vt, cnf,
                pf->data.single_op);  // recurse on the only thing available
            VarIndex x = mkFreshVariable(vt);  // variable to represent

            addBinaryClause(vt, cnf, -x, -a);  //(!x or !a)
            addBinaryClause(vt, cnf, a, x);    // and (a or x)
            return x;
        }

        case AND: {
            VarIndex a = addClauses(
                vt, cnf, pf->data.operands[0]);  // recurse on first exp
            VarIndex b = addClauses(
                vt, cnf, pf->data.operands[1]);  // recurse on 2nd exp
            VarIndex x = mkFreshVariable(vt);    // variable to represent

            addBinaryClause(vt, cnf, -x, a);       // (!x or a)
            addBinaryClause(vt, cnf, -x, b);       // and (!x or b)
            addTernaryClause(vt, cnf, -a, -b, x);  // and (!a or !b or x)
            return x;
        }

        case OR: {
            VarIndex a = addClauses(
                vt, cnf, pf->data.operands[0]);  // recurse on first exp
            VarIndex b = addClauses(
                vt, cnf, pf->data.operands[1]);  // recurse on 2nd exp
            VarIndex x = mkFreshVariable(vt);    // variable to represent

            addBinaryClause(vt, cnf, x, -b);      // (x or !b)
            addBinaryClause(vt, cnf, x, -a);      // and (x or !a)
            addTernaryClause(vt, cnf, a, b, -x);  // and (a or b or !x)
            return x;
        }

        case EQUIV: {
            VarIndex a = addClauses(
                vt, cnf, pf->data.operands[0]);  // recurse on first exp
            VarIndex b = addClauses(
                vt, cnf, pf->data.operands[1]);  // recurse on 2nd exp
            VarIndex x = mkFreshVariable(vt);    // variable to represent

            addTernaryClause(vt, cnf, -a, b, -x);  //(!a or b or !x)
            addTernaryClause(vt, cnf, a, -b, -x);  // and (a or !b or !x)
            addTernaryClause(vt, cnf, -a, -b, x);  // and (!a or !b or x)
            addTernaryClause(vt, cnf, x, a, b);    // and (x or a or b)
            return x;
        }

        case IMPLIES: {
            VarIndex a = addClauses(
                vt, cnf, pf->data.operands[0]);  // recurse on first exp
            VarIndex b = addClauses(
                vt, cnf, pf->data.operands[1]);  // recurse on 2nd exp
            VarIndex x = mkFreshVariable(vt);    // variable to represent

            addTernaryClause(vt, cnf, -x, -a, b);  //(!x or !a or b)
            addBinaryClause(vt, cnf, a, x);        // and (a or x)
            addBinaryClause(vt, cnf, -b, x);       // and (!b or x)
            return x;
        }
    }

    return pf->data.var;
}

CNF* getCNF(VarTable* vt, const PropFormula* f) {
    CNF* res = mkCNF();

    VarIndex x = addClauses(vt, res, f);

    addUnaryClause(vt, res, x);

    return res;
}
