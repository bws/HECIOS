#ifndef PRIORITY_DISTRIBUTION_H
#define PRIORITY_DISTRIBUTION_H
/**
 * @file priority_distribution.h
 * @brief Priority Distribution Management
 *
 * Note: These classes are adapted from the FSS simulator project and are
 * licensed only under the GPL.  The FSS project is available at:
 * http://www.omnetpp.org/filemgmt/singlefile.php?lid=104 and
 * http://www.omnetpp.org/doc/FSS-doc/neddoc/index.html
*/

#include <omnetpp.h>
#include <stdio.h>
#include <string.h>

/**
 *  This class is designed to assist in managing an allocation of resources
 *  per priority.  That resource could be anything but in relations to 
 *  filesystems, it is focused on number of accesses per period, bandwidth
 *  allocated to a priority, or allocation of cache memory per priority.
 *
 *  As a template, the instantiating program has the option to 
 *  specify the structure used to manage each priority.
 *
 *  This class supports a variable number of priorities and can load
 *  the settings from an external file.  The format of the file
 *  is the priority followed by a floating point number.  The following
 *  is an example of a file used to load a cache distribution
 *  based upon splitting the cache evenly between accesses at priority 1
 *  and 3.
 *
@verbatim
1	0.50
3	0.50
@endverbatim
 *
 *  @todo The format of the files is currently too rigid.  The second
 *     element should be allowed to be of _ELEMENT_TYPE and the C++
 *     input operator used.  This would require rewriting the file
 *     loading code to use C++ style IO.
 */
template <class _ELEMENT_TYPE, int _MAX_PRIORITY>
class PriorityDistribution {
    /**
     *  This is the actual number of priorities in this particular
     *  instance of the class.
     */
    int maximum_priority;

    /**
     *  This is an array of the information required to manage each
     *  priority.
     */
    _ELEMENT_TYPE  allocation_table[ _MAX_PRIORITY + 1 ];
  public:
    // PriorityDistribution();
    // PriorityDistribution(const char *filename);
    // int initialize();
    // void setElement(int priority, _ELEMENT_TYPE value);
    // _ELEMENT_TYPE getElement(int priority);
    // int getMaximumPriority(void);
    // int loadFromFile(const char *filename);

    /**
     *  This is the constructor for a PriorityDistribution instance.
     */
    PriorityDistribution(void) : maximum_priority(_MAX_PRIORITY)
    {
      initialize();
    }

    /**
     *  This is the constructor for a PriorityDistribution instance.
     *
     *  @param filename (in) is the name of the file that will be used
     *     to load the initial values of this instance.
     */
    PriorityDistribution(char *filename) : maximum_priority(_MAX_PRIORITY)
    {
      initialize();
      loadFromFile(filename);
    }

    /**
     *  This method initializes the per priority management information to
     *  all zeros.
     */
    void initialize()
    {
      memset( allocation_table, 0, sizeof(allocation_table) );
    }

    /**
     *  This method is used to obtain the number of priorities supported
     *  by this instance.
     *
     *  @return This method returns the number of priorities supported.
     *
     */
    int getMaximumPriority(void)
    {
      return maximum_priority;
    }

    /**
     *  This method sets the priority management information for
     *  @a priority to @a value.
     *
     *  @param priority (in) specifies the priority for which the information
     *     will be changed.
     *  @param value (in) specifies the new priority management information
     *     for this @a priority.
     */
    void setElement( int priority, _ELEMENT_TYPE value )
    {
      if ( priority <= maximum_priority )
        allocation_table[ priority ] = value;
    }

    /**
     *  This method is used to obtain the current value of the priority
     *  management information for the specified @a priority.
     *
     *  @param priority (in) specifies the priority for which the information
     *     is to be obtained.
     *
     *  @return This method returns the current value of the priority
     *     management information for this priority.
     */
    _ELEMENT_TYPE getElement( int priority )
    {
      if ( priority <= maximum_priority )
        return allocation_table[ priority ];
      return 0;
    }

    /**
     *  This method 
     */
    int loadFromFile(const char *filename)
    {
      FILE *in;
      char  line[128];
      int   current_line;
      int   num_read;
      long  priority;
      float allocation;

      ev << "PriorityDistribution: Loading from " << filename << endl;

      in = fopen( filename, "r" );
      if (!in) {
        ev << "PriorityDistribution: Can not open " << filename << endl;
        return -1;
      }

      current_line = 0;

      while (1) {
        do {
          if ( !fgets( line, sizeof(line), in ) ) {
            goto done;
          }

          current_line++;
          num_read = strlen( line );
          if ( num_read < 2 )
            continue;
          if ( line[0] != '/' || line[1] != '/' )
            break;
        } while ( 1 );

        num_read = sscanf( line, "%ld %f\n", &priority, &allocation );

        ev << "PriorityDistribution: " << priority << ", " << allocation  << endl;

        if ( num_read == -1 ) {
          goto bad_done;
        }

        if ( num_read != 2 ) {
          ev << "PriorityDistribution: Unknown error at line " <<
               current_line << " of file " << filename << endl;
          ev << "Line is:" << endl;
          ev << line << endl;
          goto bad_done;
        }

        if ( priority > maximum_priority ) {
          ev << "PriorityDistribution: Illegal priority at line " <<
               current_line << " of file " << filename << endl;
          goto bad_done;
        }
        if ( allocation_table[priority] ) {
          ev << "PriorityDistribution: Error at line " <<
               current_line << " of file " << filename << endl;
          ev << "Allocation for " << priority << " is already specified" << endl;
          goto bad_done;
        }
        allocation_table[priority] = (_ELEMENT_TYPE) allocation;
      }

    done:
      //  check that the array is proper (adds up right)
      fclose( in );
      return 0;

    bad_done:
      fclose( in );
      return -1;
    }
};

#endif
