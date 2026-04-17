#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <condition_variable>
#include <chrono>
#include <atomic>
#include <string>
#include <semaphore>
#include <cstdlib>
#include <ctime>

using namespace std;

constexpr int numtellers = 3;
constexpr int numcustomers = 50;

// Shared semaphores
counting_semaphore<2> doorSem(2);
counting_semaphore<1> managerSem(1);
counting_semaphore<2> safeSem(2);
counting_semaphore<numcustomers> customerReady(0);

mutex bankMutex;
condition_variable bankCV;
bool bankIsOpen = false;
int tellerReadyCount = 0;

mutex coutMutex;
mutex queueMutex;
mutex servedMutex;

int servedCustomers = 0;
atomic<bool> bankClosed(false);
queue<int> customerQueue;

binary_semaphore* customerAssigned[numcustomers];
binary_semaphore* customerDone[numcustomers];

binary_semaphore* tellerAskSem[numtellers];
binary_semaphore* customerTellSem[numtellers];
binary_semaphore* customerLeftSem[numtellers];

int assignedTeller[numcustomers];
string transactionType[numcustomers];

void logline(const string& actorType, int actorId, const string& targetType, int targetId, const string& message)
{
    lock_guard<mutex> lk(coutMutex);
    cout << actorType << ' ' << actorId << " [" << targetType << ' ' << targetId << "]: "
         << message << '\n';
}

void logresource(const string& actorType, int actorId,const string& resource, const string& message)
{
    lock_guard<mutex> lk(coutMutex);
    cout << actorType << ' ' << actorId << " [" << resource << "]: "
         << message << '\n';
}

void customer(int id)
{
    logline("Customer", id, "Customer", id, "created");
}

void teller(int id)
{
    logline("Teller", id, "Teller", id, "ready to serve");
}

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));

    for (int i = 0; i < numcustomers; ++i) 
    {
        customerAssigned[i] = new binary_semaphore(0);
        customerDone[i] = new binary_semaphore(0);
    }

    for (int i = 0; i < numtellers; ++i) 
    {
        tellerAskSem[i] = new binary_semaphore(0);
        customerTellSem[i] = new binary_semaphore(0);
        customerLeftSem[i] = new binary_semaphore(0);
    }

    vector<thread> tellers;
    vector<thread> customers;

    for (int i = 1; i <= numtellers; ++i) 
    {
        tellers.emplace_back(teller, i);
    }
    for (int i = 0; i < numcustomers; ++i) 
    {
        customers.emplace_back(customer, i);
    }

    for (auto& c : customers) 
    {
        c.join();
    }
    for (auto& t : tellers) 
    {
        t.join();
    }

    for (int i = 0; i < numcustomers; ++i) 
    {
        delete customerAssigned[i];
        delete customerDone[i];
    }
    for (int i = 0; i < numtellers; ++i) 
    {
        delete tellerAskSem[i];
        delete customerTellSem[i];
        delete customerLeftSem[i];
    }

    return 0;
}