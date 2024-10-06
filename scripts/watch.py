#!/usr/bin/python3

import subprocess
import threading
import time

inotify_proc = subprocess.Popen(["inotifywait", "-r", "-m", "-e", "move,modify,create,delete", "/src"], stdout=subprocess.PIPE)

last_change = 0
last_compilation = -10000

def listener_thread_entrypoint():
    global last_change
    print('listener_thread_entrypoint', flush=True)
    if not inotify_proc.stdout: raise Exception()

    while True:
        inotify_proc.stdout.read(1)
        last_change = time.time()

inotify_thread = threading.Thread(target=listener_thread_entrypoint)
inotify_thread.start()

while True:
    if last_change > last_compilation - 0.1:
        last_compilation = time.time()
        print(f'### BUILD STARTING ###', flush=True)
        subprocess.run(["/bin/bash", "/src/scripts/build.sh"])
        print(f'### BUILD COMPLETE ###', flush=True)

    time.sleep(.1)
