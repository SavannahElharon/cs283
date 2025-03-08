#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}
startServer() {
    local port=$1
    ./dsh -s -p $port &
    server_pid=$!
    sleep 1 
}
stopServer() {
    local port=$1
    echo "stop-server" | ./dsh -c -p $port
    sleep 1  # Wait for the server to stop
    if ps -p $server_pid > /dev/null; then
        kill -9 $server_pid  # Force-kill if still running
    fi
}

@test "Basic command " {
    run ./dsh <<EOF
/bin/ls
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Command with arguments" {
    run ./dsh <<EOF
/bin/echo hello world
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"hello world"* ]]
}

@test "pipe with two commands" {
    run ./dsh <<EOF
/bin/echo hello | /usr/bin/wc -w
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"1"* ]]
}

@test "execute piped commands via client" {
    startServer 12347
    result=$(echo "/bin/echo hello | /usr/bin/wc -w" | ./dsh -c -p 12347)
    stopServer 12347
    [[ "$result" == *"1"* ]]
}

@test "Threaded EC: Start and handle multiple clients" {
    ./dsh -s -p 12350 -x &
    server_pid=$!
    sleep 1
    ./dsh -c -p 12350 <<< "/bin/echo client1" &
    pid1=$!
    ./dsh -c -p 12350 <<< "/bin/echo client2" &
    pid2=$!
    ./dsh -c -p 12350 <<< "/bin/echo client3" &
    pid3=$!
    wait $pid1 $pid2 $pid3
    stopServer 12350
    [ $? -eq 0 ]
}

@test "receive EOF character" {
    startServer 12354
    result=$(echo "/bin/ls -la /" | ./dsh -c -p 12354)
    stopServer 12354
    [ ${#result} -gt 10 ]
}

@test "Help option" {
    run ./dsh -h
    [ "$status" -eq 0 ]
    [[ "$output" == *"Usage:"* ]]
}

@test :Invalid option combination" {
    run ./dsh -c -s
    [ "$status" -ne 0 ]
}

@test "IP option without mode" {
    run ./dsh -i 127.0.0.1
    [ "$status" -ne 0 ]
}
@test "Execute command with no output" {
    startServer 12366
    result=$(echo "/bin/true" | ./dsh -c -p 12366)
    stopServer 12366
    [[ "$result" == *"Command executed with exit code: 0"* ]]
}
@test "Port option without mode" {
    run ./dsh -p 12345
    [ "$status" -ne 0 ]
}

@test "Threaded option without server mode" {
    run ./dsh -c -x
    [ "$status" -ne 0 ]
}

@test "Exit from client" {
    startServer 12355
    run ./dsh -c -p 12355 <<EOF
exit
EOF
    stopServer 12355
    [ "$status" -eq 0 ]
}

@test "Server properly cleans up sockets on exit" {
    startServer 12356
    stopServer 12356
    ./dsh -s -p 12356 &
    new_server_pid=$!
    sleep 1
    ps -p $new_server_pid
    result=$?
    stopServer 12356
    [ "$result" -eq 0 ]
}

@test "Background process" {
    run ./dsh <<EOF
/bin/sleep 2 &
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Redirection input" {
    echo "Hello World" > testfile.txt
    run ./dsh <<EOF
/bin/cat < testfile.txt
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"Hello World"* ]]
    rm testfile.txt
}

@test "Redirection output" {
    run ./dsh <<EOF
/bin/echo "Hello Output" > output.txt
exit
EOF
    [ "$status" -eq 0 ]
    grep -q "Hello Output" output.txt
    [ $? -eq 0 ]
    rm output.txt
}

