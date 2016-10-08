# Vertex Cache Optimization

### Algorithm

The algorithm is almost the same with the one described in Yu and Lin's paper except that the two tie
breaking rules for picking the Vfocus.

The general flow of the algorithm (in the perspective of Optimize()) is as follow:

* Firstly, construct a vertex list which is a set that contains all the vertices.
	In the vertex list's constructor, the input triangle array is quick-sorted and then be traversed
	once to get all the unique vertex.
	Then traverse the triangle array to get all the bounding faces for each vertex.
	Each vertex has a face list which contains all the pointers that point to all respective bounding faces.
	The pointers in this list are quick-sorted so that adjacent pointers point to adjacent faces.
	Each face is either black or white and is pointed by two or three vertices.
	The complexity for setting up all the initial data structures is O(tlogt).

* Secondly, a buffer is constructed in O(k).  

* Thirdly, enter the big while loop which loops until all vertices turn black.
	Inside the while loop, check weather the buffer is empty or if all vertices are black.

	=========================================================================

	If it is true, loop through the vertex list to pick the white vertex which has the least bounding
	number of white faces. Find the cost of this vertex. Loop through the vertex list again
	until the first occurrence of a white vertex which has the same number of bounding white faces and
	has a strictly lower cost than the previous one. If it exists, pick it as the initial
	focus. It is assumed that the number of time needed to pick the initial focus is small and grow slowly
	with the number of triangles. As the complexity of finding the cost per vertex is O(B^2), the complexity
	here is O(VB^2), where V is the number of vertices and B is the number of bounding faces of that vertex.

	If the buffer is not empty which means it has at least one gray vertex, loop through the buffer list.
	Find the cost of each gray vertex. Pick the one with the minimum cost and set it as Vfocus.
	Loop through the buffer again to pick the last occurrence of a vertex which has the same cost as Vfocus
	but has a strictly smaller C1 than that of the Vfocus. The paper suggested to use C3 but in most
	tie-breaking cases, C3s are almost the same for every vertex. But it is not the case for C1 which is a more
	significant factor in determining the cost. The complexity here is O(KB^2).

	Now Vfocus is selected. Output all of its white bounding faces.
	Each bounding face is fetched in constant time. To output a face, loop through the three vertices indefinitely until
	all are gray. For each vertex push, the new vertex will check if all of its bounding faces can be outputted.
	The checking takes O(B) for each vertex. The overall is O(B^2) for each Vfocus.
	Turn the Vfocus to black if all of its white bounding faces are outputted.

	While loop ends when the number of black vertices is equal to the number of vertices. Overall complexity is O(VKB^2).

	=========================================================================

	In particular, the _FindCost(V) is implemented as follow:

	Only back up the colors of the vertices which are the elements of the bounding face set of V and the buffer set.
	Consider V as Vfocus and pretend to output all of its white bounding faces. Recorded C1 as the number of pushes,
	C2 as the number of faces that will be outputted and C3 be the number of steps toward the end of buffer.
	Restore all colors after the operation. In addition to the cost, C1 is also returned for tie-breaking cases.

### Code

There are five structs. They are Vertex, Face, VertexList, FaceList and Buffer.

VertexList stores all Vertex(s). Each Vertex stores a FaceList which stores all Face(s). Each Face stores three Vertex(s).
Each Vertex with unique index value is constructed once in the whole program. Given an index value, the corresponding position
in the VertexList is computed via binary search. Thus the index value does not need to be consecutive integers starting from zero.
Each Face is also constructed once. Object factory is used. Destructor only destroys the Face when its reference count goes to zero.

### To Run
``` bash
make
```
