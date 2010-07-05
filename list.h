#ifndef LIST_H__
#define LIST_H__

struct node
{
	struct node *next;
	void * data;
};

struct node * createNewList();

void destroyList(struct node*);

void * popData(struct node *list);

void pushData(struct node *object, void *data);

int listSize();


#endif
