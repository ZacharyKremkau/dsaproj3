#ifndef DATALOADER_H
#define DATALOADER_H

#include "Movie.h"
#include <vector>
#include <string>
using namespace std;

class DataLoader {
public:
    static vector<Movie> loadMovies(
        const string &metaFile,
        const string &ratingsFile,
        const string &creditsFile
    );

    static vector<Movie> filterMovies(
        const vector<Movie> &movies,
        const string &genre = "",
        const string &director = "",
        const string &actor = ""
    );
};

#endif // DATALOADER_H
