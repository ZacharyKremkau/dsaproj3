#ifndef MOVIE_H
#define MOVIE_H

#include <string>
#include <vector>

struct Movie {
    std::string title;
    double rating;
    std::vector<std::string> genres;
    std::string director;
    std::vector<std::string> actors;

    Movie(const std::string &t, double r,
          const std::vector<std::string> &g,
          const std::string &d,
          const std::vector<std::string> &a)
      : title(t), rating(r), genres(g), director(d), actors(a) {}
};

#endif // MOVIE_H
