#include <bits/stdc++.h>
#include <getopt.h>
#include <math.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

struct Point {
    int x;
    int y;
};
struct Line {
    Point src;
    Point dst;
};
std::vector<std::string> names;
// Given three collinear points p, q, r, the function checks if
// point q lies on line segment 'pr'
bool onSegment(Point p, Point q, Point r) {
    if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
        q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
        return true;

    return false;
}

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are collinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int orientation(Point p, Point q, Point r) {
    // for details of below formula.
    int val = (q.y - p.y) * (r.x - q.x) -
              (q.x - p.x) * (r.y - q.y);

    if (val == 0) return 0;  // collinear

    return (val > 0) ? 1 : 2;  // clock or counterclock wise
}

// The main function that returns true if line segment 'p1q1'
// and 'p2q2' intersect.
bool doIntersect(Point p1, Point q1, Point p2, Point q2) {
    // Find the four orientations needed for general and
    // special cases
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    // General case
    if (o1 != o2 && o3 != o4)
        return true;

    // Special Cases
    // p1, q1 and p2 are collinear and p2 lies on segment p1q1
    if (o1 == 0 && onSegment(p1, p2, q1)) return true;

    // p1, q1 and q2 are collinear and q2 lies on segment p1q1
    if (o2 == 0 && onSegment(p1, q2, q1)) return true;

    // p2, q2 and p1 are collinear and p1 lies on segment p2q2
    if (o3 == 0 && onSegment(p2, p1, q2)) return true;

    // p2, q2 and q1 are collinear and q1 lies on segment p2q2
    if (o4 == 0 && onSegment(p2, q1, q2)) return true;

    return false;  // Doesn't fall in any of the above cases
}
float slope(Line line) {
    float x;
    if (line.dst.x != line.src.x) {
        x = (float)(line.dst.y - line.src.y) / (line.dst.x - line.src.x);
        return x;
    } else
        return FLT_MAX;
}
float line_length(Point p1, Point p2) {
    return sqrt(pow((p2.x - p1.x), 2) + pow((p2.y - p1.y), 2));
}
bool is_overlap(Line l1, Line l2) {
    float slope1 = slope(l1);
    float slope2 = slope(l2);
    if (slope1 != slope2) return false;
    bool instersect = doIntersect(l1.src, l1.dst, l2.src, l2.dst);
    if (!instersect) return false;

    float sum_length = line_length(l1.src, l1.dst) + line_length(l2.src, l2.dst);
    // if (line_length(l1.src, l2.src) == sum_length) return false;
    // if (line_length(l1.src, l2.dst) == sum_length) return false;
    // if (line_length(l1.dst, l2.src) == sum_length) return false;
    // if (line_length(l1.dst, l2.dst) == sum_length) return false;
    if (line_length(l1.src, l2.src) == sum_length) {
        return false;
    }
    if (line_length(l1.src, l2.dst) == sum_length) {
        return false;
    }
    if (line_length(l1.dst, l2.src) == sum_length) {
        return false;
    }
    if (line_length(l1.dst, l2.dst) == sum_length) {
        return false;
    }
    return true;
}

std::vector<Line> add_lines(std::vector<Point> line_segments) {
    int size = line_segments.size();
    std::vector<Line> lines;
    for (int j = 0; j < size - 1; j++) {
        Line line;
        line.src = line_segments[j];
        line.dst = line_segments[j + 1];
        lines.push_back(line);
    }
    return lines;
}
bool is_self_overlap(std::vector<Line> lines) {
    int size = lines.size();
    int overlap = 0;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (is_overlap(lines[i], lines[j])) {
                overlap++;
            }
        }
    }
    if (overlap == size) return false;
    return true;
}
bool is_self_intersect(std::vector<Line> lines) {
    int size = lines.size();

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (j == (i - 1)) continue;
            if (j == i) continue;
            if (j == (i + 1)) continue;

            if (doIntersect(lines[i].src, lines[i].dst, lines[j].src, lines[j].dst))
                return true;
        }
    }
    return false;
}
bool have_zero_length(std::vector<Line> lines) {
    int size = lines.size();
    for (int i = 0; i < size; i++) {
        if (lines[i].src.x == lines[i].dst.x && lines[i].src.y == lines[i].dst.y) {
            return true;
        }
    }
    return false;
}
bool is_valid(std::vector<Line> lines, std::vector<Line> ex_lines) {
    if (have_zero_length(lines)) return false;
    if (is_self_overlap(lines)) return false;
    if (is_self_intersect(lines)) return false;
    int newsize = lines.size();
    int oldsize = ex_lines.size();
    for (int i = 0; i < newsize; i++) {
        for (int j = 0; j < oldsize; j++) {
            if (is_overlap(lines[i], ex_lines[j])) {
                return false;
            }
        }
    }
    return true;
}

