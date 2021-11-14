#include <random>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <vector>
#include <climits>
#include <iostream>

#define NUMBER_OF_FRAMES 256 // 2^8
#define DESIRED_STRING_LENGTH 50000
#define RATE_OF_MOTION 50
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
    @return the generated reference string, an array of integers
*/
vector<int> generateRS(int P, int s, int e, int m, double t, int size)
{
    vector<int> rs;
    while (rs.size() < size)
    {
        random_device rdi;
        mt19937 rng(rdi());
        uniform_int_distribution<mt19937::result_type> disti(s, min(P, s + e) );
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
    for (int i = 0; i < nPages; i++)
    {
        // Grab the current page from the RS
        int currentPage = rs.at(i);
        // Check if we are within our number of frames
        if (residentPageSet.size() < nFrames)
        {
            // Is the page resident?
            if (residentPageSet.find(currentPage) == residentPageSet.end())
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
        else
        {
            // Is the page resident?
            if (residentPageSet.find(currentPage) == residentPageSet.end())
            {
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
    for (int i = 0; i < nPages; i++)
    {
        // Grab the current page from the RS
        int currentPage = rs.at(i);
        // Check if we are within our number of frames
        if (residentPages.size() < nFrames)
        {
            // Is the page resident?
            if (residentPages.find(currentPage) == residentPages.end())
            {
                // Page is not already resident, so add it to the set
                residentPages.insert(currentPage);
                // Increase the number of faults
                nFaults++;
            }
            // Add the page number to our numbersOfRS map
            numbersOfRS[currentPage] = i;
        }

        // When all frames are full and one needs to be removed
        else
        {
            // Is the page resident?
            if (residentPages.find(currentPage) == residentPages.end())
            {
                // Set initial lru value to large int for comparison
                int lru = INT_MAX;
                // The page number of the lru page
                int val;
                // Go through all resident pages
                for (auto it = residentPages.begin(); it != residentPages.end(); it++)
                {
                    // Get value of lru page
                    if (numbersOfRS[*it] < lru)
                    {
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

int main()
{
    vector<vector<int>> RS_set;
    vector<int> e_set = { 100, 130, 160, 200, 230, 260, 300};
    vector<float> t_set = { 0.01, 0.02, 0.03, 0.04, 0.05 };
    unordered_map<int, int> page_faults_FIFO;
    unordered_map<int, int> page_faults_LRU;

    // frame initialization
    vector<int> frames(NUMBER_OF_FRAMES);
    int resident_page = 0;
    for (auto& frame : frames)
    {
        frame = resident_page;
        int frameSize = VIRTUAL_MEMORY_SIZE / NUMBER_OF_FRAMES;
        resident_page += frameSize;
    }

    for (auto e : e_set)
    {
        for (auto t : t_set)
        {
            auto rs = generateRS(VIRTUAL_MEMORY_SIZE,
                START_LOCATION, e, RATE_OF_MOTION, t, DESIRED_STRING_LENGTH);
            int nFaultsFIFO = firstInLastOutAlgorithm(rs, frames);
            int nFaultsLRU  = leastRecentlyUsedAlgorithm(rs, frames);
            page_faults_FIFO[e] += nFaultsFIFO;
            page_faults_LRU[e]  += nFaultsLRU;
        }
    }

    // print results and plot by hand.
    cout << "First In, First Out Algorithm (varying e)\n";
    for (auto e : e_set)
    {
        cout << "e = " << e << ", faults = " << page_faults_FIFO[e] << "\n";
    }
    cout << "\nLeast Recently Used Algorithm (varying e)\n";
    for (auto e : e_set)
    {
        cout << "e = " << e << ", faults = " << page_faults_LRU[e] << "\n";
    }
}