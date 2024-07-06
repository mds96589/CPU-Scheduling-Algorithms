#include <bits/stdc++.h>
#include "parser.h"
#define all(v) v.begin(), v.end()
using namespace std;

/** Global Constants **/
const string ALGORITHMS[7] = {"", "FCFS", "RR-", "SJF", "SRTN", "FB-1", "FB-2i"};

bool sortByServiceTime(const tuple<string, int, int> &a, const tuple<string, int, int> &b)
{
    return get<2>(a) < get<2>(b);
}
bool sortByArrivalTime(const tuple<string, int, int> &a, const tuple<string, int, int> &b)
{
    return get<1>(a) < get<1>(b);
}

void clear_timeline()
{
    for(int i = 0; i < last_instant; i++)
    {
        for(int j = 0; j < process_count; j++)
        {
            timeline[i][j] = ' ';
        }
    }

}

string getProcessName(tuple<string, int, int> &a)
{
    return get<0>(a);
}

int getArrivalTime(tuple<string, int, int> &a)
{
    return get<1>(a);
}

int getServiceTime(tuple<string, int, int> &a)
{
    return get<2>(a);
}

void fillInWaitTime()
{
    for (int i = 0; i < process_count; i++)
    {
        int arrivalTime = getArrivalTime(processes[i]);
        for (int k = arrivalTime; k < finishTime[i]; k++)
        {
            if (timeline[k][i] != '*')
            {
                timeline[k][i] = '.';
            }
        }
    }
}

void firstComeFirstServe()
{
    int time = getArrivalTime(processes[0]);
    for (int i = 0; i < process_count; i++)
    {
        int processIndex = i;
        int arrivalTime = getArrivalTime(processes[i]);
        int serviceTime = getServiceTime(processes[i]);

        finishTime[processIndex] = (time + serviceTime);
        turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
        waitingTime[processIndex] = (turnAroundTime[processIndex] - serviceTime);

        for (int j = time; j < finishTime[processIndex]; j++)
            timeline[j][processIndex] = '*';
        for (int j = arrivalTime; j < time; j++)
            timeline[j][processIndex] = '.';
        time += serviceTime;
    }
}

void roundRobin(int originalQuantum)
{
    queue<pair<int,int>> q;
    int j = 0;
    if(getArrivalTime(processes[j]) == 0)
    {
        q.push(make_pair(j, getServiceTime(processes[j])));
        j++;
    }
    int currentQuantum = originalQuantum;
    for(int time = 0; time < last_instant; time++){
        if(!q.empty())
        {
            int processIndex = q.front().first;
            q.front().second = q.front().second-1;
            int remainingServiceTime = q.front().second;
            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getServiceTime(processes[processIndex]);
            currentQuantum--;
            timeline[time][processIndex]='*';
            while(j < process_count && getArrivalTime(processes[j]) == time + 1)
            {
                q.push(make_pair(j, getServiceTime(processes[j])));
                j++;
            }

            if(currentQuantum == 0 && remainingServiceTime == 0)
            {
                finishTime[processIndex] = time+1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                waitingTime[processIndex] = (turnAroundTime[processIndex] - serviceTime);
                currentQuantum = originalQuantum;
                q.pop();
            }
            else if(currentQuantum == 0 && remainingServiceTime != 0)
            {
                q.pop();
                q.push(make_pair(processIndex, remainingServiceTime));
                currentQuantum = originalQuantum;
            }
            else if(currentQuantum !=0 && remainingServiceTime == 0)
            {
                finishTime[processIndex] = time+1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                waitingTime[processIndex] = (turnAroundTime[processIndex] - serviceTime);
                q.pop();
                currentQuantum = originalQuantum;
            }
        }
        while(j < process_count && getArrivalTime(processes[j]) == time+1)
        {
            q.push(make_pair(j, getServiceTime(processes[j])));
            j++;
        }
    }
    fillInWaitTime();
}

