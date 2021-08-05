/*! \file triangle.hpp

    \brief
    Piece of Art Mesh Generation Tool, 
    Winner of the 2003 James Hardy Wilkinson Prize in Numerical Software.                                                     

    \details                                                
    Include file for programs that call Triangle.                            

    \version Accompanies Triangle Version 1.6                                         
    \date July 28, 2005                                                            

    \copyright 1996, 2005                                                     
    \author Jonathan Richard Shewchuk                                                
    2360 Woolsey \#H                                                          
    Berkeley, California  94705-1927                                         
    jrs@cs.berkeley.edu                                                      
*/

///\cond
typedef long double longdouble;
#define REAL longdouble

#include <vector>
///\endcond

#include "point.hpp"

/*!
    \brief
    Used to pass data into and out of the triangulate() procedure.           

    \details
    Arrays are used to store points, triangles, markers, and so forth.  In   
    all cases, the first item in any array is stored starting at index [0].  
    However, that item is item number `1' unless the `z' switch is used, in  
    which case it is item number `0'.  Hence, you may find it easier to      
    index points (and triangles in the neighbor list) if you use the `z'     
    switch.  Unless, of course, you're calling Triangle from a Fortran       
    program.                                                                 

    Description of fields (except the "numberof" fields, which are obvious): 

    `pointlist':  An array of point coordinates.  The first point's x        
      coordinate is at index [0] and its y coordinate at index [1], followed 
      by the coordinates of the remaining points.  Each point occupies two   
      REALs.                                                                 
    `pointattributelist':  An array of point attributes.  Each point's       
      attributes occupy `numberofpointattributes' REALs.                     
    `pointmarkerlist':  An array of point markers; one int per point.        

    `trianglelist':  An array of triangle corners.  The first triangle's     
      first corner is at index [0], followed by its other two corners in     
      counterclockwise order, followed by any other nodes if the triangle    
      represents a nonlinear element.  Each triangle occupies                
      `numberofcorners' ints.                                                
    `triangleattributelist':  An array of triangle attributes.  Each         
      triangle's attributes occupy `numberoftriangleattributes' REALs.       
    `trianglearealist':  An array of triangle area constraints; one REAL per 
      triangle.  Input only.                                                 
    `neighborlist':  An array of triangle neighbors; three ints per          
      triangle.  Output only.                                                

    `segmentlist':  An array of segment endpoints.  The first segment's      
      endpoints are at indices [0] and [1], followed by the remaining        
      segments.  Two ints per segment.                                       
    `segmentmarkerlist':  An array of segment markers; one int per segment.  

    `holelist':  An array of holes.  The first hole's x and y coordinates    
      are at indices [0] and [1], followed by the remaining holes.  Two      
      REALs per hole.  Input only, although the pointer is copied to the     
      output structure for your convenience.                                 

    `regionlist':  An array of regional attributes and area constraints.     
      The first constraint's x and y coordinates are at indices [0] and [1], 
      followed by the regional attribute at index [2], followed by the       
      maximum area at index [3], followed by the remaining area constraints. 
      Four REALs per area constraint.  Note that each regional attribute is  
      used only if you select the `A' switch, and each area constraint is    
      used only if you select the `a' switch (with no number following), but 
      omitting one of these switches does not change the memory layout.      
      Input only, although the pointer is copied to the output structure for 
      your convenience.                                                      

    `edgelist':  An array of edge endpoints.  The first edge's endpoints are 
      at indices [0] and [1], followed by the remaining edges.  Two ints per 
      edge.  Output only.                                                    
    `edgemarkerlist':  An array of edge markers; one int per edge.  Output   
      only.                                                                  
    `normlist':  An array of normal vectors, used for infinite rays in       
      Voronoi diagrams.  The first normal vector's x and y magnitudes are    
      at indices [0] and [1], followed by the remaining vectors.  For each   
      finite edge in a Voronoi diagram, the normal vector written is the     
      zero vector.  Two REALs per edge.  Output only.                        


    Any input fields that Triangle will examine must be initialized.         
    Furthermore, for each output array that Triangle will write to, you      
    must either provide space by setting the appropriate pointer to point    
    to the space you want the data written to, or you must initialize the    
    pointer to NULL, which tells Triangle to allocate space for the results. 
    The latter option is preferable, because Triangle always knows exactly   
    how much space to allocate.  The former option is provided mainly for    
    people who need to call Triangle from Fortran code, though it also makes 
    possible some nasty space-saving tricks, like writing the output to the  
    same arrays as the input.                                                

    Triangle will not free() any input or output arrays, including those it  
    allocates itself; that's up to you.  You should free arrays allocated by 
    Triangle by calling the trifree() procedure defined below.  (By default, 
    trifree() just calls the standard free() library procedure, but          
    applications that call triangulate() may replace trimalloc() and         
    trifree() in triangle.c to use specialized memory allocators.)           

    Here's a guide to help you decide which fields you must initialize       
    before you call triangulate().                                           

    `in':                                                                    

      - `pointlist' must always point to a list of points; `numberofpoints'  
        and `numberofpointattributes' must be properly set.                  
        `pointmarkerlist' must either be set to NULL (in which case all      
        markers default to zero), or must point to a list of markers.  If    
        `numberofpointattributes' is not zero, `pointattributelist' must     
        point to a list of point attributes.                                 
      - If the `r' switch is used, `trianglelist' must point to a list of    
        triangles, and `numberoftriangles', `numberofcorners', and           
        `numberoftriangleattributes' must be properly set.  If               
        `numberoftriangleattributes' is not zero, `triangleattributelist'    
        must point to a list of triangle attributes.  If the `a' switch is   
        used (with no number following), `trianglearealist' must point to a  
        list of triangle area constraints.  `neighborlist' may be ignored.   
      - If the `p' switch is used, `segmentlist' must point to a list of     
        segments, `numberofsegments' must be properly set, and               
        `segmentmarkerlist' must either be set to NULL (in which case all    
        markers default to zero), or must point to a list of markers.        
      - If the `p' switch is used without the `r' switch, then               
        `numberofholes' and `numberofregions' must be properly set.  If      
        `numberofholes' is not zero, `holelist' must point to a list of      
        holes.  If `numberofregions' is not zero, `regionlist' must point to 
        a list of region constraints.                                        
      - If the `p' switch is used, `holelist', `numberofholes',              
        `regionlist', and `numberofregions' is copied to `out'.  (You can    
        nonetheless get away with not initializing them if the `r' switch is 
        used.)                                                               
      - `edgelist', `edgemarkerlist', `normlist', and `numberofedges' may be 
        ignored.                                                             

    `out':                                                                   

      - `pointlist' must be initialized (NULL or pointing to memory) unless  
        the `N' switch is used.  `pointmarkerlist' must be initialized       
        unless the `N' or `B' switch is used.  If `N' is not used and        
        `in->numberofpointattributes' is not zero, `pointattributelist' must 
        be initialized.                                                      
      - `trianglelist' must be initialized unless the `E' switch is used.    
        `neighborlist' must be initialized if the `n' switch is used.  If    
        the `E' switch is not used and (`in->numberofelementattributes' is   
        not zero or the `A' switch is used), `elementattributelist' must be  
        initialized.  `trianglearealist' may be ignored.                     
      - `segmentlist' must be initialized if the `p' or `c' switch is used,  
        and the `P' switch is not used.  `segmentmarkerlist' must also be    
        initialized under these circumstances unless the `B' switch is used. 
      - `edgelist' must be initialized if the `e' switch is used.            
        `edgemarkerlist' must be initialized if the `e' switch is used and   
        the `B' switch is not.                                               
      - `holelist', `regionlist', `normlist', and all scalars may be ignored.

    `vorout' (only needed if `v' switch is used):                            

      - `pointlist' must be initialized.  If `in->numberofpointattributes'   
        is not zero, `pointattributelist' must be initialized.               
        `pointmarkerlist' may be ignored.                                    
      - `edgelist' and `normlist' must both be initialized.                  
        `edgemarkerlist' may be ignored.                                     
      - Everything else may be ignored.                                      

    After a call to triangulate(), the valid fields of `out' and `vorout'    
    will depend, in an obvious way, on the choice of switches used.  Note    
    that when the `p' switch is used, the pointers `holelist' and            
    `regionlist' are copied from `in' to `out', but no new space is          
    allocated; be careful that you don't free() the same array twice.  On    
    the other hand, Triangle will never copy the `pointlist' pointer (or any 
    others); new space is allocated for `out->pointlist', or if the `N'      
    switch is used, `out->pointlist' remains uninitialized.                  

    All of the meaningful `numberof' fields will be properly set; for        
    instance, `numberofedges' will represent the number of edges in the      
    triangulation whether or not the edges were written.  If segments are    
    not used, `numberofsegments' will indicate the number of boundary edges.                                                                          
 */
