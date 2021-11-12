#include <random>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <vector>
#include <climits>
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
    Processes a reference string using FIFO algorithm, 
    which removes the page that has been resident in memory for the longest time.

    @param rs reference string to be processed
    @param frames the frames containing resident pages
    @return number of page faults occurred.
*/
int firstInLastOutAlgorithm(const vector<int>& rs, vector<int> frames)
{
    // Number of Page Faults Found
    int nFaults = 0;
    // Number of Pages
    int nPages = rs.size();
    // Number of Frames
    int nFrames = frames.size();
    // An unordered set of our resident pages currently in memory
    unordered_set<int> residentPageSet;
    // A FIFO queue data structure to track pages
    queue<int> fifoPages;

    // Go through all pages in the RS
    for(int i = 0; i < nPages; i++){
        // Grab the current page from the RS
        int currentPage = rs.at(i);
        // Check if we are within our number of frames
        if(residentPageSet.size() < nFrames){
            // Is the page resident?
            if(residentPageSet.find(currentPage)==residentPageSet.end())
            {
                // Page is not already resident, so add it to the set
                residentPageSet.insert(currentPage);
                // Increase the number of faults
                nFaults++;
                // Add this new page to the end of our queue
                fifoPages.push(currentPage);
            }
        }
        // When all frames are full and one needs to be removed
        else {
            // Is the page resident?
            if(residentPageSet.find(currentPage)==residentPageSet.end()){
                // Page is not already resident
                // Save the resident page that was first in the queue
                int firstIn = fifoPages.front();
                // Remove it from queue
                fifoPages.pop();
                // Remove it from our resident page set
                residentPageSet.erase(firstIn);
                // Add in our current non resident page
                residentPageSet.insert(currentPage);
                // Add it to the queue
                fifoPages.push(currentPage);
                // Increase the number of faults
                nFaults++;
            }
        }
    }
    return nFaults;
}

/*
    Processes a reference string using LRU algorithm, 
    which selects the page that has not been referenced for the longest time.
    Returns the number of page faults occurred.

    @param rs reference string to be processed
    @param frames the frames containing resident pages
    @return number of page faults occurred.
*/
int leastRecentlyUsedAlgorithm(const vector<int>& rs, vector<int> frames)
{
    // Number of Page Faults Found
    int nFaults = 0;
    // Number of Pages
    int nPages = rs.size();
    // Number of Frames
    int nFrames = frames.size();
    // An unordered set of our resident pages currently in memory
    unordered_set<int> residentPages;
    // An unordered map of the least recently used pages in memory
    unordered_map<int, int> numbersOfRS;
 
    // Go through all pages in the RS
    for (int i = 0; i < nPages; i++){
        // Grab the current page from the RS
        int currentPage = rs.at(i);
        // Check if we are within our number of frames
        if (residentPages.size() < nFrames){
            // Is the page resident?
            if (residentPages.find(currentPage)==residentPages.end()){
                // Page is not already resident, so add it to the set
                residentPages.insert(currentPage);
                // Increase the number of faults
                nFaults++;
            }
            // Add the page number to our numbersOfRS map
            numbersOfRS[currentPage] = i;
        }
 
        // When all frames are full and one needs to be removed
        else{
            // Is the page resident?
            if (residentPages.find(currentPage) == residentPages.end()){
                // Set initial lru value to large int for comparison
                int lru = INT_MAX;
                // The page number of the lru page
                int val;
                // Go through all resident pages
                for (auto it=residentPages.begin(); it!=residentPages.end(); it++){
                    // Get value of lru page
                    if (numbersOfRS[*it] < lru){
                        lru = numbersOfRS[*it];
                        val = *it;
                    }
                }
                // Remove the lru page from our resident page set
                residentPages.erase(val);
                // Add in our current non resident page
                residentPages.insert(currentPage);
                // Increment page faults
                nFaults++;
            }
            // Increase the number of faults
            numbersOfRS[currentPage] = i;
        }
    }
    return nFaults;
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
        page_faults1[e] += firstInLastOutAlgorithm(rs, frames);
        page_faults2[e] += leastRecentlyUsedAlgorithm(rs, frames);
        i = (i + 1) % 7;
    }

    // print results and plot by hand.
    cout << "First In, First Out Algorithm\n";
    for (auto e : e_set)
    {
        cout << "e = " << e << ", faults = " << page_faults1[e] << "\n";
    }
    cout << "\nLeast Recently Used Algorithm\n";
    for (auto e : e_set)
    {
        cout << "e = " << e << ", faults = " << page_faults2[e] << "\n";
    }
}