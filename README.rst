================================
ddplus - update systems simbly
================================
HanishKVC, GPL

Overview
===========

Allows one to copy disk contents from one disk to the other,
in a controlled way. It is a linux based program.

Beware that one specifies the disks using name given by vendor as prefix.
Translated Offsets are what one uses to decide on what and to where.

One can tie its usage to availability of a specific disk.
Which inturn could be the update disk itself or a seperate disk
(which acts like a key disk). This is especially useful if wwid is
available and usable (in the target environment) for the disks.

It obfuscates the data that needs to be transfered. And is implemented
as a simple reversible operation, so that same logic can be used for both
creating the update disk as well as for doing the actual update.

The obfuscation is tied to the disk id and few user specified tokens.
It provides a basic level of obfuscation, which is good enough for simple
uses, but is not necessarily fool proof, if one puts enough effort.
By adding hashing to the key during data operation, it can be made more
secure, in a relatively simple way. The key used keeps evolving as it
goes through the data, so the same key is not used for more than once.

This ensures that just duplicating the update disk in itself is not
sufficient. And also ensures that the update is done in a controlled way,
and not randomly (knowingly or unknowingly) by anyone.

Misc
======

One can also use the devpath while testing to remap to temp files/...
instead of directly accessing the devices in ones system. Also provides
a thin layer of protection from unknowingly destroying ones disk data,
while testing or preparing things.

