#include "uv-posix-polyfills.h"

#if OS(LINUX) || OS(DARWIN) || OS(FREEBSD)

#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>

// libuv does the annoying thing of #undef'ing these
#include <errno.h>
#if EDOM > 0
#define UV__ERR(x) (-(x))
#else
#define UV__ERR(x) (x)
#endif

void __bun_throw_not_implemented(const char* symbol_name)
{
    CrashHandler__unsupportedUVFunction(symbol_name);
}

// Internals

uint64_t uv__hrtime(uv_clocktype_t type);

#if defined(__linux__)
#include "uv-posix-polyfills-linux.c"
// #elif defined(__MVS__)
// #include "uv/os390.h"
// #elif defined(__PASE__) /* __PASE__ and _AIX are both defined on IBM i */
// #include "uv/posix.h" /* IBM i needs uv/posix.h, not uv/aix.h */
// #elif defined(_AIX)
// #include "uv/aix.h"
// #elif defined(__sun)
// #include "uv/sunos.h"
#elif defined(__APPLE__)
#include "uv-posix-polyfills-darwin.c"
#elif defined(__FreeBSD__)
#include "uv-posix-polyfills-posix.c"
#elif defined(__CYGWIN__) || defined(__MSYS__) || defined(__HAIKU__) || defined(__QNX__) || defined(__GNU__)
#include "uv-posix-polyfills-posix.c"
#endif

uv_pid_t uv_os_getpid()
{
    return getpid();
}

uv_pid_t uv_os_getppid()
{
    return getppid();
}

UV_EXTERN void uv_once(uv_once_t* guard, void (*callback)(void))
{
    if (pthread_once(guard, callback))
        abort();
}

UV_EXTERN uint64_t uv_hrtime(void)
{
    return uv__hrtime(UV_CLOCK_PRECISE);
}

UV_EXTERN int uv_clock_gettime(uv_clock_id clock_id, uv_timespec64_t* ts)
{
    clockid_t clock;

    switch (clock_id) {
    case UV_CLOCK_MONOTONIC:
        clock = CLOCK_MONOTONIC;
        break;
    case UV_CLOCK_REALTIME:
        clock = CLOCK_REALTIME;
        break;
    default:
        return UV_EINVAL;
    }

    struct timespec time;
    if (clock_gettime(clock, &time))
        return UV__ERR(errno);

    ts->tv_sec = time.tv_sec;
    ts->tv_nsec = time.tv_nsec;
    return 0;
}

UV_EXTERN int uv_gettimeofday(uv_timeval64_t* tv)
{
    struct timeval time;
    if (gettimeofday(&time, NULL))
        return UV__ERR(errno);

    tv->tv_sec = time.tv_sec;
    tv->tv_usec = time.tv_usec;
    return 0;
}

// Copy-pasted from libuv
UV_EXTERN void uv_mutex_destroy(uv_mutex_t* mutex)
{
    if (pthread_mutex_destroy(mutex))
        abort();
}

// Copy-pasted from libuv
UV_EXTERN int uv_mutex_init(uv_mutex_t* mutex)
{
    pthread_mutexattr_t attr;
    int err;

    if (pthread_mutexattr_init(&attr))
        abort();

    if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK))
        abort();

    err = pthread_mutex_init(mutex, &attr);

    if (pthread_mutexattr_destroy(&attr))
        abort();

    return UV__ERR(err);
}

// Copy-pasted from libuv
UV_EXTERN int uv_mutex_init_recursive(uv_mutex_t* mutex)
{
    pthread_mutexattr_t attr;
    int err;

    if (pthread_mutexattr_init(&attr))
        abort();

    if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE))
        abort();

    err = pthread_mutex_init(mutex, &attr);

    if (pthread_mutexattr_destroy(&attr))
        abort();

    return UV__ERR(err);
}

// Copy-pasted from libuv
UV_EXTERN void uv_mutex_lock(uv_mutex_t* mutex)
{
    if (pthread_mutex_lock(mutex))
        abort();
}

