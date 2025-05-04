#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <omp.h>

using namespace std;

const int N = 7; // number of nodes in the tree
vector<int> tree[N];
vector<bool> visited_bfs(N, false);
vector<bool> visited_dfs(N, false);

// Add edge between parent and child (undirected)
void addEdge(int u, int v) {
    tree[u].push_back(v);
    tree[v].push_back(u);
}

// Parallel Breadth First Search (BFS) on tree
void parallelBFS(int root) {
    queue<int> q;
    visited_bfs[root] = true;
    q.push(root);

    cout << "Parallel BFS (Tree): ";

    while (!q.empty()) {
        int size = q.size();

        #pragma omp parallel for
        for (int i = 0; i < size; i++) {
            int node;

            #pragma omp critical
            {
                node = q.front();
                q.pop();
                cout << node << " ";
            }

            for (int child : tree[node]) {
                if (!visited_bfs[child]) {
                    #pragma omp critical
                    {
                        if (!visited_bfs[child]) {
                            visited_bfs[child] = true;
                            q.push(child);
                        }
                    }
                }
            }
        }
    }

    cout << endl;
}

// Parallel Depth First Search (DFS) on tree
void parallelDFS(int root) {
    stack<int> s;
    visited_dfs[root] = true;
    s.push(root);

    cout << "Parallel DFS (Tree): ";

    while (!s.empty()) {
        int node;

        #pragma omp critical
        {
            node = s.top();
            s.pop();
            cout << node << " ";
        }

        #pragma omp parallel for
        for (int i = 0; i < tree[node].size(); i++) {
            int child = tree[node][i];

            if (!visited_dfs[child]) {
                #pragma omp critical
                {
                    if (!visited_dfs[child]) {
                        visited_dfs[child] = true;
                        s.push(child);
                    }
                }
            }
        }
    }

    cout << endl;
}

int main() {
    // Tree structure (Example):
    //        0
    //      / | \
    //     1  2  3
    //        |
    //       4
    //      / \
    //     5   6

    addEdge(0, 1);
    addEdge(0, 2);
    addEdge(0, 3);
    addEdge(2, 4);
    addEdge(4, 5);
    addEdge(4, 6);

    parallelBFS(0); // root node is 0
    parallelDFS(0); // root node is 0

    return 0;
}
