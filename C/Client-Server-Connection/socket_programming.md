# Socket Programming Fundamentals

## Internet Layer
The **Internet Protocol** (IP) part of the internet layer and is a set of rules or procedure that is used to define how data is formatted and sent across the internet.

### Key Responsibilities
- **Addressing** - Provides a way to uniquely identify each device with a number using IPv4(32 bits) or IPv6(128 bits)
- **Encapsulation** - Wraps collection of data into packets (datagrams). Each packet is given a source and destination address.
- **Routing** - Specifies how these packets are sent through the routers.
- **Fragmentation** - If data is too large it is split up even more into smaller packets and then reassemble them when it reaches the destination.

## IPv4 (Internet Protocol Version 4)
- Uses a **32 bit address** to label devices.
- Divided into **4 bytes**, with each byte seperated by a dot(.).
- Each number in between the dots ranges from **0-255** (since 2^8 = 256).
### Example
123.243.1.1

## IPv6 (Internet Protocol Version 6)
- Uses a **128 bit address** to label devices
- Divded into **16 bytes**, with every 2 bytes seperated by a colon(:)
### Rules
- Leading 0s are removed.
- Use double colon (::) to collapse a group of zeros.
- If there are multiple seperate groups of zeros you can only choose one to collapse otherwise the address becomes ambiguous because there is no way to know how many zero groups are in each spot.

### Example
Full Address: FA10:0000:0000:0000:0202:B3FF:FE1E:1234<br>
Collapsed Address: FA10::202:B3FF:FE1E:1234

## Transport Protocol

Transport Protocols are a set of rules and procedures that actuallly does the action of sending the data.

| Protocol | Description |
|---|---|
| Transmission Control Protocol (TCP) | Reliable but slow, ensures that data actually arrives and in order. |
| User Datagram Protocol (UDP) | Less Reliable but fast, unconnected, does not check whether datagram actually arrives or not, datagrams are also not guaranteed to arrive in order. |

## Endianess

The standard network uses Big Endian while different computer architectures can use different Endianess.

- Big Endian - The most significant byte (left most byte) is stored in the lowest memory address.
- Little Endian - The least signigicant byte (right most byte) is stored in the lowest memory address.

## Byte Order Conversion Functions
| Function | Description
|----|---|
| htons() | **h**ost **to** **n**etwork **s**hort, for Ports
| htohl() | **h**ost **to** **n**etwork **l**ong, for IP addresses
| ntohs() | **n**etwork **to** **h**ost **s**hort, for Ports
| ntohl() | **n**etwork **to** **h**ost **l**ong, for IP addresses

## Socket (IP Address + Port Number)
* IP address - Unique numerical label for every device connected to a network.
* Port number - A numeric label that is used in networking to direct data to a specific process, service, or application on a device.

## Address Info
```C
struct addrinfo
{
    int             ai_flags;
    int             ai_family;
    int             ai_socktype;
    int             ai_protocol;
    socklen_t       ai_addrlen;
    struct sockaddr *ai_addr;
    char            *ai_canonname;
    struct addrinfo *ai_next;
};
```

* **ai_flags** - Integer bitmask flags that are used to customize behavior of getaddrinfo().
    |Flags|Purpose|
    |---|---|
    | AI_PASSIVE | Sets the address for passive socket for binding.<br><br>When AI_PASSIVE is paired with a NULL hostname in getaddrinfo(), a wildcard IP address is returned as 0.0.0.0 in IPv4 or :: in IPv6, which means the Server will listen to all their IP addresses in their network, (e.g.Wifi, VPN). The general meaning becomes: <br>"Listen on port # for all network interfaces (The things that connect you to the internet) on this device. <br> Or <br>"Bind the socket to port # on all local IP addresses across all network interfaces on the host." |
    | AI_NUMERICHOST | Prevents DNS resolution, input hostname must be numeric address string. |
    | AI_NUMERICSERC | Prevents Service name resolution, The service input must be a numerioc port number.|
    | AI_CANNONNAME | Requests canonical name of remote host (server). |
    | AI_ADDRCONFIG | Returns IPv4 addersses only if IPv4 is configured on the system, and IPv6 only if IPv6 is configured. |
    | AI_V4MAPPED | If no IPv6 addresses found, return IPv4 mapped IPv6 addresses (e.g. ::ffff:192:0.2.1). |
    | AI_ALL | Used with AIV4MAPPED to return both IPv6 and IPv4 mapped IPv6 addresses. |

