---
layout: docs
title: Usage Rules
prev_section: home
next_section: license
permalink: /docs/usage-rules/
---

BitThunder gives you alot, and its only fair that you give back to BitThunder where at all possible.
Any changes to the core "kernel" MUST be contributed upstream. Application layer, or custom BSP code is
not required to be shared unless you want to.

## Drivers

Any driver that you write for publicly available devices, e.g. a component that anyone can buy, and
where a publicly available datasheet is available, MUST be contributed upstream.

Any driver that utilises custom, or internal hardware i.e. IP residing within an FPGA, or custom I2C device,
can be kept proprietary. In most cases such code is not useful to others, unless you want
others to integrate your hardware.

## Board Support Packages

Board support packages are by nature board specific. Like drivers, BSPs for publicly available boards,
like reference/development boards or things like the <a href="http://raspberrypi.org">RaspberryPi</a> SHOULD
be contributed.

# Pushing Upstream

Pushing upstream is simple! Simply fork BitThunder on github, and push your changes to your fork.
You should keep these changes permanently available in your github fork, or on a branch of your fork.

To get your changes into the mainline kernel you should create a Pull Request on github. If accepted you
no longer have to keep your fork.