struct triangulateio
{
    /*! \brief An array of point coordinates.  The first point's x        
      coordinate is at index [0] and its y coordinate at index [1], followed 
      by the coordinates of the remaining points.  Each point occupies two   
      REALs.      
    */
    REAL *pointlist; /* In / out */

    /*! \brief An array of point attributes.  Each point's       
      attributes occupy `numberofpointattributes' REALs.
    */
    REAL *pointattributelist;    /* In / out */

    /*! \brief An array of point markers; one int per point.*/
    int *pointmarkerlist;        /* In / out */

    ///Number of points.
    int numberofpoints;          /* In / out */

    ///Number of point attributes.
    int numberofpointattributes; /* In / out */

    /*! \brief An array of triangle corners.  The first triangle's     
      first corner is at index [0], followed by its other two corners in     
      counterclockwise order, followed by any other nodes if the triangle    
      represents a nonlinear element.  Each triangle occupies                
      `numberofcorners' ints.
    */
    int *trianglelist;              /* In / out */

    /*! \brief An array of triangle attributes.  Each         
      triangle's attributes occupy `numberoftriangleattributes' REALs.
    */
    REAL *triangleattributelist;    /* In / out */

    /*! \brief An array of triangle area constraints; one REAL per 
      triangle.  Input only.
    */
    REAL *trianglearealist;         /* In only */

