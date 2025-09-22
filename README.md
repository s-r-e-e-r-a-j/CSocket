## CSocket 

**CSocket** is a powerful, header-only C networking library that simplifies working with TCP, UDP, and RAW sockets for IPv4 and IPv6. It includes SSL/TLS support, threaded TCP servers, host resolution, and advanced socket options.

## Features

- **Socket Types**: TCP, UDP, RAW

- **IP Versions**: IPv4, IPv6

- **SSL/TLS**: Supports SSL/TLS communication. The server can use a custom certificate and private key, or automatically generate a self-signed certificate and key if none are provided. Clients can securely connect over SSL/TLS.

- **Blocking/Non-blocking sockets**

- **Timeouts & Reuse address options**

- **Threaded TCP servers** with automatic client handling

- **Host resolution** (DNS → IP)

- **Raw socket support** for advanced packet crafting

- **Header-only**: just include the header, no extra compilation required

- **Powerful**: supports SSL, threaded servers, and RAW sockets in a single, easy-to-use API.


## Setup

1. **Place `CSocket.h` in the same directory as your `.c` source file.**
   
Example:

```bash
project/
├── CSocket.h
└── your_program.c
```

2. **Include the header**
   
At the top of your `.c` file, include the library:

```c
#include "CSocket.h"
```

3. **Install OpenSSL**(required for SSL/TLS features).

**Debian/Ubuntu/Parrot/Kali:**
```bash
sudo apt update
sudo apt install libssl-dev
```

**RHEL/CentOS/Fedora:**
```bash
sudo dnf install openssl-devel
```

**Arch Linux/Manjaro:**
```bash
sudo pacman -S openssl
```

**Termux (Android):**
```bash
pkg install openssl
```

4. **Compile your program**
Link with **pthread** and **OpenSSL**:

```bash
gcc your_program.c -o your_program -pthread -lssl -lcrypto
```

5. **Run your program**

```bash
./your_program
```

## API Examples


1. **Create Socket**
```c
CSocket CSocket_create(CS_Type type, CS_Family family);
```
- **type** → `CS_TCP`, `CS_UDP`, `CS_RAW`

- **family** → `CS_AF_INET` (IPv4), `CS_AF_INET6` (IPv6)

**Example:**

```c
CSocket tcp_sock = CSocket_create(CS_TCP, CS_AF_INET);
CSocket udp_sock = CSocket_create(CS_UDP, CS_AF_INET6);
```

---

2. **Close Socket**
```c
void CSocket_close(CSocket *s);
```
**Example:**
```c
CSocket_close(&tcp_sock);
CSocket_close(&udp_sock);
```

---

3. **Set Blocking / Non-blocking**
```c
void CSocket_set_blocking(CSocket *s, bool blocking);
```
- **Non-blocking:** sockets return immediately if no data is available, instead of waiting.

- **Blocking:** sockets wait until data is available before returning.

**Example:**
```c
CSocket_set_blocking(&tcp_sock, false); // Non-blocking
```

---

4. **Set Timeout**
```c
void CSocket_set_timeout(CSocket *s, int sec);
```
**Example:**
```c
CSocket_set_timeout(&udp_sock, 5); // 5 seconds timeout
```

---

5. **Set Reuse Address**
```c
void CSocket_set_reuseaddr(CSocket *s, bool reuse);
```
- `CSocket_set_reuseaddr` – Allows a server socket to reuse a port immediately after it has been closed, preventing “Address already in use” errors when restarting the server.

**Example:**
```c
CSocket_set_reuseaddr(&tcp_sock, true);
```

---

6. **CSocket_bind**
```c
bool CSocket_bind(CSocket *s, const char *host, uint16_t port);
```
- `CSocket_bind` – Assigns IP and port to a socket.

**Example (TCP):**
```c
CSocket_bind(&tcp_sock, "127.0.0.1", 5555);
```

**Example (UDP):**
```c
CSocket_bind(&udp_sock, "0.0.0.0", 5000);
```

---

7. **CSocket_listen (TCP only)**
```c
bool CSocket_listen(CSocket *s, int backlog);
```
- `CSocket_listen` – Prepares a TCP socket to accept incoming connections.
  
**Example:**
  
```c
CSocket_listen(&tcp_sock, 10); // Max 10 pending connections in the queue
```

---

