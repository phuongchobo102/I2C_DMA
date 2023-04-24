# ELINKSWITCH 
<p align="center">
  <a href="https://example.com/">
    <img src="https://www.elinkgate.com/wp-content/uploads/2020/02/bg_section.png" alt="Elinkswitch" width=223 height=267>
  </a>

  <h3 align="center">Elinkswitch</h3>

  <p align="center">
    Program for stm32f070rbt6 
    <br>
  </p>
</p>


## Table of contents

- [ELINKSWITCH](#elinkswitch)
  - [Table of contents](#table-of-contents)
  - [Quick start](#quick-start)
  - [Status](#status)
  - [What's included](#whats-included)
  - [Bugs and feature requests](#bugs-and-feature-requests)
  - [Contributing](#contributing)
  - [Creators](#creators)
  - [Thanks](#thanks)
  - [Copyright and license](#copyright-and-license)


## Quick start

Hướng dẫn 
- Editor: IAR Embedded workbench for STM32
- Nạp chương trình 
- Kêt nối VGA, usb với KVM
- Sử dụng : dùng nút nhấn để chuyển, dùng kvm chuyển kênh kết nối 

## Status

Finished

## What's included

Projects structure

```text
stm32f070cb_elinkgate/
└── Core/
    ├── Inc/
    │   ├── main.h
    │   └── vga.h
    │   └── IS31FL3218.h
    │   └── usb_sw_selector.h
    │   └── system_switch.h 
    │   └── elinkswitch_gpio.h      
    └── Src/
    │   ├── main.c
    │   └── vga.c
    │   └── IS31FL3218.c
    │   └── usb_sw_selector.c
    │   └── system_switch.c
    │   └── elinkswitch_gpio.c
    │   └── stm32f0xx_hal_msp.c
    │   └── stm32f0xx_it.c
    └── Startup/
└── Driver/
└── EWARM/
└── MDK-ARM/
└── Middlewares/
└── USB_DEVICE/

```

## Bugs and feature requests

Have a bug or a feature request? Please first read the [issue guidelines](https://reponame/blob/master/CONTRIBUTING.md) and search for existing and closed issues. If your problem or idea is not addressed yet, [please open a new issue](https://reponame/issues/new).

## Contributing

Please read through our [contributing guidelines](https://reponame/blob/master/CONTRIBUTING.md). Included are directions for opening issues, coding standards, and notes on development.

Moreover, all HTML and CSS should conform to the [Code Guide](https://github.com/mdo/code-guide), maintained by [Main author](https://github.com/usernamemainauthor).

Editor preferences are available in the [editor config](https://reponame/blob/master/.editorconfig) for easy use in common text editors. Read more and download plugins at <https://editorconfig.org/>.

## Creators

**Creator 1**

- <https://github.com/usernamecreator1>

## Thanks

Some Text

## Copyright and license

Code and documentation copyright 2011-2018 the authors. Code released under the [MIT License](https://reponame/blob/master/LICENSE).

Enjoy :metal: