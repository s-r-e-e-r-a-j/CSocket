// Developer: Sreeraj
// GitHub: https://github.com/s-r-e-e-r-a-j

#ifndef CSOCKET_H
#define CSOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <netdb.h>
#include <pthread.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>

#define CS_MAX_ADDR_LEN 128
#define CS_BUFFER_SIZE 4096

typedef enum { CS_TCP, CS_UDP, CS_RAW } CS_Type;
typedef enum { CS_AF_INET = AF_INET, CS_AF_INET6 = AF_INET6 } CS_Family;
typedef enum { CS_PLAIN, CS_SSL } CS_Protocol;

typedef struct {
    int fd;
    CS_Type type;
    CS_Family family;
    bool is_server;
    int last_error;
    bool blocking;
    CS_Protocol protocol;
    SSL *ssl;
    SSL_CTX *ctx;
} CSocket;

// OpenSSL Init
static inline void CSocket_init_ssl(){
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

static inline void CSocket_cleanup_ssl(){
    EVP_cleanup();
}

// Auto-generate Self-signed Certificate 
static bool CSocket_generate_cert(const char *cert_file, const char *key_file){
    int ret = system("openssl req -x509 -newkey rsa:2048 -nodes -keyout server.key -out server.crt -days 365 -subj \"/CN=localhost\"");
    return ret == 0;
}

// Socket Creation 
static inline CSocket CSocket_create(CS_Type type, CS_Family family){
    CSocket s;
    s.type = type;
    s.family = family;
    s.is_server = false;
    s.last_error = 0;
    s.blocking = true;
    s.protocol = CS_PLAIN;
    s.ssl = NULL;
    s.ctx = NULL;
    s.fd = socket(family, type==CS_TCP?SOCK_STREAM:(type==CS_UDP?SOCK_DGRAM:SOCK_RAW), 0);
    return s;
}

static inline void CSocket_close(CSocket *s){
    if(s->ssl) { SSL_shutdown(s->ssl); SSL_free(s->ssl); s->ssl=NULL; }
    if(s->ctx) { SSL_CTX_free(s->ctx); s->ctx=NULL; }
    if(s->fd>=0) close(s->fd);
    s->fd=-1;
}

// Socket Options 
static inline void CSocket_set_blocking(CSocket *s,bool blocking){
    int flags=fcntl(s->fd,F_GETFL,0);
    if(blocking) flags &= ~O_NONBLOCK;
    else flags |= O_NONBLOCK;
    fcntl(s->fd,F_SETFL,flags);
    s->blocking=blocking;
}

static inline void CSocket_set_timeout(CSocket *s,int sec){
    struct timeval tv={.tv_sec=sec,.tv_usec=0};
    setsockopt(s->fd,SOL_SOCKET,SO_RCVTIMEO,(const char*)&tv,sizeof(tv));
    setsockopt(s->fd,SOL_SOCKET,SO_SNDTIMEO,(const char*)&tv,sizeof(tv));
}

static inline void CSocket_set_reuseaddr(CSocket *s,bool reuse){
    int opt = reuse?1:0;
    setsockopt(s->fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
}

// SSL/TLS Functions 
static bool CSocket_enable_ssl(CSocket *s,bool server, const char *cert_file, const char *key_file){
    s->ctx = server ? SSL_CTX_new(TLS_server_method()) : SSL_CTX_new(TLS_client_method());
    if(!s->ctx) { s->last_error = ERR_get_error(); return false; }

    // Auto-generate cert if missing
    FILE *f = fopen(cert_file,"r");
    if(!f) CSocket_generate_cert(cert_file, key_file);
    else fclose(f);

    if(server){
        if(SSL_CTX_use_certificate_file(s->ctx, cert_file, SSL_FILETYPE_PEM) <=0) { s->last_error=ERR_get_error(); return false; }
        if(SSL_CTX_use_PrivateKey_file(s->ctx, key_file, SSL_FILETYPE_PEM) <=0) { s->last_error=ERR_get_error(); return false; }
    }

    s->ssl = SSL_new(s->ctx);
    if(!s->ssl) { s->last_error=ERR_get_error(); return false; }

    SSL_set_fd(s->ssl, s->fd);
    s->protocol = CS_SSL;
    return true;
}

static inline ssize_t CSocket_ssl_send(CSocket *s,const void *buf,size_t len){
    if(s->protocol!=CS_SSL) return -1;
    return SSL_write(s->ssl, buf, len);
}

static inline ssize_t CSocket_ssl_recv(CSocket *s,void *buf,size_t len){
    if(s->protocol!=CS_SSL) return -1;
    return SSL_read(s->ssl, buf, len);
}

// Server Functions 
static bool CSocket_bind(CSocket *s,const char *host,uint16_t port){
    struct sockaddr_in addr4;
    struct sockaddr_in6 addr6;
    int r=-1;
    if(s->family==CS_AF_INET){
        addr4.sin_family=AF_INET;
        addr4.sin_port=htons(port);
        addr4.sin_addr.s_addr=inet_addr(host);
        r=bind(s->fd,(struct sockaddr*)&addr4,sizeof(addr4));
    } else {
        addr6.sin6_family=AF_INET6;
        addr6.sin6_port=htons(port);
        inet_pton(AF_INET6,host,&addr6.sin6_addr);
        r=bind(s->fd,(struct sockaddr*)&addr6,sizeof(addr6));
    }
    if(r<0){ s->last_error=errno; return false;}
    s->is_server=true;
    return true;
}

static bool CSocket_listen(CSocket *s,int backlog){
    if(s->type!=CS_TCP) return false;
    if(listen(s->fd,backlog)<0){ s->last_error=errno; return false; }
    return true;
}

static CSocket CSocket_accept(CSocket *s){
    CSocket client = CSocket_create(s->type,s->family);
    struct sockaddr_storage addr;
    socklen_t len=sizeof(addr);
    client.fd = accept(s->fd,(struct sockaddr*)&addr,&len);
    if(s->protocol==CS_SSL && s->ssl){
        client.ssl = SSL_new(s->ctx);
        SSL_set_fd(client.ssl, client.fd);
        SSL_accept(client.ssl);
        client.protocol = CS_SSL;
    }
    return client;
}

// Client Functions
static bool CSocket_connect(CSocket *s,const char *host,uint16_t port){
    struct sockaddr_in addr4;
    struct sockaddr_in6 addr6;
    int r=-1;
    if(s->family==CS_AF_INET){
        addr4.sin_family=AF_INET;
        addr4.sin_port=htons(port);
        addr4.sin_addr.s_addr=inet_addr(host);
        r=connect(s->fd,(struct sockaddr*)&addr4,sizeof(addr4));
    } else {
        addr6.sin6_family=AF_INET6;
        addr6.sin6_port=htons(port);
        inet_pton(AF_INET6,host,&addr6.sin6_addr);
        r=connect(s->fd,(struct sockaddr*)&addr6,sizeof(addr6));
    }
    if(r<0){ s->last_error=errno; return false;}
    if(s->protocol==CS_SSL && s->ssl) SSL_connect(s->ssl);
    return true;
}

// Data Transmission 
static inline ssize_t CSocket_send(CSocket *s,const void *buf,size_t len,const char *host,uint16_t port){
    if(s->protocol==CS_SSL) return CSocket_ssl_send(s, buf, len);
    if(s->type==CS_TCP) return send(s->fd,buf,len,0);
    else {
        struct sockaddr_in addr4; struct sockaddr_in6 addr6;
        if(s->family==CS_AF_INET){
            addr4.sin_family=AF_INET; addr4.sin_port=htons(port); addr4.sin_addr.s_addr=inet_addr(host);
            return sendto(s->fd,buf,len,0,(struct sockaddr*)&addr4,sizeof(addr4));
        } else {
            addr6.sin6_family=AF_INET6; addr6.sin6_port=htons(port); inet_pton(AF_INET6,host,&addr6.sin6_addr);
            return sendto(s->fd,buf,len,0,(struct sockaddr*)&addr6,sizeof(addr6));
        }
    }
}

static inline bool CSocket_sendall(CSocket *s,const void *buf,size_t len){
    size_t total=0; const char *ptr=(const char*)buf;
    while(total<len){
        ssize_t sent = (s->protocol==CS_SSL) ? SSL_write(s->ssl, ptr+total, len-total) : send(s->fd,ptr+total,len-total,0);
        if(sent<=0){ s->last_error=errno; return false; }
        total+=sent;
    }
    return true;
}

static inline ssize_t CSocket_recv(CSocket *s,void *buf,size_t len,char *host,uint16_t *port){
    if(s->protocol==CS_SSL) return SSL_read(s->ssl, buf, len);
    if(s->type==CS_TCP) return recv(s->fd,buf,len,0);
    else {
        struct sockaddr_in addr4; struct sockaddr_in6 addr6;
        socklen_t addrlen=sizeof(addr4);
        ssize_t r = recvfrom(s->fd,buf,len,0,(struct sockaddr*)&addr4,&addrlen);
        if(host) strcpy(host,inet_ntoa(addr4.sin_addr));
        if(port) *port=ntohs(addr4.sin_port);
        return r;
    }
}

static inline size_t CSocket_recv_until(CSocket *s,char *buf,size_t max_len,char delim){
    size_t i=0;
    while(i<max_len-1){
        char c;
        ssize_t r = (s->protocol==CS_SSL) ? SSL_read(s->ssl,&c,1) : recv(s->fd,&c,1,0);
        if(r<=0) break;
        buf[i++] = c;
        if(c==delim) break;
    }
    buf[i]='\0';
    return i;
}

static inline size_t CSocket_recv_line(CSocket *s,char *buf,size_t max_len){
    return CSocket_recv_until(s,buf,max_len,'\n');
}

// Host Utilities 
static inline bool CSocket_resolve_host(const char *hostname,char *ip,CS_Family family){
    struct addrinfo hints,*res;
    memset(&hints,0,sizeof(hints));
    hints.ai_family = family==CS_AF_INET?AF_INET:AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    if(getaddrinfo(hostname,NULL,&hints,&res)!=0) return false;
    if(family==CS_AF_INET){ struct sockaddr_in *a=(struct sockaddr_in*)res->ai_addr; strcpy(ip,inet_ntoa(a->sin_addr)); }
    else { struct sockaddr_in6 *a=(struct sockaddr_in6*)res->ai_addr; inet_ntop(AF_INET6,&a->sin6_addr,ip,CS_MAX_ADDR_LEN); }
    freeaddrinfo(res); return true;
}

// Threaded TCP Server 
typedef void (*CSocket_client_handler)(CSocket client, void *userdata);

typedef struct {
    CSocket client;
    CSocket_client_handler handler;
    void *userdata;
} CSocketThreadArg;

static void *CSocket_client_thread(void *arg){
    CSocketThreadArg *data = (CSocketThreadArg*)arg;
    data->handler(data->client, data->userdata);
    CSocket_close(&data->client);
    free(data);
    return NULL;
}

static bool CSocket_start_threaded_server(CSocket *server, int backlog, CSocket_client_handler handler, void *userdata){
    if(!CSocket_listen(server, backlog)) return false;
    while(1){
        CSocket client = CSocket_accept(server);

        CSocketThreadArg *arg = malloc(sizeof(CSocketThreadArg));
        arg->client = client;
        arg->handler = handler;
        arg->userdata = userdata;

        pthread_t tid;
        pthread_create(&tid,NULL,CSocket_client_thread,arg);
        pthread_detach(tid);
    }
    return true;
}

#endif
