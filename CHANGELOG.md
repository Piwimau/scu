# Changelog

All notable changes to SCU will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and SCU adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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

[0.4.1]: https://github.com/Piwimau/scu/compare/v0.4.0...v0.4.1
[0.4.0]: https://github.com/Piwimau/scu/compare/v0.3.0...v0.4.0
[0.3.0]: https://github.com/Piwimau/scu/compare/v0.2.0...v0.3.0
[0.2.0]: https://github.com/Piwimau/scu/compare/v0.1.0...v0.2.0
[0.1.0]: https://github.com/Piwimau/scu/releases/tag/v0.1.0