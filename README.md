Getting Started
======

Installation and Configuration
---

To simplify our description, we refer the directory of CSIM-20 as "CSIM
directory" and the directory containing this README file as "project
directory".

1. We assume that you have purchased CSIM-20 and had it installed in your
   machine.

2. Go to CSIM directory, you can find a file csim.gpp and a directory lib/.
   Create links of csim.gpp and lib/ in the project directory.
> ln -s *CSIM-directory*/csim.gpp *project-directory*/csim.gpp  
> ln -s *CSIM-directory*/lib/ *project-directory*/lib/

3. In the project directory, configure the system settings in
   config/sysSetting.xml.

4. In the project directory, compile by "make" and call "./main" to generate
   the output.  If you want to store the output of program, redirect the output
   to a target file by calling "./main 2> target file".

Explanation of Output
---
The output can be interpreted line-by-line.  Each line contains information of
one operation.  The format is:
> [operation code]: begins [starting time] ends [ending time]  

For example, 
> stripeOp: begins 1175.538349 ends 1193.102391  

indicates that one striping operation starts at 1175.5s and ends at 1193.1s.

There are three types of operations:

- encodeOp: the encoding operation that transforms the replicated data of one
stripe into erasure coded format.
- inClusWriteOp: the write operation issued by a DataNode.
- writeOp: the write operation issued from a client located outside the
  cluster.

For Developers
===

Below are the major modules of the simulator.  

 * `TrafficManager`:  manages stripe (encode) stream and write stream.
 * `Topology`:        manages transmission resources (links) and (tree-structured) network topology.
 * `PlacementManager`:generated replicated block layouts with either EAR or conventional placement.
 * `FlowGraph`:       implementation of the flow graph algorithm in EAR.

You can go to src/ and find the source files of each component.

There is a doxygen-generated documentation in the directory html/.

      ---------------------------------------------
      |                TrafficManager             |
      ---------------------------------------------
      |                      |  PlacementManager  |
      |        Topology      |        ------------|
      |                      |        | FlowGraph |
      ---------------------------------------------

