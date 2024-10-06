import subprocess
import threading
import time

inotify_proc = subprocess.Popen(["inotifywait", "-r", "-m", "-e", "move,modify,create,delete", "/src"], stdout=subprocess.PIPE)

def listener_thread_entrypoint():
    while True:
        in_bytes = inotify_proc.stdout.read()
        print(f'{in_bytes=}')

inotify_thread = threading.Thread(listener_thread_entrypoint)
inotify_thread.start()

while True:
    need_to_compile = False

    time.sleep(1)
