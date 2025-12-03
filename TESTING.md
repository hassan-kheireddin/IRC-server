# IRC Server Testing Guide

## Table of Contents
1. [Prerequisites](#prerequisites)
2. [Basic Server Tests](#basic-server-tests)
3. [Authentication Tests](#authentication-tests)
4. [Channel Tests](#channel-tests)
5. [Channel Modes Tests](#channel-modes-tests)
6. [Private Messages Tests](#private-messages-tests)
7. [Multi-Client Tests](#multi-client-tests)
8. [Error Handling Tests](#error-handling-tests)

---

## Prerequisites

### Required Tools
- `nc` (netcat) - for manual testing
- `telnet` - alternative to netcat
- IRC client (optional): `irssi`, `weechat`, or `hexchat`

### Starting the Server
```bash
# Compile the server
make

# Start the server (port: 6667, password: mypass)
./ircserv 6667 mypass

# Or run in background
./ircserv 6667 mypass &
```

### Connecting to the Server
```bash
# Using netcat
nc localhost 6667

# Using telnet
telnet localhost 6667
```

---

## Basic Server Tests

### Test 1: Server Startup
**Purpose**: Verify server starts correctly

```bash
./ircserv 6667 testpass
```

**Expected Output**:
```
testpass
 Server is up and running on port 6667
```

**✅ Pass**: Server starts without errors
**❌ Fail**: Server crashes or shows error messages

---

### Test 2: Invalid Arguments
**Purpose**: Test error handling for wrong arguments

```bash
# No arguments
./ircserv

# Missing password
./ircserv 6667

# Invalid port
./ircserv 99999 pass
./ircserv -1 pass
./ircserv abc pass

# Password with spaces
./ircserv 6667 "pass word"
```

**Expected Output**: Error messages for each case

---

### Test 3: Port Already in Use
**Purpose**: Test socket reuse

```bash
# Start first server
./ircserv 6667 pass1 &

# Try to start second server on same port
./ircserv 6667 pass2
```

**Expected**: Should handle gracefully with SO_REUSEADDR

---

## Authentication Tests

### Test 4: Basic Authentication Flow
**Purpose**: Complete authentication sequence

```bash
nc localhost 6667
```

**Commands**:
```irc
PASS mypass
NICK alice
USER alice 0 * :Alice User
AUTHENTICATE
```

**Expected Output**:
```
alice, Welcome to the server! If you want to Join Channels you must be authenticated.
alice, You have been successfully authenticated!
```

**✅ Pass**: All commands accepted, user authenticated
**❌ Fail**: Any errors or rejection

---

### Test 5: Wrong Password
**Purpose**: Test password validation

```bash
nc localhost 6667
```

**Commands**:
```irc
PASS wrongpass
NICK bob
USER bob 0 * :Bob User
```

**Expected Output**:
```
Error: Password mismatch for client.
```

---

### Test 6: Duplicate Nickname
**Purpose**: Test nickname uniqueness

**Terminal 1**:
```bash
nc localhost 6667
PASS mypass
NICK alice
USER alice 0 * :Alice User
```

**Terminal 2**:
```bash
nc localhost 6667
PASS mypass
NICK alice
USER alice2 0 * :Alice Two
```

**Expected Output (Terminal 2)**:
```
Error: Nickname alice is already in use.
```

---

### Test 7: Invalid Nicknames
**Purpose**: Test nickname validation

```bash
nc localhost 6667
PASS mypass
```

**Commands to test**:
```irc
NICK 123abc          # Starts with digit
NICK #channel        # Starts with #
NICK user@host       # Contains @
NICK user name       # Contains space
NICK verylongname123 # Too long (>9 chars)
NICK valid           # Should work
```

**Expected**: First 5 should fail, last should succeed

---

### Test 8: Authentication Without PASS
**Purpose**: Test authentication requirements

```bash
nc localhost 6667
NICK alice
USER alice 0 * :Alice User
AUTHENTICATE
```

**Expected Output**:
```
Error: client alice is not authenticated. You Should provide Password for the Server, Nickname and Username.
```

---

## Channel Tests

### Test 9: Create and Join Channel
**Purpose**: Basic channel operations

```bash
nc localhost 6667
PASS mypass
NICK alice
USER alice 0 * :Alice User
AUTHENTICATE
JOIN #test mykey
```

**Expected Output**:
```
:alice JOIN #test
alice #test :No topic is set
alice=#test :@alice
```

**Note**: `@alice` means alice is channel operator

---

### Test 10: Join Existing Channel
**Purpose**: Multiple users in same channel

**Terminal 1** (alice):
```bash
nc localhost 6667
PASS mypass
NICK alice
USER alice 0 * :Alice User
AUTHENTICATE
JOIN #room key123
```

**Terminal 2** (bob):
```bash
nc localhost 6667
PASS mypass
NICK bob
USER bob 0 * :Bob User
AUTHENTICATE
JOIN #room key123
```

**Expected (Terminal 1 sees)**:
```
:bob JOIN #room
```

**Expected (Terminal 2 sees)**:
```
:alice JOIN #room
:bob JOIN #room
bob #room :No topic is set
bob=#room :@alice bob
```

---

### Test 11: Topic Management
**Purpose**: Set and view channel topics

```bash
# Set topic (must be operator)
TOPIC #test :This is our test channel

# View topic
TOPIC #test
```

**Expected Output**:
```
:alice TOPIC #test :This is our test channel
```

---

### Test 12: KICK Command
**Purpose**: Remove user from channel

**Setup**: alice (operator) and bob in #test

**Alice executes**:
```irc
KICK #test bob :You're out!
```

**Expected Output**:
```
:alice KICK #test bob :You're out!
```

---

### Test 13: INVITE Command
**Purpose**: Invite user to channel

**Alice executes**:
```irc
INVITE charlie #test
```

**Charlie should receive**:
```
:alice INVITE charlie :#test
```

---

## Channel Modes Tests

### Test 14: Invite-Only Mode (+i)
**Purpose**: Test invite-only channels

**Alice (operator)**:
```irc
MODE #test +i
```

**Bob (not invited) tries to join**:
```irc
JOIN #test key
```

**Expected**:
```
Error: Channel #test is invite-only. (+i)
```

**Alice invites Bob**:
```irc
INVITE bob #test
```

**Bob can now join**:
```irc
JOIN #test key
```

---

### Test 15: Key Mode (+k)
**Purpose**: Test channel password

**Alice sets key**:
```irc
MODE #test +k secretkey
```

**Bob tries wrong key**:
```irc
JOIN #test wrongkey
```

**Expected**:
```
Error: Bad channel key for #test. (+k)
```

**Bob uses correct key**:
```irc
JOIN #test secretkey
```

**Expected**: Join succeeds

---

### Test 16: User Limit Mode (+l)
**Purpose**: Test channel capacity limits

**Alice sets limit to 2**:
```irc
MODE #test +l 2
```

**Third user tries to join**:
```
Error: Channel #test is full. (+l)
```

**Remove limit**:
```irc
MODE #test -l
```

---

### Test 17: Topic Restriction Mode (+t)
**Purpose**: Test topic change restrictions

**Alice sets +t mode**:
```irc
MODE #test +t
```

**Bob (not operator) tries to change topic**:
```irc
TOPIC #test :New topic
```

**Expected**:
```
:ircserv 482 #test :You're not a channel operator
```

---

### Test 18: Operator Mode (+o/-o)
**Purpose**: Test operator management

**Alice gives operator to Bob**:
```irc
MODE #test +o bob
```

**Expected**:
```
:alice MODE #test +o bob
```

**Remove operator**:
```irc
MODE #test -o bob
```

---

## Private Messages Tests

### Test 19: User to User PRIVMSG
**Purpose**: Direct messages between users

**Bob sends to Alice**:
```irc
PRIVMSG alice :Hey Alice!
```

**Alice receives**:
```
:bob PRIVMSG alice :Hey Alice!
```

---

### Test 20: Channel PRIVMSG
**Purpose**: Messages to channel

**Setup**: alice, bob, charlie in #test

**Alice sends**:
```irc
PRIVMSG #test :Hello everyone!
```

**Bob and Charlie receive**:
```
:alice PRIVMSG #test :Hello everyone!
```

**Note**: Alice doesn't receive her own message

---

## Multi-Client Tests

### Test 21: Three Users Same Channel
**Purpose**: Test concurrent users

**Terminal 1** (alice):
```bash
nc localhost 6667
PASS mypass
NICK alice
USER alice 0 * :Alice
AUTHENTICATE
JOIN #party key123
```

**Terminal 2** (bob):
```bash
nc localhost 6667
PASS mypass
NICK bob
USER bob 0 * :Bob
AUTHENTICATE
JOIN #party key123
```

**Terminal 3** (charlie):
```bash
nc localhost 6667
PASS mypass
NICK charlie
USER charlie 0 * :Charlie
AUTHENTICATE
JOIN #party key123
```

**Verify**:
- Each user sees others join
- PRIVMSG reaches all users
- Operator commands work

---

### Test 22: Multiple Channels
**Purpose**: Users in different channels

**Alice**:
```irc
JOIN #chan1 key1
JOIN #chan2 key2
```

**Bob**:
```irc
JOIN #chan1 key1
```

**Charlie**:
```irc
JOIN #chan2 key2
```

**Test**:
```irc
# Alice sends to #chan1
PRIVMSG #chan1 :Message to chan1

# Only Bob should receive, not Charlie
```

---

## Error Handling Tests

### Test 23: Missing Parameters
**Purpose**: Test error messages

```irc
PASS              # Missing password
NICK              # Missing nickname
USER alice        # Not enough params
JOIN              # Missing channel
JOIN #test        # Missing key
KICK #test        # Missing user
INVITE            # Missing params
TOPIC             # Missing channel
MODE              # Missing params
PRIVMSG           # Missing target
```

**Expected**: Error messages for each

---

### Test 24: Non-existent Targets
**Purpose**: Test validation

```irc
# Non-existent channel
PRIVMSG #nonexistent :Hello

# Non-existent user
PRIVMSG nobody :Hello

# Not in channel
PRIVMSG #notjoined :Hello
```

**Expected**: Error messages

---

### Test 25: Unauthorized Operations
**Purpose**: Test permission checks

**Bob (not operator) tries**:
```irc
KICK #test charlie :Bye
INVITE dave #test
MODE #test +i
```

**Expected**:
```
Error: You need channel operator privileges for channel #test.
```

---

### Test 26: Client Disconnect Handling
**Purpose**: Test cleanup on disconnect

**Terminal 1** (alice):
```bash
nc localhost 6667
PASS mypass
NICK alice
USER alice 0 * :Alice
AUTHENTICATE
JOIN #test key
# Press Ctrl+C to disconnect
```

**Server should output**:
```
Client disconnected: <fd>
```

**Terminal 2** (bob can now use the nickname):
```bash
nc localhost 6667
PASS mypass
NICK alice    # Should work now
```

---

## Quick Test Script

### Automated Test Runner

Create `test.sh`:
```bash
#!/bin/bash

PORT=6667
PASS="testpass"

echo "Starting server..."
./ircserv $PORT $PASS &
SERVER_PID=$!
sleep 1

echo "Test 1: Basic connection and authentication"
(
  sleep 1
  echo "PASS $PASS"
  echo "NICK testuser"
  echo "USER testuser 0 * :Test User"
  echo "AUTHENTICATE"
  sleep 1
) | nc localhost $PORT &

sleep 2

echo "Test 2: Join channel"
(
  sleep 1
  echo "PASS $PASS"
  echo "NICK alice"
  echo "USER alice 0 * :Alice"
  echo "AUTHENTICATE"
  echo "JOIN #test mykey"
  sleep 2
) | nc localhost $PORT &

sleep 3

echo "Stopping server..."
kill $SERVER_PID

echo "Tests complete!"
```

Run with:
```bash
chmod +x test.sh
./test.sh
```

---

## IRC Client Testing

### Using irssi

```bash
irssi

# In irssi:
/connect localhost 6667 mypass alice
/join #test mykey
/msg bob Hello!
/topic #test New Topic
/mode #test +i
/invite charlie #test
/kick bob reason
```

### Using weechat

```bash
weechat

# In weechat:
/server add local localhost/6667 -password=mypass
/connect local
/nick alice
/join #test mykey
```

---

## Expected Server Logs

### Successful Authentication:
```
New client connected: 4 (IP: 127.0.0.1)
Parsing command from client 4 ((unknown)): PASS mypass
Parsing command from client 4 ((unknown)): NICK alice
Parsing command from client 4 (alice): USER alice 0 * :Alice User
Parsing command from client 4 (alice): AUTHENTICATE
alice, You have been successfully authenticated!
```

### Channel Join:
```
Parsing command from client 4 (alice): JOIN #test key123
No topic is set for channel #test.
```

### Private Message:
```
Parsing command from client 4 (alice): PRIVMSG #test :Hello everyone!
```

---

## Troubleshooting

### Port Already in Use
```bash
# Find process using port
lsof -i :6667

# Kill process
kill -9 <PID>
```

### Server Won't Start
```bash
# Check compilation
make clean
make

# Check permissions
ls -l ircserv

# Run with errors visible
./ircserv 6667 pass
```

### Client Can't Connect
```bash
# Check server is running
ps aux | grep ircserv

# Check port is listening
netstat -tuln | grep 6667

# Test with telnet
telnet localhost 6667
```

---

## Test Checklist

- [ ] Server starts with valid arguments
- [ ] Server rejects invalid arguments
- [ ] Client can connect and authenticate
- [ ] Wrong password is rejected
- [ ] Duplicate nicknames are rejected
- [ ] Invalid nicknames are rejected
- [ ] User can create and join channel
- [ ] Multiple users can join same channel
- [ ] Channel operator is assigned correctly
- [ ] Topic can be set and viewed
- [ ] KICK removes user from channel
- [ ] INVITE allows user to join
- [ ] Mode +i (invite-only) works
- [ ] Mode +k (key) works
- [ ] Mode +l (limit) works
- [ ] Mode +t (topic restriction) works
- [ ] Mode +o/-o (operator) works
- [ ] PRIVMSG to user works
- [ ] PRIVMSG to channel works
- [ ] Multiple concurrent clients work
- [ ] Disconnect cleanup works properly
- [ ] Error messages are appropriate

---

## Performance Testing

### Stress Test: Many Connections
```bash
# Create 10 simultaneous connections
for i in {1..10}; do
  (
    echo "PASS mypass"
    echo "NICK user$i"
    echo "USER user$i 0 * :User $i"
    echo "AUTHENTICATE"
    sleep 5
  ) | nc localhost 6667 &
done

wait
```

### Load Test: Many Messages
```bash
# Send 100 messages rapidly
for i in {1..100}; do
  (
    echo "PASS mypass"
    echo "NICK sender$i"
    echo "USER sender$i 0 * :Sender $i"
    echo "AUTHENTICATE"
    echo "JOIN #spam key"
    echo "PRIVMSG #spam :Message $i"
  ) | nc localhost 6667
done
```

---

## Notes

- Server password is set at startup: `./ircserv <port> <password>`
- First user in a channel becomes operator automatically
- Nicknames are limited to 9 characters
- Channel names must start with `#`
- Commands are case-sensitive (use UPPERCASE)
- Each IRC command should end with `\r\n` (netcat handles this)

---

**Last Updated**: December 3, 2025
