/******************************************************************/
/**** This file contains the support code for VCO.             ****/
/******************************************************************/

#ifdef WIN32
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

void Optimize(const int *pTris, const int nTris,
              const int nCacheSize, int * pOutTris);

//sorting function used in CheckResult
static int trisort_N;
static int trisort_N2;
int trisort(const void *av, const void *bv)
{

   int *a = (int *)av;
   int *b = (int *)bv;
   if(*a < *b) return -1;
   if(*a > *b) return 1;
   a++; b++;
   if(*a < *b) return -1;
   if(*a > *b) return 1;
   a++; b++;
   if(*a < *b) return -1;
   if(*a > *b) return 1;
   return 0;
}

//check if optimized result still contains all triangles
int CheckResult(int *pTris, int *pOutTris, int nTris)
{
   int cmp, tmp, i;
   int *pTrisS = malloc(sizeof(int) * 3 * nTris);
   int *pOutTrisS = malloc(sizeof(int) * 3 * nTris);

   //sort input and output triangles based on IDs
   memcpy(pTrisS, pTris, sizeof(int) * 3 * nTris);
   memcpy(pOutTrisS, pOutTris, sizeof(int) * 3 * nTris);

   //put vertex with minimum ID first for consistency
   for(i = 0; i < nTris * 3; i+=3)
   {
      if(pOutTrisS[i+1] < pOutTrisS[i] && pOutTrisS[i+1] < pOutTrisS[i+2])
      {
         tmp = pOutTrisS[i];
         pOutTrisS[i] = pOutTrisS[i+1];
         pOutTrisS[i+1] = pOutTrisS[i+2];
         pOutTrisS[i+2] = tmp;
      }
      else if(pOutTrisS[i+2] <= pOutTrisS[i]) //<= here ensures consistency in degenerate cases
      {
         tmp = pOutTrisS[i];
         pOutTrisS[i] = pOutTrisS[i+2];
         pOutTrisS[i+2] = pOutTrisS[i+1];
         pOutTrisS[i+1] = tmp;
      }
      if(pTrisS[i+1] < pTrisS[i] && pTrisS[i+1] < pTrisS[i+2])
      {
         tmp = pTrisS[i];
         pTrisS[i] = pTrisS[i+1];
         pTrisS[i+1] = pTrisS[i+2];
         pTrisS[i+2] = tmp;
      }
      else if(pTrisS[i+2] <= pTrisS[i]) //<= here ensures consistency in degenerate cases
      {
         tmp = pTrisS[i];
         pTrisS[i] = pTrisS[i+2];
         pTrisS[i+2] = pTrisS[i+1];
         pTrisS[i+1] = tmp;
      }
   }

   //sort triangle lists
   qsort(pTrisS, nTris, sizeof(int) * 3, trisort);
   qsort(pOutTrisS, nTris, sizeof(int) * 3, trisort);

   //compare to ensure resulting arrays match
   cmp = memcmp(pTrisS, pOutTrisS, sizeof(int) * 3 * nTris);

   //free temp data
   free(pTrisS);
   free(pOutTrisS);

   return(cmp == 0);
}

//computes the ACMR of your output mesh
float ComputeACMR(int *pTris, int nTris, int iCacheSize)
{
   int i, j;
   int found;
   int head = 0;
   int iProc = 0;
   int *piCache = malloc(sizeof(int) * iCacheSize);
   for(i = 0; i < iCacheSize; i++)
   {
      piCache[i] = -1;
   }
   for(i = 0; i < nTris * 3; i++)
   {
      found = 0;
      for(j = 0; j < iCacheSize; j++)
      {
         if(pTris[i] == piCache[j])
         {
            found = 1;
            break;
         }
      }
      if(!found)
      {
         iProc++;
         piCache[head] = pTris[i];
         head = (head + 1) % iCacheSize;
      }
   }
   free(piCache);
   return iProc / (float)nTris;
}

//Load input ply file from disk (mesh must adhere formatting of the provided meshes exactly)
int LoadMesh(char *file, float **ppVerts, int **ppTris, int *pnVerts, int *pnTris)
{
   int i;
   char s[1024];
   FILE *f = fopen(file, "r");
   if(!f) return 0;

   *pnVerts = 0;
   *pnTris = 0;
   while(!strstr(s, "end_header") && !feof(f))
   {
      fgets(s, 1024, f);
      if(strstr(s, "element vertex") != NULL)
      {
         *pnVerts = atoi(s+15);
      }
      if(strstr(s, "element face") != NULL)
      {
         *pnTris = atoi(s+13);
      }
   }
   if(*pnVerts * *pnTris == 0)
   {
      fclose(f);
      return 0;
   }
   *ppVerts = (float *)malloc(*pnVerts * sizeof(float) * 3);
   *ppTris = (int *)malloc(*pnTris * sizeof(int) * 3);

   for(i = 0; i < *pnVerts * 3; i++)
   {
      fscanf(f, "%f", (*ppVerts)+i);
   }
   for(i = 0; i < *pnTris * 3; i+=3)
   {
      fscanf(f, "%s", s);  //dummy; should always be 3
      fscanf(f, "%i", (*ppTris)+i);
      fscanf(f, "%i", (*ppTris)+i+1);
      fscanf(f, "%i", (*ppTris)+i+2);
   }
   fclose(f);
   return 1;
}

