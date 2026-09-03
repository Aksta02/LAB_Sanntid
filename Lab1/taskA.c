#include <sys/times.h>
#include <unistd.h>

void busy_wait(int t)
{
    struct tms buf;

    long ticks_per_sec = sysconf(_SC_CLK_TCK);

    clock_t start = times(&buf);

    while (1) {
        clock_t now = times(&buf);

        if ((now - start) >= t * ticks_per_sec) {
            break;
        }
    }
}

int main(){
	busy_wait(1);
	return 0;
}
