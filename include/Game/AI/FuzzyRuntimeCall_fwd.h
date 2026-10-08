#ifndef GAME_AI_FUZZYRUNTIMECALL_FWD_H
#define GAME_AI_FUZZYRUNTIMECALL_FWD_H

class InterpreterCore;
class cFielder;
class DesireUpdate;

DesireUpdate CallFielderFuzzyFunction(InterpreterCore*, const char*, cFielder*);
DesireUpdate CallFielderFuzzyFunction(void*, cFielder*, const char*);
DesireUpdate CallFielderFuzzyFunction(void*, const unsigned int&, cFielder*);

#endif // GAME_AI_FUZZYRUNTIMECALL_FWD_H
