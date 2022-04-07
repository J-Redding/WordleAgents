
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
    m_word_set = GetGame().GetWordSet();
}


// return the agent's next guess at the solution
std::string MyAgent::GetNextGuess()
{
    // if (there has been a previous guess) then
    auto gr = GetGame().GetLastGuessResult();
    if (gr)
    {
        // update our knowledge based on the result of the last guess

        // first, remove the guess from the list as it obviously wasn't correct
        m_word_set.erase(gr->guess);

        // for the result code associated with each letter of the guess
        for (size_t letterIndex = 0; letterIndex < gr->result.size(); letterIndex++)
        {
            // if (this letter was in the correct position) then
            if (gr->result[letterIndex] == Colour::GREEN)
            {
                // for our word set, filter out any words which don't have this letter in that position
                for (WordSet::iterator wordSetIterator = m_word_set.begin(); wordSetIterator != m_word_set.end();)
                {
                    // if (this letter of this word doesn't match this letter of the guess word) then
                    if ((*wordSetIterator)[letterIndex] != gr->guess[letterIndex])
                    {
                        wordSetIterator = m_word_set.erase(wordSetIterator);
                    }

                    else
                    {
                        ++wordSetIterator;
                    }
                }
            }

            // if this letter is not in the word
            if (gr->result[letterIndex] == Colour::BLACK)
            {
                // filter out any words that include this letter from the word set
                for (WordSet::iterator wordSetIterator = m_word_set.begin(); wordSetIterator != m_word_set.end(); ++wordSetIterator)
                {
                    // look at all of the letters in the word set word to see if it includes the filtered letter
                    for (size_t wordSetLetterIndex = 0; wordSetLetterIndex < (*wordSetIterator).size(); wordSetLetterIndex++)
                    {
                        // if the letter is in the word, and the letter is not in the correct position
                        // have to check if the letter is in the correct position, because some words can have multiple occurences of the same letter
                        // i.e. say we guess "skies" and the first "s" is green, but the last "s" is black
                        // that would mean the first "s" is in the correct position, but the word has no further occurences of "s"
                        if ((*wordSetIterator)[wordSetLetterIndex] == gr->guess[letterIndex] && gr->result[wordSetLetterIndex] != Colour::GREEN)
                        {
                            // if the word is not the final word in the word set
                            if (wordSetIterator != --m_word_set.end())
                            {
                                // delete the word and reset the letter index back to -1, ready for the next word
                                // -1 because the index will increment back up to 0
                                wordSetLetterIndex = -1;
                                wordSetIterator = m_word_set.erase(wordSetIterator);
                            }

                            else
                            {
                                // delete the word without exceeding the bounds of the word set and exit the letter index for loop
                                wordSetIterator = --m_word_set.erase(wordSetIterator);
                                break;
                            }
                        }
                    }
                }
            }

            if (gr->result[letterIndex] == Colour::YELLOW)
            {
                for (WordSet::iterator wordSetIterator = m_word_set.begin(); wordSetIterator != m_word_set.end();)
                {
                    if ((*wordSetIterator)[letterIndex] == gr->guess[letterIndex])
                    {
                        wordSetIterator = m_word_set.erase(wordSetIterator);
                    }

                    else
                    {
                        bool letterFound = false;

                        for (size_t wordSetLetterIndex = 0; wordSetLetterIndex < (*wordSetIterator).size(); wordSetLetterIndex++)
                        {
                            if ((*wordSetIterator)[wordSetLetterIndex] == gr->guess[letterIndex])
                            {
                                letterFound = true;
                                break;
                            }
                        }

                        if (letterFound)
                        {
                            ++wordSetIterator;
                        }

                        else
                        {
                            wordSetIterator = m_word_set.erase(wordSetIterator);
                        }
                    }
                }
            }
        }
    }

    // debugging
    //std::cout << "m_word_set size = " << m_word_set.size() << std::endl;

    // return a random item from our remaining word set

    WordSet mostVowelsWords;
    size_t maximumVowels = 0;
    std::string vowels;

    for (WordSet::iterator wordSetIterator = m_word_set.begin(); wordSetIterator != m_word_set.end(); ++wordSetIterator)
    {
        size_t vowelCount = 0;
        vowels = "AEIOU";

        for (size_t letterIndex = 0; letterIndex < (*wordSetIterator).size(); letterIndex++)
        {
            for (size_t vowelIndex = 0; vowelIndex < vowels.size();)
            {
                if ((*wordSetIterator)[letterIndex] == vowels[vowelIndex])
                {
                    vowelCount++;
                    vowels.erase(vowelIndex);
                }

                else
                {
                    vowelIndex++;
                }
            }

            if (vowelCount == maximumVowels)
            {
                mostVowelsWords.insert(*wordSetIterator);
            }

            else if (vowelCount > maximumVowels)
            {
                mostVowelsWords.clear();
                mostVowelsWords.insert(*wordSetIterator);
                maximumVowels = vowelCount;
            }
        }
    }

    size_t minimumDuplicateLetters = INT_MAX;
    WordSet nextWords;

    for (WordSet::iterator mostVowelsIterator = mostVowelsWords.begin(); mostVowelsIterator != mostVowelsWords.end(); ++mostVowelsIterator)
    {
        size_t duplicateLettersCount = 0;

        for (size_t letterIndex = 0; letterIndex < (*mostVowelsIterator).size(); letterIndex++)
        {
            for (size_t duplicateLetterIndex = 0; duplicateLetterIndex < (*mostVowelsIterator).size(); duplicateLetterIndex++)
            {
                if (letterIndex != duplicateLetterIndex && (*mostVowelsIterator)[letterIndex] == (*mostVowelsIterator)[duplicateLetterIndex])
                {
                    duplicateLettersCount++;
                }
            }
        }

        if (duplicateLettersCount == minimumDuplicateLetters)
        {
            nextWords.insert(*mostVowelsIterator);
        }

        if (duplicateLettersCount < minimumDuplicateLetters)
        {
            nextWords.clear();
            nextWords.insert(*mostVowelsIterator);
            minimumDuplicateLetters = duplicateLettersCount;
        }
    }



    int index = Random::GetRandomInt(0, static_cast<int>(nextWords.size() - 1));
    return *std::next(nextWords.begin(), index);
}
