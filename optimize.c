#include "optimize.h"

// ------------------------
// Optimization Code
// ------------------------
typedef int Bool;
enum e_truth { FALSE = 0, TRUE};
enum e_color { BLACK = 1, GRAY, WHITE };
typedef enum e_color Color;

struct Face;
struct FaceList;
typedef struct Face Face;
typedef struct FaceList FaceList;

FaceList *FaceList_Constructor(FaceList *this_ptr, int NoOfFace);
void FaceList_Destructor(FaceList *this_ptr, Bool dynamic);
void FaceList_AddFace(FaceList *this_ptr, Face *newFace);
void FaceList_SortFace(FaceList *this_ptr);

typedef struct
{
	int index;
	Color color;
	int NoOfBoundingFace;
	FaceList *BoundingFaceList;
} Vertex;


Vertex *Vertex_Constructor(Vertex *this_ptr, int index)
{
	if (this_ptr == NULL)
	{
		this_ptr = (Vertex *) malloc(sizeof(Vertex));
	}
	if (this_ptr)
	{
		this_ptr->index = index;
		this_ptr->color = WHITE;
		this_ptr->NoOfBoundingFace = 0;
		this_ptr->BoundingFaceList = NULL;
	}
	return this_ptr;
}

void Vertex_Destructor(Vertex *this_ptr, Bool dynamic)
{
	if (this_ptr->BoundingFaceList)
	{
		FaceList_Destructor(this_ptr->BoundingFaceList, TRUE);
	}
	if (dynamic)
	{
		free(this_ptr);
	}
}

void Vertex_SetColor(Vertex *this_ptr, Color color)
{
	if (this_ptr)
	{
		this_ptr->color = color;
	}
}

Color Vertex_GetColor(Vertex *this_ptr)
{
	Color co = -1;
	if(this_ptr)
	{
		co = this_ptr->color;
	}
	return co;
}

void Vertex_SetNoOfBoundingFace(Vertex *this_ptr, int no)
{
	if(this_ptr)
	{
		this_ptr->NoOfBoundingFace = no;
	}
}

int Vertex_GetNoOfBoundingFace(Vertex *this_ptr)
{
	int no = -1;
	if(this_ptr)
	{
		no = this_ptr->NoOfBoundingFace;
	}
	return no;
}

struct Face
{
	Vertex *x, *y, *z;
	Color color;
	int ReferenceCount;
};

Face *Face_Constructor(Face *this_ptr, Vertex *x, Vertex *y, Vertex *z)
{
	if (this_ptr == NULL)
	{
		this_ptr = (Face *) malloc(sizeof(Face));
	}
	if (this_ptr)
	{
		if (y->index < x->index && y->index < z->index)
		{
			this_ptr->x = y;
			this_ptr->y = z;
			this_ptr->z = x;
		}
		else if (z->index <= x->index)
		{
			this_ptr->x = z;
			this_ptr->y = x;
			this_ptr->z = y;
		}
		else
		{
			this_ptr->x = x;
			this_ptr->y = y;
			this_ptr->z = z;
		}
		this_ptr->color = WHITE;
		this_ptr->ReferenceCount = 1;
	}
	return this_ptr;
}

void Face_Destructor(Face *this_ptr, Bool dynamic)
{
	this_ptr->ReferenceCount = this_ptr->ReferenceCount - 1;
	if(this_ptr->ReferenceCount == 0 && dynamic)
	{
		free(this_ptr);
	}
}

typedef struct
{
	Vertex **pVertexList;
	int Size;
} VertexList;

Vertex * VertexList_SearchByIndex(VertexList *this_ptr, int index);

static int _Vertex_compare_int(const void *X, const void *Y)
{
	return ( *(int*)X - *(int*)Y );
}

static int _Vertex_compare_pVertex(const void *X, const void *Y)
{
	return ( (*(Vertex**)X)->index - (*(Vertex**)Y)->index );
}

