// Алгоритм Форда — Фалкерсона c++
#include <iostream>
#include <fstream>
using namespace std;

struct edge {
    int onEnd;
    int nextEdge;
    int firstEdge;
    int capacity;
    int visited;
};                          

void fillVisited(struct edge* edges, int num_of_edges);
void fillFirstEdge(struct edge* edges, int num_of_edges);
int addEdge(struct edge* edges, int u, int v, int cap, int edgeCount);
int findFlow(int u, int flow, int destinationVertex, struct edge* edges);
int** createMatrix(int** g, int n);
void printMatrix(int** g, int n);
void writeMatrixFile(int** g, int n, ofstream& out);
void freeMatrix(int** g, int n);

int main() {
    setlocale(LC_ALL, "");
    int numOfVertex, numOfEdge;                 // кол-во вершин, кол-во рёбер
    int sourceVertex, destinationVertex;        //исток, сток
    int edgeCount = 0, maxValue = 0, maxFlow = 0, iterationResult = 0;
    struct edge* edges;
    int** matrix = NULL, ** result_used = NULL, ** result_flow = NULL;
    const char inputFile[] = "matrix1.txt", outputFile[] = "result1.txt";
    ifstream in(inputFile);
    in >> numOfVertex;
    in >> numOfEdge;
    in >> sourceVertex;
    in >> destinationVertex;
    cout << "Источник = " << sourceVertex << endl << "Сток = " << destinationVertex << endl << endl;
    sourceVertex--;
    destinationVertex--;
    edges = (edge*)malloc(2 * numOfEdge * sizeof(edge));
    fillVisited(edges, numOfEdge);
    fillFirstEdge(edges, numOfEdge);
    matrix = createMatrix(matrix, numOfVertex);
    for (int i = 0; i < numOfVertex; i++)  // цикл по строкам
    {
        for (int j = 0; j < numOfVertex; j++)  // цикл по столбцам строки
        {
            in >> matrix[i][j];
            if (matrix[i][j] > 0) {
                if (matrix[i][j] > maxValue) {
                    maxValue = matrix[i][j];
                }
                edgeCount = addEdge(edges, i, j, matrix[i][j], edgeCount);
            }
        }
    }
    maxValue++;
    printMatrix(matrix, numOfVertex);
    in.close();
    // Нахождение максимального потока
    while ((iterationResult = findFlow(sourceVertex, maxValue, destinationVertex, edges)) > 0) {
        fillVisited(edges, numOfEdge);
        maxFlow += iterationResult;
    }

    result_flow = createMatrix(result_flow, numOfVertex);
    result_used = createMatrix(result_used, numOfVertex);
    for (int i = 0; i < (2 * numOfEdge); i = i + 2) {
        result_flow[(edges + i + 1)->onEnd][(edges + i)->onEnd] = (edges + i)->capacity;
        result_used[(edges + i + 1)->onEnd][(edges + i)->onEnd] = (edges + i + 1)->capacity;
    }
    printMatrix(result_flow, numOfVertex);
    printMatrix(result_used, numOfVertex);
    // Выводим максимальный поток
    cout << "Максимальный поток - " << maxFlow << endl;

    ofstream out(outputFile);
    writeMatrixFile(result_flow, numOfVertex, out);
    writeMatrixFile(result_used, numOfVertex, out);
    out << "Максимальный поток - " << maxFlow << endl;
    out.close();
    freeMatrix(matrix, numOfVertex);
    freeMatrix(result_used, numOfVertex);
    freeMatrix(result_flow, numOfVertex);
    free(edges);
    return 0;
}

void fillVisited(struct edge* edges, int num_of_edges) {
    for (int i = 0; i < (num_of_edges * 2); i++) {
        (edges + i)->visited = false;
    }
}

void fillFirstEdge(struct edge* edges, int num_of_edges) {
    for (int i = 0; i < (num_of_edges * 2); i++) {
        (edges + i)->firstEdge = -1;
    }
}

int addEdge(struct edge* edges, int u, int v, int cap, int edgeCount) {
    // Прямое ребро
    (edges + edgeCount)->onEnd = v;
    (edges + edgeCount)->nextEdge = (edges + u)->firstEdge;
    (edges + u)->firstEdge = edgeCount;
    (edges + edgeCount)->capacity = cap;
    edgeCount++;
    // Обратное ребро
    (edges + edgeCount)->onEnd = u;
    (edges + edgeCount)->nextEdge = (edges + v)->firstEdge;
    (edges + v)->firstEdge = edgeCount;
    (edges + edgeCount)->capacity = 0;
    edgeCount++;
    return edgeCount;
}

int findFlow(int u, int flow, int destinationVertex, struct edge* edges) {
    if (u == destinationVertex) return flow; // возвращяем полученный минимум на пути
    (edges + u)->visited = true;
    for (int edge = (edges + u)->firstEdge; edge != -1; edge = (edges + edge)->nextEdge) {
        int to = (edges + edge)->onEnd;
        if (!(edges + to)->visited && (edges + edge)->capacity > 0) {
            int minResult = findFlow(to, min(flow, (edges + edge)->capacity), destinationVertex, edges); // ищем поток в поддереве
            if (minResult > 0) {                                // если нашли
                (edges + edge)->capacity -= minResult;          // у прямых ребер вычетаем поток
                (edges + (edge ^ 1))->capacity += minResult;    // к обратным прибавляем
                return minResult;
            }
        }
    }
    return 0; // если не нашли поток из этой вершины вернем 0
}

int** createMatrix(int** g, int n) {
    g = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        *(g + i) = (int*)malloc(n * sizeof(int)); //выделяем память под значения столбцов
        for (int j = 0; j < n; j++) {
            g[i][j] = 0;
        }
    }
    return g;
}

void printMatrix(int** g, int n) {
    cout << "  ";
    for (int i = 0; i < n; i++) {
        cout.width(3);
        cout << i + 1;
    }
    cout << endl << endl;
    for (int i = 0; i < n; i++) {
        cout << i + 1 << " ";
        for (int j = 0; j < n; j++)
        {
            cout.width(3);
            cout << *(*(g + i) + j);
        }
        cout << endl;
    }
    cout << endl << endl;
}

void writeMatrixFile(int** g, int n, ofstream& out) {
    out << "  ";
    for (int i = 0; i < n; i++) {
        out.width(3);
        out << i + 1;
    }
    out << endl << endl;
    for (int i = 0; i < n; i++) {
        out << i + 1 << " ";
        for (int j = 0; j < n; j++)
        {
            out.width(3);
            out << *(*(g + i) + j);
        }
        out << endl;
    }
    out << endl << endl;
}

void freeMatrix(int** g, int n) {
    for (int i = 0; i < n; i++) {
        free(*(g + i)); //освобождаем память
    }
    free(g); //освобождаем память
}