void shortestJobFirst()
{
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq; // pair of service time and index
    int j = 0;
    for (int i = 0; i < last_instant; i++)
    {
        while(j < process_count && getArrivalTime(processes[j]) <= i){
            pq.push(make_pair(getServiceTime(processes[j]), j));
            j++;
        }
        if (!pq.empty())
        {
            int processIndex = pq.top().second;
            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getServiceTime(processes[processIndex]);
            pq.pop();

            int temp = arrivalTime;
            for (; temp < i; temp++)
                timeline[temp][processIndex] = '.';

            temp = i;
            for (; temp < i + serviceTime; temp++)
                timeline[temp][processIndex] = '*';

            finishTime[processIndex] = (i + serviceTime);
            turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
            waitingTime[processIndex] = (turnAroundTime[processIndex] - serviceTime);
            i = temp - 1;
        }
    }
}

void shortestRemainingTimeNext()
{
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    int j = 0;
    for (int i = 0; i < last_instant; i++)
    {
        while(j < process_count &&getArrivalTime(processes[j]) == i){
            pq.push(make_pair(getServiceTime(processes[j]), j));
            j++;
        }
        if (!pq.empty())
        {
            int processIndex = pq.top().second;
            int remainingTime = pq.top().first;
            pq.pop();
            int serviceTime = getServiceTime(processes[processIndex]);
            int arrivalTime = getArrivalTime(processes[processIndex]);
            timeline[i][processIndex] = '*';

            if (remainingTime == 1) // process finished
            {
                finishTime[processIndex] = i + 1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                waitingTime[processIndex] = (turnAroundTime[processIndex] - serviceTime);
            }
            else
            {
                pq.push(make_pair(remainingTime - 1, processIndex));
            }
        }
    }
    fillInWaitTime();
}

void feedbackQ1()
{
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq; //pair of priority level and process index
    unordered_map<int, int> remainingServiceTime; //map from process index to the remaining service time
    int j = 0;
    if(getArrivalTime(processes[0]) == 0)
    {
        pq.push(make_pair(0, j));
        remainingServiceTime[j] = getServiceTime(processes[j]);
        j++;
    }
    for(int time = 0;time < last_instant; time++)
    {
        if(!pq.empty())
        {
            int priorityLevel = pq.top().first;
            int processIndex =pq.top().second;
            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getServiceTime(processes[processIndex]);
            pq.pop();
            while(j < process_count && getArrivalTime(processes[j]) == time+1)
            {
                    pq.push(make_pair(0, j));
                    remainingServiceTime[j] = getServiceTime(processes[j]);
                    j++;
            }
            remainingServiceTime[processIndex]--;
            timeline[time][processIndex] ='*';
            if(remainingServiceTime[processIndex] == 0)
            {
                finishTime[processIndex] = time + 1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                waitingTime[processIndex] = (turnAroundTime[processIndex] - serviceTime);
            }
            else
            {
                if(pq.size() >= 1)
                    pq.push(make_pair(priorityLevel + 1, processIndex));
                else
                    pq.push(make_pair(priorityLevel, processIndex));
            }
        }
        while(j < process_count && getArrivalTime(processes[j]) == time+1){
                pq.push(make_pair(0, j));
                remainingServiceTime[j] = getServiceTime(processes[j]);
                j++;
        }
    }
    fillInWaitTime();
}

