# Node-RED Flow (ThinkIOT DHT22 MQTT)

This folder contains the Node-RED artifacts for the project **esp32-dht22-mqtt-nodered**.

## What’s in this folder
- `flows-thinkiot-dht22.json`:
  Exported Node-RED flow JSON (MQTT-in → payload-to-number → Dashboard gauges + Debug).

## Runtime prerequisites (shared dev platform)
This project expects you to have the shared IoT dev platform running:
- Mosquitto broker (MQTT) exposed on port 1883
- Node-RED exposed on port 1880

(We run them via Docker Compose in `~/workspace/iot-dev-platform`.)

Node-RED in Docker persists flows and installed nodes when `/data` is backed by a volume.  
Avoid `docker compose down -v` if you want to keep Node-RED’s `/data`.  

## Node-RED dependencies (required nodes)
This flow requires:
- `@flowfuse/node-red-dashboard` (Dashboard 2.0 / FlowFuse Dashboard)

Core Node-RED nodes used (come with Node-RED):
- `mqtt in`
- `change`
- `debug`

See: [dependencies.md](dependencies.md)

## Install the dependency in Node-RED
In Node-RED editor:
Menu → Manage Palette → Install → search “node-red-dashboard” → install `@flowfuse/node-red-dashboard`.

(Do **not** install the deprecated `node-red-dashboard` package.)

## Import the flow
In Node-RED editor:
Menu → Import → select `flows-thinkiot-dht22.json` (or paste its JSON) → Import → Deploy.

Node-RED supports importing by uploading a JSON file or pasting JSON, and you can import into the current flow or create a new flow tab.

## MQTT topics
The ESP32 publishes:
- `/thinkiot/esp32-dht22-mqtt-nodered/temp` (°C)
- `/thinkiot/esp32-dht22-mqtt-nodered/hum` (%)

The Node-RED flow subscribes to the same topics.

## Dashboard
Dashboard page:
- `http://localhost:1880/dashboard/thinkiot`

## Quick broker test (optional)
Subscribe:
- `docker exec -it mosquitto mosquitto_sub -v -t "/ThinkIOT/#"`

Publish:
- `docker exec -it mosquitto mosquitto_pub -t "/ThinkIOT/temp" -m "24.80"`
- `docker exec -it mosquitto mosquitto_pub -t "/ThinkIOT/hum"  -m "56.1"`
