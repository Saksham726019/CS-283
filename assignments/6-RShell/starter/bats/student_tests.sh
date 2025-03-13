#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Local mode: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF
    # Assertions
    [ "$status" -eq 0 ]
}

@test "Local mode: Check pwd gives correct current working directory" {
    run "./dsh" <<EOF                
pwd
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="$(pwd)localmodedsh4>dsh4>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Local mode: Piping with multiple spaces" {
    run "./dsh" <<EOF
ls     |    wc -l
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="$(ls | wc -l)localmodedsh4>dsh4>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]

    [ "$status" -eq 0 ]
}

@test "Local mode: Echo quoted and unquoted arguments" {
    run ./dsh <<EOF
echo Test "Test Multi   word unquoted"   unquoted
EOF

    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')

    expected_output="Test Test Multi   word unquoted unquotedlocal modedsh4> dsh4> cmd loop returned 0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Local mode: tr to lowercase" {
    run "./dsh" <<EOF
echo "ABCdef" | tr 'A-Z' 'a-z'
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="abcdeflocalmodedsh4>dsh4>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Local mode: echo piped to cat" {
    run "./dsh" <<EOF
echo "Hello, Pipeline" | cat
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="Hello,Pipelinelocalmodedsh4>dsh4>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Local mode: Piping with no spaces between piped commands" {
    run "./dsh" <<EOF
ls|wc -l
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="$(ls|wc -l)localmodedsh4>dsh4>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]

    [ "$status" -eq 0 ]
}

@test "Local mode: Piping with leading and trailing spaces" {
    run "./dsh" <<EOF
    ls   |   wc -l    
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="$(ls   |   wc -l)localmodedsh4>dsh4>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]

    [ "$status" -eq 0 ]
}

@test "Local mode: cd to tmp" {
    run "./dsh" <<EOF                
cd /tmp
pwd
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="/tmplocalmodedsh4>dsh4>dsh4>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Expected output should contain "/"
    [ "$stripped_output" = "$expected_output" ]

    [ "$status" -eq 0 ]
}

@test "Local mode: cd to non-existent path" {
    run "./dsh" <<EOF
cd /nonexistentpath
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="localmodedsh4>error:commandfaileddsh4>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    
    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}