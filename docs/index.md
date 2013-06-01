---
layout: docs
title: Welcome
next_section: usage-rules
permalink: /docs/home/
---

This site aims to become a central resource for BitThunder documentation. As well as how-to guides
and blog-posts, we'll also try to provide as much comprehensive API documentation here as possible.

## So what is exactly is BitThunder?

BitThunder is a real-time, robust, embedded, operating system and application framework.

## Quick-build guide

For the impatient, here's how to get a vanilla kernel built:

{% highlight bash %}
~ $ make menuconfig
# Create a valid BSP configuration
~ $ make
# Load target with the generated kernel.elf file.
{% endhighlight %}

## WizTips™, Notes, and Warnings

Throughout this site, there are a number of small and handy pieces of information
that can be useful while developing with BitThunder. Some make your life easier,
some are interesting, and some keep you safe! Here's what to look out for:


<div class="note">
  <h5>WizTips™ help you get more from BitThunder</h5>
  <p>These are tips that will help you become a BitThunder Wizard!</p>
</div>

<div class="note info">
  <h5>Notes are handy pieces of information</h5>
  <p>These often give you useful background information, or reasoning behind design
  decisions which are necessary to completely understand BitThunder.</p>
</div>

<div class="note warning">
  <h5>Warnings help you stay safe!</h5>
  <p>Be aware of these messages if you wish to avoid certain death.</p>
</div>

If you come across anything along the way that we haven’t covered, or if you
know of a tip you think others would find handy, please [file an
issue](https://github.com/jameswalmsley/bitthunder/issues/new) and we’ll see about
including it on this site.
