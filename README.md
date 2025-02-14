# TryHackMe-KoTH-Defender-LKM_rootkit
A Defensive Kernel Module for TryHackMe "King of the Hill" Competitions

This educational proof-of-concept Linux kernel module demonstrates defensive techniques for Capture-the-Flag (CTF) competitions, particularly in TryHackMe's "King of the Hill" (KoTH) matches. It showcases kernel-level system protections and monitoring concepts through various defensive mechanisms.
🔒 Defensive Features

    Stealth Monitoring – Temporary process and file hiding for rule-compliant competition strategies.
    Connection Obfuscation – Port masking techniques for service protection challenges.
    Alert System – Triggers a reverse shell notification upon unauthorized access attempts.
    Kernel Hardening – Demonstrates module self-protection techniques.

🎯 TryHackMe Use Case

This module is designed for educational KoTH competitions, allowing participants to:

✔️ Understand Linux kernel module (LKM) development
✔️ Practice detecting rootkit-like protections
✔️ Learn system hardening techniques
✔️ Study defensive hooking mechanisms
⚙️ Technical Components

    Syscall Hooking – Intercepts getdents and getdents64 for process hiding.
    ProcFS Manipulation – Hides entries in /proc to prevent detection.
    TCP Sequence Filtering – Alters TCP behavior for obfuscation.
    Kernel Workqueues – Implements periodic tasks for stealthy execution.
    UMH (User Mode Helper) Triggers – Launches user-space commands from the kernel.

⚠️ Important Notes

🔹 For Educational Use Only – Created strictly for TryHackMe competitions.
🔹 Not for Real Systems – Contains intentional vulnerabilities for CTF analysis.
🔹 Ethical Use Only – This module should only be used in:

    TryHackMe labs
    Local VMs with no network access
    Pre-authorized CTF competitions
