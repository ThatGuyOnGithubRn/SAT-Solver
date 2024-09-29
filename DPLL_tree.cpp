#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

using namespace std;

struct Node {
    unordered_map<int, bool> assignment;
    vector<vector<int>> clauses;
    vector<Node*> children;

    Node(const unordered_map<int, bool>& assign, const vector<vector<int>>& cls) 
        : assignment(assign), clauses(cls) {}
};

struct Tree {
    Node* root;

    Tree() {
        root = new Node(unordered_map<int, bool>(), vector<vector<int>>());
    }

    ~Tree() {
        deleteTree(root);
    }

    void deleteTree(Node* node) {
        for (auto child : node->children) {
            deleteTree(child);
        }
        delete node;
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

Node* dpll(Node* node) {
    if (isSatisfied(node->clauses, node->assignment)) {
        return node;
    }

    for (const auto& clause : node->clauses) {
        for (int lit : clause) {
            int var = abs(lit);
            if (node->assignment.find(var) == node->assignment.end()) {
                auto true_assignment = node->assignment;
                true_assignment[var] = true;
                auto new_clauses = propagate(node->clauses, var, true);
                Node* true_node = new Node(true_assignment, new_clauses);
                node->children.push_back(true_node);

                auto result = dpll(true_node);
                if (result) {
                    return result;
                }

                auto false_assignment = node->assignment;
                false_assignment[var] = false;
                new_clauses = propagate(node->clauses, var, false);
                Node* false_node = new Node(false_assignment, new_clauses);
                node->children.push_back(false_node);

                result = dpll(false_node);
                if (result) {
                    return result;
                }

                return nullptr;
            }
        }
    }
    return nullptr;
}

int main() {
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

    Node* result = dpll(decisionTree.root);

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