    /*! \brief An array of triangle neighbors; three ints per          
      triangle.  Output only.
    */
    int *neighborlist;              /* Out only */

    ///Number of triangles.
    int numberoftriangles;          /* In / out */

    ///Number of corners.
    int numberofcorners;            /* In / out */

    ///Number of triangle attributes.
    int numberoftriangleattributes; /* In / out */
    //int *triangleedgelist;

    /*! \brief An array of segment endpoints.  The first segment's      
      endpoints are at indices [0] and [1], followed by the remaining        
      segments.  Two ints per segment.
    */
    int *segmentlist;       /* In / out */

    /*! \brief An array of segment markers; one int per segment. */
    int *segmentmarkerlist; /* In / out */

    ///Number of segments.
    int numberofsegments;   /* In / out */

    /*! \brief An array of holes.  The first hole's x and y coordinates    
      are at indices [0] and [1], followed by the remaining holes.  Two      
      REALs per hole.  Input only, although the pointer is copied to the     
      output structure for your convenience.
    */
    REAL *holelist;    /* In / pointer to array copied out */

    ///Number of holes.
    int numberofholes; /* In / copied out */

    /*! \brief An array of regional attributes and area constraints.     
      The first constraint's x and y coordinates are at indices [0] and [1], 
      followed by the regional attribute at index [2], followed by the       
      maximum area at index [3], followed by the remaining area constraints. 
      Four REALs per area constraint.  Note that each regional attribute is  
      used only if you select the `A' switch, and each area constraint is    
      used only if you select the `a' switch (with no number following), but 
      omitting one of these switches does not change the memory layout.      
      Input only, although the pointer is copied to the output structure for 
      your convenience.
    */
    REAL *regionlist;    /* In / pointer to array copied out */

    ///Number of regions.
    int numberofregions; /* In / copied out */

    /*! \brief An array of edge endpoints.  The first edge's endpoints are 
      at indices [0] and [1], followed by the remaining edges.  Two ints per 
      edge.  Output only.
    */
    int *edgelist;       /* Out only */

    /*! \brief An array of edge markers; one int per edge.  Output   
      only.
    */
    int *edgemarkerlist; /* Not used with Voronoi diagram; out only */

    /*! \brief An array of normal vectors, used for infinite rays in       
      Voronoi diagrams.  The first normal vector's x and y magnitudes are    
      at indices [0] and [1], followed by the remaining vectors.  For each   
      finite edge in a Voronoi diagram, the normal vector written is the     
      zero vector.  Two REALs per edge.  Output only.
    */
    REAL *normlist;      /* Used only with Voronoi diagram; out only */

    ///Number of edges.
    int numberofedges;   /* Out only */
};