void feedbackQ2i()
{
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq; //pair of priority level and process index
    unordered_map<int, int>remainingServiceTime; //map from process index to the remaining service time
    int j = 0;
    if(getArrivalTime(processes[0]) == 0)
    {
        pq.push(make_pair(0, j));
        remainingServiceTime[j] = getServiceTime(processes[j]);
        j++;
    }
    for(int time = 0;time < last_instant; time++)
    {
        if(!pq.empty())
        {
            int priorityLevel = pq.top().first;
            int processIndex = pq.top().second;
            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getServiceTime(processes[processIndex]);
            pq.pop();
            while(j < process_count && getArrivalTime(processes[j]) <= time+1)
            {
                    pq.push(make_pair(0, j));
                    remainingServiceTime[j] = getServiceTime(processes[j]);
                    j++;
            }

            int currentQuantum = pow(2,priorityLevel);
            int temp = time;
            while(currentQuantum && remainingServiceTime[processIndex])
            {
                currentQuantum--;
                remainingServiceTime[processIndex]--;
                timeline[temp++][processIndex] = '*';
            }

            if(remainingServiceTime[processIndex] == 0)
            {
                finishTime[processIndex] = temp;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                waitingTime[processIndex] = (turnAroundTime[processIndex] - serviceTime);
            }
            else
            {
                if(pq.size() >= 1)
                    pq.push(make_pair(priorityLevel + 1, processIndex));
                else
                    pq.push(make_pair(priorityLevel, processIndex));
            }
            time = temp - 1;
        }
        while(j < process_count && getArrivalTime(processes[j]) <= time+1)
        {
                pq.push(make_pair(0, j));
                remainingServiceTime[j] = getServiceTime(processes[j]);
                j++;
        }
    }
    fillInWaitTime();
}

void printProcesses()
{
    cout << "Process    ";
    for (int i = 0; i < process_count; i++)
        cout << "|  " << getProcessName(processes[i]) << "  ";
    cout << "|\n";
}
void printArrivalTime()
{
    cout << "Arrival    ";
    for (int i = 0; i < process_count; i++)
        printf("|%3d  ",getArrivalTime(processes[i]));
    cout<<"|\n";
}
void printServiceTime()
{
    cout << "Service    |";
    for (int i = 0; i < process_count; i++)
        printf("%3d  |",getServiceTime(processes[i]));
    cout << " Mean|\n";
}
void printFinishTime()
{
    cout << "Finish     ";
    for (int i = 0; i < process_count; i++)
        printf("|%3d  ",finishTime[i]);
    cout << "|-----|\n";
}
void printTurnAroundTime()
{
    cout << "Turnaround |";
    int sum = 0;
    for (int i = 0; i < process_count; i++)
    {
        printf("%3d  |",turnAroundTime[i]);
        sum += turnAroundTime[i];
    }
    if((1.0 * sum / turnAroundTime.size())>=10)
		printf("%2.2f|\n",(1.0 * sum / turnAroundTime.size()));
    else
	 	printf(" %2.2f|\n",(1.0 * sum / turnAroundTime.size()));
}

void printWaitingTime()
{
    cout << "WaitTime   |";
    int sum = 0;
    for (int i = 0; i < process_count; i++)
    {
        printf("%3d  |", waitingTime[i]);
        sum += waitingTime[i];
    }

    if( (1.0 * sum / waitingTime.size()) >=10 )
        printf("%2.2f|\n",(1.0 * sum / waitingTime.size()));
	else
        printf(" %2.2f|\n",(1.0 * sum / waitingTime.size()));
}
void printStats(int algorithm_index)
{
    printProcesses();
    printArrivalTime();
    printServiceTime();
    printFinishTime();
    printTurnAroundTime();
    printWaitingTime();
}

void printTimeline(int algorithm_index)
{
    for (int i = 0; i <= last_instant; i++)
        cout << i % 10<<" ";
    cout <<"\n";
    cout << "------------------------------------------------\n";
    for (int i = 0; i < process_count; i++)
    {
        cout << getProcessName(processes[i]) << "     |";
        for (int j = 0; j < last_instant; j++)
        {
            cout << timeline[j][i]<<"|";
        }
        cout << " \n";
    }
    cout << "------------------------------------------------\n";
}

void execute_algorithm(char algorithm_id, int quantum)
{
    switch (algorithm_id)
    {
    case '1':
        cout<<"FCFS  ";
        firstComeFirstServe();
        break;
    case '2':
        cout<<"RR-"<<quantum<<"  ";
        roundRobin(quantum);
        break;
    case '3':
        cout<<"SJF   ";
        shortestJobFirst();
        break;
    case '4':
        cout<<"SRTN   ";
        shortestRemainingTimeNext();
        break;
    case '5':
        cout<<"FB-1  ";
        feedbackQ1();
        break;
    case '6':
        cout<<"FB-2i ";
        feedbackQ2i();
        break;
    default:
        break;
    }
}