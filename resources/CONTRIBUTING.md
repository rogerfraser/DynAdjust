# Contributing Guidelines

## Contents

- [Contributing Guidelines](#contributing-guidelines)
  - [Contents](#contents)
  - [Preamble](#preamble)
  - [How to Contribute](#how-to-contribute)
    - [Getting Started](#getting-started)
    - [Raising and Addressing an Issue](#raising-and-addressing-an-issue)
    - [GitHub Workflow](#github-workflow)
    - [General Discussion](#general-discussion)
    - [Private Reports](#private-reports)
  - [Core Functionality, Algorithms and Formulae](#core-functionality-algorithms-and-formulae)
  - [Coding Conventions and Guidelines](#coding-conventions-and-guidelines)
    - [Programming Language](#programming-language)
    - [Coding Style](#coding-style)
    - [Cross Platform Compatibility](#cross-platform-compatibility)
    - [Folder and File Structure](#folder-and-file-structure)
    - [Best Practice](#best-practice)
    - [Release Schedule](#release-schedule)
  - [Automated Test Suite](#automated-test-suite)
    - [Continuous Integration](#continuous-integration)
    - [Test Code Coverage](#test-code-coverage)
    - [Static Code Analysis](#static-code-analysis)

## Preamble

Thank you for considering making a contribution to DynAdjust.

DynAdjust forms an integral component of public geospatial infrastructure and commercial software. It is used for the establishment and routine maintenance of Australia's Geospatial Reference System, the maintenance of International Terrestrial Reference Frame local ties, the computation of digital cadastre (or land parcel) fabrics, and a diverse array of engineering and geospatial projects. For these reasons, great emphasis is placed upon ensuring the DynAdjust code base is maintained using high quality coding standards.

The purpose of this page is to provide contributing guidelines to help maintain a high quality code base and foster a positive and collaborative development culture.

## How to Contribute

### Getting Started

To make any changes to the DynAdjust code base, suggest new features, or improve the documentation using GitHub, you will need a [GitHub account](https://github.com/signup/free).

Please familiarise yourself with [GitHub's pull request process](https://docs.github.com/en/github/collaborating-with-issues-and-pull-requests/proposing-changes-to-your-work-with-pull-requests/about-pull-requests). Another useful resource is Aaron Meurer's [tutorial](https://www.asmeurer.com/git-workflow/) on the git workflow.

If you plan to make changes to the code base, please ensure you have read the [installation instructions](./INSTALLING.md) and have obtained all the essential prerequisites.

### Raising and Addressing an Issue

[![GitHub Issues](https://img.shields.io/github/issues/icsm-au/DynAdjust.svg)](https://github.com/icsm-au/DynAdjust/issues)

The general process for addressing issues in DynAdjust is as follows:

1. **Search for Existing Issues**  
   Search the [issue tracker](https://github.com/icsm-au/DynAdjust/issues) for any occurrence of the issue you would like to address.
   - If it is closely related to an existing open issue, add a comment to the open issue rather than creating a new one.
   - If the issue was previously raised but is now closed, remove the `is:open` filter to search through [all issues](https://github.com/icsm-au/DynAdjust/issues?q=is%3Aissue). If you feel the issue persists, reopen it with additional comments.
   - If the issue has not been raised before, proceed to the next step.

2. **Create or Reopen an Issue**  
   - **Defects:** Provide reproduction steps, sample command line arguments, and snippets of output demonstrating the defect. Assign an appropriate [label](https://github.com/icsm-au/DynAdjust/issues/labels).
   - **Enhancements:** Describe the desired or expected behaviour not currently provided. Cite any relevant technical documents and, wherever possible, include test cases.
   - **Compilation Issues:** Include details of your operating system, compiler version, and the relevant error or warning message.
   - **Feedback:** Share your thoughts on improving DynAdjust by suggesting new functions, alternative algorithms, documentation updates, or even sample data.

3. **Refer to the GitHub Workflow**  
   For making code contributions, please follow the GitHub Workflow guidelines detailed below.

### GitHub Workflow

DynAdjust adopts a structured and collaborative Git workflow to ensure clarity, quality, and maintainability of the codebase. All contributors are expected to adhere to the following practices:

#### Forking and Branching

- **Forking:** All contributors (internal and external) should fork the official repository into their GitHub account.
- **Branching:** Create a new branch for each specific feature, issue, or enhancement.
  - **Long-term Branches on the Official Repository:**
    - `main`: Always contains stable, production-ready code.
    - **Version Branches (e.g., `1.2.0`):** Contain code for the upcoming release and integration of new features.

#### Pull Request (PR) Reviews

- **Submission:** All contributions must be submitted via a Pull Request (PR) to the latest version branch (e.g. `1.2.0`).
- **Focus:** Each PR should address one single feature, issue, or enhancement.
- **Review Process:** Every PR must undergo a code review process and receive approval from at least one designated reviewer before merging.
- **Merge Strategy:** PRs are merged using squash merges to maintain a concise history, resulting in one commit per merged PR.

#### Continuous Integration and Automated Checks

- **Automated Checks:** All PRs must pass automated continuous integration (CI) checks before approval. These checks include:
  - **Build & Test Pipelines:** Automatically build the DynAdjust C++ project and run the complete test suite.
  - **Static Analysis and Linters:** Run tools such as clang-tidy and cppcheck to detect potential issues.
  - **Coding Standards Enforcement:** Use automated clang-format checks to ensure code style compliance. Code that does not pass must be reformatted before merging.
- **Responsibility:** Contributors are expected to address any CI pipeline failures promptly. Only PRs that pass all checks are eligible for review and merging.

### General Discussion

Most discussions happen in the [issue tracker](https://github.com/icsm-au/DynAdjust/issues) or within pull requests. For general questions, please feel free to post on the repository's [discussion page](https://github.com/icsm-au/DynAdjust/discussions).

We encourage language that is objective, professional, respectful, and considerate of the diverse user and developer audience. DynAdjust’s community includes theoretical geodesists, GIS users, software developers, and senior experts. Please model positive communication behaviours.

### Private Reports

While we encourage all issues, defects, enhancements, and queries to be tracked publicly via the [issue tracker](https://github.com/icsm-au/DynAdjust/issues), if you need to submit a private report or request, please contact [geodesy@ga.gov.au](mailto:geodesy@ga.gov.au).

## Core Functionality, Algorithms and Formulae

[![Documentation (User's Guide)](https://img.shields.io/badge/docs-usersguide-red.svg)](https://github.com/icsm-au/DynAdjust/raw/master/resources/DynAdjust%20Users%20Guide.pdf)

DynAdjust implements a wide range of specialist geodetic and surveying algorithms and formulae. These have been sourced from journal articles, reference texts, published standards, and other peer-reviewed publications. All functionality, algorithms, and formulae are documented in the [DynAdjust User's Guide](https://github.com/icsm-au/DynAdjust/blob/master/resources/DynAdjust%20Users%20Guide.pdf) and are appropriately cited with full bibliographic references. When proposing a new feature that implements an undocumented algorithm or formula, please provide its full reference.

A DynAdjust Steering Committee meets periodically to review the functionality and discuss potential enhancements. If you would like the Steering Committee to consider your suggestions, please contact [geodesy@ga.gov.au](mailto:geodesy@ga.gov.au).

## Coding Conventions and Guidelines

### Programming Language

DynAdjust development has evolved over 14 years from C++98 to C++14, with heavy reliance upon the Standard Template Library (STL). Wherever possible, [Boost](https://www.boost.org/) is used to incorporate portable libraries that are not available in the C++ Standard. While some Boost libraries have become part of the C++ Standard, current development is focused on C++14. Legacy portions of the code base written in C are being gradually converted to C++.

### Coding Style

DynAdjust has been in development since late 2008, and the coding style has evolved over time. Different projects and functions may exhibit different styles; please endeavour to conform to the style of the surrounding code.

### Cross Platform Compatibility

Please write code that is compatible with a wide range of platforms (e.g. UNIX, Linux, Windows, Apple) rather than relying on system-specific libraries or functions. [Boost](https://www.boost.org/) offers many cross-platform solutions. In cases where system-specific code is unavoidable, use conditional compilation (e.g. `#if defined`) to ensure portability.

### Folder and File Structure

- **DynAdjust-Specific Code:** Located in the [`dynadjust/dynadjust/`](https://github.com/icsm-au/DynAdjust/tree/master/dynadjust/dynadjust) folder. Projects with names ending in `...wrapper` are command-line interface programs, while others are libraries.
- **General Code:** Located in the [`dynadjust/include/`](https://github.com/icsm-au/DynAdjust/tree/master/dynadjust/include) folder.
- **Documentation and Build Scripts:** Located in the [`resources/`](https://github.com/icsm-au/DynAdjust/tree/master/resources) folder.
- **Sample Data:** Located in the [`sampleData/`](https://github.com/icsm-au/DynAdjust/tree/master/sampleData) folder.

### Best Practice

Consistent, high quality coding standards are essential for delivering robust software. The coding standards and best practices for DynAdjust are inspired by various industry-respected sources, including (in alphabetical order):

- Josuttis, N. M. (1999). *The C++ Standard Library - A Tutorial and Reference*. 1st edition. Addison-Wesley.
- Lakos, J. (1996). *Large-Scale C++ Software Design*. Pearson Education Limited.
- Meyers, S. (2001). *Effective STL: 50 Specific Ways to Improve Your Use of the Standard Template Library*. Addison-Wesley Longman Ltd.
- Meyers, S. (2005). *Effective C++: 55 Specific Ways to Improve Your Programs and Designs*. 3rd edition. Addison-Wesley Professional.
- Meyers, S. (2014). *Effective Modern C++: 42 Specific Ways to Improve Your Use of C++11 and C++14*. O'Reilly Media, Inc.
- Schäling, B. (2014). *The Boost C++ Libraries*. XML Press. Available online: [https://boost.org](https://boost.org).
- Stroustrup, B. (2013). *C++ Programming Language*. 4th edition. Addison Wesley.
- Williams, A. (2012). *C++ Concurrency in Action: Practical Multithreading*. 1st edition. Manning Publications.

### Release Schedule

[![GitHub Releases](https://img.shields.io/github/v/release/icsm-au/DynAdjust.svg)](https://github.com/icsm-au/DynAdjust/releases)

Although code changes occur frequently, we aim to deliver new [releases](https://github.com/icsm-au/DynAdjust/releases) on a regular basis. The current release schedule cadence is six months.

## Automated Test Suite

The [`make_dynadjust_gcc.sh`](https://github.com/icsm-au/DynAdjust/blob/master/resources/make_dynadjust_gcc.sh)
script provides a method to build the code and run tests locally and on GitHub via our continuous integration (CI) system.

With every commit and pull request, the project is tested using a unit test suite managed by CMake (via [`CMakeLists.txt`](https://github.com/icsm-au/DynAdjust/blob/master/dynadjust/CMakeLists.txt)).

### Continuous Integration

[![cmake tests](https://img.shields.io/github/workflow/status/icsm-au/dynadjust/Build,%20test%20and%20code%20coverage?label=cmake%20tests)](https://github.com/icsm-au/DynAdjust/actions/workflows/test_coverage.yml)

DynAdjust utilises an automated test suite managed by GitHub Actions and [Travis](https://travis-ci.org/github/icsm-au/DynAdjust) to build and test code changes. The CI system is configured to run builds for Linux using the [`test_coverage.yml`](https://github.com/icsm-au/DynAdjust/actions/workflows/test_coverage.yml) script.

### Test Code Coverage

[![coveralls status](https://img.shields.io/coveralls/github/icsm-au/DynAdjust)](https://coveralls.io/github/icsm-au/DynAdjust)

To ensure that the test suite covers as much of the code base as possible, code coverage analysis is executed on every commit and pull request. The code coverage tool used is [coveralls.io](https://coveralls.io/github/icsm-au/DynAdjust).

### Static Code Analysis

[![codecov status](https://img.shields.io/codecov/c/github/icsm-au/dynadjust)](https://codecov.io/gh/icsm-au/DynAdjust)
[![codacy badge](https://img.shields.io/codacy/grade/a3944cda0c72445f8a13b1f82b64f714)](https://app.codacy.com/gh/icsm-au/DynAdjust/dashboard)

With every commit and pull request, static code analysis is performed using [Codecov](https://app.codecov.io/gh/icsm-au/DynAdjust) and [Codacy](https://app.codacy.com/gh/icsm-au/DynAdjust/dashboard). The code base is under continual review to address any issues identified.
