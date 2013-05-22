Pathfinder
==========

An arduino 0023 sketch for the http://www.dfrobot.com/index.php?route=product/product&product_id=487 RGB ring and LSM303DLH compass module.

The program accepts serial commands starting with ! and ending with * to frame a packet, with commands comma seperated.

The only command implemented so far is the DIR command, which takes a list of directions, from 1-5 arguments and outputs to a LED accordng to a difference in the compasses current heading to the desired pointer direction.




Web-GUI
==========
The web interface module is based on the turn-by-turn navigation system from https://github.com/bsmulders/PoorMansGPS

The page is provided by a node.js server, which uses socket.io and serialport to pipe directions to the Arduino module.

You'll need to set the serial port the node.js server will connect through in the config.ini file.

Start the server with "node socket.js" from the command line and naviagate to http://localhost:8000/index.html

Once you have set a route to go to, you can click around the map and simulate the compass following the path.
