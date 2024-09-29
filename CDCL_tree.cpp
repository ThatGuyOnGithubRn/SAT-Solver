#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <stack>

using namespace std;

struct Node {
    unordered_map<int, bool> assignment;
    vector<vector<int>> clauses;
    vector<Node*> children;

    Node(const unordered_map<int, bool>& assign, const vector<vector<int>>& cls)
        : assignment(assign), clauses(cls) {}
    
    ~Node() {
        for (Node* child : children) {
            delete child;
        }
    }
};

struct Tree {
    Node* root;

    Tree() {
        root = new Node(unordered_map<int, bool>(), vector<vector<int>>());
    }

    ~Tree() {
        delete root;
    }
};

bool isSatisfied(const vector<vector<int>>& clauses, const unordered_map<int, bool>& assignment) {
    for (const auto& clause : clauses) {
        bool satisfied = false;
        for (int lit : clause) {
            int var = abs(lit);
            if (assignment.count(var) && assignment.at(var) == (lit > 0)) {
                satisfied = true;
                break;
            }
        }
        if (!satisfied) return false;
    }
    return true;
}

vector<vector<int>> propagate(const vector<vector<int>>& clauses, int var, bool value) {
    vector<vector<int>> new_clauses;
    for (const auto& clause : clauses) {
        if ((value && find(clause.begin(), clause.end(), var) != clause.end()) ||
            (!value && find(clause.begin(), clause.end(), -var) != clause.end())) {
            continue;
        }
        vector<int> new_clause;
        for (int lit : clause) {
            if (lit != (value ? var : -var)) {
                new_clause.push_back(lit);
            }
        }
        if (!new_clause.empty()) {
            new_clauses.push_back(new_clause);
        }
    }
    return new_clauses;
}

vector<int> analyzeConflict(const vector<vector<int>>& clauses, const unordered_map<int, bool>& assignment) {
    unordered_set<int> conflict_literals;
    for (const auto& clause : clauses) {
        bool satisfied = false;
        for (int lit : clause) {
            int var = abs(lit);
            if (assignment.count(var) && assignment.at(var) == (lit > 0)) {
                satisfied = true;
                break;
            }
        }
        if (!satisfied) {
            for (int lit : clause) {
                conflict_literals.insert(lit);
            }
        }
    }
    return vector<int>(conflict_literals.begin(), conflict_literals.end());
}

int selectVariable(const vector<vector<int>>& clauses, const unordered_map<int, bool>& assignment) {
    unordered_map<int, int> frequency;
    for (const auto& clause : clauses) {
        for (int lit : clause) {
            int var = abs(lit);
            if (assignment.count(var) == 0) { // Only count unassigned variables
                frequency[var]++;
            }
        }
    }

    // Find the variable with the maximum frequency
    int selectedVar = -1;
    int maxCount = -1;
    for (const auto& [var, count] : frequency) {
        if (count > maxCount) {
            maxCount = count;
            selectedVar = var;
        }
    }
    return selectedVar;
}

Node* cdcl(Node* node) {
    if (isSatisfied(node->clauses, node->assignment)) {
        return node;
    }

    int var = selectVariable(node->clauses, node->assignment);
    if (var == -1) return nullptr; // No variables left to assign

    // Try assigning true to the selected variable
    auto true_assignment = node->assignment;
    true_assignment[var] = true;
    auto new_clauses = propagate(node->clauses, var, true);
    Node* true_node = new Node(true_assignment, new_clauses);
    node->children.push_back(true_node);

    Node* result = cdcl(true_node);
    if (result) {
        return result;
    }

    // Try assigning false to the selected variable
    auto false_assignment = node->assignment;
    false_assignment[var] = false;
    new_clauses = propagate(node->clauses, var, false);
    Node* false_node = new Node(false_assignment, new_clauses);
    node->children.push_back(false_node);

    result = cdcl(false_node);
    if (result) {
        return result;
    }

    // Analyze conflict and add new clause if needed
    vector<int> conflict_clause = analyzeConflict(node->clauses, node->assignment);
    if (!conflict_clause.empty()) {
        node->clauses.push_back(conflict_clause);
    }

    return nullptr;
}

int main() {
    // auto start = std::chrono::high_resolution_clock::now();
    int numCl;
    cout << "Enter number of clauses: ";
    cin >> numCl;
    vector<vector<int>> clauses(numCl);
    for (int i = 0; i < numCl; i++) {
        cout << "Enter literals for clause " << (i + 1) << " (end with 0): ";
        while (true) {
            int x;
            cin >> x;
            if (x == 0) break;
            clauses[i].push_back(x);
        }
    }

    Tree decisionTree;
    decisionTree.root->clauses = clauses;

    Node* result = cdcl(decisionTree.root);

    if (result) {
        cout << "Satisfiable with assignment: ";
        for (const auto& [var, value] : result->assignment) {
            cout << var << "=" << value << " ";
        }
        cout << endl;
    } else {
        cout << "Unsatisfiable" << endl;
    }
    return 0;
}