VertexList *VertexList_Constructor(VertexList *this_ptr, const int *pTris, const int nTris, const int K)
{
       	int *pSortedTris = malloc(sizeof(int) * 3 * nTris);
	int i, temp, Vnumber = 0, n = 0;
	Vertex *Searched, *x, *y, *z;
	Face *NewFace;
	if (this_ptr == NULL)
	{
		this_ptr = (VertexList *) malloc(sizeof(VertexList));
	}
	if (this_ptr)
	{
   		memcpy(pSortedTris, pTris, sizeof(int) * 3 * nTris);
		qsort(pSortedTris, nTris * 3, sizeof(int), _Vertex_compare_int);
		temp = -1;
		for (i = 0; i < nTris * 3; i++)
		{
			if (temp != pSortedTris[i])
			{
				Vnumber++;
				temp = pSortedTris[i];
			}
		}
		this_ptr->Size = Vnumber;
		this_ptr->pVertexList = malloc(sizeof(Vertex *) * Vnumber);
		temp = -1;
		for (i = 0; i < nTris * 3; i++)
		{
			if (temp != pSortedTris[i])
			{
				this_ptr->pVertexList[n++] = Vertex_Constructor(NULL, pSortedTris[i]);
				temp = pSortedTris[i];
			}
		}
		for (i = 0; i < nTris * 3; i++)
		{
			Searched = VertexList_SearchByIndex(this_ptr, pTris[i]);
			Vertex_SetNoOfBoundingFace(Searched, Vertex_GetNoOfBoundingFace(Searched) + 1);
		}
		for (i = 0; i < this_ptr->Size; i++)
		{
			this_ptr->pVertexList[i]->BoundingFaceList = FaceList_Constructor(NULL, this_ptr->pVertexList[i]->NoOfBoundingFace);
		}
		for (i = 0; i < nTris; i++)
		{
			x = VertexList_SearchByIndex(this_ptr, pTris[3 * i]);
			y = VertexList_SearchByIndex(this_ptr, pTris[3 * i + 1]);
			z = VertexList_SearchByIndex(this_ptr, pTris[3 * i + 2]);
			NewFace = Face_Constructor(NULL, x, y, z);
			FaceList_AddFace(x->BoundingFaceList, NewFace);
			FaceList_AddFace(y->BoundingFaceList, NewFace);
			FaceList_AddFace(z->BoundingFaceList, NewFace);
		}
		if ((K <= 10) || (22 <= K && K <= 23) || (32 <= K && K < 128))
		{
			for (i = 0; i < this_ptr->Size; i++)
			{
				FaceList_SortFace(this_ptr->pVertexList[i]->BoundingFaceList);
			}
		}
	}
	free(pSortedTris);
	return this_ptr;
}

void VertexList_Destructor(VertexList *this_ptr, Bool dynamic)
{
	int i;
	for (i = 0; i < this_ptr->Size; i++)
	{
		Vertex_Destructor(this_ptr->pVertexList[i], TRUE);
	}
	free(this_ptr->pVertexList);
	if (dynamic)
	{
		free(this_ptr);
	}
}

Vertex * VertexList_SearchByIndex(VertexList *this_ptr, int index)
{
	Vertex *vKey = Vertex_Constructor(NULL, index);
	Vertex **Searched = (Vertex**) bsearch(&vKey , this_ptr->pVertexList, this_ptr->Size, sizeof(Vertex *), _Vertex_compare_pVertex);
	Vertex_Destructor(vKey, TRUE);
	return *Searched;
}

struct FaceList
{
	Face **pFaceList;
	int Size;
	int ActualNoOfFace;
};

FaceList *FaceList_Constructor(FaceList *this_ptr, int NoOfFace)
{
	if (this_ptr == NULL)
	{
		this_ptr = (FaceList *) malloc(sizeof(FaceList));
	}
	if (this_ptr)
	{
		this_ptr->pFaceList = malloc(sizeof(Face *) * NoOfFace);
		this_ptr->Size = NoOfFace;
		this_ptr->ActualNoOfFace = 0;
	}
	return this_ptr;
}

void FaceList_Destructor(FaceList *this_ptr, Bool dynamic)
{
	int i;
	for (i = 0; i < this_ptr->ActualNoOfFace; i++)
	{
		Face_Destructor(this_ptr->pFaceList[i], TRUE);
	}
	if (this_ptr->Size > 0)
	{
		free(this_ptr->pFaceList);
	}
	if (dynamic)
	{
		free(this_ptr);
	}
}

