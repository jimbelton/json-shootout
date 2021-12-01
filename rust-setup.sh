#!/bin/bash
export PATH="$HOME/.cargo/bin:$PATH"

# Check for curl
which curl > /dev/null
if [ $? -ne 0 ]; then
    echo "Requires 'curl' to be installed. Exiting..."
    exit 1
fi

# Install rust
which cargo > /dev/null
if [ $? -ne 0 ]; then
    echo "***** Installing Rust *****"
    curl --proto '=https' --tlsv1.2 -o rustup.sh https://sh.rustup.rs &&
    chmod +x rustup.sh &&

    echo "Rust install-script stored as 'rustup.sh'" &&
    ./rustup.sh -y &&

    echo "Removing install-script 'rustup.sh'" &&
    rm -f rustup.sh
fi

echo
echo "***** Building Rust json-shootout binary *****" &&
cargo build --release &&

if [[ -f "target/release/json-shootout" ]] ; then
    echo "Copying Rust binary to target/release/json-shootout -> rust-bench"
    cp target/release/json-shootout rust-bench
fi
