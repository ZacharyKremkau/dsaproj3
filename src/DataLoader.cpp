// src/DataLoader.cpp
#include "DataLoader.h"
#include "Movie.h"
using namespace std;
#include <crow/json.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <algorithm>

// helper: lowercase a string
static string toLower(const string &s) {
    string out = s;
    transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c){ return tolower(c); });
    return out;
}

// trim whitespace
static string trim(const string &s) {
    auto b = s.find_first_not_of(" \t\r\n");
    auto e = s.find_last_not_of(" \t\r\n");
    return (b == string::npos) ? "" : s.substr(b, e - b + 1);
}

// the comma killer
static vector<string> splitCSV(const string &line) {
    vector<string> out;
    string cur;
    bool inQuotes = false;
    for (char c : line) {
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            out.push_back(cur);
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    out.push_back(cur);
    return out;
}

vector<Movie> DataLoader::loadMovies(
    const string &metaFile,
    const string &ratingsFile,
    const string &creditsFile
) {
    struct SumCount { double sum = 0; int count = 0; };
    unordered_map<int,SumCount> stats;
    {
        ifstream rf(ratingsFile);
        if (!rf) {
            cerr << "Cannot open ratings: " << ratingsFile << "\n";
            return {};
        }
        string line;
        getline(rf,line);
        while (getline(rf,line)) {
            stringstream ss(line);
            int u,m; double r;
            if (!(ss>>u)) continue;
            if (ss.peek()==',') ss.get();
            if (!(ss>>m)) continue;
            if (ss.peek()==',') ss.get();
            if (!(ss>>r)) continue;
            auto &sc = stats[m];
            sc.sum   += r;
            sc.count += 1;
        }
    }

    struct Crew { string director; vector<string> actors; };
    unordered_map<int,Crew> crewMap;
    {
        ifstream cf(creditsFile);
        if (!cf) {
            cerr << "Cannot open credits: " << creditsFile << "\n";
            return {};
        }
        string line;
        getline(cf,line);
        while (getline(cf,line)) {
            auto cols = splitCSV(line);
            if (cols.size()<3) continue;
            int id = stoi(trim(cols[0]));
            Crew c;
            c.director = trim(cols[1]);
            stringstream ss(cols[2]);
            string a;
            while (getline(ss,a,';')) {
                a = trim(a);
                if (!a.empty()) c.actors.push_back(a);
            }
            crewMap[id] = move(c);
        }
    }

    vector<Movie> out;
    ifstream mf(metaFile);
    if (!mf) {
        cerr << "Cannot open data files: " << metaFile << "\n";
        return out;
    }
    string line;
    getline(mf,line);
    while (getline(mf,line)) {
        auto cols = splitCSV(line);
        if (cols.size()<3) continue;

        int movieId = stoi(trim(cols[0]));
        string title = trim(cols[1]);

        double avgRating = 0;
        if (auto it=stats.find(movieId); it!=stats.end())
            avgRating = it->second.sum / it->second.count;

        vector<string> genres;
        auto raw = trim(cols[2]);
        if (!raw.empty()) {
            if (raw.front()=='[') {
                if (auto j=crow::json::load(raw); j && j.t()==crow::json::type::List) {
                    for (auto &item : j)
                        genres.push_back(item["name"].s());
                }
            } else {

                stringstream gs(raw);
                string g;
                while (getline(gs,g,'|')) {
                    g = trim(g);
                    if (!g.empty()) genres.push_back(g);
                }
            }
        }

        string director;
        vector<string> actors;
        if (auto it=crewMap.find(movieId); it!=crewMap.end()) {
            director = it->second.director;
            actors   = it->second.actors;
        }

        out.emplace_back(title, avgRating, genres, director, actors);
    }

    return out;
}

vector<Movie> DataLoader::filterMovies(
    const vector<Movie> &movies,
    const string &genre,
    const string &director,
    const string &actor
) {
    vector<Movie> out;
    auto lowG = toLower(genre);
    auto lowD = toLower(director);
    auto lowA = toLower(actor);

    for (auto &m : movies) {
        // genre
        if (!genre.empty()) {
            bool found=false;
            for (auto &g : m.genres)
                if (toLower(g)==lowG) { found=true; break; }
            if (!found) continue;
        }
        // director
        if (!director.empty() && toLower(m.director)!=lowD) continue;
        // actor
        if (!actor.empty()) {
            bool found=false;
            for (auto &a : m.actors)
                if (toLower(a)==lowA) { found=true; break; }
            if (!found) continue;
        }
        out.push_back(m);
    }
    return out;
}
