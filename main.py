import time
import random
import paho.mqtt.client as mqtt

# === KONFIGURASI ===
BROKER = "localhost"  # ganti ke "host.docker.internal" kalau dari luar Docker
PORT = 1883
USERNAME = "uas25_ridwan"
PASSWORD = "uas25_ridwan"

# Topik MQTT
STATUS_TOPIC = "UAS25-IOT/Status"
SUHU_TOPIC = "UAS25-IOT/33423315/SUHU"
HUM_TOPIC = "UAS25-IOT/33423315/KELEMBAPAN"
LUX_TOPIC = "UAS25-IOT/33423315/LUX"

status = "STOP"  # Default awal

# === CALLBACK ===
def on_connect(client, userdata, flags, rc):
    print(f"[CONNECTED] Status: {rc}")
    if rc == 0:
        client.subscribe(STATUS_TOPIC)
        print(f"[SUBSCRIBED] to topic {STATUS_TOPIC}")
    else:
        print("[ERROR] Failed to connect")

def on_message(client, userdata, msg):
    global status
    new_status = msg.payload.decode().strip()
    print(f"[RECEIVED STATUS] '{new_status}'")
    status = new_status

# === INISIASI CLIENT ===
client = mqtt.Client()
client.username_pw_set(USERNAME, PASSWORD)
client.on_connect = on_connect
client.on_message = on_message

client.connect(BROKER, PORT, 60)
client.loop_start()

try:
    while True:
        if status.strip().upper() == "START":
            suhu = round(random.uniform(25.0, 30.0), 1)
            hum = round(random.uniform(60.0, 80.0), 1)
            lux = random.randint(100, 1000)

            client.publish(SUHU_TOPIC, str(suhu))
            client.publish(HUM_TOPIC, str(hum))
            client.publish(LUX_TOPIC, str(lux))

            print(f"[PUBLISH] SUHU={suhu} | HUM={hum} | LUX={lux}")
        else:
            print(f"[STATUS] {status} - STOP - waiting...")

        time.sleep(5)

except KeyboardInterrupt:
    print("[EXIT] Stopped by user.")
    client.loop_stop()
    client.disconnect()
