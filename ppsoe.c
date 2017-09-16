#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>

#if 0
int itimer_init() {
    timer_create;
    timer_settime;
}
#endif
int main( void )
{
    int sock;
    struct sockaddr_in sa;
    int bytes_sent;
    const char* buffer = "Привет!";
    int buffer_length;

    buffer_length = strlen( buffer ) + 1;

    sock = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );

    if ( sock == -1 )
    {
        printf("Ошибка создания сокета");
        return 0;
    }

    sa.sin_family = PF_INET;
    sa.sin_addr.s_addr = htonl( 0x7F000001 );
    sa.sin_port = htons( 7654 );

    while(1) {
        bytes_sent =
            sendto(
                sock,
                buffer,
                strlen( buffer ) + 1,
                0,
                ( struct sockaddr* )&sa,
                sizeof( struct sockaddr_in )
            );

        if ( bytes_sent < 0 )
            printf( "Ошибка отправки пакета: %s\n", strerror( errno ) );
        sleep(1);
    }
    close( sock );
    return 0;
}

