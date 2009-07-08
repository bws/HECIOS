Build Notes:
  In order to build the simulator you will need OmNet++ (we use V3.3) with
all of its required support packages (TCL, etc.).  To build the simulator,
simply type:

./configure --with-omnet=<path to omnet directory>
make

To build the unit test drivers use:

make test

Documentation Notes:
  Documentation is stored in the doc directory, and the UML directory.  In order to build the documentation, simply type make doc at the top level.  To view the UML diagrams, you will need to use the tool ArgoUML available at http://argouml.tigris.org


Integration Notes:
  This simulator utilizes the OmNet++ event framework and the contributed network package: the INET framework.  A compatible snapshot of the inet framework is stored with this simulator under source control.  

  Current integrated INET version:  INET-20060912a

Licensing information:
  Because this software depends upon the OmNet++ simulation environment, we
accordingly use the Academic Public License.  The license is similar to the
GPL in that anyone using this version of OmNet++ must distribute the source
along with any provided executables.  If you want another license, simply
contact me (bradles@parl.clemson.edu), and I may be able to help you get
the licensing terms you prefer.
