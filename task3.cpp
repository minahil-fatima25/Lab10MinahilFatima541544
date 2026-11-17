// Task 3: Hierarchical Cloud Budget & Audit Path Finder (Binary Tree & BST)

#include <iostream>
#include <cstdlib>
using namespace std;

const int MAX_NODES = 50;

// one microservice in the BST, tree is ordered by monthlyCost
struct Node {
    int serviceID;
    int monthlyCost;
    Node* left;
    Node* right;
};

Node* createNode(int id, int cost) {
    Node* n = new Node;
    n->serviceID = id;
    n->monthlyCost = cost;
    n->left = nullptr;
    n->right = nullptr;
    return n;
}

// normal BST insert, duplicate costs are ignored because left < root < right
Node* insert(Node* root, int id, int cost) {
    if (root == nullptr) return createNode(id, cost);
    if (cost < root->monthlyCost) root->left = insert(root->left, id, cost);
    else if (cost > root->monthlyCost) root->right = insert(root->right, id, cost);
    else cout << "  (duplicate cost " << cost << " ignored for service " << id << ")" << endl;
    return root;
}

void freeTree(Node* root) {
    if (root == nullptr) return;
    freeTree(root->left);
    freeTree(root->right);
    delete root;
}

// prints the nodes stored in an array like S1(50) -> S2(30)
void printPath(Node* arr[], int len, const char* separator) {
    for (int i = 0; i < len; i++) {
        cout << "S" << arr[i]->serviceID << "(" << arr[i]->monthlyCost << ")";
        if (i < len - 1) cout << separator;
    }
}

// Part A
// goes down from one starting node, path[] works like a stack of the current path
// costs are positive so we stop going deeper once sum reaches B
void pathsFromStart(Node* node, int B, int sum, Node* path[], int& pathLen, int& count) {
    if (node == nullptr) return;

    path[pathLen++] = node;  // push node while going down
    sum += node->monthlyCost;

    if (sum == B) {
        count++;
        cout << "  Path: ";
        printPath(path, pathLen, " -> ");
        cout << endl;
    }

    if (sum < B) {
        pathsFromStart(node->left, B, sum, path, pathLen, count);
        pathsFromStart(node->right, B, sum, path, pathLen, count);
    }

    pathLen--;  // pop node while backtracking
}

// every node of the tree is tried as a starting point
void findAllPaths(Node* current, int B, int& count) {
    if (current == nullptr) return;
    Node* path[MAX_NODES];
    int pathLen = 0;
    pathsFromStart(current, B, 0, path, pathLen, count);
    findAllPaths(current->left, B, count);
    findAllPaths(current->right, B, count);
}

// Part B
// inorder traversal of a BST gives the nodes in increasing cost order
void inorder(Node* root, Node* sorted[], int& n) {
    if (root == nullptr) return;
    inorder(root->left, sorted, n);
    sorted[n++] = root;
    inorder(root->right, sorted, n);
}

void subsetBacktrack(Node* sorted[], int n, int start, int B, int sum,
                     Node* chosen[], int& chosenLen, int& count) {
    if (sum == B) {
        count++;
        cout << "  Subset: ";
        printPath(chosen, chosenLen, " + ");
        cout << " = " << B << endl;
        return;
    }

    for (int i = start; i < n; i++) {
        // pruning, array is sorted so if this node goes above B
        // all the nodes after it (bigger ones) will also go above B
        if (sum + sorted[i]->monthlyCost > B) break;

        chosen[chosenLen++] = sorted[i];  // choose
        subsetBacktrack(sorted, n, i + 1, B, sum + sorted[i]->monthlyCost,
                        chosen, chosenLen, count);  // explore
        chosenLen--;  // un-choose
    }
}

void findAllSubsets(Node* root, int B) {
    Node* sorted[MAX_NODES];
    int n = 0;
    inorder(root, sorted, n);

    Node* chosen[MAX_NODES];
    int chosenLen = 0;
    int count = 0;
    subsetBacktrack(sorted, n, 0, B, 0, chosen, chosenLen, count);

    if (count == 0) cout << "  No subset found." << endl;
    else cout << "Total subsets found: " << count << endl;
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
    Node* root = nullptr;

    int n = readInt("Enter number of microservices: ", 1, MAX_NODES);
    for (int i = 0; i < n; i++) {
        cout << "Service " << i + 1 << endl;
        int id = readInt("  Enter serviceID: ", 0, 1000000);
        int cost = readInt("  Enter monthlyCost: ", 1, 1000000);
        root = insert(root, id, cost);
    }

    int B = readInt("Enter target budget B: ", 1, 10000000);
    cout << endl << "Target budget B = " << B << endl << endl;

    cout << "Part A: downward paths with sum B" << endl;
    int countA = 0;
    findAllPaths(root, B, countA);
    if (countA == 0) cout << "  No path found." << endl;
    else cout << "Total paths found: " << countA << endl;

    cout << endl << "Part B: subsets with sum B" << endl;
    findAllSubsets(root, B);

    freeTree(root);
    return 0;
}