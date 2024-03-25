import subprocess
import os
import signal

program = "labwc"
args = ["-d"]
env = os.environ.copy()

# env["WLR_RENDERER"] = "vulkan"

process = subprocess.Popen([program] + args, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, env=env)

for line in process.stdout:
    output = line.decode().strip()
    print(output)
    if "Atomic commit failed: Permission denied" in output:
        pid = process.pid
        os.kill(pid, signal.SIGTERM)
        print("Detected ERROR in labwc. Killing...")
        break

