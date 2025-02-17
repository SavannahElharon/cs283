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
@test "Test command not found" {
    run ./dsh <<EOF
not_exists
rc
EOF
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "Command not found in PATH" ]
    [ "${lines[1]}" = "2" ]
}
@test "Test permission denied" {
    touch no_permission
    chmod -x no_permission

    run ./dsh <<EOF
./no_permission
rc
EOF
    rm -f no_permission
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "Permission denied" ]
    [ "${lines[1]}" = "13" ]
}
@test "Test executable format error" {
    echo "not a binary" > not_a_binary
    chmod +x not_a_binary

    run ./dsh <<EOF
./not_a_binary
rc
EOF
    rm -f not_a_binary
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "Executable format error" ]
    [ "${lines[1]}" = "8" ]
}

@test "Test rc command after successful command" {
    run ./dsh <<EOF
true
rc
EOF
    [ "$status" -eq 0 ]
    [ "${lines[0]}" = "0" ]
}