void FaceList_AddFace(FaceList *this_ptr, Face *newFace)
{
	if (this_ptr->ActualNoOfFace < this_ptr->Size)
	{
		this_ptr->pFaceList[this_ptr->ActualNoOfFace] = newFace;
		this_ptr->ActualNoOfFace = this_ptr->ActualNoOfFace + 1;
		newFace->ReferenceCount = newFace->ReferenceCount + 1;
	}
}

static int _FaceList_triCmp(const void *X, const void *Y)
{
	int a = (*(Face **)X)->x->index;
	int b = (*(Face **)X)->y->index;
	int c = (*(Face **)X)->z->index;
	int d = (*(Face **)Y)->x->index;
	int e = (*(Face **)Y)->y->index;
	int f = (*(Face **)Y)->z->index;

	if (a < d) return -1;
	if (a > d) return 1;
	if (b < e) return -1;
	if (b > e) return 1;
	if (c < f) return -1;
	if (c > f) return 1;
   	return 0;
}

void FaceList_SortFace(FaceList *this_ptr)
{
	qsort(this_ptr->pFaceList, this_ptr->Size, sizeof(Face *), _FaceList_triCmp);
}

typedef struct
{
	Vertex **pBuffer;
	int Size;
	int CurrentIndex;
} Buffer;

Buffer * Buffer_Constructor(Buffer *this_ptr, int Size)
{
	int i;
	if (this_ptr == NULL)
	{
		this_ptr = (Buffer *) malloc(sizeof(Buffer));
	}
	if (this_ptr)
	{
		this_ptr->pBuffer = (Vertex **) malloc(sizeof(Vertex*) * Size);
		for (i = 0; i < Size; i++)
		{
			this_ptr->pBuffer[i] = NULL;
		}
		this_ptr->Size = Size;
		this_ptr->CurrentIndex = 0;
	}
	return this_ptr;
}

void Buffer_Destructor(Buffer *this_ptr, Bool dynamic)
{
	if (this_ptr->Size > 0)
	{
		free(this_ptr->pBuffer);
	}
	if (dynamic)
	{
		free(this_ptr);
	}
}

void Buffer_PrintSelf(Buffer *this_ptr)
{
	int i;
	printf("[ ");
	for ( i = 0; i < this_ptr->Size; i++)
	{
		if (this_ptr->pBuffer[i])
		{
			printf("%d ", this_ptr->pBuffer[i]->index);
		}
		else
		{
			printf("empty ");
		}
	}
	printf("]\n");
}

void Buffer_Push(Buffer *this_ptr, Vertex *NewVertex)
{
	if (this_ptr->pBuffer[this_ptr->CurrentIndex])
	{
		if (Vertex_GetColor(this_ptr->pBuffer[this_ptr->CurrentIndex]) != BLACK)
		{
			Vertex_SetColor(this_ptr->pBuffer[this_ptr->CurrentIndex], WHITE);
		}
	}
	this_ptr->pBuffer[this_ptr->CurrentIndex] = NewVertex;
	this_ptr->CurrentIndex = (this_ptr->CurrentIndex + 1) % this_ptr->Size;
	Vertex_SetColor(NewVertex, GRAY);
}

Bool Buffer_IsEmpty(Buffer *this_ptr)
{
	Bool empty = TRUE;
	int i;
	for ( i = 0; i < this_ptr->Size; i++)
	{
		if (this_ptr->pBuffer[i])
		{
			empty = FALSE;
		}
	}
	return empty;
}

Bool Buffer_IsAllBlack(Buffer *this_ptr)
{
	Bool empty = TRUE;
	int i;
	for ( i = 0; i < this_ptr->Size; i++)
	{
		if (this_ptr->pBuffer[i] && this_ptr->pBuffer[i]->color == GRAY)
		{
			empty = FALSE;
		}
	}
	return empty;
}

static void _RenderAnyRenderable(Vertex *newVertex, int *pOutTris, int *NoOfBlackFace, Bool Simulate, int *C2)
{
	int i;
	Face *boundingFace;
	for (i = 0; i < newVertex->NoOfBoundingFace; i++)
	{
		boundingFace = newVertex->BoundingFaceList->pFaceList[i];
		if (boundingFace->color == WHITE && boundingFace->x->color == GRAY && boundingFace->y->color == GRAY && boundingFace->z->color == GRAY)
		{
			if (Simulate)
			{
				*C2 = *C2 + 1;
			}
			else
			{
				pOutTris[*NoOfBlackFace * 3] = boundingFace->x->index;
				pOutTris[*NoOfBlackFace * 3 + 1] = boundingFace->y->index;
				pOutTris[*NoOfBlackFace * 3 + 2] = boundingFace->z->index;
				boundingFace->color = BLACK;
				*NoOfBlackFace = *NoOfBlackFace + 1;
			}
		}
	}
}