8. **CSocket_accept (TCP only)**
```c
CSocket CSocket_accept(CSocket *s);
```
**Example:**
```c
CSocket client = CSocket_accept(&tcp_sock);
```

---

9. **CSocket_connect**
```c
bool CSocket_connect(CSocket *s, const char *host, uint16_t port);
```

**Example (TCP client):**
```c
CSocket_connect(&tcp_sock, "127.0.0.1", 5555);
```
**Example (UDP client, optional bind not needed):**
```c
CSocket_connect(&udp_sock, "127.0.0.1", 5000);
```

---

10. **CSocket_send**
```c
ssize_t CSocket_send(CSocket *s, const void *buf, size_t len, const char *host, uint16_t port);
```

**Examples:**

**TCP (connection-oriented, host/port not needed):**

```c
const char *msg = "Hello TCP";
CSocket_send(&tcp_sock, msg, strlen(msg), NULL, 0);
```
`len = strlen(msg)` → Number of bytes to send

`host` and `port` are ignored for TCP because the socket is already connected

**UDP (connectionless, host/port required):**
```c
const char *msg = "Hello UDP";
CSocket_send(&udp_sock, msg, strlen(msg), "127.0.0.1", 5000);
```


`len = strlen(msg)` → Number of bytes to send

`host = "127.0.0.1"` → Destination IP

`port = 5000` → Destination port

---

11. **CSocket_recv**
```c
ssize_t CSocket_recv(CSocket *s, void *buf, size_t len, char *host, uint16_t *port);
```
- **s** → Pointer to `CSocket` object.

- **buf** → Buffer to store received data.

- **len** → Size of the buffer.

- **host** → (UDP only) Optional, stores sender’s IP.

- **port** → (UDP only) Optional, stores sender’s port.

**Example (TCP):**
```c
char buf[1024];
CSocket_recv(&tcp_sock, buf, sizeof(buf), NULL, NULL);
```
For TCP, `host` and `port` are ignored, so `NULL` can be passed.    


**Example (UDP):**

```c
char buf[1024];
char sender[128];
uint16_t sender_port;
CSocket_recv(&udp_sock, buf, sizeof(buf), sender, &sender_port);
```
For UDP, you usually want to know who sent the packet, so provide `host` and `port`.

---

12. **CSocket_sendall**
```c
bool CSocket_sendall(CSocket *s, const void *buf, size_t len);
```

- **s** → Pointer to `CSocket`.

- **buf** → Data buffer to send.

- **len** → Number of bytes to send.

**Example (TCP):**
```c
const char *msg = "This will send the full message even if split into chunks";
CSocket_sendall(&tcp_sock, msg, strlen(msg));
```
- Sends all **data** (retries internally if needed).

- Works only with **TCP/SSL** (stream sockets).

- Not used for UDP (datagrams).

---

13. **CSocket_recv_line**
```c
ssize_t CSocket_recv_line(CSocket *s, char *buf, size_t max_len);
```

- Reads until a newline `\n` is received.

- **TCP-only** (stream-oriented).

**Example (TCP):**
```c
char buf[1024];
CSocket_recv_line(&tcp_sock, buf, sizeof(buf));
```

---

14. **CSocket_recv_until**
```c
ssize_t CSocket_recv_until(CSocket *s, char *buf, size_t max_len, char delim);
```

- Reads from the socket until the given delimiter character `delim` is found.

- **TCP-only** (stream-oriented).

- The delimiter is **any character you choose**. Common examples:

   - `'\n'` → read until      newline (line-based  protocol).

   - `';'` → read until semicolon.

   - `':'` → read until colon.


**Example (TCP, read until `;`):**
```c
char buf[1024];
CSocket_recv_until(&tcp_sock, buf, sizeof(buf), ';'); // reads until ';' is found
```

**Example (TCP, read until newline):**
```c
char buf[1024];
CSocket_recv_until(&tcp_sock, buf, sizeof(buf), '\n'); // reads until '\n'
```

---

15. **CSocket_resolve_host**
```c
bool CSocket_resolve_host(const char *hostname, char *ip, CS_Family family);
```

- Resolves a hostname to an IP address (IPv4 or IPv6).

- Returns `true` on success, `false` on failure.

**Example (IPv4):**
```c
char ip[CS_MAX_ADDR_LEN];
if (CSocket_resolve_host("facebook.com", ip, CS_AF_INET)) {
    printf("Resolved IP: %s\n", ip);
} else {
    printf("Failed to resolve hostname\n");
}
```

