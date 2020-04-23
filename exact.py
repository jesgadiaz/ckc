from gurobipy import *
import math
import numpy as np
import heapq

def heap_sort(items):
    heapq.heapify(items)
    items[:] = [heapq.heappop(items) for i in range(len(items))]
    return items

def createGraph(input_file, instance_format):
    global n, m , k, matrix, ordered_sizes
    if instance_format == 'orlib':
        f = open(input_file, "r")
        matrix = []
        for i in range(0,n):
            list = []
            for j in range(0,n):
                list.append(float("inf"))
            matrix.append(list)
        m = sum(1 for line in open(input_file))
        #with open(input_file, "r") as f:  
        for i in range(0, m):
            string = f.readline()
            string = string.split()
            if string is not "EOF":
                v1 = int(string[0]) - 1
                v2 = int(string[1]) - 1
                weight = int(string[2])
                matrix[v1][v2] = weight
                matrix[v2][v1] = weight
        f.close()
        
        for i in range(0, n):
            matrix[i][i] = 0
        for i in range(0, n):
            #print(i)
            for j in range(0, n):
                for l in range(0, n):
                    if matrix[i][j] == float("inf") or matrix[i][l] == float("inf"):
                        cost = float("inf")
                    else:
                        cost = matrix[i][j] + matrix[i][l]
                    if cost < matrix[j][l]:
                        matrix[j][l] = cost
                        
        ordered_sizes = []
        for i in range(0, n):
            for j in range(i, n):
                ordered_sizes.append(matrix[i][j])
        ordered_sizes = heap_sort(ordered_sizes)
    elif instance_format == 'tsplib':
        f = open(input_file, "r")
        m = n
        matrix = []
        for i in range(0,n):
            list = []
            for j in range(0,n):
                list.append(float("inf"))
            matrix.append(list)
        positions = []
        for i in range(0, m):
            string = f.readline()
            string = string.split()
            temp_position = []
            temp_position.append(int(string[0])-1)
            temp_position.append(float(string[1]))
            temp_position.append(float(string[2]))
            positions.append(temp_position)
        for i in range(0, n):
            for j in range(0, n):
                dist_temp = math.sqrt(((positions[i][1] - positions[j][1]) * (positions[i][1] - positions[j][1])) + ((positions[i][2] - positions[j][2]) * (positions[i][2] - positions[j][2])))
                matrix[i][j] = dist_temp
                matrix[j][i] = dist_temp
        f.close()
        for i in range(0, n):
            matrix[i][i] = 0
            
        ordered_sizes = []
        for i in range(0, n):
            for j in range(i, n):
                ordered_sizes.append(matrix[i][j])
        ordered_sizes = heap_sort(ordered_sizes)
    
        
    
