// Task 1: Smart Logistics Freight Convoy Partitioning (Linked List + Queue)

#include <iostream>
#include <cstdlib>
using namespace std;

const int MAX_TRUCKS = 20;
const int MAX_CONTAINERS = 50;

// node of the linked list, one container
struct Container {
    int id;
    int weight;
    Container* next;
};

// one truck, stores its remaining space and the container ids loaded on it
struct Truck {
    int truckId;
    int remaining;
    int assigned[MAX_CONTAINERS];
    int count;
};

// circular queue made with an array, holds pointers to trucks
struct Queue {
    Truck* data[MAX_TRUCKS];
    int front;
    int rear;
    int size;
};

// queue functions
void initQueue(Queue& q) {
    q.front = 0;
    q.rear = 0;
    q.size = 0;
}

void enqueue(Queue& q, Truck* t) {
    if (q.size == MAX_TRUCKS) {
        cout << "Queue is full" << endl;
        return;
    }
    q.data[q.rear] = t;
    q.rear = (q.rear + 1) % MAX_TRUCKS;
    q.size++;
}

Truck* dequeue(Queue& q) {
    if (q.size == 0) return nullptr;
    Truck* t = q.data[q.front];
    q.front = (q.front + 1) % MAX_TRUCKS;
    q.size--;
    return t;
}

// linked list functions
void appendContainer(Container*& head, Container*& tail, int id, int weight) {
    Container* n = new Container;
    n->id = id;
    n->weight = weight;
    n->next = nullptr;
    if (head == nullptr) {
        head = n;
        tail = n;
    } else {
        tail->next = n;
        tail = n;
    }
}

int totalWeight(Container* head) {
    int sum = 0;
    for (Container* c = head; c != nullptr; c = c->next)
        sum += c->weight;
    return sum;
}

int maxWeight(Container* head) {
    int m = 0;
    for (Container* c = head; c != nullptr; c = c->next)
        if (c->weight > m) m = c->weight;
    return m;
}

void freeList(Container* head) {
    while (head != nullptr) {
        Container* temp = head;
        head = head->next;
        delete temp;
    }
}

Truck* createTruck(int id, int capacity) {
    Truck* t = new Truck;
    t->truckId = id;
    t->remaining = capacity;
    t->count = 0;
    return t;
}

// checks if a remaining capacity value was already tried at this level
bool alreadyTried(int tried[], int triedCount, int value) {
    for (int i = 0; i < triedCount; i++)
        if (tried[i] == value) return true;
    return false;
}

// backtracking function
// node = current container, remWeight = weight of containers not placed yet,
// totalRemCap = total free space left in all trucks
// assumption: any container can go on any truck that has enough space
bool solve(Container* node, int remWeight, int totalRemCap, Queue& trucks) {
    // base case, all containers are placed
    if (node == nullptr) return true;

    // pruning 2, remaining weight is more than remaining capacity
    if (remWeight > totalRemCap) return false;

    int n = trucks.size;
    int tried[MAX_TRUCKS];
    int triedCount = 0;

    // one full rotation of the queue so every truck gets tried once
    for (int i = 0; i < n; i++) {
        Truck* truck = dequeue(trucks);
        enqueue(trucks, truck);  // putting it back so the order stays safe

        // pruning 1, truck cannot fit this container
        if (truck->remaining < node->weight) continue;

        // trucks with same free space give same result, so skip repeats
        if (alreadyTried(tried, triedCount, truck->remaining)) continue;
        tried[triedCount++] = truck->remaining;

        // choose
        truck->remaining -= node->weight;
        truck->assigned[truck->count++] = node->id;

        // explore
        if (solve(node->next, remWeight - node->weight, totalRemCap - node->weight, trucks))
            return true;

        // un-choose (backtrack)
        truck->count--;
        truck->remaining += node->weight;
    }
    return false;
}

void printResult(Truck* allTrucks[], int numTrucks, int W) {
    for (int i = 0; i < numTrucks; i++) {
        Truck* t = allTrucks[i];
        cout << "  Truck " << t->truckId << ": ";
        if (t->count == 0) cout << "(empty)";
        for (int j = 0; j < t->count; j++) {
            cout << "C" << t->assigned[j];
            if (j < t->count - 1) cout << ", ";
        }
        cout << "  [load " << (W - t->remaining) << "/" << W << "]" << endl;
    }
}

void runCase(int weights[], int n, int numTrucks, int W) {
    // making the linked list
    Container* head = nullptr;
    Container* tail = nullptr;
    for (int i = 0; i < n; i++)
        appendContainer(head, tail, i + 1, weights[i]);

    cout << "Containers (id:weight): ";
    for (Container* c = head; c != nullptr; c = c->next)
        cout << "C" << c->id << ":" << c->weight << " ";
    cout << endl << "Trucks: " << numTrucks << ", capacity W = " << W << endl;

    // making the queue of trucks
    Queue trucks;
    initQueue(trucks);
    Truck* allTrucks[MAX_TRUCKS];
    for (int i = 0; i < numTrucks; i++) {
        allTrucks[i] = createTruck(i + 1, W);
        enqueue(trucks, allTrucks[i]);
    }

    int totalW = totalWeight(head);
    int totalCap = numTrucks * W;

    bool possible;
    // quick check before starting the search
    if (totalW > totalCap || maxWeight(head) > W)
        possible = false;
    else
        possible = solve(head, totalW, totalCap, trucks);

    if (possible)
        printResult(allTrucks, numTrucks, W);
    else
        cout << "  Insufficient truck capacity." << endl;
    cout << "----------------------------------------" << endl;

    // cleaning memory
    for (int i = 0; i < numTrucks; i++) delete allTrucks[i];
    freeList(head);
}

// keeps asking until the user enters a number inside the allowed range
int readInt(const char* prompt, int minVal, int maxVal) {
    int x;
    while (true) {
        cout << prompt;
        if (cin >> x && x >= minVal && x <= maxVal) return x;
        if (cin.eof()) {
            cout << "Input ended" << endl;
            exit(1);
        }
        cout << "  Invalid input, enter a number between " << minVal << " and " << maxVal << endl;
        cin.clear();
        cin.ignore(1000, '\n');
    }
}

int main() {
    int weights[MAX_CONTAINERS];

    int n = readInt("Enter number of containers: ", 1, MAX_CONTAINERS);
    for (int i = 0; i < n; i++) {
        cout << "Weight of container C" << i + 1 << " (tons): ";
        weights[i] = readInt("", 1, 100000);
    }

    int numTrucks = readInt("Enter number of trucks: ", 1, MAX_TRUCKS);
    int W = readInt("Enter capacity W of each truck (tons): ", 1, 100000);

    cout << endl;
    runCase(weights, n, numTrucks, W);
    return 0;
}