* **ai_family** - The address family format.
    |Address Family|Info|
    |---|---|
    | IPv4 | 32 bits |
    | IPv6 | 128 bits |
    | AF_UNSPEC | Any address family format |
* **ai_socktype** - The socket type used for the network service.
    | Socket Type | Definition |
    |---|---|
    | SOCK_STREAM | Continuous stream, usually Transmission Control Protocol (TCP). |
    | SOCK_DGRAM | Seperate datagrams, Usually User Datagram Protocol (UDP). |
    | 0 | Any socket type. |
* **ai_protocol** - The language or set of rules the computers will use to communicate with each other when the connection is made.
* **ai_addrlen** - The length of the IP address in bytes
* **ai_addr** - IP Address + Port
    * Server (bind): Represents local address to bind to.
    * Client (connect): Represents the remote address to connect to.
* **ai_cannoname** - The canon (official) name of the remote host (server).
* **ai_next** - A pointer to the next address info struct in the linked list.

## getaddrinfo()

### Description
Uses a hostname and or service (port) to create one or more structures of address info.

### Syntax
```C
int getaddinfo(
    const char *node,               // hostname (e.g. "www.google.com")
    const char *service,            // Port (e.g. "http" or 8080)
    const struct addrinfo *hints,   // Optional used to filter results
    struct addrinfo **res           // Output linked list of results
);

// returns 0 : success
// otherwise : error (use gai_strerr())
```

## socket()

### Description
Creates a new socket endpoint

### Syntax
```C
int socket(
    int domain,   // Protocol Family (Use ai_family)
    int type,     // Socket type
    int protocol, // Protocol type (0 for default)
);

// returns >= 0 : success (socket descriptor)
// returns -1   : error
```

## bind()

### Decription
Associates a socket with a specific IP address and port number. Required by servers before accepting connections.

### Syntax
```C
int bind(
    int sockfd,
    struct sockaddr *my_addr,
    socklen_t addrlen
);

// returns -1: error
```

## connect()

### Decription
Connect the host socket to the remote host socket (client connects to server).

### Syntax
```C
int connect(
    int sockfd,
    struct sockaddr *serv_addr,
    int addrlen
);

// returns -1: error
```

## listen()

### Decription
Listen for more incoming requests or connections from remote host. (server listening for clients)

### Syntax
```C
int listen(
    int sockfd,
    int backlog, // Number of connections allowed in the queue
);

// returns -1 : error
```

## accept()

### Decription
Accept a connection request from remote host (server accept client's request).

### Syntax
```C
int accept(
    int sockfd,
    structaddr *addr, // Store client address
    socklen_t *addrlen // Store client address len
);

// returns -1 : error
// otherwise  : a new socket descriptor for communication with the client.
```

## send()

### Decription
Sends data over a connected socket (usually TCP).

### Syntax
```C
int send(
    int sockfd,
    const void *msg,
    int len,
    int flags
);

// returns -1 : error
// otherwise  : number of bytes processed
```

## recv()

### Decription
Receives data from a connected socket (usually TCP).

### Syntax
```C
int recv(
    int sockfd,
    void *buf,
    int len,
    int flags
);

// returns -1 : error
// otherwise  : number of bytes processed
```

## sendto()

### Decription
Sends data to a specific address (usually UDP).

### Syntax
```C
int sendto(
    int sockfd,
    const void *msg,
    int len,
    unsigned int flags,
    const struct sockaddr *to,
    socklen_t tolen
);

// returns -1 : error
// otherwise  : number of bytes processed
```

## recvfrom()

### Decription
Receives data from a socket and captures the sender’s address (usually UDP).

### Syntax
```C
int recvfrom(
    int sockfd,
    void *buf,
    int len,
    unsigned int flags,
    struct sockaddr *from,
    int *fromlen
);

// returns -1 : error
// otherwise  : number of bytes processed
```