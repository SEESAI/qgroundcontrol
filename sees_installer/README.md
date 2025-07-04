# Installation instructions.
Latest can be found here:
https://github.com/SEESAI/qgroundcontrol/releases/tag/v4.3.0-0.0.3

But this is the idea:

1. Download the `sees_installer.zip`. <br>
2. Extract the `sees_installer.zip` file, and run the installer script `groundcontrol_install.py`. <br>
This will copy the .AppImage file, the Qgroundcontrol settings file,  and the files necessary to run the application from a desktop shortcut to the required folders and will create them if they don't exist.
3. Copy the GitHub Token from the Notion page: https://www.notion.so/seesai/QGroundcontrol-Sees-flavour-17cf348e991880faa376c0317c9d3ce0 to the `.env` file.



# Introduction
In order to have more control over the settings on a Qgroundcontrol running on a GCS we are now storing the standard configuration file (Qgroundcontrol.ini) in git.
This way if the settings that are critical to flight are changed the user gets a warning.

The installation script creates a custom QGroundcontrolIcon that starts a QT splash script that configures the .ini file according to what type of machine QGC is running in.
There are four types of machines GCS, UAV, RTK Base and Others.

The type of machine is set on the installation script, but it can also be changed in the .yaml configuration file

## GCS
From the splash screen the user can select the drone they want to connect to and through which Base station (Lavant comms, van)

The script sshs to the base station and starts the backup-link scripts in charge of starting mavlink-routerd to the appropiate UAV and also starts a local QGC that connects to the GPS and acts as
the RTK base. 
![image (24)](https://github.com/user-attachments/assets/adbb95ca-bf67-4d56-80de-0567e7fe6e41)

The mavlink data path is: 

QGC on GCS <-> mavlink-routerd on Base Station <-> UAV 

And for RTK 

RTK GPS -> QGC on Base Station -> mavlink-routerd on Base Station -> UAV

By using mavlink-routerd it is possible to share the single serial port that the doodle modem provides. Without it there can only be a single QGC instance connected. 

NOTE: The GCS connects to the mavlink-router UDP server created in port 3000 in any of the available network interfaces on teh base station. We use the wireguard one. 

GOTCHA: The UDP server will only server a single GCS, so if you can't connect it's very likely there is anohter GCS connected. This is a mavlink-routerd characteristic, which is very helpful as makes it safe. We do not want two QGC instances acting as a GCS and controlling the drone!


## RTK Base
No splash screen is necessary so it's not shown

The splash script ensures that connection to RTK and UDP are on by default

## UAV
No splash screen is necessary so it's not shown

The splash script enables connection to UDP by default

## OTHER
QGC starts without any type of checks and no settings are enforced
