Getting Started
======

To simply our description, we refer the directory of CSIM-20 as "CSIM directory" and the directory containing this README file as "project directory".

1. We suppose you have purchased CSIM-20 and had it installed in your machine.
2. Go to CSIM directory, you can find a file csim.gpp and a directory lib/.  Create links of csim.gpp and lib/ in the project directory.
3. In the project directory, configure the system settings in config/sysSetting.xml.
4. In the project directory, compile by calling "make" and call "./main" to generate the output.  If you want to store the output of program, redirect the output to a target file by calling "./main 2> target file".

For Developers
======

Below are the major components of the simulator.  

 * `trafficManager`:  manages stripe (encode) stream and write stream.
 * `NodeTree`:        manages transmission resources (links) and (tree-structured) network topology.
 * `layoutGen`:       generated replicated block layouts with either EAR or conventional placement.
 * `graph`:           implementation of the flow graph algorithm in EAR.

You can go to src/ and find the source files of each component.

There is a doxygen-generated documentation in the directory html/.

      ---------------------------------------------
      |                trafficManager             |
      ---------------------------------------------
      |                      |     layoutGen      |
      |        NodeTree      |         -----------|
      |                      |         |  graph   |
      ---------------------------------------------

