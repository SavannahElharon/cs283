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
@test "Local: Built-in command: cd" {
    run ./dsh <<EOF
cd /tmp
pwd
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == */tmp* ]]
}

@test "Local: Built-in command: exit" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Local: Built-in command: rc" {
    run ./dsh <<EOF
ls
rc
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"0"* ]]
}

@test "Local: Built-in command: dragon" {
    run ./dsh <<EOF
dragon
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"dragon"* ]]
}

@test "Local: Command with arguments" {
    run ./dsh <<EOF
echo hello world
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"hello world"* ]]
}

@test "Local: Simple pipe with two commands" {
    run ./dsh <<EOF
echo hello | wc -w
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"1"* ]]
}

@test "Local: Multiple pipe with three commands" {
    run ./dsh <<EOF
echo "hello world" | tr ' ' '\n' | wc -l
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"2"* ]]
}

@test "Local: Maximum number of pipes (8)" {
    run ./dsh <<EOF
echo start | tr 'a-z' 'A-Z' | tr ' ' '_' | tr '_' '-' | tr '-' '+' | tr '+' '=' | tr '=' '*' | tr '*' '#'
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"START"* ]]
}

@test "Local: Attempt to exceed maximum pipes" {
    run ./dsh <<EOF
cmd1 | cmd2 | cmd3 | cmd4 | cmd5 | cmd6 | cmd7 | cmd8 | cmd9
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"error: piping limited to 8 commands"* ]]
}

@test "Local: Error handling for non-existent command" {
    run ./dsh <<EOF
non_existent_command
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"Command not found in PATH"* ]]
}

@test "Local: Redirection: output to file" {
    rm -f test_out.txt

    run ./dsh <<EOF
echo "hello, class" > test_out.txt
cat test_out.txt
rm test_out.txt
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"hello, class"* ]]
}

@test "Local: Redirection: input from file" {
    echo "hello, input test" > test_in.txt

    run ./dsh <<EOF
cat < test_in.txt
rm test_in.txt
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"hello, input test"* ]]
}

@test "Local: Redirection: combined input and output" {
    echo "hello, combined test" > test_in.txt

    run ./dsh <<EOF
cat < test_in.txt > test_out.txt
cat test_out.txt
rm test_in.txt test_out.txt
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"hello, combined test"* ]]
}

@test "Remote: Server startup and shutdown" {
    ./dsh -s -p 12340 &
    server_pid=$!

    sleep 1

    run ./dsh -c -p 12340 <<EOF
stop-server
EOF

    wait $server_pid
    [ "$?" -eq 0 ]
    [ "$status" -eq 0 ]
}

@test "Remote: Client connects and exits" {
    ./dsh -s -p 12341 &
    server_pid=$!

    sleep 1

    run ./dsh -c -p 12341 <<EOF
exit
EOF

    kill $server_pid
    [ "$status" -eq 0 ]
}

@test "Remote: Basic command execution" {
    ./dsh -s -p 12342 &
    server_pid=$!

    sleep 1

    run ./dsh -c -p 12342 <<EOF
echo "remote command test"
exit
EOF

    kill $server_pid
    [ "$status" -eq 0 ]
    [[ "$output" == *"remote command test"* ]]
}

@test "Remote: Multiple client connections" {
    ./dsh -s -p 12343 &
    server_pid=$!

    sleep 1

    run ./dsh -c -p 12343 <<EOF
echo "first client"
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"first client"* ]]

    run ./dsh -c -p 12343 <<EOF
echo "second client"
exit
EOF

    kill $server_pid
    [ "$status" -eq 0 ]
    [[ "$output" == *"second client"* ]]
}

@test "Remote: Directory change persistence" {
    ./dsh -s -p 12344 &
    server_pid=$!

    sleep 1

    run ./dsh -c -p 12344 <<EOF
cd /tmp
pwd
exit
EOF

    kill $server_pid
    [ "$status" -eq 0 ]
    [[ "$output" == */tmp* ]]
}

@test "Remote: Pipes and redirection" {
    ./dsh -s -p 12345 &
    server_pid=$!

    sleep 1

    run ./dsh -c -p 12345 <<EOF
echo "pipe test" > remote_test.txt
cat remote_test.txt | tr 'a-z' 'A-Z'
rm remote_test.txt
exit
EOF

    kill $server_pid
    [ "$status" -eq 0 ]
    [[ "$output" == *"PIPE TEST"* ]]
}

@test "Remote: Large output handling" {
    ./dsh -s -p 12346 &
    server_pid=$!

    sleep 1

    run ./dsh -c -p 12346 <<EOF
head -c 10000 /dev/urandom | base64
exit
EOF

    kill $server_pid
    [ "$status" -eq 0 ]
    [ "${#output}" -gt 1000 ]
}

@test "Remote: Command with special characters" {
    ./dsh -s -p 12347 &
    server_pid=$!

    sleep 1

    run ./dsh -c -p 12347 <<EOF
echo "special chars: !@#$%^&*()"
exit
EOF

    kill $server_pid
    [ "$status" -eq 0 ]
    [[ "$output" == *"special chars: !@#$%^&*()"* ]]
}

@test "Remote: Error handling" {
    ./dsh -s -p 12348 &
    server_pid=$!

    sleep 1

    run ./dsh -c -p 12348 <<EOF
nonexistentcommand
exit
EOF

    kill $server_pid
    [ "$status" -eq 0 ]
    [[ "$output" == *"Command not found"* ]]
}

@test "Remote: Command line interface override" {
    ./dsh -s -i 0.0.0.0 -p 12349 &
    server_pid=$!

    sleep 1

    run ./dsh -c -i 127.0.0.1 -p 12349 <<EOF
echo "CLI override test"
exit
EOF

    kill $server_pid
    [ "$status" -eq 0 ]
    [[ "$output" == *"CLI override test"* ]]
}

teardown() {
    pkill -f "./dsh -s" || true
    rm -f test_in.txt test_out.txt remote_test.txt
}

