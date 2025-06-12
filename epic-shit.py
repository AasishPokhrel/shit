# epic_shit.py

import time
import random
import sys

print("🚽 Initializing... Please wait while we flush the bugs.")

time.sleep(2)

def crash_randomly():
    errors = [
        "Segmentation Fault: Core memory ran away crying.",
        "SyntaxError: Unexpected fart in line 42",
        "NullPointerException: You pointed at nothing, just like your career.",
        "RuntimeError: Too much caffeine detected",
        "KeyboardInterrupt: Rage quit detected"
    ]
    raise Exception(random.choice(errors))

def infinite_loop_of_despair():
    print("Entering infinite loop of existential debugging.")
    while True:
        print("Fixing bugs... Making new ones...")
        time.sleep(1)

try:
    if random.random() > 0.5:
        crash_randomly()
    else:
        infinite_loop_of_despair()
except Exception as e:
    print(f"💥 Oops: {e}")
    print("💩 Shit broke. But at least it's funny.")
    sys.exit(1)
