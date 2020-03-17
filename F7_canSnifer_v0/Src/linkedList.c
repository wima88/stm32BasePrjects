/**
  ******************************************************************************
  * @file    linkedList.c
  * @author  Wimansha Wijekoon  
  * @brief   Header file for linked list data structure.
  ******************************************************************************
  * @attention
	*
  ******************************************************************************
  */
	#include "linkedList.h"

	

/**
  *@brief  Insert a note to linked list
  *@param  head_ref: pointer to head of the list 
	*@param  new_data: pointer to the data node
	*@param  data_size: size of the data to be stored 
  *@retval None
*/
	
	
void push(struct Node** head_ref, uint32_t *new_data, size_t data_size)
{
	// Allocate memory for node 
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node)); 
  
    new_node->data  = malloc(data_size); 
    new_node->next = (*head_ref); 
  
    // Copy contents of new_data to newly allocated memory. 
    // Assumption: char takes 1 byte. 
    int i; 
    for (i=0; i<data_size; i++) 
	{
        *(uint8_t *)(new_node->data + i) = *(uint8_t *)(new_data + i); 
	}
  
    // Change head pointer as new node is added at the beginning 
    (*head_ref)    = new_node;
}


