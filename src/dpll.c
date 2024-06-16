#include "dpll.h"

#include "cnf.h"
#include "err.h"
#include "list.h"
#include "util.h"
#include "variables.h"

typedef enum Reason { CHOSEN, IMPLIED } Reason;

/**
 * Struct to represent an entry in the assignment stack. Should only be created
 * and freed by pushAssignment and popAssignment.
 */
typedef struct Assignment {
    VarIndex var;
    Reason reason;
} Assignment;

/**
 * Adds a new assignment to the assignment stack.
 *
 * @param stack  an assignment stack
 * @param var    the variable to assign
 * @param r      the reason for the assignment
 */
void pushAssignment(List* stack, VarIndex var, Reason r) {
    Assignment* a = (Assignment*)malloc(sizeof(Assignment));
    a->var = var;
    a->reason = r;
    push(stack, a);
}

/**
 * Removes the head element of an assignment stack and frees it.
 *
 * @param stack  an assignment stack
 */
void popAssignment(List* stack) {
    Assignment* a = (Assignment*)peek(stack);
    free(a);
    pop(stack);
}

void backflop(List* stack, VarTable* vt) {
    while (!isEmpty(stack)) {
        Assignment* temp = peek(stack);

        if (temp->reason == CHOSEN) {
            updateVariableValue(vt, temp->var, FALSE);
            temp->reason = IMPLIED;
            return;

        } else {
            updateVariableValue(vt, temp->var, UNDEFINED);
            popAssignment(stack);
        }
    }
}
/**
 * Führt eine Iteration des DPLL Algorithmus aus.
 *
 * @param vt       die zugrunde liegende Variablentabelle
 * @param stack    der Zuweisungsstack
 * @param cnf      die zu prüfende Formel
 * @return         1 falls der Algorithmus mit SAT terminieren sollte,
 *                 0 falls der Algorithmus weiterlaufen sollte,
 *                 -1 falls der Algorithmus mit UNSAT terminieren sollte
 */
/**
 * Performs one iteration of the DPLL algorithm.
 *
 * @param vt       the underlying variable table
 * @param stack    an assignment stack
 * @param cnf      the formula to check
 * @return         1 if the algorithm should terminate with SAT,
 *                 0 if the algorithm should continue,
 *                -1 if the algorithm should terminate with UNSAT
 */

int reFlop_possible(List* stack) {
    ListIterator iter = mkIterator(stack);
    while (isValid(&iter) != 0) {
        void* ptr = getCurr(&iter);
        Assignment* assignmentPtr = (Assignment*)ptr;
        Assignment value = *assignmentPtr;
        if (value.reason == CHOSEN) {
            return 1;
        }
        next(&iter);
    }
    return 0;
}
int iterate(VarTable* vt, List* stack, CNF* cnf) {
    // TODO Implement me!

    // if all clauses are fulfilled
    if (evalCNF(cnf) == TRUE) {
        return 1;
    }

    // if one clause is false
    if (evalCNF(cnf) == FALSE) {
        if (reFlop_possible(stack) == 1) {
            backflop(stack, vt);
            return 0;
        } else {
            return -1;
        }
    }  // check if unit clause exists
    ListIterator iter = mkIterator(&cnf->clauses);

    while (isValid(&iter) != 0) {
        Literal literal = getUnitLiteral(vt, getCurr(&iter));
        /*if (literal != 0) {
            // Uni clause exists
            // update the unit literal to satisfy the clause
            updateVariableValue(vt, literal, TRUE);
            next(&iter);
            return 0;
        }
        next(&iter);*/
        if (literal > 0) {
            updateVariableValue(vt, literal, TRUE);
            pushAssignment(stack, literal, IMPLIED);
            // break;
            return 0;
        } else if (literal < 0) {
            updateVariableValue(vt, -literal, FALSE);
            pushAssignment(stack, -literal, IMPLIED);
            // break;
            return 0;
        }
        next(&iter);
    }

    // Select next free variable and set to true
    VarIndex next = getNextUndefinedVariable(vt);
    updateVariableValue(vt, next, TRUE);

    pushAssignment(stack, next, CHOSEN);
    /*Assignment for_next;
    for_next.var = next;
    for_next.reason = CHOSEN;
    push(stack, &for_next);*/
    return 0;

    // NOT_IMPLEMENTED;
    // UNUSED(vt);
    // UNUSED(stack);
    // UNUSED(cnf);
}

char isSatisfiable(VarTable* vt, CNF* cnf) {
    List stack = mkList();

    int res;
    do {
        res = iterate(vt, &stack, cnf);
    } while (res == 0);

    while (!isEmpty(&stack)) {
        popAssignment(&stack);
    }

    return (res < 0) ? 0 : 1;
}
