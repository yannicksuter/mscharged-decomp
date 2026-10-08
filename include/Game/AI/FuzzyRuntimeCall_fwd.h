#ifndef GAME_AI_FUZZYRUNTIMECALL_FWD_H
#define GAME_AI_FUZZYRUNTIMECALL_FWD_H

class InterpreterCore;
class cFielder;
class UnidentifiedVariant_80054AB8;

UnidentifiedVariant_80054AB8 CallFielderFuzzyFunction(InterpreterCore*, const char*, cFielder*);
UnidentifiedVariant_80054AB8 CallFielderFuzzyFunction(void*, cFielder*, const char*);
UnidentifiedVariant_80054AB8 CallFielderFuzzyFunction(void*, const unsigned int&, cFielder*);

#endif // GAME_AI_FUZZYRUNTIMECALL_FWD_H
