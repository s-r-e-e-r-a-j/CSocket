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
- `CSocket_close` – Closes the socket and cleans up associated resources
(SSL, context, file descriptor).

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
- `CSocket_set_timeout` – Sets a time limit for sending and receiving data on a socket.
  
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

- `CSocket_accept` – Accepts a new client connection on a server socket.

**Example:**
```c
CSocket client = CSocket_accept(&tcp_sock);
```

---

9. **CSocket_connect**
```c
bool CSocket_connect(CSocket *s, const char *host, uint16_t port);
```

- `CSocket_connect` – Connects a client socket to a server at a given IP and port.

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
-  `CSocket_start_threaded_server` – Starts a TCP server that automatically accepts clients and creates a new thread for each connection, running the provided handler function.

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

## Example program's 

### TCP Server
```c

#include "CSocket.h"  // Include our CSocket library
#include <stdio.h>    // Include standard I/O functions

int main() {
    // Create a TCP socket using IPv4
    CSocket server = CSocket_create(CS_TCP, CS_AF_INET);

    // Allow the server to reuse the port immediately after restarting
    CSocket_set_reuseaddr(&server, true);

    // Bind the server to all network interfaces on port 8080
    if (!CSocket_bind(&server, "0.0.0.0", 8080)) {
        perror("Bind failed");
        return 1;
    }

    // Start listening for incoming connections
    if (!CSocket_listen(&server, 5)) {
        perror("Listen failed");
        return 1;
    }

    printf("Server listening on port 8080...\n");

    // Accept a single client connection
    CSocket client = CSocket_accept(&server);

    if (client.fd < 0) {
        perror("Accept failed");
        CSocket_close(&server);
        return 1;
    }

    printf("Client connected!\n");

    // Set a 5-second timeout for receiving data
    CSocket_set_timeout(&client, 5);

    // Make the socket blocking(wait for responses)
    CSocket_set_blocking(&client, true);

    char buffer[CS_BUFFER_SIZE];  // Buffer for incoming data
    const char *reply = "Message received!\n";  // Custom message to send

    while (1) {
        // Receive data from client
        ssize_t r = CSocket_recv(&client, buffer, sizeof(buffer)-1, NULL, NULL);

        // If client disconnected or timed out, break
        if (r <= 0) {
            printf("Client disconnected or timeout.\n");
            break;
        }

        buffer[r] = '\0';              // Null-terminate string
        printf("Received: %s\n", buffer); // Print received message

        // Send  reply to client
        CSocket_send(&client, reply, strlen(reply), NULL, 0);
    }

    // Close sockets
    CSocket_close(&client);
    CSocket_close(&server);

    return 0;
}

```

### TCP Client

```c
#include "CSocket.h"  // Include our CSocket library
#include <stdio.h>    // Include standard I/O functions

int main() {
    // Create a TCP client socket using IPv4
    CSocket client = CSocket_create(CS_TCP, CS_AF_INET);

    // Set a 5-second timeout for receiving data
    CSocket_set_timeout(&client, 5);

    // Make the socket blocking (wait for responses)
    CSocket_set_blocking(&client, true);

    // Connect to the server at 127.0.0.1:8080
    if (!CSocket_connect(&client, "127.0.0.1", 8080)) {
        perror("Connect failed");  // Print error if connection fails
        return 1;                  // Exit program
    }

    printf("Connected to server.\n");  // Inform user that connection succeeded

    const char *msg = "Hello Server!\n";  // Message to send

    // Send the message to the server
    CSocket_send(&client, msg, strlen(msg), NULL, 0);

    char buffer[CS_BUFFER_SIZE];  // Buffer to store server response

    // Receive response from the server
    ssize_t r = CSocket_recv(&client, buffer, sizeof(buffer)-1, NULL, NULL);

    if (r > 0) {
        buffer[r] = '\0';                // Null-terminate the string
        printf("Server replied: %s\n", buffer);  // Print server reply
    } else {
        printf("No reply (timeout or error)\n"); // Inform user if no response
    }

    // Close the client socket
    CSocket_close(&client);

    return 0;  // Exit program successfully
}

```

### UDP Server
```c
#include "CSocket.h"
#include <stdio.h>
#include <string.h>

#define SERVER_PORT 9090

int main() {
    // Create a UDP socket using IPv4
    CSocket server = CSocket_create(CS_UDP, CS_AF_INET);

    // Allow the server to reuse the port immediately
    CSocket_set_reuseaddr(&server, true);

    // Bind the server to all interfaces on port 9090
    if (!CSocket_bind(&server, "0.0.0.0", SERVER_PORT)) {
        perror("Bind failed");
        return 1;
    }

    printf("[UDP Server] Listening on port %d...\n", SERVER_PORT);

    // blocking mode(wait for responses)
    CSocket_set_blocking(&server, true);

    char buffer[CS_BUFFER_SIZE];
    char client_ip[CS_MAX_ADDR_LEN];
    uint16_t client_port;

    while (1) {
        // Receive message from any client
        ssize_t r = CSocket_recv(&server, buffer, sizeof(buffer)-1, client_ip, &client_port);
        if (r <= 0) {
            printf("[UDP Server] Timeout or error, waiting for new messages...\n");
            continue;
        }

        buffer[r] = '\0';   // Null-terminate the string
        printf("[UDP Server] Received from %s:%d -> %s\n", client_ip, client_port, buffer);

        // Send a reply
        const char *reply = "UDP Message received!\n";
        CSocket_send(&server, reply, strlen(reply), client_ip, client_port);
    }

    CSocket_close(&server);
    return 0;
}
```

