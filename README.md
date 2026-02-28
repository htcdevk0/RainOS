# RainOS
<a href="https://ibb.co/x8YWSJ3Z"><img src="https://i.ibb.co/PzMKZwtk/Rain-OS.png" alt="Rain-OS" border="0" width=80>RainOS</a>

Version: 1.1.0
Last patch (Not listed): Added 64-bits and Uefi Support.

RainOS is an educational operating system kernel built for experimentation,
low-level learning, and controlled system design.

It is not meant to compete with existing operating systems.  
It exists to understand them.

> Philosophy: Just build what you enjoy.

RainOS is a space for curiosity, architecture exploration, and system design
without unnecessary abstraction layers.

---

## Purpose

RainOS is designed as:

- A learning environment for kernel development
- A playground for framebuffer graphics experimentation
- A testing ground for boot, memory management, and architecture transitions
- A controlled platform for building UI systems directly over hardware abstractions

The focus is clarity, simplicity, and intentional system design.

---

## Boot & Platform Support

RainOS uses:

- GRUB (Multiboot2)
- Framebuffer-based graphics
- 64-bit long mode
- Custom memory paging setup
- QEMU + OVMF for development

### UEFI Status

RainOS is capable of running in UEFI environments.

However, at the current stage, it is primarily executed using CSM
(Compatibility Support Module) due to ongoing driver and low-level
initialization limitations.

UEFI-native execution is supported in virtualized environments,
but real hardware UEFI boot will remain partially limited until
hardware driver layers are fully stabilized.

---

## Core Technologies

### Framebuffer Backend

RainOS operates directly on a framebuffer provided by the bootloader.
All rendering is done without external graphical stacks.

There is no dependency on external GUI frameworks.
All graphics are handled internally.

---

## RFTK — Rain Framebuffer Toolkit

RFTK is the complete framebuffer abstraction layer of RainOS.

It is responsible for:

- Pixel management
- Drawing primitives
- Font rendering
- Memory-safe framebuffer access
- Color manipulation
- Surface abstraction

RFTK attempts to reduce the complexity of low-level framebuffer handling
while maintaining full control over memory and rendering logic.

It exists to make framebuffer-based kernel development practical and structured.

---

## RWTK — Rain Window Toolkit

RWTK is implemented directly inside the kernel.

It is not a userspace library.
It operates fully in kernel space using RFTK as its backend.

RWTK provides:

- Window abstraction
- Basic UI components
- Internal application structure
- Event routing
- Desktop-like behavior

The goal of RWTK is to make application creation inside RainOS easier
without sacrificing architectural transparency.

It acts as a lightweight alternative to traditional GUI systems,
built specifically for RainOS.

---

## Kernel Characteristics

- Freestanding 64-bit kernel
- Custom GDT and paging setup
- Manual long mode transition
- Identity-mapped memory regions
- Interrupt handling (IDT + ISR)
- Basic driver layer (keyboard, mouse, RTC)
- Framebuffer graphical environment

Everything is built with direct hardware awareness in mind.

---

## Development Environment

Recommended environment:

- Linux (Ubuntu/Debian-based recommended)
- NASM
- GCC (freestanding mode)
- GRUB tools
- QEMU
- OVMF for UEFI testing

Build:
```
make
```

Run (UEFI + KVM recommended):
```
make run-cpu # Runs in QEMU using OVMF (UEFI firmware emulation), not your machine's UEFI.
```

---

## Project Philosophy

RainOS is not about feature count.

It is about:

- Understanding the boot process
- Controlling memory explicitly
- Designing UI from scratch
- Experimenting with system architecture
- Learning by building

It favors intention over complexity.

It favors clarity over convenience.

It exists because building systems is enjoyable.

---

## Status

RainOS is experimental and educational.

It is under active development.
APIs and internal structures may change.

It is a learning system first.
A product never.

## Patches

- Patches App

- Settings App

- Showcase App

- Debug App

- Improved Cursor

- Improved Icons

Made by: htcdevk0.
