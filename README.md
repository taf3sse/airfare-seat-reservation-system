
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
