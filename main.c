#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct Graph {
    bool** matrix;
    int verticesNum;
    int edgesNum;
} Graph_t;



/* checking the existence of the file */
void checkFile(const FILE* filePtr) {
    if (filePtr == NULL) {
        puts("Error opening file");
        exit(EXIT_FAILURE);
    }
}




int getFileLen(FILE* inputFilePtr) {
    fseek(inputFilePtr, 0, SEEK_END);
    int fileLen = ftell(inputFilePtr) - 1;
    fseek(inputFilePtr, 0, SEEK_SET);
    return fileLen;
}




char* getData(FILE* filePtr, int inputFileLen) {
    char* data;
    data = (char*) malloc(sizeof(char) * inputFileLen);
    fread(data, sizeof(char), inputFileLen, filePtr);
    data[inputFileLen] = '\0';
    return data;
}




int getNodes(const char* data, int inputFileLen) {
    int count = 1;
    for (int i = 0; i < inputFileLen; i++) {
        if (data[i] == '\n')
            count++;
    }
    return count;
}




int getEdgesNum(const char* data) {
    int count = 0;
    int i = 0;
    while (data[i] != '\n') {
        if (data[i] == '0' || data[i] == '1')
            count++;
        i++;
    }
    return count;
}




Graph_t* initGraph(int verticesNum, int edgesNum) {
    Graph_t* graph = (Graph_t*) malloc(sizeof(Graph_t));
    graph->verticesNum = verticesNum;
    graph->edgesNum = edgesNum;
    graph->matrix = (bool**) malloc(verticesNum * sizeof(bool*));
    for (int i = 0; i < verticesNum; i++) {
        graph->matrix[i] = (bool*) malloc(edgesNum * sizeof(bool));
    }
    return graph;
}




void strToGraph(Graph_t* graph, char* data) {
    char *token;
    for (int i = 0; i < graph->verticesNum; i++) {
        for (int j = 0; j < graph->edgesNum; j++) {
            token = strtok_r(data, "\n ", &data);
            if (*token == '0') {
                graph->matrix[i][j] = false;
            }
            else if (*token == '1') {
                graph->matrix[i][j] = true;
            }
        }
    }
}




void drawGraph(Graph_t* graph, FILE* outputFilePtr, char* outputFileName, const char* pngFileName) {
    fputs("digraph G {\n", outputFilePtr);
    fputs("\tedge[dir=none]\n", outputFilePtr);
    bool* hasConnections;
    hasConnections = (bool*) calloc(graph->verticesNum, sizeof(bool));
    for (int i = 0; i < graph->edgesNum; i++) {
        int buffer[2];
        int connectionCounter = 0;
        for (int j = 0; j < graph->verticesNum; j++) {
            if (graph->matrix[j][i] == true) {
                hasConnections[j] = true;
                buffer[connectionCounter] = j;
                connectionCounter++;
            }
        }
        if (connectionCounter > 0) {
            if (connectionCounter == 1)
                buffer[1] = buffer[0];
            fprintf(outputFilePtr, "\ta%d -> a%d\n", buffer[0], buffer[1]);
        }
    }
    for (int k = 0; k < graph->verticesNum; k++) {
        if (!hasConnections[k])
            fprintf(outputFilePtr, "\ta%d\n", k);
    }
    free(hasConnections);
    fputs("}", outputFilePtr);
    fflush(outputFilePtr);
    char* str;
    str = (char*) malloc(50 * sizeof(char));
    sprintf(str, "dot -Tpng %s -o %s", outputFileName, pngFileName);
    system(str);
    free(str);
}




void freeGraph(Graph_t* graph) {
    for (int i = 0; i < graph->verticesNum; i++) {
        free(graph->matrix[i]);
    }
    free(graph->matrix);
    free(graph);
}




/* HW3 */
void dfs(Graph_t* graph, int vertex, bool* visitedVertices) {
    visitedVertices[vertex] = true;
    for (int i = 0; i < graph->edgesNum; i++) {
        if (graph->matrix[vertex][i] == 1) {
            for (int j = 0; j < graph->verticesNum; j++) {
                if (graph->matrix[j][i] == 1 && visitedVertices[j] == 0) {
                    dfs(graph, j, visitedVertices);
                }
            }
        }
    }
}




int make_dfs(Graph_t* graph) {
    bool *visitedVertices;
    int verticesCount = 0;
    int i; // for loop

    visitedVertices = (bool*) calloc(graph->verticesNum, sizeof(bool));
    dfs(graph, 0, visitedVertices);

    for(i = 0; i < graph->verticesNum; i++) {
        if (visitedVertices[i] == true)
            verticesCount++;
    }

    free(visitedVertices);

    return verticesCount;
    }




void checkGraphConnection(Graph_t* graph) {
    int verticesCount = make_dfs(graph);

    if (graph->verticesNum == verticesCount)
        puts("Graph is connected");
    else
        puts("Graph is not connected");
}
/* HW3 */



int main() {
    FILE* inputFilePtr = fopen("../test.txt", "rb");
    checkFile(inputFilePtr);


    int inputFileLen = getFileLen(inputFilePtr);
    char* data = getData(inputFilePtr, inputFileLen);
    int verticesNum = getNodes(data, inputFileLen);
    int edgesNum = getEdgesNum(data);


    Graph_t* graph = initGraph(verticesNum, edgesNum);
    strToGraph(graph, data);
    free(data);
    fclose(inputFilePtr);


    FILE* outputFilePtr = fopen("../output.dot", "w");
    checkFile(outputFilePtr);
    drawGraph(graph, outputFilePtr, "../output.dot", "../output.png");
    fclose(outputFilePtr);

    checkGraphConnection(graph);

    freeGraph(graph);
    return EXIT_SUCCESS;
}