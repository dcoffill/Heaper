#include "heap.h"
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/mutex.h>

struct heap* init_heap(int size)
{
	struct heap *heap = (struct heap *)kmalloc(sizeof(struct heap), GFP_KERNEL);
	if (unlikely(heap == NULL)) {
		goto out;
	}

	heap->data = (char **)kmalloc(sizeof(char *) * (size + 1), GFP_KERNEL);
	if (unlikely(heap->data == NULL)) {
		kfree(heap);
		goto out;
	}
	mutex_init(&heap->lock);
	heap->size = size + 1; // array is 1 bigger than size we're given, since index 0 unused
	heap->end = 0;

out:
	return heap;
}

void free_heap(struct heap *heap)
{
	/* Ensure nobody is actually reading from the heap while we deallocate it */
	kfree(heap->data);
	kfree(heap);
}


// Insert entry into the heap
void heap_insert(struct heap *heap, char *string)
{
	++heap->end;
	heap->data[heap->end] = string;
	bubble_up(heap, heap->end); // ensure heap order property is valid
}

// Delete the minimum item in the heap, then correct order and shape properties
char* deleteMin(struct heap *heap)
{
	char *min;
	if (heap->end == 0)  // heap is empty, so don't try remove anything!
		return NULL;
	min = heap->data[1];
	heap->data[1] = heap->data[heap->end]; // move last item to top
	heap->data[heap->end] = NULL; // remove previous reference
	--heap->end;
	bubble_down(heap, 1); // verify/correct heap order property

	return min;
}

// AKA percolate up, help restore heap order property after an insertion
void bubble_up(struct heap *heap, int position)
{

	if (position == 1) // attempting bubble-up on root node, do not continue
		return;

	else if (strlen(heap->data[parent(position)]) > strlen(heap->data[position]) )
	{
		int parentIndex = parent(position);
		char *parent = heap->data[parentIndex];
		heap->data[parentIndex] = heap->data[position];
		heap->data[position] = parent;
		bubble_up(heap, parentIndex);
		return;
	}
}

// Restore heap order property after a deletion
void bubble_down(struct heap *heap, int position)
{
	// base case:
	if ((position >= heap->end) || (left(position) > heap->end && right(position) > heap->end) )
	{
		return;
	}
	// 1 child case
	else if (left(position) <= heap->end && right(position) > heap->end)
	{
		if (strlen(heap->data[position]) > strlen(heap->data[left(position)]) ) // violation of heap property
		{
			char *temp_child = heap->data[left(position)];
			heap->data[left(position)] = heap->data[position];
			heap->data[position] = temp_child;
		}
		return;
	}
	else // normal case, both children non-void
	{
		if (strlen(heap->data[position]) > strlen(heap->data[left(position)])
				|| strlen(heap->data[position]) > strlen(heap->data[right(position)]) ) // violation of heap property
		{
			int childIndex = (strlen(heap->data[left(position)]) < strlen(heap->data[right(position)]) ? left(position) : right(position));
			char *child = heap->data[childIndex];
			heap->data[childIndex] = heap->data[position];
			heap->data[position] = child;
			bubble_down(heap, childIndex); // update position to reflect swapped node's location
		}
	}
}

// return index of parent node in heap
int parent(int position)
{
	return position/2; // floor(pos/2)
}


// return index of left child node in heap
int left(int position)
{
	return 2*position;
}

// return index of right child node in heap
int right(int position)
{
	return 2*position + 1;
}
