#include "era/config/parser.h"

#include <fstream>
#include <iostream>
#include <stdlib.h>

// @TODO parse arrays

namespace era
{

namespace config
{

// ----------------------------------------------------------------------------------------------------

char readWhitespace(char c, std::istream& in, int& num_spaces)
{
    do
    {
        if (c == ' ')
            num_spaces += 1;
        else if (c == '\t')
            num_spaces += 4;
        else
            return c;
    } while(in.read(&c, 1));

    return c;
}

// ----------------------------------------------------------------------------------------------------

Variant parseVariant(const std::string& str)
{
    // Try to parse to integer
    char* pEnd;
    int i = strtol(str.c_str(), &pEnd, 10);
    if (pEnd[0] == 0)
        return i;

    // Try to parse to double
    double d = strtod(str.c_str(), &pEnd);
    if (pEnd[0] == 0)
        return d;

    return str;
}

// ----------------------------------------------------------------------------------------------------

Parser::Parser(Data& data) : writer_(data)
{
}

// ----------------------------------------------------------------------------------------------------

Parser::~Parser()
{
}

// ----------------------------------------------------------------------------------------------------

bool Parser::readFile(const std::string& filename)
{
    std::ifstream in;
    in.open(filename.c_str());

    if (!in.is_open())
        return false;

    bool success = readStream(in);

    in.close();

    return success;
}

// ----------------------------------------------------------------------------------------------------

bool Parser::readStream(std::istream& in)
{
    bool read_quoted = false;

    std::string word;
    bool word_quoted = false;
    std::string key;

    State state = READ_KEY;
    int num_start_spaces = 0;
    int current_num_spaces = 0;
    int line = 1;

    // 'levels' keeps track of which indent (number of starting spaces) belongs to
    // which configuration depth: levels[#spaces] -> config level
    int level = 0;
    std::vector<int> levels;
    levels.push_back(level);

    std::string error;

    char c;
    while(in.read(&c, 1))
    {
        if (c != '"' && read_quoted)
        {
            if (c == '\n')
                ++line;

            word += c;
            continue;
        }

        if (c == '#')
            state = READ_COMMENT;

        if (state == READ_COMMENT && c != '\n')
            continue;

        State new_state = state;

        if (state == READ_NEWLINE_WHITESPACE)
        {
            num_start_spaces = 0;
            c = readWhitespace(c, in, num_start_spaces);
            new_state = READ_KEY;
        }

        bool token_end = true;

        switch(c)
        {
        case '"':
            read_quoted = !read_quoted;
            if (read_quoted)
                word_quoted = true;
            break;
        case '\n':
            new_state = READ_NEWLINE_WHITESPACE;
            ++line;
        case ':':
            break;
        case '=':
            break;
        case ' ':
            break;
        case '\t':
            break;
        case '{':
            new_state = READ_KEY;
            break;
        case '}':
            --level;
            writer_.endGroup();
            break;
        case ',':
            new_state = READ_KEY;
            break;
        default:
            word += c;
            token_end = false;
        }

        if (token_end && ! word.empty())
        {
            if (state == READ_KEY)
            {
                if (!key.empty())
                {
                    ++level;
                    writer_.writeGroup(key);

                    if (num_start_spaces >= 0 && num_start_spaces > current_num_spaces)
                    {
                        levels.resize(num_start_spaces + 1);
                        for(unsigned int i = current_num_spaces + 1; i < num_start_spaces; ++i)
                            levels[i] = -1;
                        levels[num_start_spaces] = level;
                    }
                }

                if (num_start_spaces >= 0)
                {
                    if (num_start_spaces < current_num_spaces)
                    {
                        int new_level = levels[num_start_spaces];
                        if (new_level == -1)
                        {
                            error = "Invalid indent level";
                        }
                        else
                        {
                            for(; level > new_level; --level)
                                writer_.endGroup();
                        }
                    }

                    current_num_spaces = num_start_spaces;
                }

                key = word;
            }
            else if (state == READ_VALUE)
            {
                if (word_quoted)
                    writer_.setValue(key, word); // string
                else
                    writer_.setValue(key, parseVariant(word));

                key.clear();
            }

            if (state == READ_KEY)
                new_state = READ_VALUE;

            word.clear();
            word_quoted = false;
        }

        state = new_state;

        if (!error.empty())
        {
            std::cout << "Error at line " << line << ": " << error << std::endl;
            return false;
        }

    }

    // End open groups
    for(; level > 0; --level)
        writer_.endGroup();

    return true;
}

// ----------------------------------------------------------------------------------------------------

bool Parser::ok() const
{
    return true;
}


} // end namespace configuration

}

