# Staircase Smart Smart switch

## Description

    This is firmware for controlling staircase lights, build for Shelly 1 IoT switch.  
    It consist of web interface for control and parameter setting, wi-fi AP for configuring  
    and timezone based schedule.

## Guideline

    1. In Timer mode, on hardware switch press or MQTT {relay:on} command works for a defined period of time.  
    2. In Manual mode, on hardware switch press and hold for 1 second, or MQTT {relay:on,mode:1} command  
    works until switch is pressed again or MQTT {relay:off} is sent.  
    After the off command system goes back to Timer mode.  
    3. If Schedule mode is active, there are two types of operations: Sunrise/Sunset based schedule,  
    or predefined start/stop time.
 
## MQTT Commands

    MQTT accepts JSON formatting as shown:
    // Basic
    {relay:true,delay:5,mode:0}
    where:
        relay:true/false - On-Off
        delay:seconds for Timer mode
        mode:0-Timer,1-Manual

    // TODO
        Schedule settings

## TODO
    
    Schedule timers, web interface, timer mode delay setup, cfg.save