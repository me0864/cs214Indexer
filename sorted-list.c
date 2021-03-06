/*sorted-list.c*/
/*modified to insert for a struct node for Asst3*/


#include "sorted-list.h"
#include <stdlib.h>
#include <string.h>

/*returns a linked list node with value*/
struct node* CreateRecord(char* newObj, char* pathname ){

  struct node* newnode = (struct node*)malloc(sizeof(struct node));
    if(newnode == NULL)
        return NULL;


    newnode->value = malloc(sizeof(strlen(newObj)) + 1 );
    newnode->filepath = malloc(sizeof(strlen(pathname))  + 1);

    strcpy(newnode->value, newObj);
    newnode->value[strlen(newObj)] = '\0';
    strcpy(newnode->filepath, pathname);
    newnode->filepath[strlen(pathname)] = '\0';
    //newnode->value = newObj;
    //newnode->filepath = pathname;
    newnode->refCount = 1;
    newnode->next = NULL;

    return newnode;
}


/*
 *Frees linked list that was malloced
 *Called in SLDestroy
 */
void FreeLinkedList(struct node* ptr, DestructFuncT df){
    if(ptr == NULL){
        return;
    }

    if(ptr->next != NULL){
        FreeLinkedList(ptr->next, df);
    }

    df(ptr);

    return;
}

/*--------------------------------------------------------------------------------------------------
 * SLCreate creates a new, empty, 'SortedList'.
 *
 * SLCreate's parameters will be a comparator (cf) and destructor (df) function.
 *   Both the comparator and destructor functions will be defined by the user as per
 *     the prototypes above.
 *   Both functions must be stored in the SortedList struct.
 *
 * SLCreate must return NULL if it does not succeed, and a non-NULL SortedListPtr
 *   on success.
 */

SortedListPtr SLCreate(CompareFuncT cf, DestructFuncT df){
    SortedListPtr list = malloc(sizeof(struct SortedList));

    if(list == NULL){
        return NULL;
    }

    list->COMPARATOR = cf;
    list->DESTRUCTOR = df;
    list->head = NULL;

    return list;
}

/*--------------------------------------------------------------------------------------------
 * SLDestroy destroys a SortedList, freeing all dynamically-allocated memory.
 */
void SLDestroy(SortedListPtr list){
  if(list != NULL){
    FreeLinkedList(list->head, list->DESTRUCTOR); //Free Linked List before freeing SortedList
    free(list);
  }
}

int EqualValue(struct node* p1, struct node* p2){ //unused
  int x = strcmp(p1->value, p2->value);
  if(x != 0)
    return 0;
  return 1; //Equality
}



int EqualValue_Count(struct node* p1, struct node* p2){
  int x = strcmp(p1->value, p2->value);
  int y = p1->refCount - p2->refCount;
  if(x != 0)
    return 0;
  if(y != 0)
    return 0;
  return 1; //Equality
}

/* ONLY EITHER FINALSORT OR FIRST/SECONDSORT IS USED, NOT BOTH!!!*/
//May be left over
//Put into proper refCount order
void FirstSort(SortedListPtr list){ //n*n = O(n^2), n = length of linked list
  if(list == NULL)
    return;
  if(list->head == NULL)
    return;
  struct node* beforeprev = NULL;
  struct node* prev = list->head;
  struct node* ptr = prev->next;
  if(prev == NULL || ptr == NULL){
    return;
  }

 RESTARTSWAP:
  beforeprev = NULL;
  prev = list->head;
  ptr = prev->next;
  while(ptr != NULL){  //O(n) work
    if(EqualValue(prev, ptr) && prev->refCount < ptr->refCount){
      //Problem, swap
      struct node* after = ptr->next;                                                                                                        
      ptr->next = prev;                                                                                                                      
      prev->next = after;
      if(prev == list->head)
	list->head = ptr;
      if(beforeprev) //if there was something before prev, adjust the pointer to point to ptr                                                
	beforeprev->next = ptr;                                                                                                              
      struct node* temp = ptr; //Reset pointers back
      ptr = prev;
      prev = temp;
    }
    beforeprev = prev;
    prev = ptr;
    ptr = ptr->next;
  }

  prev = list->head;
  ptr = prev->next;
  while(ptr != NULL){
    if( EqualValue(prev, ptr) && prev->refCount < ptr->refCount ) 
      goto RESTARTSWAP;  //Will go to RESTARTSWAP at most O(n) times
    prev = ptr;
    ptr = ptr->next;
  }
  return;

}

