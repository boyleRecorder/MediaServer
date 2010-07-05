#include "list.h"

#include <stdlib.h>

struct node *createNewList()
{
	struct node *object = (struct node*)malloc(sizeof(struct node));

	object->next = NULL;
	object->data = NULL;

	return object;
}

void destroyList(struct node *object)
{
	object->next = NULL;
	object->data = NULL;

	free(object);
	object = NULL;
}

void * popData(struct node *list)
{
		struct node *obj;
	if(list != NULL)
	{
		if(list->next != NULL)
		{
			obj = list->next;
			list->next = list->next->next;
		}
	}
	return obj;
}

void pushData(struct node *object, void *data)
{
	if(object != NULL && data != NULL)
	{
		struct node *ptr = object;
		while(ptr->next != NULL)
			ptr = ptr->next;

		ptr->next = createNewList();
		ptr->next->data = data;
	}
}

int listSize(struct node *object)
{
	int counter = 0;
	if(object != NULL)
	{
		struct node *ptr = object;
		while(ptr->next != NULL)
		{
			ptr = ptr->next;
			counter ++;
		}

	}
	return counter;
}

#ifdef UNIT_TEST
int main()
{
	struct node *list = createNewList();

	struct node *data;
	char c_data[16];
	sprintf(c_data,"Hello\n");
	char *retData;

	pushData(list,&c_data);
	printf("size: %i\n",listSize(list));
	data = popData(list);
	printf("retData: %s\n",data->data);
	popData(list);
	printf("size: %i\n",listSize(list));

	destroyList(list);
	return 0;
}
#endif
