#include <random>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

#define NUMBER_OF_FRAMES 1000
#define DESIRED_STRING_LENGTH 100000
#define RATE_OF_MOTION 200
#define START_LOCATION 0
#define VIRTUAL_MEMORY_SIZE 1048576 // 2^20 

using namespace std;

/*
    Generates a reference string given parameters of desired size.

    @param p virtual memory size
    @param s starting location
    @param e size of L
    @param m rate of motion
    @param t virtual memory size
    @param size desired length of reference string
    @return returns the generated reference string, an array of integers
*/
vector<int> generateRS(int P, int s, int e, int m, double t, int size)
{
    vector<int> rs;
    while (rs.size() < size)
    {
        random_device rdi;
        mt19937 rng(rdi());
        uniform_int_distribution<mt19937::result_type> disti(s, s + e);
        for (int i = 0; i < m; ++i)
        {
            rs.emplace_back(disti(rng));
        }

        random_device rdf;
        default_random_engine eng(rdf());
        uniform_real_distribution<float> distf(0, 1);
        float r = distf(eng);

        if (r < t)
        {
            random_device rdis;
            mt19937 rngs(rdis());
            uniform_int_distribution<mt19937::result_type> distis(s, P - 1);
            s = distis(rngs);
        }
        else
        {
            ++s;
            s %= P;
        }
    }
    return rs;
}
/*
    Processes a reference string using one of optimal, FIFO, LRU algorithms.
    Returns the number of page faults occurred.

    @param rs reference string to be processed
    @param frames the frames containing resident pages
    @return number of page faults occurred.
*/
int algo1(const vector<int>& rs, vector<int> frames)
{
    return 1;
}

/*
    Processes a reference string using one of optimal, FIFO, LRU algorithms.
    Returns the number of page faults occurred.

    @param rs reference string to be processed
    @param frames the frames containing resident pages
    @return number of page faults occurred.
*/
int optimalPageReplacementAlgorithm(const vector<int>& rs, vector<int> frames)
{
    int nFaults = 0;
    return nFaults;
}

int main()
{

    vector<vector<int>> RS_set;
    vector<int> e_set = { 5, 10, 20, 50, 100, 1000, 3000 };
    vector<float> t_set = { 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0 };
    unordered_map<int, int> page_faults1;
    unordered_map<int, int> page_faults2;

    for (auto e : e_set)
    {
        for (auto t : t_set)
        {
            RS_set.emplace_back(generateRS(VIRTUAL_MEMORY_SIZE,
                START_LOCATION, e, RATE_OF_MOTION, t, DESIRED_STRING_LENGTH));
        }
    }

    // frame initialization
    vector<int> frames(NUMBER_OF_FRAMES);
    int resident_page = 0;
    for (auto& frame : frames)
    {
        frame = resident_page;
        int frameSize = VIRTUAL_MEMORY_SIZE / NUMBER_OF_FRAMES;
        resident_page += frameSize;
    }

    // implement and run 2 page replacement algorithms.
    int i = 0;
    for (const auto& rs : RS_set)
    {
        int e = e_set[i];
        page_faults1[e] += algo1(rs, frames);
        page_faults2[e] += optimalPageReplacementAlgorithm(rs, frames);
        i = (i + 1) % 7;
    }

    // print results and plot by hand.
    cout << "Algorithm 1\n";
    for (auto e : e_set)
    {
        cout << "e = " << e << ", faults = " << page_faults1[e] << "\n";
    }
    cout << "\nOptimal Page Replacement Algorithm\n";
    for (auto e : e_set)
    {
        cout << "e = " << e << ", faults = " << page_faults2[e] << "\n";
    }
}