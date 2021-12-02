#ifndef OPERATIONS_HPP
#define OPERATIONS_HPP

#include <stdlib.h>


class List {
private:
    void **items;
    size_t size;
    size_t item_size;

public:

    List(size_t item_size) {
        this->size = 0;
        this->item_size = item_size;
        this->items = 0;
    }

    void list_push(void *item) {
        this->size += 1;

        if(this->items == nullptr) {

            this->items = new void*;

        } else {

            this->items = (void**)realloc(this->items, (this->size * this->item_size));
        }

        this->items[this->size - 1] = item;
    }


    void *getItems(int i) { return this->items[i]; }


};

#endif