#ifndef SUGGESTIONENGINE_H
#define SUGGESTIONENGINE_H
using namespace std;
#include "Movie.h"
#include <vector>
#include <string>
#include <set>
#include <algorithm>

//lowercase handla
static string toLower(const string &s) {
    string out = s;
    transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c){ return tolower(c); });
    return out;
}

inline vector<string> getSuggestions(
    const vector<Movie> &movies,
    const string &category)
{
    set<string> s;
    if (category == "genre") {
        for (auto &m : movies)
            for (auto &g : m.genres)
                s.insert(g);
    } else if (category == "director") {
        for (auto &m : movies)
            if (!m.director.empty())
                s.insert(m.director);
    } else if (category == "actor") {
        for (auto &m : movies)
            for (auto &a : m.actors)
                s.insert(a);
    }
    return {s.begin(), s.end()};
}

inline vector<string> filterSuggestions(
    const vector<string> &all,
    const string &prefix)
{
    vector<string> out;
    auto lowp = toLower(prefix);
    for (auto &cand : all) {
        // compare lowercase prefixes
        if (toLower(cand).rfind(lowp, 0) == 0)
            out.push_back(cand);
    }
    return out;
}

#endif // SUGGESTIONENGINE_H