### UDP Client 
```c
#include "CSocket.h"
#include <stdio.h>
#include <string.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9090

int main() {
    // Create a UDP client socket using IPv4
    CSocket client = CSocket_create(CS_UDP, CS_AF_INET);

    // blocking mode(wait for responses)
    CSocket_set_blocking(&client, true);

    const char *msg = "Hello UDP Server!\n";

    // Send message to server
    CSocket_send(&client, msg, strlen(msg), SERVER_IP, SERVER_PORT);
    printf("[UDP Client] Sent: %s", msg);

    char buffer[CS_BUFFER_SIZE];

    // Receive reply from server
    ssize_t r = CSocket_recv(&client, buffer, sizeof(buffer)-1, NULL, NULL);
    if (r > 0) {
        buffer[r] = '\0';   // Null-terminate the string
        printf("[UDP Client] Server replied: %s", buffer);
    } else {
        printf("[UDP Client] No reply (timeout or error)\n");
    }

    CSocket_close(&client);
    return 0;
}

```
### SSL/TLS TCP Server
```c
#include "CSocket.h"   //  CSocket library
#include <stdio.h>
#include <string.h>

#define SERVER_PORT 8443
#define CERT_FILE "server.crt"   // Path to SSL certificate
#define KEY_FILE  "server.key"   // Path to SSL private key

int main() {
    // Create a TCP socket
    CSocket server = CSocket_create(CS_TCP, CS_AF_INET);

    // Initialize SSL
    CSocket_init_ssl();

    // Enable SSL for this socket
    CSocket_enable_ssl(&server, true, CERT_FILE, KEY_FILE);

    // Allow reusing the port immediately
    CSocket_set_reuseaddr(&server, true);

    // Bind to all network interfaces on SERVER_PORT
    if (!CSocket_bind(&server, "0.0.0.0", SERVER_PORT)) {
        perror("[Server] Bind failed");
        return 1;
    }

    // Start listening
    if (!CSocket_listen(&server, 5)) {
        perror("[Server] Listen failed");
        return 1;
    }

    printf("[Server] Listening on port %d (SSL)...\n", SERVER_PORT);

    // Accept a client connection
    CSocket client = CSocket_accept(&server);
    if (client.fd < 0) {
        perror("[Server] Accept failed");
        CSocket_close(&server);
        return 1;
    }

    printf("[Server] Client connected via SSL!\n");

    char buffer[CS_BUFFER_SIZE];

    // Receive a message from the client
    ssize_t r = CSocket_recv(&client, buffer, sizeof(buffer)-1, NULL, NULL);
    if (r > 0) {
        buffer[r] = '\0';  // Null-terminate the string
        printf("[Server] Received: %s\n", buffer);

        // Send a reply
        const char *reply = "Hello from SSL Server!\n";
        CSocket_send(&client, reply, strlen(reply), NULL, 0);
    } else {
        printf("[Server] No message received.\n");
    }

    // Close client and server sockets
    CSocket_close(&client);
    CSocket_close(&server);
    // Cleans up OpenSSL resources
    CSocket_cleanup_ssl();
    printf("[Server] SSL session closed.\n");
    return 0;
}
```
### SSL/TLS Client
```c
#include "CSocket.h"   // CSocket library
#include <stdio.h>
#include <string.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8443

int main() {
    // Create a TCP client socket
    CSocket client = CSocket_create(CS_TCP, CS_AF_INET);

    // Initilaize SSL
    CSocket_init_ssl();

    // Enable SSL for this client socket
    CSocket_enable_ssl(&client, false, NULL, NULL);  // No cert and key needed for client

    // Connect to SSL server
    if (!CSocket_connect(&client, SERVER_IP, SERVER_PORT)) {
        perror("[Client] Connection failed");
        return 1;
    }

    printf("[Client] Connected to SSL server!\n");

    // Send a message
    const char *msg = "Hello SSL Server!\n";
    if (CSocket_send(&client, msg, strlen(msg), NULL, 0) > 0) {
        printf("[Client] Sent: %s", msg);
    } else {
        printf("[Client] Failed to send message.\n");
    }

    char buffer[CS_BUFFER_SIZE];

    // Receive reply from server
    ssize_t r = CSocket_recv(&client, buffer, sizeof(buffer)-1, NULL, NULL);
    if (r > 0) {
        buffer[r] = '\0';   // Null-terminate the string
        printf("[Client] Server replied: %s", buffer);
    } else {
        printf("[Client] No reply or error.\n");
    }

    // Close the socket
    CSocket_close(&client);
    // Cleans up OpenSSL resources
    CSocket_cleanup_ssl();

    printf("[Client] SSL session closed.\n");
    return 0;
}
```
## License
This project is licensed under the MIT License
