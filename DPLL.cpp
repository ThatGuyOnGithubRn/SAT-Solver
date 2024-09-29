#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

bool isSatisfied(const vector<vector<int>>& clauses, const unordered_map<int, bool>& assignment) {
    for (const auto& clause : clauses) {
        bool satisfied = false;
        for (int lit : clause) {
            int var = abs(lit);
            if (assignment.find(var) == assignment.end() && assignment.at(var) == (lit > 0)) {
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

unordered_map<int, bool> dpll(vector<vector<int>> clauses, unordered_map<int, bool> assignment) {
    if (isSatisfied(clauses, assignment)) {
        return assignment;
    }

    for (const auto& clause : clauses) {
        for (int lit : clause) {
            int var = abs(lit);
            if (assignment.find(var) == assignment.end()) {
                assignment[var] = true;
                auto new_clauses = propagate(clauses, var, true);
                auto result = dpll(new_clauses, assignment);
                if (!result.empty()) {
                    return result;
                }

                assignment[var] = false;
                new_clauses = propagate(clauses, var, false);
                result = dpll(new_clauses, assignment);
                if (!result.empty()) {
                    return result;
                }

                assignment.erase(var);
                return {};
            }
        }
    }
    return {};
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

    unordered_map<int, bool> assignment;
    auto result = dpll(clauses, assignment);

    if (!result.empty()) {
        cout << "Satisfiable with assignment: ";
        for (const auto& [var, value] : result) {
            cout << var << "=" << value << " ";
        }
        cout << endl;
    } else {
        cout << "Unsatisfiable" << endl;
    }

    return 0;
}