**Example (IPv6):**
```c
char ip[CS_MAX_ADDR_LEN];
if (CSocket_resolve_host("facebook.com", ip, CS_AF_INET6)) {
    printf("Resolved IP: %s\n", ip);
} else {
    printf("Failed to resolve hostname\n");
}
```

- For **IPv4**, the resolved IP will be in dotted-decimal form (e.g., `127.0.0.1`).

- For **IPv6**, the resolved IP will be in standard IPv6 format.

- Always check the return value: `true` = success, `false` = failure.

---


16. **CSocket_init_ssl**
```c
void CSocket_init_ssl();
```

- Initializes the OpenSSL library.

- Must be called **once before using SSL sockets**.

**Example:**
```c
CSocket_init_ssl();
```
---

17. **CSocket_cleanup_ssl**
```c
void CSocket_cleanup_ssl();
```

- Cleans up OpenSSL resources.

- Call this when your program is done with SSL.

**Example:**
```c
CSocket_cleanup_ssl();
```

---

18. **CSocket_enable_ssl**
```c
bool CSocket_enable_ssl(CSocket *s, bool server, const char *cert_file, const char *key_file);
```

- Enables SSL/TLS for a socket.

- Arguments:

  - `s` → pointer to your `CSocket`.

  - `server` → `true` for server mode, `false` for client mode.

  - `cert_file` → path to certificate file (required for server).

  - `key_file` → path to private key file (required for server).

- Returns `true` on success, `false` on error.

- If the certificate/key file is missing, a self-signed certificate and key is automatically generated.

**Example (Server):**
```c
CSocket tcp_server = CSocket_create(CS_TCP, CS_AF_INET);
CSocket_bind(&tcp_server, "127.0.0.1", 4433);
CSocket_listen(&tcp_server, 5);

CSocket_enable_ssl(&tcp_server, true, "server.crt", "server.key");
```

**Example (Client):**
```c
CSocket tcp_client = CSocket_create(CS_TCP, CS_AF_INET);
CSocket_connect(&tcp_client, "127.0.0.1", 4433);
CSocket_enable_ssl(&tcp_client, false, NULL, NULL);
```

---

19. **CSocket_ssl_send**
```c
ssize_t CSocket_ssl_send(CSocket *s, const void *buf, size_t len);
```

- Sends data over an SSL/TLS connection.

- Arguments:

  - `s` → pointer to your SSL-enabled `CSocket`.

  - `buf` → data buffer to send.

  - `len` → length of data.

- Returns `-1` on error.

**Example:**
```c
CSocket_ssl_send(&tcp_client, "Hello SSL", strlen("Hello SSL"));
```

---

20. **CSocket_ssl_recv**
```c
ssize_t CSocket_ssl_recv(CSocket *s, void *buf, size_t len);
```

- Receives data over an SSL/TLS connection.

- Arguments:

  - `s` → pointer to your SSL-enabled `CSocket`.

  - `buf` → buffer to store received data.

  - `len` → size of buffer.

- Returns -1 on error.

**Example:**
```c
char buf[1024];
CSocket_ssl_recv(&tcp_client, buf, sizeof(buf));
```

---

21. **CSocket_start_threaded_server**
- **TCP-only**
```c
bool CSocket_start_threaded_server(
    CSocket *server, 
    int backlog, 
    CSocket_client_handler handler, 
    void *userdata
);
```

**Example**
```c
#include "CSocket.h"
#include <stdio.h>

void handle_client(CSocket client, void *userdata) {
    char buffer[1024];
    ssize_t n;
    while((n = CSocket_recv(&client, buffer, sizeof(buffer), NULL, NULL)) > 0) {
        // For text-based data, null-terminate for printf
        if(n < sizeof(buffer)) buffer[n] = '\0';
        printf("Received: %s\n", buffer);
        CSocket_send(&client, buffer, n, NULL, 0); // echo back
    }
}

int main() {
    CSocket server = CSocket_create(CS_TCP, CS_AF_INET);
    CSocket_bind(&server, "0.0.0.0", 5555);

    printf("Starting threaded TCP server on port 5555...\n");
    CSocket_start_threaded_server(&server, 10, handle_client, NULL);
    CSocket_close(&server);
    return 0;
}

```

## License
This project is licensed under the MIT License
