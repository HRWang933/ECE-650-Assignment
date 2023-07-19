#!/usr/bin/env python3
import sys
import re
import math

# YOUR CODE GOES HERE

name_rx = r'(\"([a-zA-Z0-9]+\s)*[a-zA-Z0-9]+\")'
num_rx = r'-?\d+'
coord_rx = r'\('+num_rx+','+num_rx+'\)'
cmd_a_rx = r'(add)\s'+name_rx+'(\s'+coord_rx+'){2,}$'
cmd_m_rx = r'(mod)\s'+name_rx+'(\s'+coord_rx+'){2,}$'
cmd_r_rx = r'(rm)\s'+name_rx+'$'
cmd_g_rx = '(gg)$'
coord_chk = re.compile(coord_rx)
cmd_a_chk = re.compile(cmd_a_rx)
cmd_m_chk = re.compile(cmd_m_rx)
cmd_r_chk = re.compile(cmd_r_rx)
cmd_g_chk = re.compile(cmd_g_rx)


def pp(x):
    """Returns a pretty-print string representation of a number.
    A float number is represented by an integer, if it is whole,
    and up to two decimal places if it isn't
    """
    if isinstance(x, float):
        if x.is_integer():
            return str(int(x))
        else:
            return "{0:.2f}".format(x)
    return str(x)

def check_negative_zero (x):
    if x==0:
        if math.copysign(-1, x)==-1:
            return True
    return False

class Point():
    """A point in a two dimensional space"""

    def __init__(self, x, y):
        self.x = float(x)
        self.y = float(y)

    def __repr__(self):
        return '(' + pp(self.x) + ',' + pp(self.y) + ')'

    def __eq__(self, other):
        if abs(self.x-other.x) < 0.0001 and abs(self.y-other.y) < 0.0001:
            return True
        return False
    def __hash__(self):
        return hash((self.x, self.y))


class Line():
    """A line between two points"""

    def __init__(self, src, dst):
        self.intersect=[]
        self.src = src
        self.dst = dst
        self.is_get_intersect=False
        parent = ""

    def __repr__(self):
        return '[' + str(self.src) + '-->' + str(self.dst) + ']'

    def __eq__(self, other):
        if self.src == other.src and self.dst == other.dst:
            return True
        if self.src == other.dst and self.dst == other.src:
            return True
        return False
    def __hash__(self):
        return hash((self.src, self.dst))

class Street():
    def __init__(self, name, coords):
        self.name = name
        self.coords = coords


class Streets():
    street = []

    def is_name_duplicated(self, name):
        for x in range(len(self.street)):
            if name.casefold() == self.street[x].name.casefold():
                return True
        return False

    def add(self, name, coords_s):
        if self.is_name_duplicated(name):
            raise Exception ("Error: Can't add street with same name.\n")
        coords = []
        for i in range(len(coords_s)):
            x, y = map(float, coords_s[i].strip('()').split(','))
            #check negative zero
            if check_negative_zero(x) or check_negative_zero(y):
                raise Exception ("Error: Don't support -0.")

            point = Point(x, y)
            coords.append(point)
        self.street.append(Street(name, coords))
        return 0

    def mod(self, name, coords_s):
        if not self.is_name_duplicated(name):
            raise Exception ("Error: Can't mod street didn't exist.\n")
        coords = []
        for i in range(len(coords_s)):
            x, y = map(float, coords_s[i].strip('()').split(','))
            #check negative zero
            if check_negative_zero(x) or check_negative_zero(y):
                raise Exception ("Error: Don't support -0.")
            point = Point(x, y)
            coords.append(point)
        for i in range(len(self.street)):
            if self.street[i].name.casefold() == name.casefold():
                self.street[i].name = name
                self.street[i].coords = coords
                break
        return 0

    def rm(self, name):
        if not self.is_name_duplicated(name):
            raise Exception ("Error: Can't rm street didn't exist.\n")
        for i in range(len(self.street)):
            if self.street[i].name.casefold() == name.casefold():
                del self.street[i]
                break
        return 0


def line_rm_du(line):
    remove_indices = []
    for i in range(len(line)):
        for j in range(i+1, len(line)):
            if line[i] == line[j]:
                remove_indices.append(j)
    line = [i for j, i in enumerate(line) if j not in remove_indices]
    return line

# Given three collinear points p, q, r, the function checks if
# point q lies on line segment 'pr'


def onSegment(p, q, r):
    if ((q.x <= max(p.x, r.x)) and (q.x >= min(p.x, r.x)) and
            (q.y <= max(p.y, r.y)) and (q.y >= min(p.y, r.y))):
        return True
    return False


