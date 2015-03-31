Getting Started
======

Installation and Configuration
---

To simply our description, we refer the directory of CSIM-20 as "CSIM directory" and the directory containing this README file as "project directory".

1. We suppose you have purchased CSIM-20 and had it installed in your machine.
2. Go to CSIM directory, you can find a file csim.gpp and a directory lib/.  Create links of csim.gpp and lib/ in the project directory.
> ln -s *CSIM-directory*/csim.gpp *project-directory*/csim.gpp  
> ln -s *CSIM-directory*/lib/ *project-directory*/lib/

3. In the project directory, configure the system settings in config/sysSetting.xml.
4. In the project directory, compile by calling "make" and call "./main" to generate the output.  If you want to store the output of program, redirect the output to a target file by calling "./main 2> target file".

Output Interpretation
---
The output can be interpreted line by line, each line contains information of one operation.  
The format is:
> [operation code]: begins [starting time] ends [ending time]
For example, 
> stripeOp: begins 1175.538349 ends 1193.102391
indicates that one striping operation starts at 1175.5s and ends at 1193.1s.

There are three types of operations:

- stripeOp: striping operation which encodes replicated data into erasure coded format.
- inClusWriteOp: write operation issued by a DataNode.
- writeOp: write operation issued from outside the cluster.


For Developers
===

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

