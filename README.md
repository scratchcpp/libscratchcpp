<!-- Improved compatibility of back to top link: See: https://github.com/othneildrew/Best-README-Template/pull/73 -->
<a name="readme-top"></a>

<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]



<!-- PROJECT LOGO -->
<br />
<div align="center">
  <!-- TODO: Add logo: <a href="https://github.com/scratchcpp/libscratchcpp">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a> -->

<h3 align="center">libscratchcpp</h3>

  <p align="center">
    A library for C++ based Scratch project players
    <br />
    <!-- TODO: Add link to documentation: <a href="https://github.com/scratchcpp/libscratchcpp"><strong>Explore the docs »</strong></a>
    <br /> -->
    <br />
    <a href="https://github.com/scratchcpp/libscratchcpp/issues">Report Bug</a>
    ·
    <a href="https://github.com/scratchcpp/libscratchcpp/issues">Request Feature</a>
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

**libscratchcpp** is a library for building C++ based Scratch project players.

It provides an API for reading and running Scratch projects which makes it easy
to create a GUI application that runs Scratch projects.

The idea is to implement Scratch blocks in C++ to build a fast project player
that is faster than Scratch modifications such as TurboWarp.

## NOTE
This library is still in development and it shouldn't be used
to build complete Scratch project players yet.

**There might be incompatible API changes anytime before version 1.0.0 releases!**

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- GETTING STARTED -->
## Getting Started

We're working on the documentation, it'll be available soon.
<!-- TODO: Add link to documentation -->

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- USAGE EXAMPLES -->
## Usage

A minimal CLI program for running Scratch projects:
```cpp
#include <scratchproject.h>

int main(int argc, char **argv) {
  libscratchcpp::ScratchProject p("/path/to/project.sb3");
  bool ret = p.load();
  if (!ret)
    return 1;

  p.run();
  return 0;
}
```

<!-- TODO: Add link to documentation -->

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ROADMAP -->
## Roadmap

- [ ] Motion blocks
- [ ] Looks blocks
- [ ] Sound blocks
- [ ] Event blocks
- [ ] Control blocks
- [ ] Sensing blocks
- [ ] Operator blocks
- [ ] Variables blocks
- [ ] Custom blocks
- [ ] Reporter blocks
- [ ] Broadcasts
- [x] Variables (monitors are not implemented yet)
- [ ] Lists
- [ ] Timer
- [ ] API for monitors
- [ ] Project metadata
- [ ] Turbo mode
- [ ] Multithreading (experimental)
- [ ] Custom FPS in the default event loop (`libscratchcpp::Engine::run()`)
- [ ] Scratch 2.0 to 3.0 converter (help needed)
- [ ] Scratch 1.4 and below to 3.0 converter (help needed)
- [ ] API for comments
- [ ] API for loading projects from URL

See the [open issues](https://github.com/scratchcpp/libscratchcpp/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Format changed source files (`clang-format src/some_file.cpp`)
4. Commit your Changes (`git add -A && git commit -m 'Add some AmazingFeature'`)
5. Push to the Branch (`git push origin feature/AmazingFeature`)
6. Open a Pull Request

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- LICENSE -->
## License

Distributed under the Apache-2.0 License. See [LICENSE](LICENSE) for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/scratchcpp/libscratchcpp.svg?style=for-the-badge
[contributors-url]: https://github.com/scratchcpp/libscratchcpp/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/scratchcpp/libscratchcpp.svg?style=for-the-badge
[forks-url]: https://github.com/scratchcpp/libscratchcpp/network/members
[stars-shield]: https://img.shields.io/github/stars/scratchcpp/libscratchcpp.svg?style=for-the-badge
[stars-url]: https://github.com/scratchcpp/libscratchcpp/stargazers
[issues-shield]: https://img.shields.io/github/issues/scratchcpp/libscratchcpp.svg?style=for-the-badge
[issues-url]: https://github.com/scratchcpp/libscratchcpp/issues
[license-shield]: https://img.shields.io/github/license/scratchcpp/libscratchcpp.svg?style=for-the-badge
[license-url]: https://github.com/scratchcpp/libscratchcpp/blob/master/LICENSE
