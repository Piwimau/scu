# Changelog

All notable changes to SCU will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and SCU generally adheres to [Semantic
Versioning](https://semver.org/spec/v2.0.0.html), though minor releases may
occasionally include breaking changes.

## [0.6.0] – 2026-10-20

### Changed

* Improve the interface of collection iterators
  ([25ecdd4](https://github.com/Piwimau/scu/commit/25ecdd4f32517dbd22a2ffcba486b799315e3239)).
* Rename `scu/common.h` to `scu/util.h`
  ([d89e23b](https://github.com/Piwimau/scu/commit/d89e23bf5241d6a148d27c9e4f3e8d7ee6dc1980)).
* Merge `scu/time.h` into `scu/bench.h`
  ([769481b](https://github.com/Piwimau/scu/commit/769481b721ce4ded9cfd83a12451320e98c6d788)).
* Clarify failure behavior of `scu_freopen()`
  ([31893fe](https://github.com/Piwimau/scu/commit/31893fedce8790d511300008f7c671271de1003f)).
* Relax `nullptr` precondition on `scu_hash_bytes()`
  ([99dd3dc](https://github.com/Piwimau/scu/commit/99dd3dc306750cf470bd215f5921bd89ce3f88b0)).
* Optimize scratch space allocation and bucket memory layout of the hash map and
  hash set
  ([fd607c8](https://github.com/Piwimau/scu/commit/fd607c89dad82dad5f9eb9f8de7d95cae2691e2d),
  [932b30c](https://github.com/Piwimau/scu/commit/932b30c8d7e8e7f515f057a37e96571d7cbcd6e7)).
* Improve `meson.build`
  ([753194e](https://github.com/Piwimau/scu/commit/753194e8e80948eefc410e4a94dd881c832513a3)).
* Rename collection deallocation functions to `*_delete()`
  ([20836fd](https://github.com/Piwimau/scu/commit/20836fdc5903fc1314a6a9c7c760545d705fdea7)).
* Rewrite the memory management module to improve support for custom allocators
  and over-aligned types
  ([0d622d9](https://github.com/Piwimau/scu/commit/0d622d90c515a312b513f8864edd2f333670a1ef)).

### Removed

* Remove the reverse overloads for comparison functions in `scu/compare.h`
  ([22b4331](https://github.com/Piwimau/scu/commit/22b4331658b66bae5717dcb31d2d4ec652b4f05f)).
* Remove pthreads dependency
  ([3aa5874](https://github.com/Piwimau/scu/commit/3aa58743011436043408748d50b08c35808aae22)).

### Fixed

* Fix possible out-of-bounds write in `scu_queue_clone()`
  ([b2bfb73](https://github.com/Piwimau/scu/commit/b2bfb73fa6563d8d7fcbd5fa3ee30d2a226c0755)).
* Fix incorrect return value calculation in `scu_strncat()`
  ([c3e691e](https://github.com/Piwimau/scu/commit/c3e691ea493c0c43a5baa53704b103c9609131f0)).
* Fix MinGW-specific bool type in `scu_process_cpu_ns()` and
  `scu_thread_cpu_ns()`
  ([cfcd4a9](https://github.com/Piwimau/scu/commit/cfcd4a93152021fd01c89d7118357ebed7183828)).
* Fix non-existent macro `UINTPTR_MIN` in `scu/types.h`
  ([b6f8963](https://github.com/Piwimau/scu/commit/b6f8963664cfca41bbc4aed3a59cdc8c48214e16)).
* Fix benchmark iterations counter on failure
  ([dd625ab](https://github.com/Piwimau/scu/commit/dd625abae379a24d07b5bcb8eb328635336a4f59)).
* Fix incorrect size constants in `scu_fread()` and `scu_fwrite()`
  ([2b5aee0](https://github.com/Piwimau/scu/commit/2b5aee03a138ad6d4d609160fae8f4730cb07e73)).
* Fix typos in `scu/bench.h` and `scu/memory.h`
  ([632b3a6](https://github.com/Piwimau/scu/commit/632b3a682979505761c59afc714184ad75a50eb1)).
* Fix a possible misalignment bug in `scu_hash_set_new_with_capacity()`
  ([fd22ad8](https://github.com/Piwimau/scu/commit/fd22ad8e32c2a3e36d5f05e96e86ecb72f3a4efc)).
* Fix the symbol visibility using a new `SCU_API` macro
  ([13dadaa](https://github.com/Piwimau/scu/commit/13dadaab9ad57515c4157bcf54c3c86a476fc96b)).

## [0.5.0] – 2026-10-14

### Changed

* Revise and simplify the documentation
  ([bdc3d77](https://github.com/Piwimau/scu/commit/bdc3d7796ea57843723308ce5fe4d6a1fdb24e21)).
* Simplify some pieces of code
  ([bdc3d77](https://github.com/Piwimau/scu/commit/bdc3d7796ea57843723308ce5fe4d6a1fdb24e21)).

## [0.4.1] – 2026-09-18

### Fixed

* Fix a possible resource leak in `scu_fopentmp()`
  ([e4694d2](https://github.com/Piwimau/scu/commit/e4694d24931249b868f46db3c2863efdc19b40c1)).

## [0.4.0] – 2026-08-21

### Fixed

* Fix parameters not declared as function pointers
  ([d1c15ac](https://github.com/Piwimau/scu/commit/d1c15ac09c788661fce88b49a3c9764f248ab89c)).

## [0.3.0] – 2026-06-29

### Changed

* Switch to [Meson](https://mesonbuild.com/) as the build system for improved
  performance and portability
  ([1653e8b](https://github.com/Piwimau/scu/commit/1653e8b85b7a87b5a9f8b8d89b8c4049be2058ac)).

## [0.2.0] – 2026-04-29

### Changed

* Change the type naming convention from `SCU` to `Scu` for consistency and
  readability (e.g., `SCUHashSet` becomes `ScuHashSet`)
  ([79ad007](https://github.com/Piwimau/scu/commit/79ad007ccb02dc4bc8199835546950f190854e2e)).

### Fixed

* Fix a few compilation warnings and possible errors
  ([ddac5ca](https://github.com/Piwimau/scu/commit/ddac5ca98ac012b4e0d963134247e0246259a96b)).

## [0.1.0] – 2026-03-21

The initial release of SCU.

[0.6.0]: https://github.com/Piwimau/scu/compare/v0.5.0...v0.6.0
[0.5.0]: https://github.com/Piwimau/scu/compare/v0.4.1...v0.5.0
[0.4.1]: https://github.com/Piwimau/scu/compare/v0.4.0...v0.4.1
[0.4.0]: https://github.com/Piwimau/scu/compare/v0.3.0...v0.4.0
[0.3.0]: https://github.com/Piwimau/scu/compare/v0.2.0...v0.3.0
[0.2.0]: https://github.com/Piwimau/scu/compare/v0.1.0...v0.2.0
[0.1.0]: https://github.com/Piwimau/scu/releases/tag/v0.1.0