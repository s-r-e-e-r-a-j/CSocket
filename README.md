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




## Socket Creation & Closing

1. **Create Socket**
2. 
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