static double _FindCost(Buffer *buffer, Vertex *vertex, int K, int *pC1)
{
	int i, C1 = 0, C2 = 0, C3 = -1;
	double k1 = 1.00000, k2 = -0.50000, k3 = 1.3000;
	Face *tempFace;
	Color *BackupVertexColor = (Color *) malloc(sizeof(Color) * vertex->NoOfBoundingFace * 3);
	Color *BackupBufferColor = (Color *) malloc(sizeof(Color) * buffer->Size);
	Buffer *BackupBuffer = Buffer_Constructor(NULL, buffer->Size);
	for (i = 0; i < vertex->NoOfBoundingFace; i++)
	{
		BackupVertexColor[i * 3] = vertex->BoundingFaceList->pFaceList[i]->x->color;
		BackupVertexColor[i * 3 + 1] = vertex->BoundingFaceList->pFaceList[i]->y->color;
		BackupVertexColor[i * 3 + 2] = vertex->BoundingFaceList->pFaceList[i]->z->color;
	}
	for (i = 0; i < buffer->Size; i++)
	{
		if (buffer->pBuffer[i])
		{
			BackupBufferColor[i] = buffer->pBuffer[i]->color;
		}
	}
	for (i = 0; i < buffer->Size; i++)
	{
		BackupBuffer->pBuffer[i] = buffer->pBuffer[i];
	}
	BackupBuffer->CurrentIndex = buffer->CurrentIndex;
	for (i = 0; i < vertex->NoOfBoundingFace; i++)
	{
		tempFace = vertex->BoundingFaceList->pFaceList[i];
		if (tempFace->color == WHITE)
		{
			while (tempFace->x->color == WHITE || tempFace->y->color == WHITE || tempFace->z->color == WHITE)
			{
				if (tempFace->x->color == WHITE)
				{
					Buffer_Push(buffer, tempFace->x);
					C1++;
					_RenderAnyRenderable(tempFace->x, NULL, NULL, TRUE, &C2);
				}
				if (tempFace->y->color == WHITE)
				{
					Buffer_Push(buffer, tempFace->y);
					C1++;
					_RenderAnyRenderable(tempFace->y, NULL, NULL, TRUE, &C2);
				}
				if (tempFace->z->color == WHITE)
				{
					Buffer_Push(buffer, tempFace->z);
					C1++;
					_RenderAnyRenderable(tempFace->z, NULL, NULL, TRUE, &C2);
				}
			}
		}
	}
	for (i = 0; i < buffer->Size; i++)
	{
		if (buffer->pBuffer[i] == vertex)
		{
			C3 = i;
		}
	}
	if (C3 == -1)
	{
		C3 = 0;
	}
	else if (C3 >= buffer->CurrentIndex)
	{
		C3 = C3 - buffer->CurrentIndex + 1;
	}
	else
	{
		C3 = buffer->Size - buffer->CurrentIndex + 1 + C3;
	}
	for (i = 0; i < vertex->NoOfBoundingFace; i++)
	{
		vertex->BoundingFaceList->pFaceList[i]->x->color = BackupVertexColor[i * 3];
		vertex->BoundingFaceList->pFaceList[i]->y->color = BackupVertexColor[i * 3 + 1];
		vertex->BoundingFaceList->pFaceList[i]->z->color = BackupVertexColor[i * 3 + 2];
	}
	for (i = 0; i < BackupBuffer->Size; i++)
	{
		buffer->pBuffer[i] = BackupBuffer->pBuffer[i];
		if (buffer->pBuffer[i])
		{
			buffer->pBuffer[i]->color = BackupBufferColor[i];
		}
	}
	buffer->CurrentIndex = BackupBuffer->CurrentIndex;
	free(BackupVertexColor);
	free(BackupBufferColor);
	Buffer_Destructor(BackupBuffer, TRUE);
	*pC1 = C1;
	return k1 * C1 + k2 * C2 + k3 * C3 / K;
}

