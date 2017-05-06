#include <stdlib.h>
#include "Fifo.h"

#include "Fifo.h"


int isEmpty(FIFO *fifo) {

  return (fifo->size == 0);
}

pid_t get_first(FIFO * fifo) {

  pid_t ret;

  if (fifo->size == 0) {
    return -1;
  }
  else {
    ret = fifo->array[0];

    for (int i = 0; i < fifo->size - 1; i++) {
      fifo->array[i] = fifo->array[i + 1];
    }
    fifo->size--;
  }

  return ret;
}

int put_last(FIFO *fifo, pid_t elem) {

  if (fifo->size == fifo->max) {
    return -1;
  } else {
    fifo->array[fifo->size] = elem;
    fifo->size++;
  }
  return 0;
}
