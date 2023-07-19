// Compile with c++ ece650-a2cpp -std=c++11 -o ece650-a2
#include <iostream>
#include <list>
#include <regex>
#include <vector>
class Graph {
    int vecter = -1;

    std::vector<std::vector<int>> board;

   public:
    int edge_flag =-1;
    Graph();
    Graph(int vecter_);
    void board_init();
    int input_edge(std::vector<int> edge);
    std::vector<int> shortest_path(int start, int end);
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
    for (int i = 0; i <(int)edge.size(); i++) {
        if (edge[i] > vecter) {
            return 1;
        }
    }
    for (int i = 0; i < (int)edge.size(); i += 2) {
        board[edge[i]][edge[i + 1]] = 1;
        board[edge[i + 1]][edge[i]] = 1;
    }
    edge_flag=1;
    return 0;
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
    int vertex = -1;
    Graph graph;
    while (std::getline(std::cin, line)) {
        
        if (std::regex_match(line, r_V)) {
            std::smatch match;
            if (std::regex_search(line, match, r_n) == true) {
                vertex = std::stoi(match.str(0));
            }
            std::cout<<line<<std::endl;
            graph = Graph(vertex);
        } else if (std::regex_match(line, r_E)) {
            if (-1 == vertex) {
                std::cerr << "Error:Should input vertex first" << std::endl;
                continue;
            }
            std::smatch match;
            std::vector<int> argument;
            std::cout<<line<<std::endl;
            while (std::regex_search(line, match, r_n)) {
                argument.push_back(std::stoi(match.str(0)));
                line = match.suffix();
            }
            
            if (graph.input_edge(argument)) {
                std::cerr << "Error:Vertex overflow" << std::endl;
                continue;
            }
        } else if (std::regex_match(line, r_s)) {
            std::smatch match;
            std::vector<int> argument;
            std::vector<int> path;
            if (-1 == vertex) {
                std::cerr << "Error:Should input vertex first" << std::endl;
                continue;
            }
            if (graph.edge_flag!=1) {
                std::cerr << "Error:Should input edge first" << std::endl;
                continue;
            }
            while (std::regex_search(line, match, r_n)) {
                argument.push_back(std::stoi(match.str(0)));
                line = match.suffix();
            }
            if(argument[0]>vertex || argument[1]>vertex){
                std::cerr << "Error:Vertex overflow" << std::endl;
                continue;
            }
            path=graph.shortest_path(argument[0], argument[1]);
            if(path.size()){
                std::cout << path[0];
                for(int i=1;i<(int)path.size();i++){
                    std::cout << "-";
                    std::cout << path[i];
                }
                std::cout<< std::endl;
                
            }
            else{
                std::cerr << "Error:Didn't find path" << std::endl;
            }
        } else {
            std::cerr << "Error: Invalid command" << std::endl;
        }
    }
    return 0;
}