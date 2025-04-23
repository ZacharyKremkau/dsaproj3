
#include <crow/crow.h>
#include <crow/json.h>
#include "DataLoader.h"
#include "SuggestionEngine.h"
#include "Heap.h"
#include "Movie.h"
using namespace std;
#include <fstream>
#include <iostream>
#include <map>
#include <functional>

// simple file‐loader for our static assets
static string loadFile(const string &path) {
    ifstream in(path, ios::binary);
    return { istreambuf_iterator<char>(in),
             istreambuf_iterator<char>() };
}

int main() {
    // 0) load ALL movies once from the small‑ml folder
    auto movies = DataLoader::loadMovies(
        "data/ml-latest-small/movies.csv",
        "data/ml-latest-small/ratings.csv",
        "data/ml-latest-small/credits.csv"
    );
    if (movies.empty()) {
        cerr << "No movies loaded so check your data files\n";
    }

    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([] {
        auto body = loadFile("public/index.html");
        crow::response res{body};
        res.set_header("Content-Type","text/html");
        return res;
    });

    CROW_ROUTE(app, "/static/<string>")([](const string &fn){
        auto body = loadFile("public/" + fn);
        crow::response res{body};
        if (fn.ends_with(".js"))  res.set_header("Content-Type","application/javascript");
        else if (fn.ends_with(".css")) res.set_header("Content-Type","text/css");
        return res;
    });

   //autosuggest
    CROW_ROUTE(app, "/suggest")
    .methods("GET"_method)
    ([&](const crow::request &req, crow::response &res){
        const char *c = req.url_params.get("category");
        const char *p = req.url_params.get("prefix");
        string category = c ? c : "";
        string prefix   = p ? p : "";

        auto all      = getSuggestions(movies, category);
        auto filtered = filterSuggestions(all, prefix);

        crow::json::wvalue arr;
        for (size_t i = 0; i < filtered.size(); ++i)
            arr[i] = filtered[i];

        res.set_header("Content-Type","application/json");
        res.write(arr.dump());
        res.end();
    });

   //main search endpoint
    CROW_ROUTE(app, "/search")
    .methods("GET"_method)
    ([&](const crow::request &req, crow::response &res){
        const char *g = req.url_params.get("genre");
        const char *a = req.url_params.get("actor");
        const char *d = req.url_params.get("director");
        const char *o = req.url_params.get("order");
        string genre    = g ? g : "";
        string actor    = a ? a : "";
        string director = d ? d : "";
        string order    = o ? o : "heap";

        // note for later jackson; filterMovies signature is (movies, genre, director, actor)
        auto filtered = DataLoader::filterMovies(movies, genre, director, actor);

        crow::json::wvalue result;
        if (order == "heap") {
            MovieHeap heap{[](const Movie &x, const Movie &y){
                return x.rating < y.rating;
            }};
            for (auto &m : filtered) heap.push(m);

            int i = 0;
            while (!heap.empty()) {
                auto mv = heap.top(); heap.pop();
                result["movies"][i]["title"]  = mv.title;
                result["movies"][i]["rating"] = mv.rating;
                ++i;
            }
        } else {
            multimap<double,Movie,greater<>> mmap;
            for (auto &m : filtered) mmap.emplace(m.rating,m);

            int i = 0;
            for (auto &p : mmap) {
                result["movies"][i]["title"]  = p.second.title;
                result["movies"][i]["rating"] = p.first;
                ++i;
            }
        }

        res.set_header("Content-Type","application/json");
        res.write(result.dump());
        res.end();
    });

    //spin up
    app.port(18080).multithreaded().run();
    return 0;
}
