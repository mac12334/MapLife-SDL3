#ifndef LINKEDLIST_H

#define LINKEDLIST_H

#include <stdlib.h>

namespace Linked {
    template <typename T>
    
    struct LinkedList { // singularly linked list
        T value; // the value at the index
        LinkedList<T>* nextItem{nullptr}; // pointer to the next item
    };

    /*
    adds an item to the end of the list
    @param list -> the linked list pointer pointer. when no item is present the pointer to the list should be a nullptr
    @param value -> the value of the item you want to add. Should be the same type as the items in the linked list
    @warning this function uses malloc, call Linked::destroyList to deallocate all items in the list
    */
    template <typename T>
    void addItem(LinkedList<T> **list, T value){
        if (*list == nullptr) { // if there are no items in the list, then start the list
            *list = (LinkedList<T>*)malloc(sizeof(LinkedList<T>)); // set the list start to an empty Linked list pointer
            LinkedList<T>* currentItem = *list;
            currentItem->value = value; // give the value to the start
            return;
        }

        LinkedList<T>* currentItem = *list; // iterate through the list until you get to a nullptr
        while (currentItem->nextItem != nullptr) {
            currentItem = currentItem->nextItem;
        }
        // allocate memory to the last item
        currentItem->nextItem = (LinkedList<T>*)malloc(sizeof(LinkedList<T>));
        currentItem->nextItem->value = value; // give the value to the last item in the list
    }

    /*
    does some functionality while iterating through every item of the list.

    - function - you function should look something like so - void funcName(T value, void** parameters) {// logic}.

    - parameters - when you need to have parameters accessed through the array use a void* array (also known as 
    a void** this will allow you to have multiple parameters of different types).


    @param list -> a pointer to the Linked list you want to iterate through
    @param func -> a function pointer that will be used for every item in the array
    @param parameterArray -> the void* array that holds every needed parameter for your function
    */
    template <typename T>
    void doWhile(LinkedList<T>* list, void (*func)(T, void**), void** parameterArray = nullptr) {
        if (list == nullptr) return;

        LinkedList<T>* currentItem = list;
        while (currentItem->nextItem != nullptr) {
            func(currentItem->value, parameterArray);
            currentItem = currentItem->nextItem;
        }
        func(currentItem->value, parameterArray);
    }
    
    /*
    Removes an item from the end of the list. This function deallocated the memory
    given to the item but does not deallocate memory given the item value if the item value
    was allocated using either malloc or new.
    @param list -> the linked list you are removing from
    @warning if the linked list type is a pointer type that has item values allocated using
    dynamic memory allocation (ie malloc or new) then deallocate that memory using the Linked::doWhile
    function to deallocate any memory you use.
    */
    template <typename T>
    void removeItem(LinkedList<T>** list) {
        if (*list == nullptr) return; // list is empty don't remove anymore
        LinkedList<T>* currentItem = *list;
        if (currentItem->nextItem == nullptr) { // if there is only one item in the list
            free(*list); // deallocate memory
            *list = nullptr; // make the list nullptr terminating
            return;
        }
        
        // iterate by looking two steps ahead instead of one step ahead
        while (currentItem->nextItem->nextItem != nullptr) {
            currentItem = currentItem->nextItem;
        }
        
        free(currentItem->nextItem); // deallocate the last item
        currentItem->nextItem = nullptr; // make the list nullptr terminating
    }
    
    /*
    Gets the length of the linked list.
    @param list -> the list you are getting the length of.
    @return the length of the linked list.
    */
    template <typename T>
    int getLength(LinkedList<T>* list) {
        if (list == nullptr) return 0;  // no items in the list, list length is 0
        
        int length = 1; // starts the list length at 1 because there is atleast one item in the list
        
        LinkedList<T>* currentItem = list; // the current is is the first index of the list
        while (currentItem->nextItem != nullptr) { // loop until nullptr-terminated
            currentItem = currentItem->nextItem; // update the item to the next item in the list
            length++; // increment the length
        }
        
        return length; // return the length
    }

    /*
    Gets the item at the intended index (index starts at 0).
    @param list -> the list you want to iterate through.
    @param index -> the index you want to grab from the list (starts at 0).
    @param defaultValue -> if the index is greater than list length this value will be returned instead.
    @return the value at the index provided.
    @warning This method of accessing items is not recommended in a loop if you want to
    access and modify your items in the loop the recommended method is using Linked::doWhile.
    */
    template <typename T>
    T getIndex(LinkedList<T>* list, int index, T defaultValue) {
        if (getLength<T>(list) <= index) return defaultValue;

        LinkedList<T>* currentItem = list;
        for (int i = 0; i < index; i++) {
            currentItem = currentItem->nextItem;
        }
        return currentItem->value;
    }
    
    /*
    destroys every item in the list and will make the list pointer into a nullptr.
    @param list -> a linked list pointer pointer to the first item in the array.
    @warning if the linked list type is a pointer type that has item values allocated using
    dynamic memory allocation (ie malloc or new) then deallocate that memory using the Linked::doWhile
    function to deallocate any memory you use.
    */
    template <typename T>
    void destroyList(LinkedList<T>** list) {
        int length = getLength<T>(*list);
        for (int i = 0; i < length; i++) {
            removeItem<T>(list);
        }
    }
}

#endif