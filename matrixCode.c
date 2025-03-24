#include <stdio.h>
#include <stdlib.h>


double *input_matrix(int *rows, int *columns, const char *name); //function to input info
int matrix_multiply(double *m1, int m1Rows, int m1Cols, double *m2, int m2Rows, int m2Cols, double *m3);
//function to multiply matrix
void output_matrix(double *matrix, int rows, int columns); //function to print matrix


int main(void) {

    double *m1,*m2,*m3; //declares pointers for matrices
    int m1_rows,m1_columns,m2_rows,m2_columns; //declares variables for storing dimensions

    //calls input for m1 and m2 
    //allocates memory for m3 to store result in
    if (((m1 = input_matrix(&m1_rows,&m1_columns,"Matrix 1")) != NULL) &&
((m2 = input_matrix(&m2_rows,&m2_columns,"Matrix 2")) != NULL) &&
((m3 = malloc(m1_rows*m2_columns*sizeof(double))) != NULL)) { 
        //prints m1
        printf("Matrix 1\n");
        output_matrix(m1,m1_rows,m1_columns);
        //prints m2
        printf("Matrix 2\n");
        output_matrix(m2,m2_rows,m2_columns);
        //multiplies m1 and m2 and displays result in m3
        if (matrix_multiply(m1,m1_rows,m1_columns,m2,m2_rows,m2_columns,m3)) {
            printf("Product\n");
            output_matrix(m3,m1_rows,m2_columns);
            free(m1);
            free(m2);
            free(m3);
            return(0);
        } else {
            printf("Error in dimensions\n");
            free(m1);
            free(m2);
            free(m3);
            return(-1);
        }
    } else { //if mem allocation fails
        free(m1);
        free(m2);
        free(m3);
        printf("Error allocating memory\n");
        return(-2);
    }
}

double *input_matrix(int *rows, int *columns, const char *name){
   printf("Enter the number of rows and columns for %s: ", name);

   if(scanf("%d %d", rows, columns) != 2 || *rows <= 0 || *columns <= 0){
      printf("Invalid matrix dimensions :(\n");
      return NULL;
   } //check for valid input

   //allocate memory for matrix
   double *matrix = (double *)malloc((*rows) * (*columns) * sizeof(double));

   if (!matrix) {
      printf("Memory allocation failed for %s.\n", name);
      return NULL;
   } //end of if

   printf("Enter the elements for %s (row by row):\n", name);
   for (int i = 0; i < *rows; i++){
      printf("Enter row %d: ", i + 1);
      for (int j = 0; j < *columns; j++){
         if(scanf("%lf", &matrix[i * (*columns) +j]) != 1) {
            printf("Invalid input.\n");
            free(matrix);
            return NULL;
         }///end of if
      }//end of j for
   }//end of i for

   return matrix;

} //end of input function

int matrix_multiply(double *m1, int m1Rows, int m1Cols, double *m2, int m2Rows, int m2Cols, double *m3){
   //check for dimension match
   if(m1Cols != m2Rows){
      return 0;
   }//end of if

   for(int i = 0; i < m1Rows; i++){
      for(int j = 0; j < m2Cols; j++){
         m3[i * m2Cols + j] = 0; //initialize that spot in m3
         for(int k = 0; k < m1Cols; k++){
            m3[i*m2Cols + j] += m1[i * m1Cols + k] * m2[k * m2Cols + j];
         }//end of k for
      }//end of j for
   }//end of i for

   return 1;

}//end of function

void output_matrix(double *matrix, int rows, int columns){
   for(int i = 0; i < rows; i++){
      for(int j = 0; j < columns; j++){
         printf("%.2lf ", matrix[i * columns + j]);
      }//end of j for
      printf("\n");
   }//end of i for
}//end of function