/*! \brief Adaptive mesh area constraint function.
        \details
        MeshParams holds all parameters used by mesh generation(see \ref triangle.hpp, \ref mesh.hpp)
     */
    class MeshParams
    {
        public:
            /*! \brief Vector of tip points.
                \details Tips - are points where mesh size should be small for better accuracy.
                in this case it corresponds to river tip points.
            */
            River::t_PointList tip_points;

            ///Radius of mesh refinment.
            double refinment_radius = 0.1;

            ///This value controlls transition slope between small mesh elements and big or course.
            double exponant = 7.;

            /*! \brief Sigma is used in exponence, also as \ref River::MeshParams::exponant controls slope. */
            double sigma = 1.9;

            /*! \brief Number of mesh refinment steps used by Deal.II mesh functionality.
                \details Refinment means splitting one rectangular element into four rectagular elements.
            */ 
            unsigned static_refinment_steps = 1;

            ///Minimal area of mesh.
            double min_area = 7e-4;

            ///Maximal area of mesh element.
            double max_area = 1e5;

            ///Minimal angle of mesh element.
            double min_angle = 30.;

            ///Maximal edge size.
            double max_edge = 1;

            ///Minimal edge size.
            double min_edge = 8.e-8;

            /*! \brief Ratio of the triangles: 
                
                \details
                Aspect ratio of a triangle is the ratio of the longest edge to shortest edge. 
                AR = abc/(8(s-a)(s-b)(s-c)) 
                Value 2 correspond to 30 degree.

                \todo check if it is implemented
                \todo handle edge values of ration which will correspond to 35 degree.
            */
            double ratio = 2.3;

            /*! \brief Width of branch.
                \details
                eps is width of splitting branch of tree into two lines, to make one border line.
                This is when tree and border is converted into one boundary line.
                \todo eps should depend on elementary step size __ds__.
            */
            double eps = 1e-6;

            /*! \brief Evaluates mesh area constraint at {x, y} point.
                \details

                ### detailed implementation of function:  

                \snippet river.hpp MeshConstrain
            */
            inline double operator()(double x, double y) const
            {
                double result_area = 10000000/*some large area value*/;
                for(auto& tip: tip_points)
                {
                    auto r = (River::Point{x, y} - tip).norm();
                    auto exp_val = exp( -pow(r/refinment_radius, exponant)/2./sigma/sigma);
                    auto cur_area = min_area + (max_area - min_area)*(1. - exp_val)/(1. + exp_val);
                    if(result_area > cur_area)
                        result_area = cur_area;   
                }
                
                return result_area;
            }

            ///Prints program options structure to output stream.
            friend ostream& operator <<(ostream& write, const MeshParams & mp);

            bool operator==(const MeshParams& mp) const;
    };

/*!
    \brief Triangle(c) triangulation entry point.
    \details                                                                 
    The calling convention for triangulate() follows.                        

        void triangulate(triswitches, in, out, vorout)                       
        char *triswitches;                                                   
        struct triangulateio *in;                                            
        struct triangulateio *out;                                           
        struct triangulateio *vorout;                                        

    `triswitches' is a string containing the command line switches you wish  
    to invoke.  No initial dash is required.  Some suggestions:              

    - You'll probably find it convenient to use the `z' switch so that       
      points (and other items) are numbered from zero.  This simplifies      
      indexing, because the first item of any type always starts at index    
      [0] of the corresponding array, whether that item's number is zero or  
      one.                                                                   
    - You'll probably want to use the `Q' (quiet) switch in your final code, 
      but you can take advantage of Triangle's printed output (including the 
      `V' switch) while debugging.                                           
    - If you are not using the `q', `a', `u', `D', `j', or `s' switches,     
      then the output points will be identical to the input points, except   
      possibly for the boundary markers.  If you don't need the boundary     
      markers, you should use the `N' (no nodes output) switch to save       
      memory.  (If you do need boundary markers, but need to save memory, a  
      good nasty trick is to set out->pointlist equal to in->pointlist       
      before calling triangulate(), so that Triangle overwrites the input    
      points with identical copies.)                                         
    - The `I' (no iteration numbers) and `g' (.off file output) switches     
      have no effect when Triangle is compiled with TRILIBRARY defined.      

    `in', `out', and `vorout' are descriptions of the input, the output,     
    and the Voronoi output.  If the `v' (Voronoi output) switch is not used, 
    `vorout' may be NULL.  `in' and `out' may never be NULL.                 

    Certain fields of the input and output structures must be initialized,   
    as described above.                                                                                                                           
*/
void triangulate(const char *, struct triangulateio *, struct triangulateio *,
                 struct triangulateio *, MeshParams *ac = NULL); //

///Free memory.
void trifree(void *memptr);

///Free up memory allocated for triangulateio structure.
void triangulateiofree(struct triangulateio *);

/*! \brief Set the triangulateio structure to default value
    \param io reference to triangulateio structure.
*/
void set_tria_to_default(struct triangulateio *io);