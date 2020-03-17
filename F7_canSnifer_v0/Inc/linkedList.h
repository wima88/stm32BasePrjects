
/**
  ******************************************************************************
  * @file    linkedList.h
  * @author  Wimansha Wijekoon  
  * @brief   Header file for linked list data structure.
  ******************************************************************************
  * @attention
  *   +None
  ******************************************************************************
  */ 


#ifndef __LINKEDLIST_H
#define __LINKEDLIST_H

#ifdef __cplusplus
extern "C" {
#endif
 
#include "stdio.h" 
#include "stdlib.h" 
#include "main.h"
  
/* A linked list node */
struct Node 
{ 
    // Any data type can be stored in this node 
    uint32_t  *data; 
  
    struct Node *next; 
};


void push(struct Node** head_ref, uint32_t *new_data, size_t data_size) ;


#endif

