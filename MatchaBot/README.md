Code for MatchaBot, which competed at the York University 2024 Robot Sumo Competition.

Similar in strategy to EspressoBot, even components are the same, but many bugs were fixed in relation to that code:
- Robot now takes noise readings into consideration for the Ultrasonic Sensor, solving issue of robot moving in an arc after detecting opponent. It now moves in a straight line.
- Robot now prioritizes attacking over not not losing, which means that if opponent in front, it keeps attacking even if going over a white line. This prevents losing in case the opponent has a white wedge.
- Code is more modular, easier to read, and better documented.

In March 2024, my teammate modeled and I coded the robot that got 1ST PLACE, by winning 15 out of 16 matches, at the annual York University Mini Robot Sumo Competition.

For this new version, we developed a strategy that evaded the opponent and attacked it from the side, ensuring victory 100% of the times we enabled it.

https://www.youtube.com/watch?v=AnpWhO1nUXE
