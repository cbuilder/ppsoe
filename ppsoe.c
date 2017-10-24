#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>

int main( void )
{
    int sock;
    struct sockaddr_in sa;
    int bytes_sent;
    char buffer[16]="Hello";
    struct timespec tp;
    struct tm *tms;

    sock = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if ( sock == -1 )
    {
        printf("Ошибка создания сокета");
        return 0;
    }
    sa.sin_family = PF_INET;
    sa.sin_addr.s_addr = htonl(0x7F000001);
    sa.sin_port = htons(7654);

    clock_gettime(CLOCK_REALTIME, &tp);
    tp.tv_sec = tp.tv_sec + 1;
    tp.tv_nsec = 0;
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &tp, NULL);

    while(1) {
        /* Шлём секундную метку */
        bytes_sent =
            sendto(
                sock,
                buffer,
                strlen(buffer),
                0,
                ( struct sockaddr* )&sa,
                sizeof( struct sockaddr_in )
            );

        if ( bytes_sent < 0 )
            printf( "Ошибка отправки пакета: %s\n", strerror( errno ) );
        /* Подготовка пакета на следующую секунду */
        tp.tv_sec = tp.tv_sec + 1;
        tms = gmtime(&tp.tv_sec);
        memset(buffer, 0, 16);
        sprintf(buffer, "%04d%02d%02d%02d%02d%02dZ",
			tms->tm_year+1900,
			tms->tm_mon+1,
			tms->tm_mday,
			tms->tm_hour,
			tms->tm_min,
			tms->tm_sec);
        /* Ожидание следующей секунды */
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &tp, NULL);
    }
    close(sock);
    return 0;
}
