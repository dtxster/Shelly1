# Staircase Smart Smart switch

## Description

    This is firmware for controlling staircase lights, build for Shelly 1 IoT switch.  
    It consist of web interface for control and parameter setting, wi-fi AP for configuring  
    and timezone based schedule.

## Guideline

    1. In <b>Timer</b> mode, on hardware switch press or MQTT "lamp_on" command works for a defined period of time.
    2. In <b>Manual</b> mode, on hardware switch press or MQTT "lamp_on" command works untill switch is pressed again or MQTT "lamp_off" is sent.
    3. If <b>Schedule</b> mode is active, there are two types of operations: Sunrise/Sunset based schedule, or predefined start/stop time.
 
## MQTT Commands

    // Basic
    "lamp_on" - On
    "lamp_off" - Off
    // Mode switching
    "mode_timer" - Sets mode to timer (5 secs(hardcoded))
    "mode_manual" - Sets mode to manual, on "lamp_on" works till "lamp_off" is sent
    // Schedule settings
    "schedule_on" - (only debug msgs in TODO list)
    "schedule_off"  - (only debug msgs in TODO list)
    "schedule_mode_hour"  - (only debug msgs in TODO list)
    "schedule_mode_sun"  - (only debug msgs in TODO list)
    // TODO
    "timer_delay (secs)"

## TODO
    
    Schedule timers, web interface, timer mode delay setup, cfg.save