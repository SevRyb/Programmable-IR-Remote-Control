# Programmable-IR-Remote-Control
An infrared programmable computer remote control device. The device is fully portable and powered by a lithium battery. The remote control can recognize control signals using common IR protocols such as NEC, RC5, RC6, SIRCS, SAMSUNG, SAMSUNG32 and more. Recognized control codes can be assigned to appropriate general-purpose buttons.

<p align="center">
  <img src="images/showcase/device-overview.png" width=50% height=50%>
</p>

## Architecture
### Structure diagram
<img src="images/diagrams/structure-diagram.svg">

### Function diagram
<img src="images/diagrams/function-diagram.svg">

### Circuit diagram
<img src="images/diagrams/circuit-diagram.svg">

## Hardware
### PCB
<table>
  <tr>
    <th>Top layer</th>
    <th>Bottom layer</th>
  </tr>
  <tr>
    <td><img src="images/pcb/pcb-top-layer.png"></td>
    <td><img src="images/pcb/pcb-bottom-layer.png"></td>
  </tr>
</table>

3D PCB renders from KiCad
<p align="center">
  <img src="images/pcb/pcb-renders.png">
</p>

Manufactured boards

<img src="images/pcb/pcb-manufactured.jpg"  width=55% height=55%>

## Usage
The device can be in 2 main operating modes and the following states.

<img src="images/diagrams/states-diagram.png" width=50% height=50%>

- Main mode:
  - Remote control ("Default" state).
- Configuration mode:
  - "Menu" state.
  - "Receiver" state
  - "Saves" state.

<table>
  <tr>
    <td><img src="images/showcase/device-state-default.jpg"></td>
    <td><img src="images/showcase/device-state-menu.jpg"></td>
    <td><img src="images/showcase/device-state-receiver.jpg"></td>
    <td><img src="images/showcase/device-state-saves.jpg"></td>
  </tr>
  <tr>
    <th>"Default"</th>
    <th>"Menu"</th>
    <th>"Receiver"</th>
    <th>"Saves"</th>
  </tr>
</table>

## Tests
