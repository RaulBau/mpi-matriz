/*
-Materia: 
    Supercómputo.

-Semestre: 
    2020-2021 I.

-Nombre de los alumnos: 
    Bautista Robles Raul                    229563
    Negrete Espino Jose Luis de Jesus       268038
    Rueda Pérez Mauricio Daniel             261582

-Carrera: 
    Ingenieria en computación.

-Nombre de tarea o programa: 
    Programa MPI: Multiplicacion de matrices.

-Periodo de evaluación: 
    Parcial 3.

-Descripción: 
    Multiplica 2 matrices paralelamente con funciones mpi_scatterv y mpi_gatherv.

-Avance logrado: 
    

-Comentarios adicionales: 
    
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mpi.h"

#define NUM_COLUMNAS1 3
#define NUM_FILAS1 3

#define NUM_COLUMNAS2 3
#define NUM_FILAS2 3

void llenar_matriz(float *arreglo, int tam_matriz);
float multiplica_RenglonColumna(float fila[], float columna[]);
void imprime_matriz_resultante(float matriz1[], float matriz2[], float matriz_resultante[]);

int main(int argc, char *argv[])
{
    int id_proceso, num_procesos, num_datos, num_datos_u; // Variables generales

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id_proceso);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procesos);

    //Verificar solo en el proceso 0 si los numeros de datos son divisibles para el numero de procesos
    if (id_proceso == 0 && NUM_COLUMNAS1 != NUM_FILAS2)
    {
        printf("\n\n El número de columnas de la Matriz 1 no es igual al número de filas de la Matriz 2. Por favor intente de nuevo.\n\n");
        return 0;
    }

    if (id_proceso == 0 && NUM_COLUMNAS1 < num_procesos)
    {
        printf("\n\n El número de columnas de la Matriz 1 y número de filas de la Matriz 2 son menores al número de procesos por lo que la operacion no es posible. Por favor intente de nuevo.\n\n");
        return 0;
    }

    //Declaración de variables para todos los procesos
    int counts[num_procesos], displs[num_procesos]; // Arreglo de numero de datos y desplazamientos de cada proceso
    int tam_matriz1 = NUM_COLUMNAS1 * NUM_FILAS1;
    int tam_matriz2 = NUM_COLUMNAS2 * NUM_FILAS2;

    //Verificacion del numero de datos a enviar
    num_datos = (NUM_COLUMNAS1 / num_procesos) * NUM_FILAS1;
    num_datos_u = num_datos + ((NUM_COLUMNAS1 % num_procesos) * NUM_FILAS1);

    if (id_proceso == (num_procesos - 1))
        num_datos = num_datos_u;

    float *matriz_original, *matriz2_original, matriz_resultante[NUM_FILAS1 * NUM_COLUMNAS2];
    float recibir_columna[NUM_COLUMNAS1], recibir_fila[NUM_FILAS2], resultado;

    //Declarar las matrices solo para el proceso 0
    if (id_proceso == 0)
    {
        matriz_original = (float *)malloc(tam_matriz1 * sizeof(float));
        matriz2_original = (float *)malloc(tam_matriz2 * sizeof(float));

        llenar_matriz(matriz_original, tam_matriz1);
        llenar_matriz(matriz2_original, tam_matriz2);

        // Inicializa los arreglos revcounts y displs
        for (int i = 0; i < num_procesos; i++)
        {
            counts[i] = num_datos;
            displs[i] = i * num_datos;
        }

        counts[num_procesos - 1] = num_datos;
    }

    //Pasar los datos a los procesos por MPI_Scatterv
    MPI_Scatterv(matriz_original, counts, displs, MPI_FLOAT, &recibir_columna, num_datos, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Scatterv(matriz2_original, counts, displs, MPI_FLOAT, &recibir_fila, num_datos, MPI_FLOAT, 0, MPI_COMM_WORLD);

    //Operar las columnas y renglones
    resultado = multiplica_RenglonColumna(recibir_fila, recibir_columna);

    //Regresar el dato al proceso root
    MPI_Gatherv(&resultado, 1, MPI_FLOAT, &matriz_resultante, counts, displs, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if (id_proceso == 0)
        imprime_matriz_resultante(matriz_original, matriz2_original, matriz_resultante);

    MPI_Finalize();
    return 0;
}

void copiaRenCol(char t, float *auxArr )
{
    switch (t)
    {
    case 'r':
        break;

    case 'c':
        break;
    }
}

void llenar_matriz(float *arreglo, int tam_matriz)
{
    float decimal;

    int indice = tam_matriz;

    //Semilla del random
    time_t t;
    srand((unsigned)time(&t));

    for (int i = 0; i < indice; i++)
    {
        arreglo[i] = rand() % 101;
        decimal = rand() % 101;
        arreglo[i] += (decimal / 100);
    }
};

float multiplica_RenglonColumna(float fila[], float columna[])
{
    float res = 0.0;
    float arreglo_mult[NUM_COLUMNAS1];

    for (int i = 0; i < NUM_COLUMNAS1; i++)
    {
        arreglo_mult[i] = fila[i] * columna[i];
    }

    for (int i = 0; i < NUM_COLUMNAS1; i++)
    {
        res += arreglo_mult[i];
    }

    return res;
};

void imprime_matriz_resultante(float matriz1[], float matriz2[], float matriz_resultante[])
{
    int indice;
    int cont_row;

    printf("\n Matriz 1:\n\n");
    indice = NUM_FILAS1 * NUM_COLUMNAS1;
    cont_row = 0;
    for (int i = 0; i < indice; i++)
    {
        printf(" %f ", matriz1[i]);
        cont_row++;
        if (cont_row == NUM_COLUMNAS1)
        {
            cont_row = 0;
            printf("\n");
        }
    }

    printf("\n Matriz 2:\n\n");
    indice = NUM_FILAS2 * NUM_COLUMNAS2;
    cont_row = 0;
    for (int i = 0; i < indice; i++)
    {
        printf(" %f ", matriz2[i]);
        cont_row++;
        if (cont_row == NUM_COLUMNAS2)
        {
            cont_row = 0;
            printf("\n");
        }
    }

    printf("\n Matriz Resultante:\n\n");
    indice = NUM_FILAS1 * NUM_COLUMNAS2;
    cont_row = 0;
    for (int i = 0; i < indice; i++)
    {
        printf(" %f ", matriz_resultante[i]);
        cont_row++;
        if (cont_row == NUM_COLUMNAS2)
        {
            cont_row = 0;
            printf("\n");
        }
    }
};
