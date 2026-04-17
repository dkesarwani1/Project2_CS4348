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

constexpr int NUM_TELLERS = 3;
constexpr int NUM_CUSTOMERS = 50;

// Doorway capacity: only two customers may pass through the door at once.
counting_semaphore<2> doorSem(2);
counting_semaphore<1> managerSem(1);
counting_semaphore<2> safeSem(2);
counting_semaphore<NUM_CUSTOMERS> customerReady(0);

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

binary_semaphore* customerAssigned[NUM_CUSTOMERS];
binary_semaphore* customerDone[NUM_CUSTOMERS];

binary_semaphore* tellerAskSem[NUM_TELLERS];
binary_semaphore* customerTellSem[NUM_TELLERS];
binary_semaphore* customerLeftSem[NUM_TELLERS];

int assignedTeller[NUM_CUSTOMERS];
string transactionType[NUM_CUSTOMERS];

void logLine(const string& actorType, int actorId,
             const string& targetType, int targetId,
             const string& message)
{
    lock_guard<mutex> lk(coutMutex);
    cout << actorType << ' ' << actorId << " [" << targetType << ' ' << targetId << "]: "
         << message << '\n';
}

void logResource(const string& actorType, int actorId,
                 const string& resource,
                 const string& message)
{
    lock_guard<mutex> lk(coutMutex);
    cout << actorType << ' ' << actorId << " [" << resource << "]: "
         << message << '\n';
}

void passThroughDoor(int customerId, const string& actionMessage)
{
    logLine("Customer", customerId, "Customer", customerId, "waiting to use door");
    doorSem.acquire();
    logLine("Customer", customerId, "Customer", customerId, actionMessage);
    this_thread::sleep_for(chrono::milliseconds(1));
    doorSem.release();
}

void customer(int id)
{
    logLine("Customer", id, "Customer", id, "created");
}

void teller(int id)
{
    logLine("Teller", id, "Teller", id, "ready to serve");
}

int main()
{
    srand(static_cast<unsigned>(time(nullptr)));

    for (int i = 0; i < NUM_CUSTOMERS; ++i) {
        customerAssigned[i] = new binary_semaphore(0);
        customerDone[i] = new binary_semaphore(0);
    }

    for (int i = 0; i < NUM_TELLERS; ++i) {
        tellerAskSem[i] = new binary_semaphore(0);
        customerTellSem[i] = new binary_semaphore(0);
        customerLeftSem[i] = new binary_semaphore(0);
    }

    vector<thread> tellers;
    vector<thread> customers;

    for (int i = 1; i <= NUM_TELLERS; ++i) {
        tellers.emplace_back(teller, i);
    }
    for (int i = 0; i < NUM_CUSTOMERS; ++i) {
        customers.emplace_back(customer, i);
    }

    for (auto& c : customers) {
        c.join();
    }
    for (auto& t : tellers) {
        t.join();
    }

    for (int i = 0; i < NUM_CUSTOMERS; ++i) {
        delete customerAssigned[i];
        delete customerDone[i];
    }
    for (int i = 0; i < NUM_TELLERS; ++i) {
        delete tellerAskSem[i];
        delete customerTellSem[i];
        delete customerLeftSem[i];
    }

    return 0;
}