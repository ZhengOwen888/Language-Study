# Socket Programming Fundamentals

## IP (Internet Protocol)
The **Internet Protocol** (IP) is a set of rules or procedure that is used to define how data is formatted and sent across the internet.

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
Collapsed Address: FA10::0202:B3FF:FE1E:1234

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
    size_t          ai_addrlen;
    struct sockaddr *ai_addr;
    char            *ai_canonname;
    struct addrinfo *ai_next;
};
```

* ai_flags - Integer bitmask flags that are used to customize behavior of getaddrinfo().
    1. AI_PASSIVE - Sets the address for passive socket for binding. Used as a wildcard by the
    2. AI_NUMERICHOST - Prevents DNS resolution, input hostname must be numeric address string.
    3. AI_NUMERICSERC - Prevents Service name resolution, The service input must be a numerioc port number.
    4. AI_CANNONNAME - Requests canonical name of remote host (server).
    5. AI_ADDRCONFIG - Returns IPv4 addersses only if IPv4 is configured on the system, and IPv6 only if IPv6 is configured.
    6. AI_V4MAPPED - If no IPv6 addresses found, return IPv4 mapped IPv6 addresses (e.g. ::ffff:192:0.2.1)
    7. AI_ALL - Used with AIV4MAPPED to return both IPv6 and IPv4 mapped IPv6 addresses.

* ai_family - The address family format.
    1. IPv4 - 32 bits
    2. IPv6 - 128 bits
    3. AF_UNSPEC - Any address family format
* ai_socktype - The socket type used for the network service.
    1. SOCK_STREAM - Continuous stream, usually Transmission Control Protocol (TCP).
    2. SOCK_DGRAM - Seperate datagrams, Usually User Datagram Protocol (UDP).
    3. 0 - Any socket type.
* ai_protocol - The language or set of rules the computers will use to communicate with each other when the connection is made.
* ai_addrlen - The length of the IP address in bytes
* ai_addr - IP Address + Port
    * Server (bind): Represents local address to bind to.
    * Client (connect): Represents the remote address to connect to.
* ai_cannoname - The canon (official) name of the remote host (server).
* ai_next - A pointer to the next address info struct in the linked list.
