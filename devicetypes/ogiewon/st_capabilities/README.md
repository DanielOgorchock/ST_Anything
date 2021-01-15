# Custom DTH for new ST app

The new ST app utilizes a 4-part topology to construct a device's UI.

```
  +-----+    
  | DTH |
  +-----+    
     |    +---------------+
     +--> | Device Config |
          +---------------+
                  |    +-------------+     +---------------+
                  +--> | Capability1 | --> | Presentation1 |
                  |    +-------------+     +---------------+
                  |    +-------------+     +---------------+
                  +--> | Capability2 | --> | Presentation2 |
                  |    +-------------+     +---------------+
                  |
                  ...
```

Starting from the bottom and working up...

- The `Presentation` describes how the capability should be rendered when shown on the dashboard or detail views.
- The `Capability` defines the attributes and commands available to the DHT and Device Config. _NOTE_: Only a single attribute per capability is currently recommended.
- The `Device Config` manages the layout and presentation of all of the capabilities for the device. It defines which capability will be rendered on the dashboard and in which order the capabilities will be rendered in the detail view. _NOTE_: The `Device Config` describes **where** each capability will be displayed. The `Presentation` describes **how** a specific capability will be display.
- The `DTH` provides the functions, handlers, and callbacks that interact with the ST APIs. It acts as the glue between the capabilities and the ST platform.

## Capability

```
CAP_ID=afterwatch06989.whatever
CAP_VERSION=1

# create
smartthings capabilities:create -j -i ./st-capability.json

# update
smartthings capabilities:update $CAP_ID $CAP_VERSION -j -i ./st-capability.json
```

## Capability Presentation

```
CAP_ID=afterwatch06989.whatever
CAP_VERSION=1

# create
smartthings capabilities:presentation:create $CAP_ID $CAP_VERSION -j -i ./st-presentation.json

# update
smartthings capabilities:presentation:update $CAP_ID $CAP_VERSION -j -i ./st-presentation.json
```

## Device Config

```
DTH_UUID=aaaaaaaa-bbbb-cccc-dddd-eeeeeeeeeeee

# generate
smartthings presentation:device-config:generate $DTH_UUID --dth -j -o ./st-device-config.json

# create
smartthings presentation:device-config:create -j -i ./st-device-config.json
```

## Resources

1. Good DTH overview for new ST app:
    
    see: https://community.smartthings.com/t/custom-capability-and-cli-developer-preview/197296/678

2. Get VID of built-in device configs:

    ```
    curl --header "Authorization: Bearer AUTHTOKEN" https://api.smartthings.com/v1/presentation/deviceconfig?"presentationId=generic-switch&mnmn=SmartThings"
    ```

    see: https://community.smartthings.com/t/custom-capability-and-cli-developer-preview/197296/633

