## April 17th 1:10PM
Bank Simulation with Threads and Semaphores
Project Goal
The goal of this project is to simulate a bank with:
3 teller threads
50 customer threads
a door that allows only 2 customers through at a time
a manager that only 1 teller can use at a time
a safe that only 2 tellers can use at a time

Each customer can choose either a Deposit or Withdrawal.
If the customer wants a withdrawal, the teller must first go to the manager for approval.
After that, the teller goes to the safe to complete the transaction.
All actions must be synchronized correctly and printed in the required format.

## Aprill 17th 1:50 
Step 1 — Build the program structure and shared resources
Build the basic simulation structure
This first step sets up the overall bank simulation:

includes the needed libraries
defines the number of tellers and customers which are 3 and 50
creates the shared synchronization objects
creates helper logging functions
sets up the thread function prototypes
starts teller and customer threads in main

At this stage, the program structure exists, but the interaction logic is still in its beginning stage.

## April 17th 2:40AM
Step 2 — Add bank opening, customer arrival, and doorway control
This step adds the first important synchronization rules:

customers randomly choose deposit or withdrawal
customers wait 0–100 ms before arriving
customers cannot enter before the bank opens
the bank opens only after all 3 tellers are ready
only 2 customers may use the door at once
customers get in line after entering

This so far does what the project’s required order of actions for customers and tellers.