void Optimize(const int * pTris,      // pointer to array containing 3 vertex indices per triangle;
                                      //  first index is 0 (not 1); length of array is 3*nTris;
              const int   nTris,      // number of triangles
              const int   nCacheSize, // size of the vertex cache
              int *       pOutTris)   // output triangle array; length of array is 3*nTris
{
	VertexList *VL = VertexList_Constructor(NULL, pTris, nTris, nCacheSize);
	Buffer *bf = Buffer_Constructor(NULL, nCacheSize);
	int i, j, degree = -1, NoOfBlackVertex = 0, NoOfBlackFace = 0, C1, lowC1, currWhiteFace;
	double cost = -1, tempD;
	Vertex *Vfocus;
	Face *BoundingFace;

	while (NoOfBlackVertex < VL->Size)
	{
		if (Buffer_IsEmpty(bf) || Buffer_IsAllBlack(bf))
		{
			for ( i = 0; i < VL->Size; i++)
			{
				if (VL->pVertexList[i]->color == WHITE)
				{
					currWhiteFace = 0;
					for ( j = 0; j < VL->pVertexList[i]->NoOfBoundingFace; j++)
					{
						if (VL->pVertexList[i]->BoundingFaceList->pFaceList[j]->color == WHITE)
						{
							currWhiteFace++;
						}
					}
					if (degree == -1 || currWhiteFace < degree)
					{
						degree = currWhiteFace;
						Vfocus = VL->pVertexList[i];
					}
				}
			}
			_FindCost(bf, Vfocus, nCacheSize, &lowC1);
			for ( i = 0; i < VL->Size; i++)
			{
				if (VL->pVertexList[i]->color == WHITE && VL->pVertexList[i]->NoOfBoundingFace == degree)
				{
					_FindCost(bf, VL->pVertexList[i], nCacheSize, &C1);
					if( C1 < lowC1)
					{
						Vfocus = VL->pVertexList[i];
						break;
					}
				}
			}
			Buffer_Push(bf, Vfocus);
		}
		else
		{
			for (i = 0; i < bf->Size; i++)
			{
				if (bf->pBuffer[i] && bf->pBuffer[i]->color != BLACK)
				{
					tempD = _FindCost(bf, bf->pBuffer[i], nCacheSize, &C1);
					if (tempD < cost || cost == -1)
					{
						cost = tempD;
						lowC1 = C1;
						Vfocus = bf->pBuffer[i];
					}
				}
			}
			for (i = 0; i < bf->Size; i++)
			{
				if (bf->pBuffer[i] && bf->pBuffer[i]->color != BLACK)
				{
					tempD = _FindCost(bf, bf->pBuffer[i], nCacheSize, &C1);
					if (tempD == cost && C1 < lowC1)
					{
						Vfocus = bf->pBuffer[i];
					}
				}
			}
		}
		for (i = 0; i < Vfocus->NoOfBoundingFace; i++)
		{
			BoundingFace = Vfocus->BoundingFaceList->pFaceList[i];
			if (BoundingFace->color == WHITE)
			{
				while ( BoundingFace->x->color == WHITE || BoundingFace->y->color == WHITE || BoundingFace->z->color == WHITE)
				{
					if (BoundingFace->x->color == WHITE)
					{
						Buffer_Push(bf, BoundingFace->x);
						_RenderAnyRenderable(BoundingFace->x, pOutTris, &NoOfBlackFace, FALSE, NULL);
					}
					if (BoundingFace->y->color == WHITE)
					{
						Buffer_Push(bf, BoundingFace->y);
						_RenderAnyRenderable(BoundingFace->y, pOutTris, &NoOfBlackFace, FALSE, NULL);
					}
					if (BoundingFace->z->color == WHITE)
					{
						Buffer_Push(bf, BoundingFace->z);
						_RenderAnyRenderable(BoundingFace->z, pOutTris, &NoOfBlackFace, FALSE, NULL);
					}
				}
			}
		}
		Vfocus->color = BLACK;
		NoOfBlackVertex++;
		cost = -1;
		degree = -1;
	}
	Buffer_Destructor(bf, TRUE);
	VertexList_Destructor(VL, TRUE);
}