//Save output ply file from disk
void SaveMesh(char *file, float *pVerts, int *pTris, int nVerts, int nTris)
{
   int i;
   FILE *f = fopen(file, "w");
   fprintf(f, "ply\n");
   fprintf(f, "format ascii 1.0\n");
   fprintf(f, "element vertex %i\n", nVerts);
   fprintf(f, "property float x\n");
   fprintf(f, "property float y\n");
   fprintf(f, "property float z\n");
   fprintf(f, "element face %i\n", nTris);
   fprintf(f, "property list char int vertex_indices\n");
   fprintf(f, "end_header\n");
   for(i = 0; i < nVerts * 3; i+=3)
   {
      fprintf(f, "%f %f %f\n", pVerts[i], pVerts[i+1], pVerts[i+2]);
   }
   for(i = 0; i < nTris * 3; i+=3)
   {
      fprintf(f, "3 %i %i %i\n", pTris[i], pTris[i+1], pTris[i+2]);
   }
   fclose(f);
}

int main(int argc, char **argv)
{
   char *psMeshes[1024];
   char psMeshOut[1024];
   int piCacheSizes[1024];
   int i, j;
   int nMeshes=0, nCacheSizes=0;
   float fACMR;
   float fACMRSum = 0.0f;
   int nACMR = 0;
   float *pVerts;
   int *pTris;
   int *pOutTris;
   int nVerts;
   int nTris;
   if(argc > 1024)
   {
      printf("Number of arguments cannot exceed 1024.\n");
      exit(0);
   }
   for(i = 1; i < argc; i++)
   {
      if(strstr(argv[i], ".") != NULL)
      {
         psMeshes[nMeshes] = argv[i];
         nMeshes++;
      }
      else if((piCacheSizes[nCacheSizes] = atoi(argv[i])) > 0)
      {
         nCacheSizes++;
      }
   }
   if(nMeshes == 0)
   {
      printf("At least one mesh file must be specified.\n");
      printf("Syntax: optimize m1.ply m2.ply ... mN.ply cachesize1 cachesize2 ... cachesizeN\n");
      printf("Example: optimize circle.ply 16\n");
      exit(0);
   }
   if(nCacheSizes == 0)
   {
      printf("At least one cache size must be specified.\n");
      printf("Syntax: optimize m1.ply m2.ply ... mN.ply cachesize1 cachesize2 ... cachesizeN\n");
      printf("Example: optimize circle.ply 16\n");
      exit(0);
   }

   for(i = 0; i < nMeshes; i++)
   {
      if(!LoadMesh(psMeshes[i], &pVerts, &pTris, &nVerts, &nTris))
      {
         printf("Unable to load %s. File missing or invalid.\n", psMeshes[i]);
         continue;
      }
      *strstr(psMeshes[i], ".") = 0; //remove extension
      pOutTris = malloc(sizeof(int) * 3 * nTris);
      for(j = 0; j < nCacheSizes; j++)
      {
         Optimize(pTris, nTris, piCacheSizes[j], pOutTris);
         if(!CheckResult(pTris, pOutTris, nTris))
         {
            printf("Mesh: %s, CacheSize: %i, INVALID RESULT!\n", psMeshes[i], piCacheSizes[j]);
         }
         else
         {
            fACMR = ComputeACMR(pOutTris, nTris, piCacheSizes[j]);
            printf("Mesh: %s, CacheSize: %i, ACMR: %f\n", psMeshes[i], piCacheSizes[j], fACMR);
            fACMRSum += fACMR;
            nACMR++;
         }
         sprintf(psMeshOut, "%s-c%i.ply", psMeshes[i], piCacheSizes[j]);
         SaveMesh(psMeshOut, pVerts, pOutTris, nVerts, nTris);
      }
      free(pTris);
      free(pOutTris);
   }
   if(nACMR > 0)
      printf("Average ACMR: %f\n", fACMRSum / (float)nACMR);
}
