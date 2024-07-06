#include <bits/stdc++.h>
#include "parser.h"
#include "methods.h"

int main()
{
    parse();
    for (int idx = 0; idx < (int)algorithms.size(); idx++)
    {
        clear_timeline();
        execute_algorithm(algorithms[idx].first, algorithms[idx].second);
        printTimeline(idx);
        printStats(idx);
        cout << "\n";
    }
    return 0;
}