def orientation(p, q, r):
    # to find the orientation of an ordered triplet (p,q,r)
    # function returns the following values:
    # 0 : Collinear points
    # 1 : Clockwise points
    # 2 : Counterclockwise
    val = (float(q.y - p.y) * (r.x - q.x)) - (float(q.x - p.x) * (r.y - q.y))
    if (val > 0):
        # Clockwise orientation
        return 1
    elif (val < 0):
        # Counterclockwise orientation
        return 2
    else:
        # Collinear orientation
        return 0

# The main function that returns true if
# the line segment 'p1q1' and 'p2q2' intersect.


def doIntersect(line_a, line_b):
    # Find the 4 orientations required for
    # the general and special cases
    p1 = line_a.src
    p2 = line_b.src
    q1 = line_a.dst
    q2 = line_b.dst
    o1 = orientation(p1, q1, p2)
    o2 = orientation(p1, q1, q2)
    o3 = orientation(p2, q2, p1)
    o4 = orientation(p2, q2, q1)
    # General case
    if ((o1 != o2) and (o3 != o4)):
        return True
    # Special Cases
    # p1 , q1 and p2 are collinear and p2 lies on segment p1q1
    if ((o1 == 0) and onSegment(p1, p2, q1)):
        return True
    # p1 , q1 and q2 are collinear and q2 lies on segment p1q1
    if ((o2 == 0) and onSegment(p1, q2, q1)):
        return True
    # p2 , q2 and p1 are collinear and p1 lies on segment p2q2
    if ((o3 == 0) and onSegment(p2, p1, q2)):
        return True
    # p2 , q2 and q1 are collinear and q1 lies on segment p2q2
    if ((o4 == 0) and onSegment(p2, q1, q2)):
        return True
    # If none of the cases
    return False


def intersect(l1, l2):
    x1, y1 = l1.src.x, l1.src.y
    x2, y2 = l1.dst.x, l1.dst.y
    x3, y3 = l2.src.x, l2.src.y
    x4, y4 = l2.dst.x, l2.dst.y

    xnum = ((x1*y2-y1*x2)*(x3-x4) - (x1-x2)*(x3*y4-y3*x4))
    xden = ((x1-x2)*(y3-y4) - (y1-y2)*(x3-x4))
    xcoor = xnum / xden

    ynum = (x1*y2 - y1*x2)*(y3-y4) - (y1-y2)*(x3*y4-y3*x4)
    yden = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4)
    ycoor = ynum / yden

    return Point(xcoor, ycoor)


def slopee(line):
    if not line.dst.x == line.src.x:
        x = (line.dst.y - line.src.y) / (line.dst.x - line.src.x)
        return x
    else:
        return None
def distance(point1,point2):

    dist = math.sqrt((point2.x - point1.x) ** 2 + (point2.y - point1.y) ** 2)
    return dist

