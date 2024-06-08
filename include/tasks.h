#define STACK_SIZE configMINIMAL_STACK_SIZE * 2

static void can_read_task(void *params);
StackType_t can_read_task_stack[STACK_SIZE];
StaticTask_t can_read_task_taskdef;

static void can_write_task(void *params);
StackType_t can_write_task_stack[STACK_SIZE];
StaticTask_t can_write_task_taskdef;
TaskHandle_t can_write_task_handle;

static void cdc_task(void *params);
StackType_t cdc_task_stack[STACK_SIZE];
StaticTask_t cdc_task_taskdef;

static void usbd_task(void *params);
StackType_t usbd_stack[STACK_SIZE];
StaticTask_t usbd_taskdef;

static void idle_task(void *params);
StackType_t idle_task_stack[128];
StaticTask_t idle_task_taskdef;