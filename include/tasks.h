#ifndef TASKS_H
#define TASKS_H

void can_read_task(void *params);

void can_write_task(void *params);

void cdc_task(void *params);

void usbd_task(void *params);

void idle_task(void *params);

#endif /* TASKS_H */