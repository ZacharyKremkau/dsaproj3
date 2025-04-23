#ifndef HEAP_H
#define HEAP_H
using namespace std;
#include "Movie.h"
#include <queue>
#include <vector>
#include <functional>


using MovieCompare = function<bool(const Movie&, const Movie&)>;

struct MovieHeap {
    priority_queue<Movie,
                        vector<Movie>,
                        MovieCompare> pq;
    MovieHeap(MovieCompare cmp)
      : pq(cmp) {}

    void push(const Movie &m) { pq.push(m); }
    const Movie& top()    const { return pq.top(); }
    void pop()                  { pq.pop(); }
    bool empty()         const { return pq.empty(); }
};

#endif // HEAP_H
