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

    expected_output="$(pwd)dsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Echo single word, no quotes" {
    run "./dsh" <<EOF
echo Hello
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="Hellodsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Echo with multiple spaces and no quotes" {
    run "./dsh" <<EOF
echo     Hello     World    
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="HelloWorlddsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Echo with quoted spaced string" {
    run "./dsh" <<EOF
echo "Hello,     World"
EOF

    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')

    expected_output="Hello,     Worlddsh2> dsh2> cmd loop returned 0"

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

    expected_output="Test Test Multi   word unquoted unquoteddsh2> dsh2> cmd loop returned 0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "cd to /tmp" {
    run "./dsh" <<EOF
cd /tmp
pwd
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="/tmpdsh2>dsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "cd to non-existent path" {
    run "./dsh" <<EOF
cd /nonexistentpath
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="dsh2>error:commandfaileddsh2>cmdloopreturned0"

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

    expected_output="dsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$status" -eq 0 ]
}

@test "Check providing Invalid command gives command not found:" {
    run "./dsh" <<EOF
invalid_command
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="dsh2>commandnotfound:invalid_commanddsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Check empty input gives warning: no command provided" {
    run "./dsh" <<EOF

EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="dsh2>warning:nocommandsprovideddsh2>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}