// Copy-pasted from libuv
UV_EXTERN int uv_mutex_trylock(uv_mutex_t* mutex)
{
    int err;

    err = pthread_mutex_trylock(mutex);
    if (err) {
        if (err != EBUSY && err != EAGAIN)
            abort();
        return UV_EBUSY;
    }

    return 0;
}

// Copy-pasted from libuv
UV_EXTERN void uv_mutex_unlock(uv_mutex_t* mutex)
{
    if (pthread_mutex_unlock(mutex))
        abort();
}

UV_EXTERN void uv_rwlock_destroy(uv_rwlock_t* rwlock)
{
    if (pthread_rwlock_destroy(rwlock))
        abort();
}

UV_EXTERN int uv_rwlock_init(uv_rwlock_t* rwlock)
{
    return UV__ERR(pthread_rwlock_init(rwlock, NULL));
}

UV_EXTERN void uv_rwlock_rdlock(uv_rwlock_t* rwlock)
{
    if (pthread_rwlock_rdlock(rwlock))
        abort();
}

UV_EXTERN void uv_rwlock_rdunlock(uv_rwlock_t* rwlock)
{
    if (pthread_rwlock_unlock(rwlock))
        abort();
}

UV_EXTERN int uv_rwlock_tryrdlock(uv_rwlock_t* rwlock)
{
    int err = pthread_rwlock_tryrdlock(rwlock);
    if (err == EBUSY || err == EAGAIN)
        return UV_EBUSY;
    return UV__ERR(err);
}

UV_EXTERN int uv_rwlock_trywrlock(uv_rwlock_t* rwlock)
{
    int err = pthread_rwlock_trywrlock(rwlock);
    if (err == EBUSY || err == EAGAIN)
        return UV_EBUSY;
    return UV__ERR(err);
}

UV_EXTERN void uv_rwlock_wrlock(uv_rwlock_t* rwlock)
{
    if (pthread_rwlock_wrlock(rwlock))
        abort();
}

UV_EXTERN void uv_rwlock_wrunlock(uv_rwlock_t* rwlock)
{
    if (pthread_rwlock_unlock(rwlock))
        abort();
}

UV_EXTERN void uv_sem_destroy(uv_sem_t* sem)
{
    if (sem_destroy(sem))
        abort();
}

UV_EXTERN int uv_sem_init(uv_sem_t* sem, unsigned int value)
{
    if (sem_init(sem, 0, value))
        return UV__ERR(errno);
    return 0;
}

UV_EXTERN void uv_sem_post(uv_sem_t* sem)
{
    if (sem_post(sem))
        abort();
}

UV_EXTERN int uv_sem_trywait(uv_sem_t* sem)
{
    if (sem_trywait(sem)) {
        if (errno == EAGAIN)
            return UV_EAGAIN;
        return UV__ERR(errno);
    }

    return 0;
}

UV_EXTERN void uv_sem_wait(uv_sem_t* sem)
{
    int err;

    do {
        err = sem_wait(sem);
    } while (err == -1 && errno == EINTR);

    if (err)
        abort();
}

UV_EXTERN void uv_cond_broadcast(uv_cond_t* cond)
{
    if (pthread_cond_broadcast(cond))
        abort();
}

UV_EXTERN void uv_cond_destroy(uv_cond_t* cond)
{
    if (pthread_cond_destroy(cond))
        abort();
}

UV_EXTERN int uv_cond_init(uv_cond_t* cond)
{
    return UV__ERR(pthread_cond_init(cond, NULL));
}

UV_EXTERN void uv_cond_signal(uv_cond_t* cond)
{
    if (pthread_cond_signal(cond))
        abort();
}

UV_EXTERN void uv_cond_wait(uv_cond_t* cond, uv_mutex_t* mutex)
{
    if (pthread_cond_wait(cond, mutex))
        abort();
}

UV_EXTERN unsigned int uv_version(void)
{
    return UV_VERSION_HEX;
}

UV_EXTERN const char* uv_version_string(void)
{
    return "1.51.0";
}

#endif