//Put into proper filename order
void SecondSort(SortedListPtr list){ //n*n = O(n^2), n = length of linked list
  if(list == NULL)
    return;
  if(list->head == NULL){
    return;
  }
  struct node* beforeprev = NULL;
  struct node* prev = list->head;
  struct node* ptr = prev->next;
  if(prev == NULL || ptr == NULL){
    return;
  }

 RESTARTSWAP:
  beforeprev = NULL;
  prev = list->head;
  ptr = prev->next;
  while(ptr != NULL){ //O(n) work
    if( EqualValue_Count(prev, ptr) && strcmp(prev->filepath, ptr->filepath) > 0){
      //Problem, swap
      struct node* after = ptr->next;                                                                                                        
      ptr->next = prev;                                                                                                                      
      prev->next = after; 
      if(prev == list->head)
	list->head = ptr;
      if(beforeprev) //if there was something before prev, adjust the pointer to point to ptr                                                
	beforeprev->next = ptr;                                                                                                              
      struct node* temp = ptr; //Reset pointers back
      ptr = prev;
      prev = temp;
    }
    beforeprev = prev;
    prev = ptr;
    ptr = ptr->next;
  }

  prev = list->head;
  ptr = prev->next;
  while(ptr != NULL){ 
    if( EqualValue_Count(prev, ptr) && strcmp(prev->filepath, ptr->filepath) > 0 )
      goto RESTARTSWAP; //Will go to RESTARTSWAP at most O(n) times
    prev = ptr;
    ptr = ptr->next;
  }
  return;
}


int SLInsert(SortedListPtr list, char* newObj, char* pathname){
  struct node* newnode = CreateRecord(newObj, pathname);

    if(newnode == NULL)
        return 0;

    // If there is nothing in the SortedList yet
    if(list->head == NULL){
        list->head = newnode;
        list->head->refCount = 1;
        return 1;
    }

    // Check for duplicate in start of list
    if(list->COMPARATOR(list->head, newnode) == 0){
        list->DESTRUCTOR(newnode);
	list->head->refCount++;
        return 0;
    }

    //struct node* beforeprev = NULL;  //Used to maintain list order
    struct node* prev = NULL;  //Used to insert and maintain list order
    struct node* ptr = list->head;

    while (ptr != NULL && list->COMPARATOR(newnode, ptr) == -1  ) { // //First argument is smaller, iterates ptr to spot of insertion
	prev = ptr;
	ptr = ptr->next;
    }


    if(ptr != NULL && list->COMPARATOR(ptr, newnode) == 0 ){ //
        list->DESTRUCTOR(newnode);
	ptr->refCount++; //Increment refCount, already exists in list
	//Check if list is now out of order BUT ONLY IF PREV IS NOT NULL,
	/********************************************************************/
	/*
	//while(?){ //Loop this somehow??, while iterating before, store the node before the nodes with equal counts and equal value?
	//Insert newnode BEFORE that node...      x+1 x+1 x x x+1 -> x+1 x+1 x+1 x x? but this problem keeps occurring. maybe need to sort
	if(prev != NULL && strcmp(prev->value, ptr->value) == 0 && prev->refCount < ptr->refCount){ //swap ptr and prev if out of order //prev wont be null
	  struct node* after = ptr->next;
	  ptr->next = prev;
	  prev->next = after;
	  if(beforeprev) //if there was something before prev, adjust the pointer to point to ptr
	    beforeprev->next = ptr;
	  ptr = ptr->next;
	}

	//}
	*/
        return 0;
    }


    if(prev == NULL){ //Head of list, prev is null
      newnode->next = ptr;
      list->head = newnode;
    }
    else if(ptr != NULL){ //Middle of list, ptr is not at end
      prev->next = newnode;
      newnode->next = ptr;
    }
    else{ //End of list, ptr is NULL
      prev->next = newnode;
    }


    return 1;
}

/* ONLY EITHER FINALSORT OR FIRST/SECONDSORT IS USED, NOT BOTH!!!*/
//May be left over
SortedListPtr finalSort(SortedListPtr list){
  if(list == NULL || list->head == NULL){
    printf("cant sort empty list\n");
    return NULL;
  }
  struct node * prev = NULL;
  struct node * current = list->head;
  struct node * newHead;
  int stuffMoved = 0;
  while(current->next != NULL){
    //check within the same word section
    struct node * temp = current;
    while((temp != NULL) && (temp->next != NULL) && strcmp(temp->value, temp->next->value) == 0){

      //bubble sort base on frequency value
      if((temp->next != NULL) && (temp->refCount < temp->next->refCount)){
        //flip the values
        if(prev == NULL){ // starting at the head
          newHead =  temp->next;
          temp->next = temp->next->next;
          newHead->next = temp;
          list->head = newHead;
          current = list->head;
          stuffMoved = 1;
        }
        else{
          prev->next = temp->next;
          temp->next = temp->next->next;
          prev->next->next =  temp;
          stuffMoved = 1;
        }
      }

      //bubble sort based on file path
      // only do it when the different paths have the same frequency
      else if((temp->next != NULL) &&
        (temp->refCount == temp->next->refCount) &&
        (strcmp(temp->filepath , temp->next->filepath) > 0 )){

        //flip the values
        if(prev == NULL){ // starting at the head
          newHead =  temp->next;
          temp->next = temp->next->next;
          newHead->next = temp;
          list->head = newHead; // set a new head for the list
          current = list->head;
          stuffMoved = 1;
        }
        else{
          prev->next = temp->next;
          temp->next = temp->next->next;
          prev->next->next =  temp;
          stuffMoved = 1;
        }
      }
      prev = temp;
      temp = temp->next;
    }

    // start sorting from the first the beginning if you reach the end after list was changed during sorting
    prev = current;
    current = current-> next;
    if((stuffMoved == 0) && current->next == NULL){
      break;
    }
    else if(current->next == NULL){
      current =  list->head;
      stuffMoved = 0;
    }
  }
  return list;
}

