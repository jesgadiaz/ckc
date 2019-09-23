#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

void create(float []);
void down_adjust(float [],int);
int my_comparator(const void*, const void*, int);

int my_comparator(const void* a, const void* b, int n)
{
    double da = ((double*)a)[n];
    double db = ((double*)b)[n];
    return (da > db) ? 1 : ((da < db) ? -1 : 0);
}

void my_qsort(void* base, size_t num, size_t size,
    int (*comparator)(const void *, const void *, int), int field)
{
    /* Internal comperator */
    int my_qsort_comperator(const void* a, const void* b)
    {
        return comparator(a, b, field);
    }

    /* Invoke the base qsort function */
    qsort(base, num, size, my_qsort_comperator);
}

int floatcomp(const void* elem1, const void* elem2)
{
    if(*(const float*)elem1 < *(const float*)elem2)
        return -1;
    return *(const float*)elem1 > *(const float*)elem2;
}

void create(float heap[])
{
	int i,n;
	n=heap[0]; //no. of elements

	for(i=n/2;i>=1;i--)
		down_adjust(heap,i);
}

void down_adjust(float heap[],int i)
{
	int j,n,flag=1;
	float temp;
	n=heap[0];

	while(2*i<=n && flag==1)
	{
		j=2*i;    //j points to left child
		if(j+1<=n && heap[j+1] > heap[j])
			j=j+1;
		if(heap[i] > heap[j])
			flag=0;
		else
		{
			temp=heap[i];
			heap[i]=heap[j];
			heap[j]=temp;
			i=j;
		}
	}
}

const char* str1;
const char* str2;
const char* str3;
const char* algorithm;

