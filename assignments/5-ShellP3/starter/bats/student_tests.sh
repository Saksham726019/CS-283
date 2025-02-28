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

@test "Check pwd gives correct current working directory" {
    run "./dsh" <<EOF                
pwd
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="$(pwd)dsh3>dsh3>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "cd to tmp" {
    run "./dsh" <<EOF                
cd /tmp
pwd
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="/tmpdsh3>dsh3>dsh3>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Expected output should contain "/"
    [ "$stripped_output" = "$expected_output" ]

    [ "$status" -eq 0 ]
}

@test "cd to non-existent path" {
    run "./dsh" <<EOF
cd /nonexistentpath
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="dsh3>error:commandfaileddsh3>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Exit command" {
    run "./dsh" <<EOF
exit
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="dsh3>exiting...cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Echo Hello world" {
    run "./dsh" <<EOF
echo "Hello,     World"
EOF

    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')

    expected_output="Hello,     Worlddsh3> dsh3> cmd loop returned 0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Echo quoted and unquoted arguments" {
    run ./dsh <<EOF
echo Test "Test Multi   word unquoted"   unquoted
EOF

    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')

    expected_output="Test Test Multi   word unquoted unquoteddsh3> dsh3> cmd loop returned 0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Invalid command in the middle of a pipeline" {
    run "./dsh" <<EOF
ls | invalidcmd | wc -l
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="^execvp:Nosuchfileordirectory[0-9]+dsh3>dsh3>cmdloopreturned0$"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [[ "$stripped_output" =~ $expected_regex ]]

    [ "$status" -eq 0 ]
}

@test "Piping with multiple spaces" {
    run "./dsh" <<EOF
ls     |    wc -l
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="^[0-9]+dsh3>dsh3>cmdloopreturned0$"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"

    [[ "$stripped_output" =~ $expected_output ]]

    [ "$status" -eq 0 ]
}

@test "tr to lowercase" {
    run "./dsh" <<EOF
echo "ABCdef" | tr 'A-Z' 'a-z'
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="abcdefdsh3>dsh3>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "echo piped to cat" {
    run "./dsh" <<EOF
echo "Hello, Pipeline" | cat
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="Hello,Pipelinedsh3>dsh3>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Piping with no spaces between piped commands" {
    run "./dsh" <<EOF
ls|wc -l
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="^[0-9]+dsh3>dsh3>cmdloopreturned0$"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"

    [[ "$stripped_output" =~ $expected_output ]]

    [ "$status" -eq 0 ]
}

@test "Piping with leading and trailing spaces" {
    run "./dsh" <<EOF
    ls   |   wc -l    
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="^[0-9]+dsh3>dsh3>cmdloopreturned0$"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"

    [[ "$stripped_output" =~ $expected_output ]]

    [ "$status" -eq 0 ]
}