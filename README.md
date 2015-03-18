Getting Started
======

1. We suppose you have purchased CSIM-20 and had it installed in your machine.
2. Go to your CSIM directory, you can find a file csim.gpp and a directory lib/, create links of csim.gpp and lib/ in the directory holding this README file.
3. Configure the system settings in config/sysSetting.xml.
4. Compile by calling "make" and call "./main" to generate the output.

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

