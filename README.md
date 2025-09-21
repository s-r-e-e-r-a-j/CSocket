## CSocket 

**CSocket** is a **powerful, header-only C socket library** that simplifies creating and managing **TCP**, **UDP**, and **RAW sockets** for **IPv4** and **IPv6**. It supports **SSL/TLS connections**, **threaded TCP servers**, and provides utilities for **host resolution** and advanced socket options.

Being **header-only**, it requires no compilation of separate source files—just include `CSocket.h` in your project.

## Features

- **Socket Types**: TCP, UDP, RAW

- **IP Versions**: IPv4, IPv6

- **SSL/TLS**: Server & client support with automatic self-signed certificate generation

- **Blocking/Non-blocking sockets**

- **Timeouts & Reuse address options**

- **Threaded TCP servers** with automatic client handling

- **Host resolution** (DNS → IP)

- **Raw socket support** for advanced packet crafting

- **Header-only**: just include the header, no extra compilation required

- **Powerful**: supports SSL, threaded servers, and RAW sockets in a single, easy-to-use API.





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
**Example:**
```c
CSocket_set_reuseaddr(&tcp_sock, true);
```

---

---

6. **CSocket_bind**
```c
bool CSocket_bind(CSocket *s, const char *host, uint16_t port);
```

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
