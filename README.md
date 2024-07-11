
# Airfare-Seat-Reservation-System

This project is a simple client-server application that simulates an airfare seat reservation system. It includes a server that manages seat reservations and multiple clients that request seat reservations. The project consists of three files: `client.cpp`, `server.cpp`, and `myinifile.txt`.

## Files

- **client.cpp**: This file contains the client-side code that connects to the server, sends seat reservation requests, and handles responses from the server.
- **server.cpp**: This file contains the server-side code that manages seat reservations, accepts connections from clients, and processes their requests.
- **myinifile.txt**: This file contains configuration settings for the client's connection to the server.

## Features

- **Seat Map Management**: The server maintains a 10x10 seat map and tracks available seats.
- **Client-Server Communication**: Clients can connect to the server, request seat reservations, and receive updates on seat availability.
- **Manual and Automatic Modes**: Clients can operate in manual mode (user specifies seat) or automatic mode (seats are randomly chosen).
- **Error Handling**: The system includes basic error handling for socket operations and invalid seat requests.

## Getting Started

### Prerequisites

- A C++ compiler (e.g., `g++`)
- Basic knowledge of socket programming and C++

### Compilation

To compile the server and client programs, use the following commands:

```bash
g++ server.cpp -o server
g++ client.cpp -o client
```
### Running the Server
Start the server by running the compiled server executable:

```bash
./server
```

### Running the Client
To run the client in manual mode:
```bash
./client manual
```
To run the client in automatic mode:
```bash
./client automatic
```

To use the configuration file (myinifile.txt) for connection settings:
```bash
./client <mode> myinifile.txt
```
Replace `<mode>` with either `manual` or `automatic`.

### Configuration File (myinifile.txt)
The configuration file should be formatted as follows:

```makefile
[Connection]
IP = <server_ip>
Port = <server_port>
Timeout = <timeout_seconds>
```

# Example Output
### Server
```
Server is running on port 5437
[ --- Airfare selling system---]
0 0 0 0 0 0 0 0 0 0 
0 0 0 0 0 0 0 0 0 0 
0 0 0 0 0 0 0 0 0 0 
0 0 0 0 0 0 0 0 0 0 
0 0 0 0 0 0 0 0 0 0 
0 0 0 0 0 0 0 0 0 0 
0 0 0 0 0 0 0 0 0 0 
0 0 0 0 0 0 0 0 0 0 
0 0 0 0 0 0 0 0 0 0 
0 0 0 0 0 0 0 0 0 0 
[ -----------------------------]
[Server]: Client 12345 connected
[Server] The system has 100 seats available out of 10 rows and 10 cols!
[Server]: Client 12345 reserved seat on row 1 col 1
[ --- Airfare selling system---]
1 0 0 0 0 0 0 0 0 0 
0 0 0 0 0 0 0 0 0 0 
0 0 0 0 0 0 0 0 0 0 
0 0 0 0 0 0 0 0 0 0 
0 0 0 0 0 0 0 0 0 0 
0 0 0 0 0 0 0 0 0 0 
0 0 0 0 0 0 0 0 0 0 
0 0 0 0 0 0 0 0 0 0 
0 0 0 0 0 0 0 0 0 0 
0 0 0 0 0 0 0 0 0 0 
[ -----------------------------]
```
### Client
```
Successfully connected to the server
[Server] The system has 100 seats available out of 10 rows and 10 cols!
Enter the row your seat should be reserved (1 - 10): 1
Enter the column your seat should be reserved (1 - 10): 1

[Client 12345] sending reservation for seat on row 1 col 1 to the server
[Server] Seat on row 1 col 1 reserved for Client 12345

[Client #12345] There are currently 99 seats available
```
### Acknowledgments
  - This project is inspired by socket programming examples and aims to demonstrate a simple client-server interaction.
