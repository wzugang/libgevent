#include <stdio.h>
#include <assert.h>
#include "gevent.h"


gevent_channel ch;

// XXX is assert replaced with '' in NDEBUG?
#define ZERO(x) assert(((x) == 0) && #x);


void func1(gevent_cothread* t) {
    fprintf(stderr, "func1\n");
    void* data;
    ZERO(gevent_channel_receive(&ch, &data));
    fprintf(stderr, "func1 received\n");
    assert(data);
    int c = (int)data;
    assert(c == 10);
    fprintf(stderr, "func1 done\n");
}

void func2(gevent_cothread* t) {
    fprintf(stderr, "func2\n");
    void* data;
    ZERO(gevent_channel_receive(&ch, &data));
    int c;
    c = (int)data;
    assert(c == 20);
    fprintf(stderr, "func2 done\n");
}

int main() {
    gevent_hub* hub = gevent_default_hub();
    gevent_cothread t1, t2;
    gevent_channel_init(hub, &ch);
    gevent_spawn(hub, &t1, func1);
    gevent_spawn(hub, &t2, func2);
    assert(t1.state == GEVENT_COTHREAD_NEW);
    assert(t2.state == GEVENT_COTHREAD_NEW);
    gevent_channel_send(&ch, (void*)10);
    assert(t1.state == GEVENT_COTHREAD_DEAD);
    assert(t2.state != GEVENT_COTHREAD_DEAD);
    gevent_channel_send(&ch, (void*)20);
    assert(t1.state == GEVENT_COTHREAD_DEAD);
    assert(t2.state == GEVENT_COTHREAD_DEAD);
    return 0;
}
