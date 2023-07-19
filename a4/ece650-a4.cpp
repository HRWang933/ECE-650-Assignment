// Compile with c++ ece650-a2cpp -std=c++11 -o ece650-a2
#include <iostream>
#include <list>
#include <memory>
#include <regex>
#include <sstream>
#include <vector>
#include <bits/stdc++.h>
#include "minisat/core/Solver.h"
#include "minisat/core/SolverTypes.h"

class Graph {
    std::vector<std::vector<int>> board;


   public:
    int vecter = -1;
    int edge_flag = -1;
    Graph();
    Graph(int vecter_);
    void board_init();
    int input_edge(std::vector<int> edge);
    std::vector<int> shortest_path(int start, int end);
    int min_vertex_cover();
    int get_board_size();
};
Graph::Graph() {}
Graph::Graph(int vecter_) : vecter(vecter_) {
    board.resize(vecter + 1, std::vector<int>(vecter + 1));
}
// void Graph::board_init() {
//     for (int i = 0; i < vecter + 1; i++) {
//         for (int j = 0; j < vecter + 1; j++) {
//             board[i][j] = 0;
//         }
//     }
// }
int Graph::input_edge(std::vector<int> edge) {
    for (int i = 0; i < (int)edge.size(); i++) {
        if (edge[i] > vecter) {
            return 1;
        }
    }
    for (int i = 0; i < (int)edge.size(); i += 2) {
        board[edge[i]][edge[i + 1]] = 1;
        //board[edge[i + 1]][edge[i]] = 1;
    }
    edge_flag = 1;
    return 0;
}
int Graph::min_vertex_cover() {
    int min = 1;
    while (min <= vecter) {
        std::unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
        solver.reset(new Minisat::Solver());
        Minisat::Lit x[vecter][min];
        for (int i = 1; i <= vecter; i++) {
            for (int j = 1; j <= min; j++) {
                x[i][j] = Minisat::mkLit(solver->newVar());
            }
        }
        //At least one vertex is the ith vertex in the vertex cover
        for (int j = 1; j <= min; j++) {
            Minisat::vec<Minisat::Lit> clauses;
            for (int i = 1; i <= vecter; i++) {
                clauses.push(x[i][j]);
            }
            solver->addClause(clauses);
        }
        //No one vertex can appear twice in a vertex cover
        for (int i = 1; i <= vecter; i++) {
            for (int j = 1; j <= min; j++) {
                for (int k = j + 1; k <= min; k++) {
                    solver->addClause(~x[i][j], ~x[i][k]);
                }
            }
        }
        //No more than one vertex appears in the mth position of the vertex cover
        for (int i = 1; i <= min; i++) {
            for (int j = 1; j <= vecter; j++) {
                for (int k = j + 1; k <= vecter; k++) {
                    solver->addClause(~x[j][i], ~x[k][i]);
                }
            }
        }
        //Every edge is incident to at least one vertex in the vertex cover.
        for (int i = 1; i <= vecter; i++) {
            for (int j = 1; j <= vecter; j++) {
                if (board[i][j] == 1) {
                    Minisat::vec<Minisat::Lit> clauses;
                    for (int k = 1; k <= min; k++) {
                        clauses.push(x[i][k]);
                        clauses.push(x[j][k]);
                    }
                    solver->addClause(clauses);
                }
            }
        }
        bool sat = solver->solve();

        if (sat) {
            std::vector<int> output;
            for (int i = 1; i <= vecter; i++) {
                for (int j = 1; j <= min; j++) {
                    int v;
                    v = Minisat::toInt(solver->modelValue(x[i][j]));
                    if (v == 0) {
                        output.push_back(i);
                    }
                }
            }
            std::sort(output.begin(),output.end());
            for (int i=0; i<(int)output.size(); ++i)
                std::cout << output[i] << ' ';
            std::cout<<std::endl;   
            return min;
        }
        min++;
    }
    return -1;
}
std::vector<int> Graph::shortest_path(int start, int end) {
    std::list<std::vector<int>> frontier;
    std::list<int> explored;
    std::vector<int> s;
    s.push_back(start);
    frontier.push_back(s);
    while (frontier.size()) {
        std::vector<int> check = frontier.front();
        int c = check.back();
        frontier.pop_front();
        bool found = (std::find(explored.begin(), explored.end(), c) != explored.end());
        if (!found) {
            explored.push_back(c);
            if (c == end)
                return check;
            for (int i = 1; i <= vecter; i++) {
                if (board[c][i] == 1) {
                    std::vector<int> next;
                    next = check;
                    next.push_back(i);
                    frontier.push_back(next);
                }
            }
        }
    }
    return std::vector<int>();
}
int main() {
    std::string line;
    std::regex r_V("^V\\s\\d+\\s*$");
    std::regex r_E("^E\\s\\{(<\\d+,\\d+>,?){0,}\\}\\s*$");
    std::regex r_s("^s\\s\\d+\\s\\d+\\s*$");
    std::regex r_n("[0-9]{1,}");
    int vertex=-1 ;
    Graph graph = Graph();
    while (std::getline(std::cin, line)) {
        // std::cout << line;
        if (std::regex_match(line, r_V)) {
            std::smatch match;
            if (std::regex_search(line, match, r_n) == true) {
                vertex = std::stoi(match.str(0));
            }
            graph = Graph(vertex);
        } else if (std::regex_match(line, r_E)) {
            if (-1 == graph.vecter) {
                std::cout << "Error:Should input vertex first" << std::endl;
                continue;
            }
            std::smatch match;
            std::vector<int> argument;
            while (std::regex_search(line, match, r_n)) {
                argument.push_back(std::stoi(match.str(0)));
                line = match.suffix();
            }
            if (graph.input_edge(argument)) {
                std::cout << "Error:Vertex overflow" << std::endl;
                continue;
            }

            if (argument.size()==0 || graph.vecter==0){
                std::cout <<std::endl;
                graph=Graph();
            }
            //std::cout<<"test" <<std::endl;
            graph.min_vertex_cover();
            graph=Graph();

        } else {
            std::cout << "Error: Invalid command" << std::endl;
        }
    }
    return 0;
}