class Graph():

    def __init__(self, streets):
        self.street = streets.street
        self.line=[]
        self.vs=[]
        self.es=[]

    def streets_to_line(self):
        for i in range(len(self.street)):
            for j in range(len(self.street[i].coords)):
                if j == len(self.street[i].coords)-1:
                    break
                t_line=Line(self.street[i].coords[j], self.street[i].coords[j+1])
                t_line.parent= self.street[i].name
                self.line.append(t_line)

    def gg(self):
        self.streets_to_line()
        # self.line=line_rm_du(self.line)
        for i in range(len(self.line)):
            for j in range(i+1, len(self.line)):
                self.line_line(self.line[i], self.line[j])
        #clean line's intersection duplicate
        for i in range(len(self.line)):
            self.line[i].intersect= list(set(self.line[i].intersect))
        #clean vs duplicate
        self.vs= list(set(self.vs))
        #intersect-intersect, intersect-end
        for i in range(len(self.vs)):
            for j in range(len(self.line)):
                if self.line[j].is_get_intersect==False:
                    continue
                if self.vs[i] == self.line[j].src:
                    t_point=self.line[j].intersect.copy()
                    t_point.append(self.line[j].dst)
                    try:
                        while True:
                            t_point.remove(self.vs[i])
                    except ValueError:
                        pass
                    if len(t_point) != 0:
                        t_point_distance=[]
                        for z in range(len(t_point)):
                            t_point_distance.append(distance(self.vs[i],t_point[z]))
                        min_index=t_point_distance.index(min(t_point_distance))
                        self.es.append(Line(self.vs[i],t_point[min_index]))
                if self.vs[i] == self.line[j].dst:
                    t_point=self.line[j].intersect.copy()
                    t_point.append(self.line[j].src)
                    try:
                        while True:
                            t_point.remove(self.vs[i])
                    except ValueError:
                        pass
                    if len(t_point) != 0:
                        t_point_distance=[]
                        for z in range(len(t_point)):
                            t_point_distance.append(distance(self.vs[i],t_point[z]))
                        min_index=t_point_distance.index(min(t_point_distance))
                        self.es.append(Line(self.vs[i],t_point[min_index]))
                if self.vs[i] in self.line[j].intersect:
                    t_point=[]
                    t_point.append(self.line[j].src)
                    for x in range(len(self.line[j].intersect)):
                        if onSegment(self.line[j].src,self.line[j].intersect[x],self.vs[i]):
                            t_point.append(self.line[j].intersect[x])
                    try:
                        while True:
                            t_point.remove(self.vs[i])
                    except ValueError:
                        pass
                    if len(t_point) != 0:
                        t_point_distance=[]
                        for z in range(len(t_point)):
                            t_point_distance.append(distance(self.vs[i],t_point[z]))
                        min_index=t_point_distance.index(min(t_point_distance))
                        self.es.append(Line(self.vs[i],t_point[min_index]))

                    t_point=[]
                    t_point.append(self.line[j].dst)
                    for x in range(len(self.line[j].intersect)):
                        if onSegment(self.vs[i],self.line[j].intersect[x],self.line[j].dst):
                            t_point.append(self.line[j].intersect[x])
                    try:
                        while True:
                            t_point.remove(self.vs[i])
                    except ValueError:
                        pass
                    if len(t_point) != 0:
                        t_point_distance=[]
                        for z in range(len(t_point)):
                            t_point_distance.append(distance(self.vs[i],t_point[z]))
                        min_index=t_point_distance.index(min(t_point_distance))
                        self.es.append(Line(self.vs[i],t_point[min_index]))
        # #end-end   
        vs_end=[]
        for i in range(len(self.line)):
            if self.line[i].is_get_intersect==True:
                vs_end.append(self.line[i].src)
                vs_end.append(self.line[i].dst)
        # for i in range(len(self.line)):
        #     if self.line[i].is_get_intersect==False:
        #         if self.line[i].src in vs_end:
        #             if self.line[i].dst in vs_end:
        #                 self.es.append(self.line[i])
        self.vs.extend(vs_end)
        #clean vs duplicate
        self.vs= list(set(self.vs))
        #clean es duplicate
        self.es= line_rm_du(self.es)
        self.display()

        






    def line_line(self, line_a, line_b):
        if line_a.parent==line_b.parent:
            return 'same street'

        if line_a == line_b:
            line_a.is_get_intersect = True
            line_b.is_get_intersect = True
            line_a.intersect.append(line_b.src)
            line_a.intersect.append(line_b.dst)
            line_b.intersect.append(line_a.src)
            line_b.intersect.append(line_a.dst)
            self.vs.append(line_a.src)
            self.vs.append(line_a.dst)
            return 'same line'

        if doIntersect(line_a, line_b):
            if slopee(line_a) == slopee(line_b):  # overlap
                if onSegment(line_a.src, line_b.src, line_a.dst) == True and onSegment(line_a.src, line_b.dst, line_a.dst) == True:
                    line_a.is_get_intersect = True
                    line_b.is_get_intersect = True
                    line_a.intersect.append(line_b.src)
                    line_a.intersect.append(line_b.dst)
                    line_b.intersect.append(line_b.src)
                    line_b.intersect.append(line_b.dst)
                    self.vs.append(line_b.src)
                    self.vs.append(line_b.dst)
                    return 'b on a'
                elif onSegment(line_b.src, line_a.src, line_b.dst) == True and onSegment(line_b.src, line_a.dst, line_b.dst) == True:
                    line_a.is_get_intersect = True
                    line_b.is_get_intersect = True
                    line_a.intersect.append(line_a.src)
                    line_a.intersect.append(line_a.dst)
                    line_b.intersect.append(line_a.src)
                    line_b.intersect.append(line_a.dst)
                    self.vs.append(line_a.src)
                    self.vs.append(line_a.dst)
                    return 'a on b'
                elif onSegment(line_a.src, line_b.src, line_a.dst) == True:
                    if onSegment(line_b.src, line_a.src, line_b.dst) == True:
                        line_a.is_get_intersect = True
                        line_b.is_get_intersect = True
                        line_a.intersect.append(line_a.src)
                        line_a.intersect.append(line_b.src)
                        line_b.intersect.append(line_a.src)
                        line_b.intersect.append(line_b.src)
                        self.vs.append(line_a.src)
                        self.vs.append(line_b.src)
                        return 'b2a1b1a2'
                    elif onSegment(line_b.src, line_a.dst, line_b.dst) == True:
                        line_a.is_get_intersect = True
                        line_b.is_get_intersect = True
                        line_a.intersect.append(line_b.src)
                        line_a.intersect.append(line_a.dst)
                        line_b.intersect.append(line_b.src)
                        line_b.intersect.append(line_a.dst)
                        self.vs.append(line_b.src)
                        self.vs.append(line_a.dst)
                        return 'a1b1a2b2'
                elif onSegment(line_a.src, line_b.dst, line_a.dst) == True:
                    if onSegment(line_b.src, line_a.src, line_b.dst) == True:
                        line_a.is_get_intersect = True
                        line_b.is_get_intersect = True
                        line_a.intersect.append(line_a.src)
                        line_a.intersect.append(line_b.dst)
                        line_b.intersect.append(line_a.src)
                        line_b.intersect.append(line_b.dst)
                        self.vs.append(line_a.src)
                        self.vs.append(line_b.dst)
                        return 'b1a1b2a2'
                    elif onSegment(line_b.src, line_a.dst, line_b.dst) == True:
                        line_a.is_get_intersect = True
                        line_b.is_get_intersect = True
                        line_a.intersect.append(line_a.dst)
                        line_a.intersect.append(line_b.dst)
                        line_b.intersect.append(line_a.dst)
                        line_b.intersect.append(line_b.dst)
                        self.vs.append(line_a.dst)
                        self.vs.append(line_b.dst)
                        return 'a1b2a2b1'

            else:
                point = intersect(line_a, line_b)
                line_a.is_get_intersect = True
                line_b.is_get_intersect = True
                line_a.intersect.append(point)
                line_b.intersect.append(point)
                self.vs.append(point)
                return 'intersect'
        return 'not intersect'

    def coord2index(self, coord):
        if coord in self.vs:
            return self.vs.index(coord)
        else:
            return False
    def display(self):
        sys.stdout.flush()
        sys.stdout.write('V '+str(len(self.vs))+'\n')
        sys.stdout.write( 'E {')
        for x in range(0, len(self.es)):
            if x != len(self.es) - 1:
                sys.stdout.write('<' + str(self.coord2index(self.es[x].src)+1)+ ',' + str(self.coord2index(self.es[x].dst)+1) + '>' + ',')
            else:
                sys.stdout.write('<' + str(self.coord2index(self.es[x].src)+1) + ',' + str(self.coord2index(self.es[x].dst)+1) + '>')
        sys.stdout.write ('}'+'\n')
        sys.stdout.flush()


