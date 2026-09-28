#ifndef GAME_INTERPRETER_OPERATIONS_H
#define GAME_INTERPRETER_OPERATIONS_H

class InterpreterCore;

typedef void (*InterpreterOperation)(InterpreterCore*);
extern InterpreterOperation gInterpreterOperations[];

#endif // GAME_INTERPRETER_OPERATIONS_H
