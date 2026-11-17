// Task 2: IDE Undo/Redo Macro Synthesizer (Stack + Backtracking)

#include <iostream>
#include <cstdlib>
using namespace std;

const int MAX_STACK = 20;
const int MAX_OPS = 10;

// one atomic operation like +3 or *2
struct Operation {
    char type;
    int value;
};

// operation stack made with an array
struct Stack {
    Operation data[MAX_STACK];
    int top;
};

// stack functions
void initStack(Stack& s) {
    s.top = 0;
}

void push(Stack& s, Operation op) {
    if (s.top == MAX_STACK) {
        cout << "Stack overflow" << endl;
        return;
    }
    s.data[s.top++] = op;
}

void pop(Stack& s) {
    if (s.top > 0) s.top--;
}

// applies one operation on the current value
// returns false if the result is negative, above Mmax or division by zero
// note: / is integer division
bool applyOp(Operation op, int cur, int Mmax, int& result) {
    long long r = cur;
    if (op.type == '+') r = (long long)cur + op.value;
    else if (op.type == '-') r = (long long)cur - op.value;
    else if (op.type == '*') r = (long long)cur * op.value;
    else if (op.type == '/') {
        if (op.value == 0) return false;
        r = cur / op.value;
    } else return false;

    if (r < 0 || r > Mmax) return false;
    result = (int)r;
    return true;
}

void printOp(Operation op) {
    cout << op.type << op.value;
}

// prints the sequence stored in the stack from bottom to top
void printSequence(Stack& s, int Sstart, int Mmax) {
    cout << "  " << Sstart;
    int cur = Sstart;
    for (int i = 0; i < s.top; i++) {
        int next;
        applyOp(s.data[i], cur, Mmax, next);
        cout << " --(";
        printOp(s.data[i]);
        cout << ")--> " << next;
        cur = next;
    }
    cout << "   [" << s.top << " ops]" << endl;
}

// backtracking function
void backtrack(int cur, int Sstart, int Starget, int K, int Mmax,
               Operation ops[], int numOps, Stack& s, int& count) {
    // if we reached the target, store this sequence
    // we do not stop here because a longer sequence can come back to the target
    if (cur == Starget) {
        count++;
        if (s.top == 0) cout << "  " << Sstart << "   [0 ops, already at target]" << endl;
        else printSequence(s, Sstart, Mmax);
    }

    // no operations left in the budget
    if (s.top == K) return;

    for (int i = 0; i < numOps; i++) {
        int next;
        // pruning, skip operations that make the value invalid
        if (!applyOp(ops[i], cur, Mmax, next)) continue;

        push(s, ops[i]);  // choose
        backtrack(next, Sstart, Starget, K, Mmax, ops, numOps, s, count);  // explore
        pop(s);  // undo
    }
}

void runCase(int Sstart, int Starget, int K, int Mmax, Operation ops[], int numOps) {
    cout << "Sstart = " << Sstart << ", Starget = " << Starget
         << ", K = " << K << ", Mmax = " << Mmax << endl;
    cout << "Operations: ";
    for (int i = 0; i < numOps; i++) {
        printOp(ops[i]);
        cout << " ";
    }
    cout << endl;

    if (Sstart < 0 || Sstart > Mmax) {
        cout << "  Start value is outside the allowed range." << endl;
        cout << "----------------------------------------" << endl;
        return;
    }

    Stack s;
    initStack(s);
    int count = 0;
    backtrack(Sstart, Sstart, Starget, K, Mmax, ops, numOps, s, count);

    if (count == 0) cout << "  No valid sequence found." << endl;
    else cout << "Total sequences found: " << count << endl;
    cout << "----------------------------------------" << endl;
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

// reads one operation like + 3 or * 2 from the user
Operation readOperation(int number) {
    Operation op;
    while (true) {
        cout << "Operation " << number << " (type then value, example + 3): ";
        if (cin >> op.type >> op.value) {
            bool validType = (op.type == '+' || op.type == '-' || op.type == '*' || op.type == '/');
            if (!validType) {
                cout << "  Type must be one of + - * /" << endl;
            } else if (op.value < 0) {
                cout << "  Value cannot be negative" << endl;
            } else if (op.type == '/' && op.value == 0) {
                cout << "  Cannot divide by zero" << endl;
            } else {
                return op;
            }
        } else {
            if (cin.eof()) {
                cout << "Input ended" << endl;
                exit(1);
            }
            cout << "  Invalid input" << endl;
            cin.clear();
        }
        cin.ignore(1000, '\n');
    }
}

int main() {
    int Mmax = readInt("Enter overflow bound Mmax: ", 1, 1000000);
    int Sstart = readInt("Enter start value Sstart: ", 0, Mmax);
    int Starget = readInt("Enter target value Starget: ", 0, Mmax);
    int K = readInt("Enter max number of operations K: ", 1, MAX_STACK);

    int numOps = readInt("Enter number of allowed operations: ", 1, MAX_OPS);
    Operation ops[MAX_OPS];
    for (int i = 0; i < numOps; i++)
        ops[i] = readOperation(i + 1);

    cout << endl;
    runCase(Sstart, Starget, K, Mmax, ops, numOps);
    return 0;
}