
#include <stdlib.h> // malloc, free

#include "CueFacesV_Ortholin.h"


//From J.H.Wilkinson and C.Reinsch,
//contribution I/9,
//F.L.Bauer,
//Elimination with Weighted Row Combination



/* -------------------------------------------------------------------------------- */

static double RowProduct (double Ra[], double Rb[], long RowSize)
{
   double S = 0.0;
   for (register long J=0; J<RowSize; ++J)  S += Ra[J] * Rb[J];
   return S;
}


static double ColumnProduct (double Ma[], double Rb[], long ColumnSize, long RowSize)
{
   double S = 0.0;
   for (register long J=0; J<ColumnSize; ++J)  S += Ma[J*RowSize] * Rb[J];
   return S;
}


bool Ortholin2 (double A[], long Nrows, long Ncols, double B[], double EPS, double X[], long MaxIterations)
/* finds the best fit for the equation [A] [X] = [B],
   where A is a matrix, Nrows * Ncols,
         X is a vector, Ncols * 1,
         B is a vector, Nrows * 1

   EPS and MaxIterations are tuning constants.
*/
{
   register long ColIndex;
   double *RowVector,*ColVector,*Uspace,*Triangle;

   RowVector = (double *)malloc(Nrows * sizeof(double));
   ColVector = (double *)malloc(Ncols * sizeof(double));
   Uspace    = (double *)malloc(Nrows * Ncols * sizeof(double));
   Triangle  = (double *)malloc((Ncols*(Ncols+1)/2) * sizeof(double));

   for (ColIndex=0; ColIndex<Nrows*Ncols; ColIndex++)  Uspace[ColIndex] = A[ColIndex];

   register long L = 0;

   for (ColIndex=0; ColIndex<Ncols; ColIndex++)
   {
      double S = 0.0;

      for (register long RowIndex=0; RowIndex<Nrows; RowIndex++)
      {
         double T = RowVector[RowIndex] = Uspace[RowIndex*Ncols+ColIndex];
         S += T*T;
      }
      Triangle[L++] = S;

      X[ColIndex] = RowProduct(RowVector,B,Nrows);
      for (register long H=ColIndex+1; H<Ncols; ++H)
      {
         double T = (Triangle[L++] = ColumnProduct(&Uspace[H],RowVector,Nrows,Ncols)) / S;
         for (register long r=0; r<Nrows; ++r)  Uspace[r*Ncols+H] -= RowVector[r]*T;
      }
   }

   long MaxL = L;

   /* backsubstitution */
   for (ColIndex=Ncols-1; ColIndex>=0; ColIndex--)
   {
      X[ColIndex] = (X[ColIndex] - RowProduct(&Triangle[L],&X[ColIndex+1],Ncols-ColIndex-1))/Triangle[L-1];
      L -= (Ncols+1)-ColIndex;
   } /* backsubstitition */

   /* iterative improvement on the solution */
   double S0 = 0;

   double EPS2 = EPS*EPS;

   for (register long Iteration=0; Iteration<MaxIterations; ++Iteration)  /* iteration */
   {
      double S1 = 0;
      double S2 = 0;
      L = MaxL;

      /* residual vector */
      for (register long RowIndex=0; RowIndex<Nrows; RowIndex++)  RowVector[RowIndex] = B[RowIndex]-RowProduct(&A[RowIndex*Ncols],X,Ncols);
      /* vector on the right side */
      for (register long c=0; c<Ncols; c++)  ColVector[c] = ColumnProduct(&Uspace[c],RowVector,Nrows,Ncols);

      /* backsubstitution */
      for (register long ColIndex=Ncols-1; ColIndex>=0; ColIndex--)
      {
         double T = X[ColIndex];
         double S = ColVector[ColIndex] = (ColVector[ColIndex] - RowProduct(&Triangle[L],&ColVector[ColIndex+1],Ncols-(ColIndex+1)))/Triangle[L-1];
         S1 += S*S;
         S2 += T*T;
         X[ColIndex] += S;
         L -= (Ncols+1)-ColIndex;
      } /* backsubstitution */

      if (S1 >= S2/4)  return false;
      if ((S1 >= S2*EPS2) || ((S0 == 0) && (S1 <= S0 * 0.01)))  S0 = S1;  else break;
   } /* iterative improvement */

   free(RowVector);
   free(ColVector);
   free(Uspace);
   free(Triangle);

   return true;
}

/* -------------------------------------------------------------------------------- */

