
#include "MyAgent.h"
#include "WordleAgent/Random.h"
#include <iostream>
#include <set>


// constructor.
MyAgent::MyAgent(Game& game)
    : Agent(game)
{
    // do any preprocessing here

    // start with our own copy of the full word set
    potentialWords = GetGame().GetWordSet();

    entireWordSet = GetGame().GetWordSet();
    remainingGuesses = 6;
    knownLetters = "";
}


// return the agent's next guess at the solution
std::string MyAgent::GetNextGuess()
{

    // if (there has been a previous guess) then
    auto gr = GetGame().GetLastGuessResult();
    if (gr)
    {
        remainingGuesses--;
        knownLetters += gr->guess;
        //std::cout << knownLetters << std::endl;

        // update our knowledge based on the result of the last guess

        // first, remove the guess from the list as it obviously wasn't correct
        potentialWords.erase(gr->guess);

        // for the result code associated with each letter of the guess
        for (size_t letterIndex = 0; letterIndex < gr->result.size(); letterIndex++)
        {
            // if (this letter was in the correct position) then
            if (gr->result[letterIndex] == Colour::GREEN)
            {
                // for our word set, filter out any words which don't have this letter in that position
                for (WordSet::iterator potentialWordsIterator = potentialWords.begin(); potentialWordsIterator != potentialWords.end();)
                {
                    // if (this letter of this word doesn't match this letter of the guess word) then
                    if ((*potentialWordsIterator)[letterIndex] != gr->guess[letterIndex])
                    {
                        potentialWordsIterator = potentialWords.erase(potentialWordsIterator);
                    }

                    else
                    {
                        ++potentialWordsIterator;
                    }
                }
            }

            // if this letter is not in the word
            if (gr->result[letterIndex] == Colour::BLACK)
            {
                // filter out any words that include this letter from the word set
                for (WordSet::iterator potentialWordsIterator = potentialWords.begin(); potentialWordsIterator != potentialWords.end(); ++potentialWordsIterator)
                {
                    // look at all of the letters in the word set word to see if it includes the filtered letter
                    for (size_t wordSetLetterIndex = 0; wordSetLetterIndex < (*potentialWordsIterator).size(); wordSetLetterIndex++)
                    {
                        // if the letter is in the word, and the letter is not in the correct position
                        // have to check if the letter is in the correct position, because some words can have multiple occurences of the same letter
                        // i.e. say we guess "skies" and the first "s" is green, but the last "s" is black
                        // that would mean the first "s" is in the correct position, but the word has no further occurences of "s"
                        if ((*potentialWordsIterator)[wordSetLetterIndex] == gr->guess[letterIndex] && gr->result[wordSetLetterIndex] != Colour::GREEN)
                        {
                            // if the word is not the final word in the word set
                            if (potentialWordsIterator != --potentialWords.end())
                            {
                                // delete the word and reset the letter index back to -1, ready for the next word
                                // -1 because the index will increment back up to 0
                                wordSetLetterIndex = -1;
                                potentialWordsIterator = potentialWords.erase(potentialWordsIterator);
                            }

                            else
                            {
                                // delete the word without exceeding the bounds of the word set and exit the letter index for loop
                                potentialWordsIterator = --potentialWords.erase(potentialWordsIterator);
                                break;
                            }
                        }
                    }
                }
            }

            if (gr->result[letterIndex] == Colour::YELLOW)
            {
                for (WordSet::iterator potentialWordsIterator = potentialWords.begin(); potentialWordsIterator != potentialWords.end();)
                {
                    if ((*potentialWordsIterator)[letterIndex] == gr->guess[letterIndex])
                    {
                        potentialWordsIterator = potentialWords.erase(potentialWordsIterator);
                    }

                    else
                    {
                        bool letterFound = false;

                        for (size_t wordSetLetterIndex = 0; wordSetLetterIndex < (*potentialWordsIterator).size(); wordSetLetterIndex++)
                        {
                            if ((*potentialWordsIterator)[wordSetLetterIndex] == gr->guess[letterIndex])
                            {
                                letterFound = true;
                                break;
                            }
                        }

                        if (letterFound)
                        {
                            ++potentialWordsIterator;
                        }

                        else
                        {
                            potentialWordsIterator = potentialWords.erase(potentialWordsIterator);
                        }
                    }
                }
            }
        }
    }

    // debugging
    //std::cout << "m_word_set size = " << m_word_set.size() << std::endl;

    // return a random item from our remaining word set

    WordSet nextWords;
    if (potentialWords.size() <= remainingGuesses)
    {
        nextWords = potentialWords;
    }

    else
    {
        size_t minimumDuplicateLetters = INT_MAX;
        WordSet leastDuplicateLettersWords;

        for (WordSet::iterator wordSetIterator = entireWordSet.begin(); wordSetIterator != entireWordSet.end(); ++wordSetIterator)
        {
            size_t duplicateLettersCount = 0;

            for (size_t letterIndex = 0; letterIndex < (*wordSetIterator).size(); letterIndex++)
            {
                for (size_t duplicateLetterIndex = 0; duplicateLetterIndex < (*wordSetIterator).size(); duplicateLetterIndex++)
                {
                    if (letterIndex != duplicateLetterIndex && (*wordSetIterator)[letterIndex] == (*wordSetIterator)[duplicateLetterIndex])
                    {
                        duplicateLettersCount++;
                    }
                }
            }

            if (duplicateLettersCount == minimumDuplicateLetters)
            {
                leastDuplicateLettersWords.insert(*wordSetIterator);
            }

            if (duplicateLettersCount < minimumDuplicateLetters)
            {
                leastDuplicateLettersWords.clear();
                leastDuplicateLettersWords.insert(*wordSetIterator);
                minimumDuplicateLetters = duplicateLettersCount;
            }
        }

        WordSet mostUnknownLettersWords;
        size_t maximumUnknownLetters = 0;
        for (WordSet::iterator leastDuplicateLettersIterator = leastDuplicateLettersWords.begin(); leastDuplicateLettersIterator != leastDuplicateLettersWords.end(); ++leastDuplicateLettersIterator)
        {
            size_t unknownLettersCount = 0;

            for (size_t letterIndex = 0; letterIndex < (*leastDuplicateLettersIterator).size(); letterIndex++)
            {
                bool letterKnown = false;

                for (size_t knownLetterIndex = 0; knownLetterIndex < knownLetters.size(); knownLetterIndex++)
                {
                    if ((*leastDuplicateLettersIterator)[letterIndex] == knownLetters[knownLetterIndex])
                    {
                        letterKnown = true;
                        break;
                    }
                }

                if (!letterKnown)
                {
                    unknownLettersCount++;
                }
            }

            if (unknownLettersCount == maximumUnknownLetters)
            {
                mostUnknownLettersWords.insert(*leastDuplicateLettersIterator);
            }

            if (unknownLettersCount > maximumUnknownLetters)
            {
                mostUnknownLettersWords.clear();
                mostUnknownLettersWords.insert(*leastDuplicateLettersIterator);
                maximumUnknownLetters = unknownLettersCount;
            }
        }

        nextWords = mostUnknownLettersWords;
    }

    int index = Random::GetRandomInt(0, static_cast<int>(nextWords.size() - 1));
    return *std::next(nextWords.begin(), index);
}
