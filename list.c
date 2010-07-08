#include "list.h"

#include <stdlib.h>

#ifdef UNIT_TEST
#include <stdio.h>
#endif

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

struct node* popData(struct node *list)
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

void * getElement(struct node *object, int index)
{
  int counter = 0;
  void*  ret = NULL;
  if(object != NULL)
  {
    struct node *ptr = object->next;
    while(counter != index)
    {
      ptr = ptr->next;
      counter ++;
    }

    ret = ptr->data;

  }
  return ret;
}

/**
  Removes the element from the list.
  memory for 'element' is not freeied
 */
void popElement(struct node *list, void *element)
{
  if(list != NULL && element != NULL)
  {
    struct node *ptr = list;
    while(ptr->next->data != element)
    {
      ptr = ptr->next;
    }

    if(ptr->next->data == element)
    {
      struct node *destroyElement = ptr->next;
      ptr->next = ptr->next->next;
      destroyList(destroyElement);
    }
  }
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
  struct node *data2;
  char c_data[16];
  char c_data2[16];
  sprintf(c_data,"Hello\n");
  sprintf(c_data2,"Hello2\n");
  char *retData;

  pushData(list,&c_data);
  pushData(list,&c_data2);

  printf("size: %i\n",listSize(list));
  printf("element 0: %s\n",(char *)getElement(list,0));
  printf("element 1: %s\n",(char *)getElement(list,1));

  popElement(list,&c_data2);
  data = popData(list);
  printf("retData: %s\n",(char*)data->data);
  popData(list);
  printf("size: %i\n",listSize(list));

  destroyList(list);
  return 0;
}
#endif
