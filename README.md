# IRC Server

A lightweight, fully functional IRC (Internet Relay Chat) server implementation in C++98, designed for multi-client communication with channel management and user authentication.

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Architecture](#architecture)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
- [IRC Commands](#irc-commands)
- [Channel Modes](#channel-modes)
- [Project Structure](#project-structure)
- [How It Works](#how-it-works)
- [Testing](#testing)
- [Technical Details](#technical-details)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)
- [License](#license)

---

## 🌟 Overview

This IRC server enables real-time text communication between multiple clients. Users can:
- Authenticate with a server password
- Choose unique nicknames
- Create and join channels
- Send private messages
- Manage channel permissions and modes
- Execute operator commands (KICK, INVITE, TOPIC, MODE)

The server is built following the **C++98 standard** and uses **poll()** for efficient I/O multiplexing, allowing it to handle multiple simultaneous connections without threading.

---

## ✨ Features

### Core Functionality
- ✅ **Multi-client support** - Handle unlimited concurrent connections
- ✅ **Non-blocking I/O** - Efficient event-driven architecture using `poll()`
- ✅ **User authentication** - Password-protected server access
- ✅ **Nickname management** - Unique username system with validation
- ✅ **Channel system** - Create and join multiple chat rooms
- ✅ **Private messaging** - Direct user-to-user and channel messages

### Channel Management
- ✅ **Automatic operators** - First user in channel becomes operator
- ✅ **Topic control** - Set and view channel topics
- ✅ **User management** - KICK and INVITE commands
- ✅ **Multiple modes** - Invite-only, password-protected, user limits, topic restrictions

### Security & Validation
- ✅ **Password authentication** - Server-level access control
- ✅ **Nickname validation** - Enforce proper nickname format and uniqueness
- ✅ **Permission checks** - Operator-only commands
- ✅ **Error handling** - Comprehensive error messages

---

## 🏗️ Architecture

### Server Components

```
┌─────────────────────────────────────────┐
│           IRC Server                     │
├─────────────────────────────────────────┤
│  ┌─────────────┐    ┌────────────────┐ │
│  │   Server    │───▶│   Command      │ │
│  │  (Socket)   │    │   Processor    │ │
│  └─────────────┘    └────────────────┘ │
│         │                    │          │
│         ▼                    ▼          │
│  ┌─────────────┐    ┌────────────────┐ │
│  │   Client    │    │    Channel     │ │
│  │  Manager    │◀───│    Manager     │ │
│  └─────────────┘    └────────────────┘ │
└─────────────────────────────────────────┘
```

### Class Hierarchy

- **Server**: Main server class handling socket operations and client connections
- **Client**: Represents individual users with authentication state and buffers
- **Channel**: Manages chat rooms with members, operators, modes, and invitations
- **Command**: Static class for parsing and executing IRC commands

---

## 📦 Requirements

### System Requirements
- **Operating System**: Linux, macOS, or Windows (WSL)
- **Compiler**: g++ or clang++ with C++98 support
- **Build Tool**: GNU Make

### Dependencies
- Standard C++ library (C++98)
- POSIX socket API
- poll() system call

### Optional Tools (for testing)
- `nc` (netcat) - Command-line client
- `telnet` - Alternative testing client
- IRC clients: `irssi`, `weechat`, or `hexchat`

---

## 🔧 Installation

### Clone the Repository

```bash
git clone https://github.com/hassan-kheireddin/IRC-server.git
cd IRC-server
```

### Compile

```bash
make
```

This will create the `ircserv` executable.

### Clean Build

```bash
# Remove object files
make clean

# Remove object files and executable
make fclean

# Rebuild from scratch
make re
```

---

## 🚀 Usage

### Starting the Server

```bash
./ircserv <port> <password>
```

**Parameters:**
- `<port>`: Port number (1-65535) for the server to listen on
- `<password>`: Server password required for client authentication

**Example:**

```bash
./ircserv 6667 mySecretPass
```

**Output:**

```
mySecretPass
 Server is up and running on port 6667
```

### Connecting as a Client

#### Using netcat (nc)

```bash
nc localhost 6667
```

#### Using telnet

```bash
telnet localhost 6667
```

#### Using IRC Client (irssi)

```bash
irssi
/connect localhost 6667 mySecretPass nickname
```

### Basic Session Example

```
# Connect
nc localhost 6667

# Send commands (press Enter after each)
PASS mySecretPass
NICK alice
USER alice 0 * :Alice Wonderland
AUTHENTICATE
JOIN #lobby mykey
PRIVMSG #lobby :Hello everyone!
```

---

## 📡 IRC Commands

### Authentication Commands

#### PASS
**Syntax**: `PASS <password>`

Authenticate with the server password.

**Example:**
```irc
PASS mySecretPass
```

**Response:**
```
alice, Welcome to the server! If you want to Join Channels you must be authenticated.
```

---

#### NICK
**Syntax**: `NICK <nickname>`

Set your nickname. Must be unique and follow validation rules.

**Rules:**
- 1-9 characters long
- Cannot start with: digit, `#`, `$`, `&`, `+`, `~`, `@`, `%`, `:`
- Cannot contain: `*`, space, `,`, `.`, `@`, `?`, `!`, tab, newline

**Example:**
```irc
NICK alice
```

**Errors:**
```
Error: Nickname alice is already in use.
Error: Erroneous nickname 123abc.
```

---

#### USER
**Syntax**: `USER <username> <mode> <unused> :<realname>`

Set user information.

**Example:**
```irc
USER alice 0 * :Alice Wonderland
```

---

#### AUTHENTICATE
**Syntax**: `AUTHENTICATE`

Complete the authentication process. Must have sent PASS, NICK, and USER first.

**Example:**
```irc
AUTHENTICATE
```

**Response:**
```
alice, You have been successfully authenticated!
```

---

### Channel Commands

#### JOIN
**Syntax**: `JOIN <#channel> <key>`

Join or create a channel. Channel names must start with `#`.

**Example:**
```irc
JOIN #general mykey
```

**Response:**
```
:alice JOIN #general
alice #general :No topic is set
alice=#general :@alice
```

**Errors:**
```
Error: Channel #general is invite-only. (+i)
Error: Bad channel key for #general. (+k)
Error: Channel #general is full. (+l)
```

---

#### TOPIC
**Syntax**: 
- View: `TOPIC <#channel>`
- Set: `TOPIC <#channel> :<new topic>`

View or change the channel topic.

**Examples:**
```irc
# View topic
TOPIC #general

# Set topic (operator only if +t is set)
TOPIC #general :Welcome to the general channel!
```

**Response:**
```
:alice TOPIC #general :Welcome to the general channel!
```

---

#### KICK
**Syntax**: `KICK <#channel> <nickname> :<reason>`

Remove a user from the channel. **Operator only**.

**Example:**
```irc
KICK #general bob :Spamming
```

**Response:**
```
:alice KICK #general bob :Spamming
```

---

#### INVITE
**Syntax**: `INVITE <nickname> <#channel>`

Invite a user to a channel. Required for invite-only channels. **Operator only**.

**Example:**
```irc
INVITE charlie #general
```

**Response:**
```
:alice INVITE charlie :#general
```

---

#### MODE
**Syntax**: `MODE <#channel> <+|-><modes> [parameters]`

Change channel modes. **Operator only**.

**Example:**
```irc
MODE #general +i          # Set invite-only
MODE #general +k secret   # Set key
MODE #general +l 50       # Set user limit
MODE #general +t          # Restrict topic changes
MODE #general +o bob      # Give operator status
MODE #general -i          # Remove invite-only
```

**Response:**
```
:alice MODE #general +i
```

---

### Messaging Commands

#### PRIVMSG
**Syntax**: 
- To user: `PRIVMSG <nickname> :<message>`
- To channel: `PRIVMSG <#channel> :<message>`

Send a private message to a user or channel.

**Examples:**
```irc
# Message to user
PRIVMSG bob :Hey, how are you?

# Message to channel
PRIVMSG #general :Hello everyone!
```

**Response:**
```
:alice PRIVMSG #general :Hello everyone!
```

---

## 🔧 Channel Modes

| Mode | Name | Description | Parameters |
|------|------|-------------|------------|
| `+i` | Invite-only | Only invited users can join | None |
| `-i` | Remove invite-only | Anyone can join | None |
| `+t` | Topic restriction | Only operators can change topic | None |
| `-t` | Remove topic restriction | Anyone can change topic | None |
| `+k` | Key (password) | Channel requires password to join | `<key>` |
| `-k` | Remove key | Remove channel password | None |
| `+l` | User limit | Limit number of users in channel | `<limit>` |
| `-l` | Remove limit | Remove user limit | None |
| `+o` | Operator | Give operator status to user | `<nickname>` |
| `-o` | De-operator | Remove operator status from user | `<nickname>` |

### Mode Examples

```irc
# Make channel invite-only with password and limit
MODE #private +ikl secretpass 10

# Give operator to bob and charlie
MODE #general +o bob
MODE #general +o charlie

# Restrict topic changes to operators only
MODE #general +t

# Remove all restrictions
MODE #general -itk
MODE #general -l
```

---

## 📁 Project Structure

```
IRC-server/
├── main.cpp                 # Entry point and argument validation
├── Makefile                 # Build configuration
├── README.md                # This file
├── TESTING.md               # Comprehensive testing guide
├── message-used.md          # IRC message format reference
│
├── includes/                # Header files
│   ├── Server.hpp           # Server class declaration
│   ├── Client.hpp           # Client class declaration
│   ├── Channel.hpp          # Channel class declaration
│   └── Command.hpp          # Command parser declaration
│
└── srcs/                    # Implementation files
    ├── Server.cpp           # Socket management and client handling
    ├── Client.cpp           # User state and authentication
    ├── Channel.cpp          # Channel management and modes
    └── Command.cpp          # Command parsing and execution
```

### File Descriptions

#### main.cpp
- Validates command-line arguments (port and password)
- Checks password format (no spaces/tabs)
- Instantiates and runs the Server
- Handles top-level exceptions

#### Server.hpp / Server.cpp
**Responsibilities:**
- Create and configure TCP socket
- Bind to specified port with `SO_REUSEADDR`
- Listen for incoming connections
- Accept new clients
- Use `poll()` for I/O multiplexing
- Manage client connections and disconnections
- Route incoming data to Command processor
- Maintain nickname registry
- Manage channel registry

**Key Methods:**
- `setupServerSocket()` - Initialize socket
- `run()` - Main event loop with poll()
- `acceptNewConnection()` - Handle new clients
- `handleClientData()` - Process client messages
- `manageNickname()` - Register/unregister nicknames
- `createOrGetChannel()` - Channel factory method

#### Client.hpp / Client.cpp
**Responsibilities:**
- Store client connection information (socket FD, IP)
- Track authentication state (PASS, NICK, USER sent)
- Maintain user identity (nickname, username, realname)
- Buffer incomplete messages

**Key Attributes:**
- `_socketFd` - File descriptor
- `_nickname`, `_username`, `_realname` - User info
- `_buffer` - Incoming data buffer
- `_isAuthenticated`, `_isRegistered` - State flags

#### Channel.hpp / Channel.cpp
**Responsibilities:**
- Store channel properties (name, topic, key)
- Maintain member list
- Track operators and invited users
- Manage channel modes (i, t, k, l)
- Enforce user limits

**Key Methods:**
- `addClient()` / `removeClient()` - Member management
- `setOperator()` / `isOperator()` - Operator management
- `addInvitation()` / `isInvited()` - Invitation system
- `addMode()` / `removeMode()` / `hasMode()` - Mode management
- `setKey()` / `setClientLimit()` - Mode parameters

#### Command.hpp / Command.cpp
**Responsibilities:**
- Parse IRC command strings
- Validate command syntax
- Execute command logic
- Generate responses
- Enforce permissions

**Key Methods:**
- `executeCommand()` - Main dispatcher
- `PASS()`, `NICK()`, `USER()`, `AUTHENTICATE()` - Auth commands
- `JOIN()`, `TOPIC()`, `KICK()`, `INVITE()` - Channel commands
- `MODE()` - Mode management
- `PRIVMSG()` - Messaging
- `isValidNickname()` - Validation helper

---

## 🔍 How It Works

### 1. Server Initialization

```cpp
Server server(port, password);
```

**What happens:**
1. Creates TCP socket with `socket(AF_INET, SOCK_STREAM, 0)`
2. Sets `SO_REUSEADDR` for immediate port reuse
3. Sets socket to non-blocking mode with `fcntl()`
4. Binds to `0.0.0.0:<port>` (listens on all interfaces)
5. Starts listening with backlog of 10 connections
6. Adds server socket to poll file descriptor list

**Code Flow:**
```
main() → Server constructor → setupServerSocket() → server.run()
```

---

### 2. Event Loop (poll)

```cpp
while (true) {
    int ret = poll(&_pollFds[0], _pollFds.size(), -1);
    // Process events...
}
```

**How poll() works:**
- Monitors multiple file descriptors simultaneously
- Blocks until at least one FD has an event
- Returns number of FDs with events
- `POLLIN` event = data ready to read

**Event Types:**
1. **Server socket event** → New client connection
2. **Client socket event** → Data from existing client

---

### 3. Client Connection Flow

```
Client                          Server
  │                               │
  ├─────── TCP Connect ──────────▶│
  │                               │ accept()
  │                               │ create Client object
  │                               │ add to poll list
  │◀────── Connection OK ─────────┤
  │                               │
  ├─────── PASS mypass ──────────▶│ validate password
  │◀────── Welcome ───────────────┤
  │                               │
  ├─────── NICK alice ───────────▶│ check uniqueness
  │                               │ register nickname
  │                               │
  ├─────── USER alice 0 * :Alice ▶│ store user info
  │                               │
  ├─────── AUTHENTICATE ─────────▶│ check completeness
  │◀────── Authenticated ─────────┤ set authenticated flag
  │                               │
  ├─────── JOIN #test key ───────▶│ create/get channel
  │◀────── :alice JOIN #test ────┤ add to channel
  │◀────── Names list ────────────┤ send member list
  │                               │
```

**Step-by-step:**

1. **TCP Handshake**: Client connects, server accepts
2. **Socket Setup**: Server sets client socket to non-blocking
3. **Client Object**: Server creates `Client` instance
4. **Poll Registration**: Adds client FD to poll list
5. **Authentication**: Client sends PASS, NICK, USER, AUTHENTICATE
6. **Validation**: Server validates each command
7. **Channel Join**: Client can now join channels
8. **Communication**: Client can send/receive messages

---

### 4. Message Processing

```cpp
void Server::handleClientData(int clientFd) {
    // Read data
    recv(clientFd, buffer, sizeof(buffer), 0);
    
    // Append to client buffer
    client->appendToBuffer(string(buffer, bytes));
    
    // Process complete lines (ending with \n)
    while (buffer contains "\n") {
        string commandLine = extract line;
        Command::executeCommand(commandLine, *client, *this);
    }
}
```

**Why buffering?**
- TCP doesn't guarantee complete messages in one `recv()`
- Commands may arrive in fragments: `"PA"` then `"SS mypass\r\n"`
- Buffer accumulates data until complete line is received
- Multiple commands may arrive in one packet: `"NICK alice\r\nUSER alice 0 * :Alice\r\n"`

**Command Parsing:**
```
Input: "JOIN #test mykey"
         ↓
Tokenize by spaces
         ↓
["JOIN", "#test", "mykey"]
         ↓
command = "JOIN"
params = ["#test", "mykey"]
         ↓
Command::JOIN(params, client, server)
```

---

### 5. Channel Operations

#### Creating/Joining a Channel

```cpp
Channel* channel = server.createOrGetChannel("#test");
channel->addClient(&client);

// First user becomes operator
if (channel->getClients().size() == 1)
    channel->setOperator(client.getNickname());
```

#### Broadcasting to Channel

```cpp
string msg = ":alice PRIVMSG #test :Hello!";
for (iterator it = channel->getClients().begin(); 
     it != channel->getClients().end(); ++it) {
    if ((*it)->getNickname() != "alice") { // Don't echo to sender
        send((*it)->getFd(), msg.c_str(), msg.length(), 0);
    }
}
```

#### Mode Enforcement

```cpp
// Check invite-only
if (channel->hasMode('i') && !channel->isInvited(client))
    return error("Channel is invite-only");

// Check key
if (channel->hasKey() && key != channel->getKey())
    return error("Bad channel key");

// Check limit
if (channel->hasClientLimit() && channel->isFull())
    return error("Channel is full");
```

---

### 6. Disconnect Handling

```cpp
if (recv() returns <= 0) {
    // Client disconnected
    
    // 1. Cleanup nickname
    string nick = client->getNickname();
    server.manageNickname(nick, NULL, UNREGISTER);
    
    // 2. Close socket
    close(clientFd);
    
    // 3. Remove from poll list
    _pollFds.erase(find clientFd);
    
    // 4. Delete client object
    delete client;
    _clients.erase(clientFd);
}
```

**Why this order?**
1. Nickname cleanup first (allows reuse)
2. Close socket (release OS resources)
3. Remove from poll (stop monitoring)
4. Delete object (free memory)

---

## 🧪 Testing

For comprehensive testing instructions, see **[TESTING.md](TESTING.md)**.

### Quick Test

```bash
# Terminal 1: Start server
./ircserv 6667 testpass

# Terminal 2: Connect as alice
nc localhost 6667
PASS testpass
NICK alice
USER alice 0 * :Alice
AUTHENTICATE
JOIN #test mykey
PRIVMSG #test :Hello from Alice!

# Terminal 3: Connect as bob
nc localhost 6667
PASS testpass
NICK bob
USER bob 0 * :Bob
AUTHENTICATE
JOIN #test mykey
PRIVMSG #test :Hi Alice, Bob here!
```

### Expected Results

**Terminal 1 (server)**:
```
 Server is up and running on port 6667
New client connected: 4 (IP: 127.0.0.1)
Parsing command from client 4 ((unknown)): PASS testpass
Parsing command from client 4 ((unknown)): NICK alice
Parsing command from client 4 (alice): USER alice 0 * :Alice
Parsing command from client 4 (alice): AUTHENTICATE
alice, You have been successfully authenticated!
Parsing command from client 4 (alice): JOIN #test mykey
No topic is set for channel #test.
Parsing command from client 4 (alice): PRIVMSG #test :Hello from Alice!
New client connected: 5 (IP: 127.0.0.1)
...
```

**Terminal 2 (alice)**:
```
alice, Welcome to the server! If you want to Join Channels you must be authenticated.
alice, You have been successfully authenticated!
:alice JOIN #test
alice #test :No topic is set
alice=#test :@alice
:bob JOIN #test
:bob PRIVMSG #test :Hi Alice, Bob here!
```

**Terminal 3 (bob)**:
```
bob, Welcome to the server! If you want to Join Channels you must be authenticated.
bob, You have been successfully authenticated!
:alice JOIN #test
:bob JOIN #test
bob #test :No topic is set
bob=#test :@alice bob
:alice PRIVMSG #test :Hello from Alice!
```

---

## 🔬 Technical Details

### Socket Programming

**Non-blocking Sockets:**
```cpp
fcntl(socketFd, F_SETFL, O_NONBLOCK);
```
- Prevents `recv()` from blocking if no data available
- Prevents `send()` from blocking if buffer full
- Returns immediately with error code `EWOULDBLOCK`/`EAGAIN`

**SO_REUSEADDR:**
```cpp
setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
```
- Allows immediate port reuse after server restart
- Prevents "Address already in use" errors
- Essential for development/testing

### I/O Multiplexing (poll)

**Why poll() instead of select()?**
- No limit on number of file descriptors (select limited to 1024)
- More efficient with many connections
- Cleaner API with `pollfd` struct

**pollfd Structure:**
```cpp
struct pollfd {
    int   fd;        // File descriptor
    short events;    // Events to monitor (POLLIN)
    short revents;   // Events that occurred
};
```

### Memory Management

**Client Objects:**
- Dynamically allocated: `new Client(fd, ip)`
- Stored in map: `_clients[fd] = client`
- Deleted on disconnect: `delete client`

**Channel Objects:**
- Dynamically allocated on first JOIN
- Persist until server shutdown
- **Note**: Channels are never deleted (potential memory leak for production)

### Data Structures

```cpp
// Server.hpp
std::vector<pollfd> _pollFds;                    // O(n) iteration, O(1) append
std::map<int, Client*> _clients;                 // O(log n) lookup by FD
std::map<std::string, Client*> _registeredNicknames;  // O(log n) nickname lookup
std::map<std::string, Channel*> _channels;       // O(log n) channel lookup

// Channel.hpp
std::set<Client*> _clients;      // O(log n) insert/remove/find
std::set<Client*> _operators;    // O(log n) operator checks
std::set<Client*> _invited;      // O(log n) invitation checks
std::set<char> _modes;           // O(log n) mode checks
```

### C++98 Compatibility

**Why C++98?**
- Required by 42 school curriculum
- Ensures compatibility with older systems
- Teaches fundamental C++ without modern conveniences

**Key Differences:**
```cpp
// Modern C++ (C++11+)
nullptr
auto it = map.begin()
std::thread
std::shared_ptr

// C++98 (used in this project)
NULL
std::map<int, Client*>::iterator it = map.begin()
// No threading (use poll() instead)
// Manual memory management
```

---

## 🐛 Troubleshooting

### Compilation Errors

**Problem**: `error: 'nullptr' was not declared in this scope`

**Solution**: Use `NULL` instead of `nullptr` (C++98 compatibility)

```cpp
// Wrong
if (client == nullptr)

// Correct
if (client == NULL)
```

---

**Problem**: `no matching function for call to 'std::string::string(const char [6])'`

**Solution**: Explicit string construction

```cpp
// Wrong
string msg = "Hello";

// Correct
std::string msg = std::string("Hello");
```

---

### Runtime Errors

**Problem**: `Bind failed - port may already be in use`

**Solution**: 

```bash
# Find process using port
lsof -i :6667

# Kill the process
kill -9 <PID>

# Or use a different port
./ircserv 6668 mypass
```

---

**Problem**: `Client disconnected` immediately after connection

**Solution**: Ensure proper authentication sequence

```
1. PASS <password>
2. NICK <nickname>
3. USER <username> 0 * :<realname>
4. AUTHENTICATE
```

---

**Problem**: `Error: Nickname is already in use`

**Solution**: 
- Choose a different nickname
- Check if previous connection is still active
- Restart server to clear nickname registry

---

**Problem**: `Error: Channel is invite-only`

**Solution**:
- Ask channel operator to invite you: `/invite <yournick> #channel`
- Or operator can remove invite-only: `MODE #channel -i`

---

### Performance Issues

**Problem**: Server becomes unresponsive with many clients

**Solution**:
- Check for infinite loops in command processing
- Verify poll timeout is set correctly (`-1` for infinite wait)
- Monitor CPU usage: `top -p $(pgrep ircserv)`

---

**Problem**: Messages delayed or lost

**Solution**:
- Ensure `send()` return value is checked
- Verify client buffer is cleared after processing
- Check network connectivity: `ping localhost`

---

## 🛠️ Development Tips

### Debugging

**Enable verbose logging:**
```cpp
// In Server.cpp, add more std::cout statements
std::cout << "DEBUG: Received " << bytes << " bytes from client " << clientFd << std::endl;
std::cout << "DEBUG: Buffer content: [" << buffer << "]" << std::endl;
```

**Use gdb:**
```bash
# Compile with debug symbols
make CXXFLAGS="-g"

# Run in debugger
gdb ./ircserv
(gdb) run 6667 testpass
(gdb) break Server.cpp:100
(gdb) continue
(gdb) print clientFd
(gdb) backtrace
```

**Monitor file descriptors:**
```bash
# List open FDs for running server
lsof -p $(pgrep ircserv)
```

### Testing Tools

**Netcat flags:**
```bash
# Verbose output
nc -v localhost 6667

# Keep connection open
nc -k localhost 6667

# UDP instead of TCP (don't use for IRC)
nc -u localhost 6667
```

**Send file as commands:**
```bash
cat commands.txt | nc localhost 6667
```

**Script multiple clients:**
```bash
# test_script.sh
for i in {1..10}; do
  (
    echo "PASS testpass"
    echo "NICK user$i"
    echo "USER user$i 0 * :User $i"
    echo "AUTHENTICATE"
    sleep 5
  ) | nc localhost 6667 &
done
```

---

## 📚 Resources

### IRC Protocol
- [RFC 1459](https://tools.ietf.org/html/rfc1459) - Original IRC protocol
- [RFC 2812](https://tools.ietf.org/html/rfc2812) - Updated IRC protocol
- [Modern IRC](https://modern.ircdocs.horse/) - Contemporary documentation

### Socket Programming
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- [Linux man pages](https://man7.org/linux/man-pages/)
  - `man 2 socket`
  - `man 2 bind`
  - `man 2 listen`
  - `man 2 accept`
  - `man 2 poll`

### C++ Resources
- [C++98 Standard Reference](https://en.cppreference.com/w/cpp/98)
- [C++ STL Documentation](https://cplusplus.com/reference/stl/)

---

## 🤝 Contributing

Contributions are welcome! Please follow these guidelines:

1. **Fork the repository**
2. **Create a feature branch**: `git checkout -b feature/amazing-feature`
3. **Follow C++98 standard** (no modern C++ features)
4. **Add tests** for new functionality
5. **Update documentation** (README, TESTING.md)
6. **Commit your changes**: `git commit -m 'Add amazing feature'`
7. **Push to branch**: `git push origin feature/amazing-feature`
8. **Open a Pull Request**

### Code Style
- Use tabs for indentation
- Follow existing naming conventions
- Add comments for complex logic
- Use `std::` prefix (no `using namespace std`)

---

## 📄 License

This project is part of the 42 school curriculum and is for educational purposes.

---

## 👥 Authors

- **Hassan Kheireddin** - [@hassan-kheireddin](https://github.com/hassan-kheireddin)

---

## 🙏 Acknowledgments

- 42 Network for the project specification
- IRC protocol designers (RFC 1459, RFC 2812)
- The IRC community for decades of real-time communication

---

## 📞 Support

For questions or issues:
- Open an issue on GitHub
- Check [TESTING.md](TESTING.md) for troubleshooting
- Review IRC protocol documentation

---

**Last Updated**: December 3, 2025

**Version**: 1.0.0

**Status**: ✅ Complete and functional
