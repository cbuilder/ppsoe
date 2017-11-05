#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
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
    int bc = 1;
    struct ifreq ifr;
    int ttl = 1;

    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if ( sock == -1 ) {
        printf("Ошибка создания сокета");
        return 0;
    }
    if (-1 == setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &bc, sizeof(bc))) {
        printf( "Ошибка настройки broadcast: %s\n", strerror(errno));
        return 0;
    }
    sa.sin_family = AF_INET;
    setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
    if (ioctl(sock, SIOCGIFBRDADDR, &ifr) < 0)
        perror("ioctl");
    struct sockaddr_in *sinp = &ifr.ifr_broadaddr;
    sa.sin_addr.s_addr = sinp->sin_addr.s_addr;
    sa.sin_port = htons(7654);

    clock_gettime(CLOCK_REALTIME, &tp);
    tp.tv_sec = tp.tv_sec + 1;
    tp.tv_nsec = 0;
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &tp, NULL);

    while(1) {
        /* Шлём секундную метку */
        bytes_sent = sendto(
                sock,
                buffer,
                strlen(buffer),
                0,
                (struct sockaddr *)&sa,
                sizeof(struct sockaddr_in));
        if ( bytes_sent < 0 )
            printf( "Ошибка отправки пакета: %s\n", strerror(errno));
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