def main():
    # YOUR MAIN CODE GOES HERE

    # sample code to read from stdin.
    # make sure to remove all spurious print statements as required
    # by the assignment
    streets = Streets()
    def parse(line):
        regexList = [cmd_a_chk, cmd_m_chk, cmd_r_chk, cmd_g_chk]
        for regex in regexList:
            match = re.match(regex, line)
            if match is not None:
                match_groups = match.groups()
                #strip coords
                coords_s = re.findall(coord_chk, line)
                #strip whitespace
                # for x in range(len(coords_s)):
                #     coords_s[x]=coords_s[x].replace(" ", "")
                # #check -0
                # for x in range(len(coords_s)):
                #     if "(-0," in coords_s[x]:
                #         raise Exception ("Error: Don't support -0.")

                #     if "-0)" in coords_s[x]:
                #         raise Exception ("Error: Don't support -0.")

                if match_groups[0] == "add":
                    streets.add(match_groups[1], coords_s)
                elif match_groups[0] == "mod":
                    streets.mod(match_groups[1], coords_s)
                elif match_groups[0] == "rm":
                    pass
                    #streets.rm(match_groups[1])
                elif match_groups[0] == "gg":
                    graph = Graph(streets)
                    graph.gg()
                    streets.street=[]
                break
        if line == "":
            return    
        if match == None:
            raise Exception ("Error: Didn't found a valid commend.")

    while True:
        line = sys.stdin.readline()
        #print("read a line:", line)
        try:
            parse(line)
        except Exception as error:
            print('Caught this error: ' + repr(error))
        if line == "":
            break

    #print("Finished reading input")
    # return exit code 0 on successful termination
    sys.exit(0)


if __name__ == "__main__":
    main()
