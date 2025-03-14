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

    expected_output="$(ls | wc -l)localmodedsh4>dsh4>cmdloopreturned0"

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

@test "Start Client without server throws error" {
    run ./dsh -c -i 0.0.0.0 -p 3676
    
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="socketclientmode:addr:0.0.0.0:3676Theserverisdown.startclient:Connectionrefusedcmdloopreturned-52"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Start the server" {
    # Start the server in the background using &, redirecting output.
    ./dsh -s -i 0.0.0.0 -p 3676 > server.log 2>&1 &

    sleep 1

    # Kill the server process.
    pkill -f "./dsh -s -i 0.0.0.0 -p 3676"
    
    # Capture and strip whitespace from the server log.
    output=$(cat server.log)
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    
    # Construct the expected output without whitespace.
    expected_output="socketservermode:addr:0.0.0.0:3676->Single-ThreadedModeServerbootedsuccessfullyon0.0.0.0:3676"
    
    echo "Captured stdout:"
    echo "Output: $output"
    echo "${stripped_output} -> ${expected_output}"
    
    [ "$stripped_output" = "$expected_output" ]
}

@test "Server-Client: client exit" {
    # Start the server in the background and log output.
    ./dsh -s -i 0.0.0.0 -p 3676 > server.log 2>&1 &

    # Wait until client gets the output from server.
    sleep 2

    run ./dsh -c -i 0.0.0.0 -p 3676 <<EOF
exit
EOF

    # Kill the server process.
    pkill -f "./dsh -s -i 0.0.0.0 -p 3676"
    
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="socketclientmode:addr:0.0.0.0:3676Connectedtoserverat0.0.0.0:3676dsh4>clientexited:gettingnextconnection...cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Start server, start client, stop server" {
    # Start the server in the background and log output.
    ./dsh -s -i 0.0.0.0 -p 3676 > server.log 2>&1 &

    # Wait until client gets the output from server.
    sleep 2

    run ./dsh -c -i 0.0.0.0 -p 3676 <<EOF
stop-server
EOF
    
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="socketclientmode:addr:0.0.0.0:3676Connectedtoserverat0.0.0.0:3676dsh4>clientrequestedservertostop,stopping...cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Server-Client: pwd gives current working directory" {
    # Start the server in the background and log output.
    ./dsh -s -i 0.0.0.0 -p 3676 > server.log 2>&1 &

    # Wait until client gets the output from server.
    sleep 2

    run ./dsh -c -i 0.0.0.0 -p 3676 <<EOF
pwd
stop-server
EOF
    
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="socketclientmode:addr:0.0.0.0:3676Connectedtoserverat0.0.0.0:3676dsh4>$(pwd)Commandexecutedwithreturncode:0dsh4>clientrequestedservertostop,stopping...cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Server-Client: Piping with multiple spaces" {
    # Start the server in the background and log output.
    ./dsh -s -i 0.0.0.0 -p 3676 > server.log 2>&1 &

    # Wait until client gets the output from server.
    sleep 2

    run ./dsh -c -i 0.0.0.0 -p 3676 <<EOF
ls  |  wc -l
stop-server
EOF
    
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="socketclientmode:addr:0.0.0.0:3676Connectedtoserverat0.0.0.0:3676dsh4>$(ls  |  wc -l)Commandexecutedwithreturncode:0dsh4>clientrequestedservertostop,stopping...cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Server-Client: Echo quoted and unquoted arguments" {
    # Start the server in the background and log output.
    ./dsh -s -i 0.0.0.0 -p 3676 > server.log 2>&1 &

    # Wait until client gets the output from server.
    sleep 2

    run ./dsh -c -i 0.0.0.0 -p 3676 <<EOF
echo Test "Test Multi   word unquoted"   unquoted
stop-server
EOF
    
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')

    expected_output="socket client mode:  addr:0.0.0.0:3676Connected to server at 0.0.0.0:3676dsh4> Test Test Multi   word unquoted unquotedCommand executed with return code: 0dsh4> client requested server to stop, stopping...cmd loop returned 0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Server-Client: tr to lowercase" {
    # Start the server in the background and log output.
    ./dsh -s -i 0.0.0.0 -p 3676 > server.log 2>&1 &

    # Wait until client gets the output from server.
    sleep 2

    run ./dsh -c -i 0.0.0.0 -p 3676 <<EOF
echo "ABCdef" | tr 'A-Z' 'a-z'
stop-server
EOF
    
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="socketclientmode:addr:0.0.0.0:3676Connectedtoserverat0.0.0.0:3676dsh4>abcdefCommandexecutedwithreturncode:0dsh4>clientrequestedservertostop,stopping...cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Server-Client: echo piped to cat" {
    # Start the server in the background and log output.
    ./dsh -s -i 0.0.0.0 -p 3676 > server.log 2>&1 &

    # Wait until client gets the output from server.
    sleep 2

    run ./dsh -c -i 0.0.0.0 -p 3676 <<EOF
echo "Hello, Pipeline" | cat
stop-server
EOF
    
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="socketclientmode:addr:0.0.0.0:3676Connectedtoserverat0.0.0.0:3676dsh4>Hello,PipelineCommandexecutedwithreturncode:0dsh4>clientrequestedservertostop,stopping...cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Server-Client: Piping with no spaces between commands" {
    # Start the server in the background and log output.
    ./dsh -s -i 0.0.0.0 -p 3676 > server.log 2>&1 &

    # Wait until client gets the output from server.
    sleep 2

    run ./dsh -c -i 0.0.0.0 -p 3676 <<EOF
ls|wc -l
stop-server
EOF
    
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="socketclientmode:addr:0.0.0.0:3676Connectedtoserverat0.0.0.0:3676dsh4>$(ls | wc -l)Commandexecutedwithreturncode:0dsh4>clientrequestedservertostop,stopping...cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Server-Client: Piping with leading and trailing spaces" {
    # Start the server in the background and log output.
    ./dsh -s -i 0.0.0.0 -p 3676 > server.log 2>&1 &

    # Wait until client gets the output from server.
    sleep 2

    run ./dsh -c -i 0.0.0.0 -p 3676 <<EOF
    ls    |    wc -l    
stop-server
EOF
    
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="socketclientmode:addr:0.0.0.0:3676Connectedtoserverat0.0.0.0:3676dsh4>$(ls | wc -l)Commandexecutedwithreturncode:0dsh4>clientrequestedservertostop,stopping...cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Server-Client: cd to tmp" {
    # Start the server in the background and log output.
    ./dsh -s -i 0.0.0.0 -p 3676 > server.log 2>&1 &

    # Wait until client gets the output from server.
    sleep 2

    run ./dsh -c -i 0.0.0.0 -p 3676 <<EOF
cd /tmp
pwd
stop-server
EOF
    
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="socketclientmode:addr:0.0.0.0:3676Connectedtoserverat0.0.0.0:3676dsh4>dsh4>/tmpCommandexecutedwithreturncode:0dsh4>clientrequestedservertostop,stopping...cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Server-Client: cd to non-existent path" {
    # Start the server in the background and log output.
    ./dsh -s -i 0.0.0.0 -p 3676 > server.log 2>&1 &

    # Wait until client gets the output from server.
    sleep 2

    run ./dsh -c -i 0.0.0.0 -p 3676 <<EOF
cd nonexistentpath
stop-server
EOF
    
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="socketclientmode:addr:0.0.0.0:3676Connectedtoserverat0.0.0.0:3676dsh4>error:commandfaileddsh4>clientrequestedservertostop,stopping...cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}
