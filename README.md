# Staircase Smart Smart switch

## Description

    This is firmware for controlling staircase lights, build for Shelly 1 IoT switch.  
    It consist of web iterface for control and parameter set, wi-fi AP for config  
    and timezone based schedule.

## MQTT Commands

    // Basic
    "lamp_on" - On
    "lamp_off" - Off
    "mode_timer" - Sets mode to timer (5 secs(hardcoded))
    "mode_manual" - Sets mode to manual, on "lamp_on" works till "lamp_off" is sent
    "schedule_on" - (only debug msgs in TODO list)
    "schedule_off"  - (only debug msgs in TODO list)
    "schedule_mode_hour"  - (only debug msgs in TODO list)
    "schedule_mode_sun"  - (only debug msgs in TODO list)

## TODO
    
    Schedule timers, web interface, timer mode delay setup, cfg.save