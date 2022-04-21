#pragma once
#include "framework.h"
#include "WordleAgent/Agent.h"


// custom wordle player agent implementation
class MyAgent : public Agent
{
public:

    // constructor.
    MyAgent(Game& game);

    // return the agent's next guess at the solution.
    virtual std::string GetNextGuess();

private:

    WordSet game_word_set;
    WordSet iteration_word_set;
    int attempt;
};
