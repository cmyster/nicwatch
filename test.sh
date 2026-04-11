#!/bin/bash
set -e

# --- Configuration ---
BIN="./nicwatch"
JOURNAL_DIR="$HOME/.nicwatch"

# Helper function to report failures
fail() {
    echo "FAIL: $1: $2"
    exit 1
}

echo "Starting Black-Box Testing for nicwatch..."

# -----------------------------------------------------------------------------
# SANITY: Build and Basic Execution
# -----------------------------------------------------------------------------
echo "Running Sanity Checks..."
make || fail "Build" "Project failed to compile"

# Run without arguments - should show usage and exit gracefully (0)
$BIN > /dev/null 2>&1 || fail "Sanity" "Tool crashed or exited with error when run without arguments"
echo "SANITY: OK"

# -----------------------------------------------------------------------------
# SMOKE TESTS
# -----------------------------------------------------------------------------

# Test 1: nicwatch snap
echo "Testing 'snap'..."
# Ensure clean state for this test
rm -rf "$JOURNAL_DIR"
$BIN snap > /dev/null 2>&1

if [ ! -d "$JOURNAL_DIR" ]; then
    fail "snap" "Journal directory $JOURNAL_DIR was not created"
fi

SNAP_COUNT=$(ls "$JOURNAL_DIR"/*.snap 2>/dev/null | wc -l)
if [ "$SNAP_COUNT" -eq 0 ]; then
    fail "snap" "No .snap files were created in $JOURNAL_DIR"
fi
echo "PASS: snap"

# Test 2: nicwatch status
echo "Testing 'status'..."
STATUS_OUT=$($BIN status 2>&1)
if echo "$STATUS_OUT" | grep -qE "GREEN|YELLOW|RED"; then
    echo "PASS: status"
else
    fail "status" "Output did not contain health colors (GREEN/YELLOW/RED). Output: $STATUS_OUT"
fi

# Test 3: nicwatch diff (no arguments)
echo "Testing 'diff' (default)..."
# Create a baseline snap
$BIN snap > /dev/null 2>&1
# Small delay to ensure timestamp difference if the tool relies on system clock
sleep 1 
# Create a second snap to ensure there is something to compare
$BIN snap > /dev/null 2>&1

DIFF_OUT=$($BIN diff 2>&1)
# We expect some output describing the diff, even if 0 changes are detected.
# If it crashes or returns empty, it's a fail.
if [ -n "$DIFF_OUT" ]; then
    echo "PASS: diff"
else
    fail "diff" "Diff produced no output"
fi

# Test 4: nicwatch diff <timestamp>
echo "Testing 'diff <timestamp>'..."
# Extract the timestamp from the most recent snapshot file
LATEST_SNAP=$(ls -t "$JOURNAL_DIR"/*.snap | head -n 1)
# Extract numeric part of the filename (assuming filename contains timestamp)
TIMESTAMP=$(basename "$LATEST_SNAP" .snap)

# Run diff against a specific snapshot
TIMESTAMP_DIFF_OUT=$($BIN diff "$TIMESTAMP" 2>&1)
if [ -n "$TIMESTAMP_DIFF_OUT" ]; then
    echo "PASS: diff <timestamp>"
else
    fail "diff <timestamp>" "Diff with timestamp produced no output"
fi

# Test 5: nicwatch watch <interval>
echo "Testing 'watch' daemon..."
# Start watch in background (1 second interval)
$BIN watch 1 > /dev/null 2>&1 &
WATCH_PID=$!

# Wait for a few snapshots to be generated
sleep 3

# Count snapshots before and after the watch period
COUNT_AFTER=$(ls "$JOURNAL_DIR"/*.snap | wc -l)
# Since we already had at least 2 from previous tests, we expect this to have increased
if [ "$COUNT_AFTER" -gt 2 ]; then
    echo "PASS: watch"
else
    fail "watch" "Daemon did not appear to create additional snapshots"
fi

# Cleanup daemon
kill $WATCH_PID 2>/dev/null || true

# Test 6: nicwatch history <iface>
echo "Testing 'history'..."
# Use a common interface name like lo (loopback) which always exists
HISTORY_OUT=$($BIN history lo 2>&1)
if echo "$HISTORY_OUT" | grep -qi "not fully implemented"; then
    echo "PASS: history"
else
    fail "history" "Did not receive 'not fully implemented' message. Output: $HISTORY_OUT"
fi

# Test 7: Usage Guide/Invalid Commands
echo "Testing usage/invalid commands..."
INVALID_OUT=$($BIN nonexistent_command 2>&1)
if echo "$INVALID_OUT" | grep -qiE "usage|invalid"; then
    echo "PASS: usage"
else
    fail "usage" "Invalid command did not trigger usage guide. Output: $INVALID_OUT"
fi

echo "-------------------------------------------------------"
echo "ALL BLACK-BOX TESTS: OK"
exit 0