int main(int argc, char** argv) {
    int seed;
    int n, k, L;
    float out;
    str1 = argv[1];
    n = atoi(argv[2]);
    k = atoi(argv[3]);
    L = atoi(argv[4]);
    out = atof(argv[5]);
    str2 = argv[6];
    str3 = argv[9];
    double total_time = 0;
    double exec_time[100];  // 100 executions with different seed
    static float matrix[400][400]; // maximum number of vertices = 400
    static float costs[160000];
    float vertices_x_y[400][2];
    int last_zero;
    int len = strlen(str1);
    int max_iter;
    max_iter = atoi(argv[7]);
    int num_repetitions = atoi(argv[8]);
    bool simple_interchange = false;
    bool multicenter;
    if(strcmp(str2,"true") == 0){
        multicenter = true;
    }else{
        multicenter = false;
    }

    // Extra variables
    static int NgL[400];
    static int NNgL[400];
    static float heap[400];
    static float heap2[400][2];
    static int bool_C[400];
    float temp;
    int last;
    static int capacity[100]; // max k
    static int assigned_center[400];
    static int best_assigned_center[400];
    int best_C[k];
    static int best_overall_assigned_center[400];
    int best_overall_C[k];

    // LOAD INSTANCE
    int i = 0;
    float cost;
    FILE* file = fopen(str1, "r");
    char line[50];
    if (file == NULL){
        printf("FILE NOT FOUND");
    }else{
        int v;
        float x, y;
        while(fgets(line, sizeof(line), file)) {
            int j = 0;
            char* token = strtok(line, " ");
            while (token) {
                if(j == 0){
                    v = atoi(token);
                }
                if(j == 1){
                    x = atof(token);
                }
                if(j == 2){
                    y = atof(token);
                    vertices_x_y[v-1][0] = x;
                    vertices_x_y[v-1][1] = y;
                }
                token = strtok(NULL, " ");
                j++;
            }
            free(token);
            i++;
        }

        fclose(file);
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                matrix[i][j] = sqrt( pow(vertices_x_y[i][0] - vertices_x_y[j][0],2)
                                   + pow(vertices_x_y[i][1] - vertices_x_y[j][1],2) );
            }
        }

        // GET ORDERED SET OF EDGE COSTS
        for(int i=0;i<sizeof(costs)/sizeof(float);i++){
                costs[i] = 0;
            }
        int p = 0;
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                costs[p] = matrix[i][j];
                p++;
            }
        }
        qsort(costs, sizeof(costs)/sizeof(float), sizeof(float), floatcomp);
        last_zero = 0;
        for(int i=0;i<sizeof(costs)/sizeof(float);i++){
            if(costs[i]!=0){
                last_zero = i - 1;
                break;
            }
        }
    }

    int seed_counter = 0;
    for(int iter=0;iter<max_iter;iter++){
        //out = iter;
        clock_t begin = clock();

        int C[k];
        float best_size;

        int farthest_vertex;
        float max_dist;
        int S[n];
        float distance[n];
        bool assigned[n];
        int score[n];
        int high = sizeof(costs)/sizeof(float);
        int low = last_zero;
        int mid;

        float overall_best_size = +INFINITY;
        while(high - low > 1){
            mid = (high + low) / 2;
            float r = costs[mid];
            float best_size = +INFINITY;
            for(int d=0;d<num_repetitions;d++){
                seed = seed_counter++;
                srand(seed);
                // INITIALIZATION
                for(int i=0;i<n;i++){
                    distance[i] = +INFINITY;
                    assigned[i] = false;
                    bool_C[i] = false;
                    score[i] = 0;
                }
                for(int i=0;i<n;i++){
                    for(int j=0;j<n;j++){
                        if(i!=j & matrix[i][j] <= r){
                            score[i] = score[i]+ 1;
                        }
                    }
                }
                for(int i=0;i<sizeof(capacity)/sizeof(capacity[0]);i++){
                    capacity[i] = L;
                }
                for(int i=0;i<n;i++){
                    assigned_center[i] = -1;
                }

                for(int i=0;i<k;i++){
                    if(i>0){
                        // UPDATE DISTANCE
                        for(int j=0;j<n;j++){
                            if(matrix[j][C[i-1]] < distance[j]){
                                distance[j] = matrix[j][C[i-1]];
                            }
                        }
                        max_dist = 0;
                        for(int j=0;j<n;j++){
                            if(distance[j] > max_dist){
                                max_dist = distance[j];
                                farthest_vertex = j;
                            }
                        }
                    }else{
                        if(num_repetitions == n){
                            farthest_vertex = d;
                        }else{
                            farthest_vertex = rand() % n;
                            //farthest_vertex = max_score_initial_vertex;
                        }
                    }

                    // IS THERE A CRITICAL NEIGHBOR WITH SCORE GREATER THAN L?
                    bool big_score = false;
                    for(int j=0;j<n;j++){
                        if(matrix[farthest_vertex][j] <= r && score[j] > L){
                            big_score = true;
                            break;
                        }
                    }

                    int s_size;
                    if(big_score){
                        int index = 0;
                        int card_NgL = 0;
                        // GET ALL THE NEIGHBORS WITH SCORE GREATER THAN L
                        for(int j=0;j<n;j++){
                            if(matrix[farthest_vertex][j] <= r && score[j] > L){
                                if(multicenter){
                                    NgL[index] = j;
                                    index++;
                                    card_NgL = index;
                                    //printf(", %d", j);
                                }else{
                                    if(!bool_C[j]){
                                        NgL[index] = j;
                                        index++;
                                        card_NgL = index;
                                        //printf(", %d", j);
                                    }
                                }
                            }
                        }
                        // EXPLORE ALL THE ELEMENTS IN NgL
                        // GET THE ONE WITH THE SMALLEST d_{L-1}
                        float min_dL = +INFINITY;
                        int min_dL_index = -1;
                        int f_ref_min = -1;
                        for(int j=0;j<card_NgL;j++){
                            index = 0;
                            int card_NNgL = 0;
                            // GET ALL THE NEIGHBORS OF NgL[j]
                            for(int t=0;t<n;t++){
                                if(matrix[NgL[j]][t] <= r && assigned[t] == false){
                                    NNgL[index] = t;
                                    index++;
                                    card_NNgL = index;
                                }
                            }
                            // GET THE (f_ref) FARTHEST VERTEX TO C union NgL[j]
                            max_dist = 0;
                            int f_ref = -1;

                            for(int t=0;t<n;t++){
                                if(matrix[t][NgL[j]] < distance[t]){
                                    if(matrix[t][NgL[j]] > max_dist){
                                        max_dist = matrix[t][NgL[j]];
                                        f_ref = t;
                                    }
                                }else{
                                    if(distance[t] > max_dist){
                                        max_dist = distance[t];
                                        f_ref = t;
                                    }
                                }
                            }

                            //GET THE DISTANCE FROM EVERY ELEMENT OF NNgL TO f_ref
                            for(int t=0;t<card_NNgL;t++){
                                heap[t] = matrix[NNgL[t]][f_ref];
                            }
                            //qsort(heap, sizeof(heap)/sizeof(float), sizeof(float), floatcomp);
                            qsort(heap, card_NNgL, sizeof(float), floatcomp);

                            if(heap[card_NNgL - L - 1] < min_dL){
                                min_dL = heap[card_NNgL - L - 1];
                                min_dL_index = j;
                                f_ref_min = f_ref;
                            }
                        }

                        // GET (AGAIN) ALL THE NEIGHBORS OF NgL[min_dL_index]
                        index = 0;
                        int card_NNgL = 0;
                        for(int t=0;t<n;t++){
                            if(matrix[NgL[min_dL_index]][t] <= r && assigned[t] == false){
                                NNgL[index] = t;
                                index++;
                                card_NNgL = index;
                            }
                        }
                        //GET THE DISTANCE FROM EVERY ELEMENT OF NNgL TO f_ref_min
                        for(int t=0;t<card_NNgL;t++){
                            heap2[t][0] = NNgL[t];
                            heap2[t][1] = matrix[NNgL[t]][f_ref_min];
                        }
                        my_qsort(heap2, card_NNgL, sizeof(heap2[0]), my_comparator, 0);

                        // Select NgL[min_dL_index] as center
                        C[i] = NgL[min_dL_index];
                        bool_C[C[i]] = true;
                        // ASSIGN THE L FARTHEST VERTICES TO NgL[min_dL_index]
                        s_size = 0;
                        for(int t=card_NNgL-L;t<card_NNgL;t++){
                            assigned_center[(int)heap2[t][0]] = C[i];
                            capacity[i] = capacity[i] - 1;
                            assigned[(int)heap2[t][0]] = true;
                            S[s_size] = (int)heap2[t][0];
                            s_size++;
                        }
                    }else{
                        // GET MAX SCORE CRITICAL NEIGHBOR
                        int max_score = -1;
                        int max_score_neighbor;
                        if(i > 0){
                            for(int j=0;j<n;j++){
                                if(matrix[j][farthest_vertex] <= r){
                                    if(score[j] > max_score){
                                        if(multicenter){
                                            max_score = score[j];
                                            max_score_neighbor = j;
                                        }else{
                                            if(!bool_C[j]){
                                                max_score = score[j];
                                                max_score_neighbor = j;
                                            }
                                        }

                                    }
                                }
                            }
                        }else{
                            max_score_neighbor = farthest_vertex;
                        }
                        C[i] = max_score_neighbor;
                        bool_C[C[i]] = true;
                        // UPDATE SCORE
                        s_size = 0;
                        for(int t=0;t<n;t++){
                            if(matrix[t][C[i]] <= r && assigned[t] == false){
                                assigned_center[t] = C[i];
                                capacity[i] = capacity[i] - 1;
                                assigned[t] = true;
                                S[s_size] = t;
                                s_size++;
                            }
                        }
                    }

                    // UPDATE SCORE
                    for(int j=0;j<s_size;j++){
                        for(int b=0;b<n;b++){
                            if(matrix[S[j]][b] <= r
                                    && S[j] != b){
                                score[b] = score[b] - 1;
                            }
                        }
                    }
                }
                // Assign the unassigned vertices to its nearest center with capacity
                for(int j=0;j<n;j++){
                    if(!assigned[j]){
                        float min_dist = +INFINITY;
                        int min_dist_center = -1;
                        for(int t=0;t<k;t++){
                            if(capacity[t] > 0 && matrix[j][C[t]] < min_dist){
                                min_dist = matrix[j][C[t]];
                                min_dist_center = t;
                            }
                        }
                        //assigned_center[j] = min_dist_center;
                        assigned_center[j] = C[min_dist_center];
                        assigned[j] = true;
                        capacity[min_dist_center] = capacity[min_dist_center] - 1;
                    }
                }

                max_dist = 0;
                bool improvement = true;
                int critical_vertex = -1;
                if(simple_interchange){
                    while(improvement){
                        //get the critical vertex
                        for(int j=0;j<n;j++){
                            if(matrix[j][assigned_center[j]] > max_dist){
                                max_dist = matrix[j][assigned_center[j]];
                                critical_vertex = j;
                            }
                        }
                        // get the nearest centers to critical_vertex
                        int nearest_centers[k];
                        int t = 0;
                        for(int j=0;j<k;j++){
                            if(matrix[critical_vertex][C[j]] < matrix[critical_vertex][assigned_center[critical_vertex]]){
                                nearest_centers[t] = C[j];
                                t++;
                            }
                        }
                        // check if the vertices assigned to closest center are closer to
                        // the center originally assigned to critical_vertex
                        bool flag = false;
                        for(int j=0;j<n;j++){
                            bool another_flag = false;
                            for(int g=0;g<t;g++){
                                if(assigned_center[j] == nearest_centers[g]){
                                    another_flag = true;
                                    break;
                                }
                            }
                            if(another_flag){
                                if(matrix[j][assigned_center[critical_vertex]] < matrix[critical_vertex][assigned_center[critical_vertex]]){
                                    int temp = assigned_center[j];
                                    assigned_center[j] = assigned_center[critical_vertex];
                                    assigned_center[critical_vertex] = temp;
                                    if(temp == assigned_center[j]){
                                        improvement = false;
                                    }else{
                                        improvement = true;
                                    }
                                    flag = true;
                                    break;
                                }
                            }
                        }
                        if(!flag){
                            improvement = false;
                        }
                    }
                }

                for(int j=0;j<n;j++){
                    heap[j] = matrix[j][assigned_center[j]];
                }
                qsort(heap, n, sizeof(float), floatcomp);
                // GET SOLUTION SIZE
                max_dist = heap[n - 1 - (int)ceil(out * (float)n)];

                if(max_dist < best_size){
                    best_size = max_dist;
                    for(int j=0;j<k;j++){
                        best_C[j] = C[j];
                    }
                    for(int j=0;j<n;j++){
                        best_assigned_center[j] = assigned_center[j];
                    }

                }
            }
            if(best_size < overall_best_size){
                overall_best_size = best_size;
                for(int j=0;j<n;j++){
                    best_overall_assigned_center[j] = best_assigned_center[j];
                }
                for(int j=0;j<k;j++){
                    best_overall_C[j] = best_C[j];
                }
            }
            if(overall_best_size <= r){
                high = mid;
            }else{
                low = mid;
            }
        }
        clock_t end = clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        total_time = total_time + time_spent;
        exec_time[iter] = time_spent;

        max_dist = 0;
        for(int j=0;j<n;j++){
            if(matrix[j][best_overall_assigned_center[j]] > max_dist){
                max_dist = matrix[j][best_overall_assigned_center[j]];
            }
        }


        // Get the outliers
        for(int j=0;j<n;j++){
            heap2[j][0] = j;
            heap2[j][1] = matrix[j][best_overall_assigned_center[j]];
        }
        my_qsort(heap2, n, sizeof(heap2[0]), my_comparator, 0);

        printf("\nSolution size (0 perc. outliers) = %f \n", heap2[n-1][1]);
        printf("Solution size (%.2f perc. outliers) = %f \n", out * 100, heap2[n - 1 - (int)ceil(out * (float)n)][1]);

    if(strcmp(str3,"true") == 0){
        printf("\n");
        printf("{ \"instance\": \"%s\", \n", str1);
        printf(" \"outliers\": [");
        int count2 = 0;
        for(int j = n-(int)ceil(out * (float)n);j<n;j++){
            if(count2 == 0){
                printf("%d",(int)heap2[j][0]);
                count2++;
            }else{
                printf(",%d",(int)heap2[j][0]);
            }
        }
        printf("], \n ");
        printf(" \"centers\": [ ");
        for(int j=0;j<k;j++){
            printf("{ \"center\": %d,", best_overall_C[j]);
            printf(" \"nodes\": [");
            count2 = 0;
            for(int t=0;t<n;t++){
                if(best_overall_assigned_center[t] == best_overall_C[j]){
                    if(count2 == 0){
                        printf("%d",t);

                        count2++;
                    }else{
                        printf(",%d",t);
                    }
                }
            }
            if(j == k-1){
                printf("]}");
            }else{
                printf("]},");
            }
            printf("\n");
        }
        printf("]}");
    }

    }

    printf("\n Exec. time per repetition: \n");
    for(int i=0;i<max_iter;i++){
        printf("%f, ", exec_time[i]);
    }
    printf("\n Total time: %f \n", total_time);
    printf("Average time: %f", total_time / max_iter);
    return (EXIT_SUCCESS);
}
