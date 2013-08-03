#!/bin/bash

echo "Removing old logs..."
rm -f /var/wireless/Library/Logs/ccsnoop.log

echo "Saving default plist..."
cp /System/Library/LaunchDaemons/com.apple.CommCenterClassic.plist /System/Library/LaunchDaemons/com.apple.CommCenterClassic.plist.vanilla

echo "Moving injection plist into place..."
cp /tmp/com.apple.CommCenterClassic.plist /System/Library/LaunchDaemons/com.apple.CommCenterClassic.plist

# Restart CommCenter
echo "Unloading CommCenterClassic..."
launchctl unload -w /System/Library/LaunchDaemons/com.apple.CommCenterClassic.plist
echo "Loading CommCenterClassic..."
launchctl load -w /System/Library/LaunchDaemons/com.apple.CommCenterClassic.plist

echo "Restoring original plist..."
cp /System/Library/LaunchDaemons/com.apple.CommCenterClassic.plist.vanilla /System/Library/LaunchDaemons/com.apple.CommCenterClassic.plist
