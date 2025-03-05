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

@test "Built-in command: cd" {
    run ./dsh <<EOF
cd /tmp
pwd
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Built-in command: exit" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Built-in command: rc (return code)" {
    run ./dsh <<EOF
ls
rc
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Built-in command: dragon" {
    run ./dsh <<EOF
dragon
exit
EOF
    [ "$status" -eq 0 ]
}

@test "External command execution" {
    run ./dsh <<EOF
ls
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Command with arguments" {
    run ./dsh <<EOF
echo hello world
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Simple pipe with two commands" {
    run ./dsh <<EOF
echo hello | wc -w
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Multiple pipe with three commands" {
    run ./dsh <<EOF
echo "hello world" | tr ' ' '\n' | wc -l
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Pipe with arguments" {
    run ./dsh <<EOF
ls -l | grep '^d' | wc -l
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Maximum number of pipes (8)" {
    run ./dsh <<EOF
echo start | tr 'a-z' 'A-Z' | tr ' ' '_' | tr '_' '-' | tr '-' '+' | tr '+' '=' | tr '=' '*' | tr '*' '#'
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Attempt to exceed maximum pipes" {
    run ./dsh <<EOF
cmd1 | cmd2 | cmd3 | cmd4 | cmd5 | cmd6 | cmd7 | cmd8 | cmd9
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"error: piping limited to 8 commands"* ]]
}
@test "Quoted argument handling" {
    run ./dsh <<EOF
echo "hello world"
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"hello world"* ]]
}

@test "Error handling for non-existent command" {
    run ./dsh <<EOF
non_existent_command
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"Command not found in PATH"* ]]
}
@test "Redirection: output to file" {
    run ./dsh <<EOF
echo "hello, class" > out.txt
cat out.txt
exit
EOF
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "hello, class" ]
}

@test "Redirection: input from file" {
    echo "hello, class" > in.txt
    run ./dsh <<EOF
cat < in.txt
exit
EOF
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "hello, class" ]
    rm in.txt
}

@test "Redirection: combined input and output" {
    echo "hello, class" > in.txt
    run ./dsh <<EOF
cat < in.txt > out.txt
cat out.txt
exit
EOF
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "hello, class" ]
    rm in.txt out.txt
}
