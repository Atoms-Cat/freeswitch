import threading
import time
import random

# Mock Sofia-SIP structures
class SofiaProfile:
    def __init__(self, name, port):
        self.name = name
        self.port = port
        self.running = False
        self.event_queue = []

    def process_events(self):
        # Simulate su_root_step
        if self.event_queue:
            event = self.event_queue.pop(0)
            print(f"[{self.name}] Processing event: {event}")
        else:
            # Idle wait (simulating poll/select)
            time.sleep(0.1)

def sofia_profile_thread_run(profile):
    print(f"[{profile.name}] Thread started on port {profile.port}")
    profile.running = True
    
    # Initialize NUA (simulated)
    print(f"[{profile.name}] NUA Stack Initialized")
    
    # Event Loop (su_root_run / su_root_step)
    while profile.running:
        try:
            profile.process_events()
            # Simulate random incoming SIP packet
            if random.random() < 0.05:
                profile.event_queue.append("INVITE sip:1000@" + profile.name)
        except Exception as e:
            print(f"[{profile.name}] Error: {e}")
            break
            
    print(f"[{profile.name}] Thread stopped")

def mod_sofia_load():
    print("[Main] mod_sofia loading...")
    
    # Simulate parsing sofia.conf
    config = [
        {"name": "internal", "port": 5060},
        {"name": "external", "port": 5080}
    ]
    
    threads = []
    profiles = []
    
    for cfg in config:
        profile = SofiaProfile(cfg["name"], cfg["port"])
        profiles.append(profile)
        
        # launch_sofia_profile_thread
        t = threading.Thread(target=sofia_profile_thread_run, args=(profile,))
        t.daemon = True
        t.start()
        threads.append(t)
        print(f"[Main] Launched thread for profile: {profile.name}")
        
    return profiles, threads

if __name__ == "__main__":
    profiles, threads = mod_sofia_load()
    
    # Run simulation for 5 seconds
    time.sleep(5)
    
    print("[Main] Shutting down...")
    for p in profiles:
        p.running = False
        
    for t in threads:
        t.join()