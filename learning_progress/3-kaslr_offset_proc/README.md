# KASLR Offset Finder — Linux Kernel Module

A Linux loadable kernel module that calculates the **KASLR (Kernel Address Space Layout Randomization) offset** at runtime and exposes it through a `/proc` filesystem entry.

---

## Background

KASLR is a security mitigation that randomizes the base address at which the kernel loads into memory on every boot. This means every symbol's runtime address differs from its compile-time address by a constant slide value — the KASLR offset. Knowing this offset is useful for kernel debugging, security research, and introspection tooling.

---

## How It Works

The module resolves `kallsyms_lookup_name` at runtime using a kprobe (necessary because this function has been unexported since kernel 5.7). It then uses that function to look up the live addresses of three known syscall symbols and compares them against their static addresses extracted from `/boot/System.map` at build time. Since KASLR applies a uniform slide to the entire kernel image, all three should yield the same offset.

If all three offsets agree, the result is stored and exposed at `/proc/kaslr_offset`. If they disagree, the module flags a mismatch — which may indicate kernel symbol table tampering by a rootkit.

---

## Building & Usage

**Requirements:** kernel headers for the running kernel, `CONFIG_KPROBES=y`, and root access.

```bash
make                               # builds the module, reads System.map automatically
sudo insmod 3-kaslr_offset_proc.ko
cat /proc/kaslr_offset             # prints the offset, e.g. 0x1234560000
sudo rmmod 3-kaslr_offset_proc
```

---

## Makefile

The Makefile automatically extracts the static addresses of three anchor syscalls (`__x64_sys_getpid`, `__x64_sys_getuid`, `__x64_sys_getgid`) from `/boot/System.map` for the running kernel and passes them into the module as compile-time defines. No manual configuration is needed.

---

## Output

Reading `/proc/kaslr_offset` returns one of the following:

- `0x<hex>` — successful offset calculation, all anchors agreed. The offset will be 0 for virtual machines as they usually dont have enough entropy to create an offset
- `Mismatch of offsets.` — anchors resolved but returned inconsistent offsets
- `Couldnt resolve anchor at runtime.` — one or more symbols could not be found

---

## Notes

Loading any kernel module requires root. On systems with Secure Boot enabled, the module must be signed. The mismatch detection is a lightweight heuristic — a sufficiently sophisticated rootkit could potentially defeat it.