def run(r):
    global total_runtime, k, runtime, num_centers, m, cap, input_file
    prunedMatrix = []
    for i in range(0,n):
        list = []
        for j in range(0,n):
            list.append(float(0))
        prunedMatrix.append(list)
    for i in range(0,n):
        for j in range(0,n):
            if matrix[i][j] <= r:
                prunedMatrix[i][j] = 1
    
    try:
        global m, num_centers, runtime, cap
        m = Model("mip1")
        
        #******************************************************************************************************
        m.setParam("MIPGap", 0.0);
        #******************************************************************************************************
        
        y = []
        for i in range(n):
            y.append(0)
        
        for i in range(n):
            y[i] = m.addVar(vtype=GRB.BINARY, name="y%s" % str(i+1))
        
        
        m.setObjective(sum(y), GRB.MINIMIZE)
        
        temp_list = np.array(prunedMatrix).T.tolist()
        
        for i in range(n):
            m.addConstr(sum(np.multiply(temp_list[i], y).tolist()) >= 1)
        
        x = []
        
        for i in range(n):
            temp = []
            for j in range(n):
                temp.append(0)
            x.append(temp)
        
        for i in range(n):
            for j in range(n):
                x[i][j] = m.addVar(vtype=GRB.BINARY, name="x%s%s" % (str(i+1), str(j+1)))
        
        temp_list_2 = np.array(x).T.tolist()
        
        for i in range(n):
            m.addConstr(sum(temp_list_2[i]) * y[i] <= L)
    
        for i in range(n):
            for j in range(n):    
                #m.addConstr(x[i][j] <= y[j] * prunedMatrix[i][j])
                #******************************************************************************************************
                m.addConstr(x[i][j] <= y[j] * prunedMatrix[i][j] * (1-y[i]))
                #******************************************************************************************************
        
        for i in range(n):
            #m.addConstr(sum(x[i]) == 1)
            #******************************************************************************************************
            m.addConstr(sum(x[i]) == 1 * (1-y[i]))
            #******************************************************************************************************
        
        m.optimize()
        runtime = m.Runtime
        print("The run time is %f" % runtime)
        print("Obj:", m.objVal)
        
        #******************************************************************************************************
        dom_set_size = 0
        solution = []
        assignment = []
        center = 0
        vertex_j = 1
        vertex_i = 1
        for v in m.getVars():
            varName = v.varName
            if varName[0] == 'y':
                if v.x == 1.0:
                    dom_set_size = dom_set_size + 1
                    solution.append(varName[1:])
            else:
                if vertex_j <= n:
                    if v.x == 1.0:
                        assignment.append([vertex_i, vertex_j])
                else:
                    vertex_i = vertex_i + 1
                    vertex_j = 1
                vertex_j = vertex_j + 1
        print("Cap. dom. set cardinality: " + str(dom_set_size))
        solution = [int(i) for i in solution] 
        #print("solution: " + str(solution))
        #print("assignment: " + str(assignment))
        
        print('{"instance": "%s",' % input_file)
        print('"centers": [')
        counter = 0
        for center in solution:
            counter = counter + 1
            nodes = []
            for node in assignment:
                if node[1] == center:
                    nodes.append(node[0])
            if counter == len(solution):
                print('{ "center": ' + str(center) + ', "nodes": ' + str(nodes) + '}')
            else:
                print('{ "center": ' + str(center) + ', "nodes": ' + str(nodes) + '},')
        print(']}')
        
            #print('%s %g' % (v.varName, v.x))
        #******************************************************************************************************
            
        
   # {"instance": "/home/ckc/Escritorio/pr124.tsp",
   #  "outliers": [83,40,115,114], 
   #  "centers": [ { "center": 59, "nodes": [28,32,33,34,35,54,57,58,59,60,61,64,65]},
   #              { "center": 102, "nodes": [101,102,103,104,105,106,107,108,109,110,111,112,113]},
   #              { "center": 8, "nodes": [8,9,10,11,12,13,14,15,16,46,47,48,49]},
   #              { "center": 79, "nodes": [77,78,79,91,92,93,94,95,96,97,98,99,123]},
   #              { "center": 6, "nodes": [0,1,2,3,4,5,6,7,26,27,29,30,31]},
   #              { "center": 36, "nodes": [19,20,21,22,23,24,25,36,37,38,39,55,56]},
   #              { "center": 16, "nodes": [17,18,40,41,42,43,44,45,50,51,52,53]},
   #              { "center": 96, "nodes": [72,73,74,75,76,80,116,117,118,119,120,121,122]},
   #              { "center": 89, "nodes": [84,85,86,87,88,89,90,100]},
   #              { "center": 64, "nodes": [62,63,66,67,68,69,70,71,81,82,83,114,115]}
   #  ]}
        
        
        num_centers = dom_set_size
#        num_centers = m.objVal
        
    except GurobiError:
        print("Error reported")
        
        
def binarySearch():
    global total_runtime, k, runtime, num_centers, input_file
    total_runtime = 0
    not_done = True
    upper = len(ordered_sizes) - 1
    lower = 0
    best_solution_size = float("inf")
    while not_done:
        #mid = math.ceil(lower + ((upper - lower)/2))
        mid = math.ceil((upper + lower) /2)
        mid_value = ordered_sizes[int(mid)]
        if mid == upper:
            not_done = False
            run(mid_value)
            total_runtime = total_runtime + runtime
        else:
            run(mid_value)
            total_runtime = total_runtime + runtime
            if num_centers <= k:
                upper = mid
                print("UPPER = MID")
                if mid_value <= best_solution_size:
                    best_solution_size = mid_value
            else:
                lower = mid
                print("LOWER = MID")
    print("best solution size: " + str(best_solution_size))
    print("total runtime: " + str(total_runtime))
    
if __name__ == "__main__":
    global total_runtime, k, runtime, num_centers, L, n
    if len(sys.argv) != 6:
        print ("Wrong number of arguments")
        print ("exact input_file_path n k L instance_format")
        sys.exit()
    input_file  = sys.argv[1]
    n = int(sys.argv[2])
    k = int(sys.argv[3])
    L = int(sys.argv[4])
    instance_format = sys.argv[5]
    createGraph(input_file, instance_format)
    binarySearch()
