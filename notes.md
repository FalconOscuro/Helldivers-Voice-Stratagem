Flow:
- Main thread for gui
- Secondary thread for core functionality:
    - Load configuration
    - Await hotkey:
        - Interpret: only loop on fail
        - pause until hotkey not held
