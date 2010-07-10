
/** 
  @file list.h
  @ingroup common
  \brief A linked list implementation 

 */ 

#ifndef LIST_H__
#define LIST_H__

/**
  \struct node
  \brief A node which is used to create a linked list.
  */
struct node
{
  /// The next object in the linked list.
	struct node *next;
  /// The data for this element in the list.
	void * data;
};

/**
  Creates a new list node 
  */
struct node * createNewList();

/**
 Destroys the list node.
 \warning Does not do a deep destroy.
  */
void destroyList(struct node*);

/**
  Pops the node from the beginning of the list and returns the node.
  */
struct node* popData(struct node *list);

/**
  Returns a pointer to the data at the given index.
  */
void * getElement(struct node *object, int index);

/**
  Pushes the data onto the list. 
  */
void pushData(struct node *object, void *data);

/**
  Removed the list node which has data 'element'
  */
void popElement(struct node *list, void *element);

/**
  Returns the size of the given list.
  */
int listSize(struct node *object);


#endif
