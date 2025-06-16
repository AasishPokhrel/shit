# Tests

This folder groups all files needed to run the test suite.

## Hierarchy

- **[`data/`](data/)**: Data files needed to perform tests.
- **[`unit/`](unit/)**: Unit test files designed to fully test a single specific file in the [src](../src/) folder at the root of the repository. The folder structure and file naming should be identical in both folders.

Files that aren't in any folder are part of the legacy test suite.

Test suites which perform coverage differently (e. g. integration tests, running the game from the point of view of the user, etc) should be located in their own folder within `test/`.
