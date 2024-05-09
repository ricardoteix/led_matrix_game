[<img src="https://em-content.zobj.net/thumbs/160/openmoji/338/flag-brazil_1f1e7-1f1f7.png" alt="us flag" width="48"/>](./README.md)

# LED Matrix Game

![](assets/led_matrix_capa.jpg)

This repository contains the basic code for a project focused on teaching programming with Arduino.

The proposal is to create a small and simple game using Arduino and the 4 LED matrix module and the JoyStick shield.

The module uses the MAX7219 integrated circuit to control each matrix and thus simplify matrix control through serial communication via digital pins.

In this example, we use the [LedControl](https://wayoda.github.io/LedControl/pages/software.html) library to further simplify the code.

## Project Assembly

The image below shows the project assembly on a breadboard. For better visualization, consult the PDF [here](docs/led_matrix_schematic.pdf).

![Project](assets/led_matrix_bb.png)

## Joystick Shield Functionality

The [JoyStick shield](https://s.click.aliexpress.com/e/_DmHztbD) contains an analog joystick and 4 buttons. We will use these controls to move the elements on the display.

![](assets/Dual-Axis-Joystick-Shield-Pinout.jpg)

## Project Video

To learn more about the project, watch the video on YouTube (brazilian portuguese audio only).

[![LED Matrix Game](assets/yt_logo_rgb_light.jpg)](https://www.youtube.com/watch?v=BCVkyoe9aoI)

### Presentation Slides

For those who wish to access the slides used in the video, you can find them in this repository in the folder **./docs**. Here's the file: [led_matrix_slides](docs/led_matrix_slides.pdf)
