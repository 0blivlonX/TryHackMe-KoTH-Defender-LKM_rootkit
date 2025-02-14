📌 What Are Linux Kernel Modules (LKMs)?

Linux Kernel Modules (LKMs) are loadable pieces of code that extend the Linux kernel's functionality without requiring a system reboot. LKMs are used for device drivers, security enhancements, and system monitoring. However, they can also be exploited in rootkits to stealthily control a compromised system.

This project focuses on defensive LKM techniques to protect systems in TryHackMe's King of the Hill (KoTH) competitions.
🔒 About KoTH Defender

This educational proof-of-concept LKM is designed for defensive security research in TryHackMe’s "King of the Hill" (KoTH) matches. It simulates kernel-level protections and stealth techniques, helping participants understand rootkit detection, Linux system hardening, and low-level monitoring.
🎯 Key Features

🔹 Stealth Monitoring – Temporarily hides processes and files to simulate real-world rootkit detection.
🔹 Connection Obfuscation – Masks active ports to protect critical services from attackers.
🔹 Alert System – Triggers reverse-shell notifications upon detecting unauthorized access attempts.
🔹 Kernel Hardening – Demonstrates module self-protection techniques to prevent tampering.
📌 TryHackMe Use Case

This module is intended for educational KoTH competitions, enabling participants to:

✔️ Understand Linux Kernel Module (LKM) development
✔️ Practice detecting and bypassing rootkit protections
✔️ Learn system hardening techniques against kernel threats
✔️ Study real-world kernel hooking and stealth mechanisms
⚙️ Technical Components

⚙️ Syscall Hooking – Hooks getdents and getdents64 to modify file listings.
⚙️ ProcFS Manipulation – Hides entries in /proc for stealthy process concealment.
⚙️ TCP Sequence Filtering – Alters TCP connection behavior for obfuscation.
⚙️ Kernel Workqueues – Schedules tasks within the kernel for stealth execution.
⚙️ UMH (User Mode Helper) Triggers – Executes user-space commands from kernel space.
⚠️ Ethical & Legal Considerations

⚠️ For Educational Use Only – This module is designed strictly for TryHackMe competitions.
⚠️ Not for Real-World Systems – The module contains intentional vulnerabilities for CTF analysis and should not be deployed on production environments.
⚠️ Responsible Use – This module is permitted only in:

    TryHackMe labs
    Local virtual machines (VMs) with no external network access
    Pre-approved Capture-the-Flag (CTF) competitions
