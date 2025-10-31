Koto 4.5.7
<img align="right" width="120" height="120" src="doc/imgs/logo.png">
===========

What is Koto?
--------------

[Koto](https://ko-to.org/) is fork of [Zcash](https://z.cash/) which is an implementation of the "Zerocash" protocol.
Based on Bitcoin's code, Zcash intends to offer a far higher standard of privacy
through a sophisticated zero-knowledge proving scheme that preserves
confidentiality of transaction metadata. More technical details are available
in our [Protocol Specification](https://zips.z.cash/protocol/protocol.pdf).

This software is the Koto client. It downloads and stores the entire history
of Koto transactions; depending on the speed of your computer and network
connection, the synchronization process could take a day or more once the
blockchain has reached a significant size.

<!--
<p align="center">
  <img src="doc/imgs/zcashd_screen.gif" height="500">
</p>
-->

#### :lock: Security Warnings

See important security warnings on the
[Security Information page](https://z.cash/support/security/).

**Koto is experimental and a work-in-progress.** Use at your own risk.

####  :ledger: Deprecation Policy

This release is considered deprecated 16 weeks after the release day. There
is an automatic deprecation shutdown feature which will halt the node some
time after this 16-week period. The automatic feature is based on block
height.

## Getting Started

Please see our [user guide](https://github.com/KotoDevelopers/docs/wiki) for joining the main Koto network.

### Need Help?

* :blue_book: See the documentation at the [Koto Wiki](https://github.com/KotoDevelopers/docs/wiki)
  for help and more information.
* :incoming_envelope: Ask for help on the [Koto](https://discourse.ko-to.org/) forum.
* :speech_balloon: Join our community on [Discord](https://discord.gg/HmsTDww)

Participation in the Koto project is subject to a
[Code of Conduct](code_of_conduct.md).

### Building

Build Koto along with most dependencies from source by running the following command:

```
./zcutil/build.sh -j$(nproc)
```

License
-------

For license information see the file [COPYING](COPYING).
