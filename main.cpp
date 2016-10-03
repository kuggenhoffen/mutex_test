
#define TRACE_GROUP "blaa"
#include <assert.h>
#include "mbed.h"
#include "mbed-trace/mbed_trace.h"

static RawSerial pc(USBTX, USBRX);

InterruptIn button(SW2);
static osThreadId t1;
static osThreadId t2;
static Mutex SerialOutMutex;

void stdout_mutex_init() { }
void stdout_mutex_wait()
{
    SerialOutMutex.lock();
}
void stdout_mutex_release()
{
    SerialOutMutex.unlock();
}

static Mutex TraceMutex;
void trace_mutex_init() { }
void trace_mutex_wait()
{
	TraceMutex.lock();
}
void trace_mutex_release()
{
	TraceMutex.unlock();
}

void thread1(void const *args) {
	while (1) {
		osSignalWait(1, osWaitForever);
		tr_info("1111");
	}
}

void thread2(void const *args) {
	while (1) {
		for (int i = 0; i < 10; i++) {
			tr_info("22222222222222222222");
		}
		Thread::wait(10);
	}
}

void trace_printer(const char* str)
{
	stdout_mutex_wait();
    printf("%s\r\n", str);
    fflush(stdout);
    stdout_mutex_release();
}

static void butoun() {
	osSignalSet(t1, 1);
}

int main(void) {
    pc.baud(115200);

    button.fall(&butoun);

    stdout_mutex_init();
    trace_mutex_init();
    mbed_trace_mutex_wait_function_set( trace_mutex_wait );
    mbed_trace_mutex_release_function_set( trace_mutex_release );
    mbed_trace_init();
    mbed_trace_print_function_set( trace_printer );

    static osThreadDef(thread1, osPriorityRealtime, /*1,*/ 2*1024);
    t1 = osThreadCreate(osThread(thread1), NULL);

    static osThreadDef(thread2, osPriorityNormal, /*1,*/ 2*1024);
    t2 = osThreadCreate(osThread(thread2), NULL);


    while(1) {
        Thread::wait(1000);
    }

}
