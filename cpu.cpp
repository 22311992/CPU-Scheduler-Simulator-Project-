#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <climits>
#include <algorithm>
using namespace std;

struct Process {
    int burstTime;
    int priority;
    int arrivalTime;
    int queueID;
};

struct Node {
    Process data;
    Node* next;
};

void insertProcess(Node*& head, Process p) {
    Node* newNode = new Node;
    newNode->data = p;
    newNode->next = NULL;

    if (head == NULL) {
        head = newNode;
        return;
    }

    Node* temp = head;
    while (temp->next != NULL)
        temp = temp->next;

    temp->next = newNode;
}

void readInputFile(const string& filename, Node*& head) {
    ifstream file(filename);
    string line;

    if (!file) return;

    while (getline(file, line)) {
        Process p;
        char colon;

        stringstream ss(line);
        ss >> p.burstTime >> colon
           >> p.priority >> colon
           >> p.arrivalTime >> colon
           >> p.queueID;

        insertProcess(head, p);
    }
}

bool hasProcessInQueue(Node* head, int queueID) {
    while (head != NULL) {
        if (head->data.queueID == queueID)
            return true;
        head = head->next;
    }
    return false;
}

Node* findEarliestArrival(Node* head, int queueID) {
    Node* selected = NULL;

    while (head != NULL) {
        if (head->data.queueID == queueID) {
            if (selected == NULL ||
                head->data.arrivalTime < selected->data.arrivalTime) {
                selected = head;
            }
        }
        head = head->next;
    }
    return selected;
}

void removeNode(Node*& head, Node* target) {
    if (!head || !target) return;

    if (head == target) {
        head = head->next;
        delete target;
        return;
    }

    Node* temp = head;
    while (temp->next && temp->next != target)
        temp = temp->next;

    if (temp->next == target) {
        temp->next = target->next;
        delete target;
    }
}

Node* copyList(Node* head) {
    Node* newHead = NULL;
    while (head != NULL) {
        insertProcess(newHead, head->data);
        head = head->next;
    }
    return newHead;
}

void runFCFS(Node* head, int queueID, ofstream& out) {
    Node* tempHead = copyList(head);

    int currentTime = 0, totalWT = 0, count = 0;
    string line = to_string(queueID) + ":1:";

    // FCFS = arrival order
    while (true) {
        Node* nextProcess = NULL;
        Node* temp = tempHead;

        // find earliest arrival that is not processed yet
        while (temp != NULL) {
            if (temp->data.queueID == queueID) {
                if (!nextProcess ||
                    temp->data.arrivalTime < nextProcess->data.arrivalTime) {
                    nextProcess = temp;
                }
            }
            temp = temp->next;
        }

        if (!nextProcess) break;

        if (currentTime < nextProcess->data.arrivalTime)
            currentTime = nextProcess->data.arrivalTime;

        int wt = currentTime - nextProcess->data.arrivalTime;
        line += to_string(wt) + ":";

        totalWT += wt;
        count++;
        currentTime += nextProcess->data.burstTime;

        removeNode(tempHead, nextProcess);
    }

    double avg = (count == 0) ? 0.0 : (double)totalWT / count;
    stringstream ss;
    ss << fixed << setprecision(2) << avg;
    line += ss.str();

    out << line << endl;

    while (tempHead) {
        Node* next = tempHead->next;
        delete tempHead;
        tempHead = next;
    }
}


void runSJF(Node* head, int queueID, ofstream& out) {
    Node* tempHead = copyList(head);

    int currentTime = 0, totalWT = 0, count = 0;
    out << queueID << ":2:";

    while (hasProcessInQueue(tempHead, queueID)) {
        Node* shortest = NULL;
        Node* temp = tempHead;

        while (temp != NULL) {
            if (temp->data.queueID == queueID &&
                temp->data.arrivalTime <= currentTime) {
                if (!shortest || temp->data.burstTime < shortest->data.burstTime)
                    shortest = temp;
            }
            temp = temp->next;
        }

        if (!shortest) {
            int nextArrival = INT_MAX;
            temp = tempHead;

            while (temp != NULL) {
                if (temp->data.queueID == queueID &&
                    temp->data.arrivalTime > currentTime)
                    nextArrival = min(nextArrival, temp->data.arrivalTime);
                temp = temp->next;
            }

            if (nextArrival == INT_MAX) break;
            currentTime = nextArrival;
            continue;
        }

        int wt = max(0, currentTime - shortest->data.arrivalTime);
        out << wt << ":";

        totalWT += wt;
        count++;
        currentTime += shortest->data.burstTime;
        removeNode(tempHead, shortest);
    }

    double avg = (count == 0) ? 0.0 : (double)totalWT / count;
    out << fixed << setprecision(2) << avg << endl;

    while (tempHead) {
        Node* next = tempHead->next;
        delete tempHead;
        tempHead = next;
    }
}


void runPriority(Node* head, int queueID, ofstream& out) {
    Node* tempHead = copyList(head);
    
    int currentTime = 0, totalWT = 0, count = 0;
    out << queueID << ":3:";  
    
    while (hasProcessInQueue(tempHead, queueID)) {
        Node* highestPriority = NULL;
        Node* temp = tempHead;
        
        while (temp != NULL) {
            if (temp->data.queueID == queueID && 
                temp->data.arrivalTime <= currentTime) {
                
                if (!highestPriority || 
                    temp->data.priority < highestPriority->data.priority ||
                    (temp->data.priority == highestPriority->data.priority &&
                     temp->data.arrivalTime < highestPriority->data.arrivalTime)) {
                    highestPriority = temp;
                }
            }
            temp = temp->next;
        }
        
        if (!highestPriority) {
            int nextArrival = INT_MAX;
            temp = tempHead;
            
            while (temp != NULL) {
                if (temp->data.queueID == queueID && 
                    temp->data.arrivalTime > currentTime) {
                    nextArrival = min(nextArrival, temp->data.arrivalTime);
                }
                temp = temp->next;
            }
            
            if (nextArrival == INT_MAX) break;
            currentTime = nextArrival;
            continue;
        }
        
        int wt = max(0, currentTime - highestPriority->data.arrivalTime);
        out << wt << ":";
        
        totalWT += wt;
        count++;
        
        currentTime += highestPriority->data.burstTime;
        
        removeNode(tempHead, highestPriority);
    }
    
    double avg = (count == 0) ? 0.0 : (double)totalWT / count;
    out << fixed << setprecision(2) << avg << endl;
    
    while (tempHead) {
        Node* next = tempHead->next;
        delete tempHead;
        tempHead = next;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) return 1;

    Node* head = NULL;
    readInputFile(argv[1], head);

    ofstream out(argv[2]);
    if (!out) return 1;

    Node* temp = head;
    while (temp != NULL) {
        int qid = temp->data.queueID;

        Node* check = head;
        bool printed = false;
        while (check != temp) {
            if (check->data.queueID == qid) {
                printed = true;
                break;
            }
            check = check->next;
        }

        if (!printed) {
            runFCFS(head, qid, out);
            runSJF(head, qid, out);
             runPriority(head, qid, out);
        }
        temp = temp->next;
    }

    out.close();

    while (head) {
        Node* next = head->next;
        delete head;
        head = next;
    }

    return 0;
}
       


