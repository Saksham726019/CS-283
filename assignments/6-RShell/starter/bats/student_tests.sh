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

@test "Start the server" {
    # Start the server in the background using &, redirecting output.
    ./dsh -s -i 0.0.0.0 -p 7890 > server.log 2>&1 &

    # Kill the server process.
    pkill -f "./dsh -s -i 0.0.0.0 -p 7890"
    
    # Capture and strip whitespace from the server log.
    output=$(cat server.log)
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    
    # Construct the expected output without whitespace.
    expected_output="socketservermode:addr:0.0.0.0:7890->Single-ThreadedModeServerbootedsuccessfullyon0.0.0.0:7890"
    
    echo "Captured stdout:"
    echo "Output: $output"
    echo "${stripped_output} -> ${expected_output}"
    
    [ "$stripped_output" = "$expected_output" ]
}

@test "Start server, start client, stop server" {
    # Start the server in the background and log output.
    ./dsh -s -i 0.0.0.0 -p 7890 > server.log 2>&1 &

    # Wait until client gets the output from server.
    sleep 2

    run ./dsh -c -i 0.0.0.0 -p 7890 <<EOF
stop-server
EOF
    
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="socketclientmode:addr:0.0.0.0:7890Connectedtoserverat0.0.0.0:7890dsh4>clientrequestedservertostop,stopping...cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Server-Client: pwd gives current working directory" {
    # Start the server in the background and log output.
    ./dsh -s -i 0.0.0.0 -p 7890 > server.log 2>&1 &

    # Wait until client gets the output from server.
    sleep 2

    run ./dsh -c -i 0.0.0.0 -p 7890 <<EOF
pwd
stop-server
EOF
    
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="socketclientmode:addr:0.0.0.0:7890Connectedtoserverat0.0.0.0:7890dsh4>$(pwd)Commandexecutedwithreturncode:0dsh4>clientrequestedservertostop,stopping...cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Server-Client: Piping with multiple spaces" {
    # Start the server in the background and log output.
    ./dsh -s -i 0.0.0.0 -p 7890 > server.log 2>&1 &

    # Wait until client gets the output from server.
    sleep 2

    run ./dsh -c -i 0.0.0.0 -p 7890 <<EOF
ls  |  wc -l
stop-server
EOF
    
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="socketclientmode:addr:0.0.0.0:7890Connectedtoserverat0.0.0.0:7890dsh4>$(ls  |  wc -l)Commandexecutedwithreturncode:0dsh4>clientrequestedservertostop,stopping...cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Server-Client: Echo quoted and unquoted arguments" {
    # Start the server in the background and log output.
    ./dsh -s -i 0.0.0.0 -p 7890 > server.log 2>&1 &

    # Wait until client gets the output from server.
    sleep 2

    run ./dsh -c -i 0.0.0.0 -p 7890 <<EOF
echo Test "Test Multi   word unquoted"   unquoted
stop-server
EOF
    
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')

    expected_output="socket client mode:  addr:0.0.0.0:7890Connected to server at 0.0.0.0:7890dsh4> Test Test Multi   word unquoted unquotedCommand executed with return code: 0dsh4> client requested server to stop, stopping...cmd loop returned 0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Server-Client: tr to lowercase" {
    # Start the server in the background and log output.
    ./dsh -s -i 0.0.0.0 -p 7890 > server.log 2>&1 &

    # Wait until client gets the output from server.
    sleep 2

    run ./dsh -c -i 0.0.0.0 -p 7890 <<EOF
echo "ABCdef" | tr 'A-Z' 'a-z'
stop-server
EOF
    
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="socketclientmode:addr:0.0.0.0:7890Connectedtoserverat0.0.0.0:7890dsh4>abcdefCommandexecutedwithreturncode:0dsh4>clientrequestedservertostop,stopping...cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}