struct Street {
    std::vector<Point> line_segments;
    std::string name;
};

template <class T>
T read_urandom() {
    union {
        T value;
        char cs[sizeof(T)];
    } u;
    std::ifstream rfin("/dev/urandom");
    rfin.read(u.cs, sizeof(u.cs));
    rfin.close();
    return (u.value);
}
std::string gen_str(const int len) {
    static const char alphanum[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[read_urandom<int>() % (sizeof(alphanum) - 1)];
    }

    return tmp_s;
}

int gen_rint(int min, int max) {
    int ran;
    ran = abs(read_urandom<int>());
    ran = ran % (max + 1 - min) + min;
    return ran;
}
Point gen_coordinate(int c) {
    int r_i, r_p;
    Point p;
    r_i = gen_rint(0, c);
    r_p = read_urandom<int>();
    r_i = r_i * (r_p / abs(r_p));
    p.x = r_i;
    r_i = gen_rint(0, c);
    r_p = read_urandom<int>();
    r_i = r_i * (r_p / abs(r_p));
    p.y = r_i;
    return p;
}
std::vector<Point> gen_line_segments(int n, int c) {
    int r_n;
    std::vector<Point> line_segments;
    r_n = gen_rint(1, n);
    for (int i = 0; i < r_n + 1; i++) {
        line_segments.push_back(gen_coordinate(c));
    }
    return line_segments;
}
Street gen_street(int n, int c) {
    Street street;
    std::vector<Point> line_segments;
    street.line_segments = gen_line_segments(n, c);
    street.name = gen_str(8);
    return street;
}
int gen_streets(int s, int n,int c) {
    
    std::vector<Line> ex_lines;
    int r_s = gen_rint(2,s);
    for (int i = 0; i < r_s; i++) {
        bool valid;
        int attempt = 0;
        std::vector<Line> lines;
        Street street;
        do {
            if (attempt < 25) {
                attempt++;
            } else {
                return 1;
            }
            street = gen_street(n, c);
            lines = add_lines(street.line_segments);
            valid = is_valid(lines, ex_lines);
        } while (!valid);

        ex_lines.insert(ex_lines.end(), lines.begin(), lines.end());
        names.push_back(street.name);
        
        std::cout << "add " << '\"' << street.name << '\"';
        int size = street.line_segments.size();
        for (int j = 0; j < size; j++) {
            std::cout << ' ' << "(" << street.line_segments[j].x << "," << street.line_segments[j].y << ")";
        }
        std::cout << std::endl;

    }
    return 0;
}
int remove_names(){
    int size = names.size();
    for(int j = 0; j < size; j++){
        std::cout << "rm " <<  '\"' << names[j] << '\"'<<std::endl;
    }
    names.clear();
    return 0;
}
int main(int argc, char *argv[]) {
    int opt;
    int s = 10, n = 5, l = 5, c = 20;

    while ((opt = getopt(argc, argv, "-:s:n:l:c:")) != -1) {
        int t;
        switch (opt) {
            case 's':
                t = atoi(optarg);
                if (t < 2) {
                    fprintf(stderr, "Error: Option %c arg too small\n", optopt);
                    return 1;
                } else
                    s = t;
                break;
            case 'n':
                t = atoi(optarg);
                if (t < 1) {
                    fprintf(stderr, "Error: Option %c arg too small\n", optopt);
                    return 1;
                } else
                    n = t;
                break;
            case 'l':
                t = atoi(optarg);
                if (t < 5) {
                    fprintf(stderr, "Error: Option %c arg too small\n", optopt);
                    return 1;
                } else
                    l = t;
                break;
            case 'c':
                t = atoi(optarg);
                if (t < 1) {
                    fprintf(stderr, "Error: Option %c arg too small\n", optopt);
                    return 1;
                } else
                    c = t;
                break;
            case '?':
                fprintf(stderr, "Error: Unknown option: %c\n", optopt);
                return 1;
            case ':':
                fprintf(stderr, "Error: Missing arg for %c\n", optopt);
                return 1;
            case 1:
                fprintf(stderr, "Error: Non-option arg: %s\n", optarg);
                return 1;
        }
    }
    while(true){
        remove_names();
        if (gen_streets(s,n,c)) {
            fprintf(stderr, "Error: failed to generate valid input for 25 simultaneous attempts\n");
            return 1;
        }
        std::cout << "gg" << std::endl;
        int r_l = gen_rint(5, l);
        sleep(r_l);
    }
    return 0;
}