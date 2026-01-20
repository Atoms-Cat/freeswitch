import time
import queue
import threading
from enum import Enum, auto

# --- 1. The "Two Worlds" Definitions ---

# World A: FreeSWITCH Core (Abstract)
class SwitchMessage(Enum):
    INDICATE_RINGING = auto()
    INDICATE_ANSWER = auto()
    INDICATE_HANGUP = auto()

class SwitchEvent(Enum):
    CUSTOM = auto()
    CHANNEL_CREATE = auto()
    CHANNEL_DESTROY = auto()

# World B: Sofia-SIP (Concrete Protocol)
class NuaEvent(Enum):
    nua_i_invite = auto()
    nua_i_bye = auto()
    nua_r_ok = auto() # 200 OK response

# --- 2. The Translator (mod_sofia) ---

class ModSofia:
    def __init__(self, core, sip_stack):
        self.core = core
        self.sip_stack = sip_stack
        self.event_queue = queue.Queue()
        self.running = True
        
        # Start the Event Dispatcher Thread (Network -> Core)
        threading.Thread(target=self.event_dispatch_loop, daemon=True).start()

    # [Core -> Endpoint] Translator
    # Corresponds to: sofia_receive_message in mod_sofia.c
    def receive_message(self, session_id, msg_type):
        print(f"[ModSofia] Translating Core Message: {msg_type.name} -> SIP")
        
        if msg_type == SwitchMessage.INDICATE_RINGING:
            # Core says "Ringing" -> SIP says "180 Ringing"
            self.sip_stack.nua_respond(session_id, 180, "Ringing")
            
        elif msg_type == SwitchMessage.INDICATE_ANSWER:
            # Core says "Answer" -> SIP says "200 OK"
            self.sip_stack.nua_respond(session_id, 200, "OK")
            
        elif msg_type == SwitchMessage.INDICATE_HANGUP:
            # Core says "Hangup" -> SIP says "BYE"
            self.sip_stack.nua_bye(session_id)

    # [Network -> Endpoint] Callback
    # Corresponds to: sofia_event_callback in sofia.c
    def on_sip_event(self, event_type, session_id, status, phrase):
        # Enqueue for processing (avoid blocking the SIP stack)
        self.event_queue.put((event_type, session_id, status, phrase))

    # [Network -> Endpoint] Dispatcher
    # Corresponds to: sofia_process_dispatch_event / our_sofia_event_callback
    def event_dispatch_loop(self):
        while self.running:
            try:
                event = self.event_queue.get(timeout=1)
                self.process_event(*event)
            except queue.Empty:
                continue

    def process_event(self, event_type, session_id, status, phrase):
        print(f"[ModSofia] Dispatching SIP Event: {event_type.name} ({status} {phrase}) -> Core")
        
        if event_type == NuaEvent.nua_i_invite:
            # SIP INVITE -> Create New Session in Core
            self.core.create_session(session_id)
            
        elif event_type == NuaEvent.nua_i_bye:
            # SIP BYE -> Hangup Session in Core
            self.core.hangup_session(session_id)
            
        elif event_type == NuaEvent.nua_r_ok:
            # SIP 200 OK -> Fire Event or Change State
            print(f"   -> Notifying Core: Call Answered")

# --- 3. Mocks for Context ---

class MockCore:
    def create_session(self, uuid):
        print(f"[Core] Creating new session: {uuid}")

    def hangup_session(self, uuid):
        print(f"[Core] Hanging up session: {uuid}")

class MockSipStack:
    def __init__(self):
        self.callback = None

    def set_callback(self, cb):
        self.callback = cb

    def nua_respond(self, session_id, status, phrase):
        print(f"   [SIP-Stack] Sending Response: {status} {phrase}")

    def nua_bye(self, session_id):
        print(f"   [SIP-Stack] Sending Request: BYE")

    # Simulate incoming network traffic
    def simulate_incoming_invite(self, session_id):
        if self.callback:
            self.callback(NuaEvent.nua_i_invite, session_id, 0, "INVITE")

    def simulate_incoming_bye(self, session_id):
        if self.callback:
            self.callback(NuaEvent.nua_i_bye, session_id, 0, "BYE")

# --- 4. Execution ---

if __name__ == "__main__":
    core = MockCore()
    sip = MockSipStack()
    mod = ModSofia(core, sip)
    sip.set_callback(mod.on_sip_event)

    print("--- Scenario 1: Inbound Call (Network -> Core) ---")
    sip.simulate_incoming_invite("uuid-1234")
    time.sleep(0.1)

    print("\n--- Scenario 2: Core Answers (Core -> Network) ---")
    mod.receive_message("uuid-1234", SwitchMessage.INDICATE_RINGING)
    mod.receive_message("uuid-1234", SwitchMessage.INDICATE_ANSWER)

    print("\n--- Scenario 3: Remote Hangup (Network -> Core) ---")
    sip.simulate_incoming_bye("uuid-1234")
    time.sleep(0.1)