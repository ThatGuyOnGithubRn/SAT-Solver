#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <thread>
#include <future>
using namespace std;



struct Node {
    unordered_map<int, bool> assignment;
    vector<vector<int>> clauses;
    Node(unordered_map<int, bool> assign, vector<vector<int>> cls)
        : assignment(move(assign)), clauses(move(cls)) {}
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


vector<vector<int>> propagate(vector<vector<int>> clauses, int var, bool value) {
    vector<vector<int>> new_clauses;
    for (auto& clause : clauses) {
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
            new_clauses.push_back(move(new_clause));
        }
    }
    return new_clauses;
}

Node* cdcl(Node* node) {
    if (isSatisfied(node->clauses, node->assignment)) {
        return node;
    }

    for (auto& clause : node->clauses) {
        for (int lit : clause) {
            int var = abs(lit);
            if (node->assignment.count(var) == 0) {
                auto true_assignment = node->assignment;
                true_assignment[var] = true;
                auto new_clauses_true = propagate(node->clauses, var, true);
                Node* true_node = new Node(move(true_assignment), move(new_clauses_true));

                auto false_assignment = node->assignment;
                false_assignment[var] = false;
                auto new_clauses_false = propagate(node->clauses, var, false);
                Node* false_node = new Node(move(false_assignment), move(new_clauses_false));

                auto future_true = async(launch::async, cdcl, true_node);
                auto future_false = async(launch::async, cdcl, false_node);

                Node* result = future_true.get();
                if (result) {
                    return result;
                }
                result = future_false.get();
                if (result) {
                    return result;
                }

                delete true_node;
                delete false_node;

                return nullptr;
            }
        }
    }
    return nullptr;
}

int main() {
    // ulimit -u 4096;
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

    Node* root = new Node(unordered_map<int, bool>(), move(clauses));
    Node* result = cdcl(root);

    if (result) {
        cout << "Satisfiable with assignment: ";
        for (const auto& [var, value] : result->assignment) {
            cout << var << "=" << value << " ";
        }
        cout << endl;
    } else {
        cout << "Unsatisfiable" << endl;
    }

    delete root;
    return 0;
}
