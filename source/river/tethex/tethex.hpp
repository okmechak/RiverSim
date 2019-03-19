/*
 * tethex - tetrahedra to hexahedra conversion
 * Copyright (c) 2013 Mikhail Artemyev
 * Report issues: github.com/martemyev/tethex/issues
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#pragma once

#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace tethex {

//-------------------------------------------------------
//
// d2s - convert data to string
//
//-------------------------------------------------------
template <typename T>
inline std::string d2s(T data)
{
  std::ostringstream o;
  if (!(o << data))
    throw std::runtime_error("Bad conversion of data to string!");
  return o.str();
}




//-------------------------------------------------------
//
// expect and require
// (the idea was firstly discovered in deal.II sources,
//  so thanks to its authors for that)
//
//-------------------------------------------------------
// in release (or release-like) versions
// nothing happens
#define expect(condition, message) { }

#define require(condition, message) \
  if (!(condition))                 \
    requirement_fails(__FILE__,     \
                      __LINE__,     \
                      message)

                /**
                 * Throw an informative exception,
                 * if requirement or expectation fails
                 */
void requirement_fails(const char *file,
                       int line,
                       std::string message);




//-------------------------------------------------------
//
// Point
//
//-------------------------------------------------------
/**
 * Point in 3-dimensional space.
 * It's used for 2-dimensional triangles as well, and
 * in this case one of the coordinates is 0 (usually it's z-coordinate).
 */
class Point
{
public:
  /**
      * The number of Cartesian coordinates, that describe the point.
      * Here we always use 3 coordinates to describe a point.
      */
  static const int n_coord = 3;

  int regionTag;
  double meshSize;

  /**
      * Default constructor.
      * Coordinates are initialized by 0.
      */
  Point();

  /**
      * Constructor with parameter.
      * Coordinates are initialized by array of numbers.
      * @param coordinates - array of point coordinates
      */
  Point(const double coordinates[]);

  /**
     * Constructor with parameters.
     * Coordinates are initialized by numbers.
     * @param x_coord - x-coordinate of the point
     * @param y_coord - y-coordinate of the point
     * @param z_coord - z-coordinate of the point
     */

  Point(const double x_coord,
        const double y_coord = 0,
        const double z_coord =0 ,
        const int regionIdTag = 0,
        const double mesh_size = 0);

  /**
      * Copy constructor
      */
  Point(const Point &p);

  /**
      * Copy assignment operator
      */
  Point& operator =(const Point &p);

                /**
                 * Get the coordinate of the point
                 * @param number - the serial number of coordinate [0, n_coord)
                 */
  double get_coord(int number) const;

                /**
                 * Set the value of specific coordinate
                 * @param number - the number of coordinate that we want to set
                 * @param value - new value of coordinate
                 */
  void set_coord(int number, double value);

  friend std::ostream & operator<< (std::ostream &out, const Point &p);

private:
      /**
                 * Cartesian coordinates of the point
                 */
  double coord[n_coord];
};





//-------------------------------------------------------
//
// MeshElement
//
//-------------------------------------------------------
/**
 * This class implements the most part of functionality of
 * all elements of mesh: triangles, tetrahedra, quadrangles, hexahedra, etc.
 * All these elements are declared as pointers to base (this) class.
 * It's not an abstract class, because it has no pure virtual functions,
 * but you can't create objects of this class, because its constructor is protected.
 */
class MeshElement
{
public:

                /**
                 * Destructor
                 */
  virtual ~MeshElement();

                /**
                 * Get the number of vertices
                 */
  int get_n_vertices() const;

                /**
                 * Get the number of edges
                 */
  int get_n_edges() const;

                /**
                 * Get type of the element that is used in Gmsh
                 */
  int get_gmsh_el_type() const;

                /**
                 * Get the material ID of the element.
                 * It's a number that describes the physical domain
                 * to which the element belongs.
                 */
  int get_material_id() const;

                /**
                 * Get the number of vertex describing the element
                 * @param number - local number of vertex [0, n_vertices)
                 * @return global number of vertex (among other mesh vertices)
                 */
  int get_vertex(int number) const;

                /**
                 * Get the number of edge describing the element
                 * @param number - local number of edge [0, n_edges)
                 * @return global number of edge (among other mesh edges)
                 */
  int get_edge(int number) const;

                /**
                 * Set the number of vertex
                 * @param local_number - the number of vertex inside the element [0, n_vertices)
                 * @param global_number - the number of vertex among other vertices of the mesh
                 */
  void set_vertex(int local_number, int global_number);

                /**
                 * Set the number of edge
                 * @param local_number - the number of edge inside the element [0, n_edges)
                 * @param global_number - the number of edge among other edges of the mesh
                 */
  void set_edge(int local_number, int global_number);


                /**
                 * Check - whether the element contains the vertex or not
                 * @param vertex - the number of vertex that we want to check
                 */
  bool contains(const int vertex) const;

  friend std::ostream & operator<< (std::ostream &out, const MeshElement &el);

protected:
                /**
                 * The number of vertices describing the element.
                 * It must be defined in each derived class,
                 * because it's 0 by default.
                 */
  int n_vertices;

                /**
                 * Vertices (i.e. their global numbers) describing the element
                 */
  std::vector<int> vertices;

                /**
                 * The number of edges describing the element.
                 * It must be defined in each derived class,
                 * because it's 0 by default.
                 */
  int n_edges;

                /**
                 * Edges (i.e. their global numbers) describing the element
                 * It's not always used.
                 */
  std::vector<int> edges;

                /**
                 * ID of the physical domain where the element takes place.
                 * It's necessary to distinguish media with different physical properties.
                 */
  int material_id;

                /**
                 * Type of the element (its number actually) like in Gmsh.
                 * It must be defined in every derived class.
                 * It's 0 by default.
                 */
  int gmsh_el_type;

                /**
                 * Constructor is protected to prevent creating MeshElement objects directly
                 * @param n_ver - number of vertices
                 * @param n_edg - number of edges
                 * @param n_fac - number of faces
                 * @param el_type - type of the element in Gmsh
                 */
  MeshElement(int n_ver = 0,
              int n_edg = 0,
              int el_type = 0);

                /**
                 * Copy constructor
                 */
  MeshElement(const MeshElement &elem);

                /**
                 * Copy assignment operator
                 */
  MeshElement& operator =(const MeshElement &elem);
};




//-------------------------------------------------------
//
// PhysPoint (physical entity)
//
//-------------------------------------------------------
/**
 * PhysPoint keep 2 numbers - the number of the vertex associated with the point,
 * and the number of physical domain
 * where this point takes place (material identificator - in other words).
 */
class PhysPoint : public MeshElement
{
public:
                /**
                 * Point is a vertex itself
                 */
  static const int n_vertices = 1;

                /**
                 * It's 0-dimensional shape, and it's a boundary for edge
                 */
  static const int n_edges = 0;

                /**
                 * In Gmsh physical point is defined by number 15
                 */
  static const int gmsh_el_type = 15;

                /**
                 * Constructor
                 */
  PhysPoint();

                /**
                 * Constructor with parameters
                 * @param ver - the list of vertices
                 * @param mat_id - the material ID
                 */
  PhysPoint(const std::vector<int> &ver,
            int mat_id = 0);

                /**
                 * Constructor with parameters
                 * @param ver - a vertex
                 * @param mat_id - material ID
                 */
  PhysPoint(const int ver,
            int mat_id = 0);
};





//-------------------------------------------------------
//
// Line
//
//-------------------------------------------------------
/**
 * Line keeps 3 numbers - the number of beginning vertex,
 * the number of the ending one, and a number of physical domain
 * where this line takes place (material identificator - in other words).
 * Line is not oriented.
 */
class Line : public MeshElement
{
public:
                /**
                 * There are 2 vertices to describe a line
                 */
  static const int n_vertices = 2;

                /**
                 * Line is edge itself, so the number of edges is 1
                 */
  static const int n_edges = 1;

                /**
                 * In Gmsh line (physical line) is defined by number 1
                 */
  static const int gmsh_el_type = 1;

                /**
                 * Constructor
                 */
  Line();

                /**
                 * Constructor with parameters
                 * @param ver - the list of vertices
                 * @param mat_id - the material ID
                 */
  Line(const std::vector<int> &ver,
       const int mat_id = 0);

                /**
                 * Constructor with parameters
                 * @param v1 - one vertex
                 * @param v2 - another vertex
                 * @param mat_id - material ID
                 */
  Line(int v1,
       int v2,
       int mat_id = 0);

                /**
                 * Find common vertex between two lines
                 * @param line - second line for seeking common vertex
                 */
  int common_vertex(const Line& line) const;

                /**
                 * Get another vertex (different from that we have)
                 * @param vertex - we have the number of one vertex (this one),
                 *                 and we want to find the number of another vertex
                 */
  int another_vertex(int vertex) const;
};




//-------------------------------------------------------
//
// Triangle
//
//-------------------------------------------------------
/**
 * Triangle - 2-dimensional simplex.
 * The simplest shape in 2D.
 * It's an element of mesh,
 * therefore it inherits the most part of
 * functionality from MeshElement class.
 */
class Triangle : public MeshElement
{
public:
                /**
                 * The number of vertices of triangle
                 */
  static const int n_vertices = 3;

                /**
                 * The number of edges of triangle
                 */
  static const int n_edges = 3;

                /**
                 * In Gmsh triangle is defined by number 2
                 */
  static const int gmsh_el_type = 2;

                /**
                 * Default constructor
                 */
  Triangle();

                /**
                 * Constructor with parameters
                 * @param ver - triangle vertices
                 * @param mat_id - material ID
                 */
  Triangle(const std::vector<int> &ver,
           int mat_id = 0);

                /**
                 * Constructor with parameters
                 * @param v1 - first vertex
                 * @param v2 - second vertex
                 * @param v3 - third vertex
                 * @param mat_id - material ID
                 */
  Triangle(int v1,
           int v2,
           int v3,
           int mat_id = 0);
};




//-------------------------------------------------------
//
// Quadrangle
//
//-------------------------------------------------------
/**
 * Quadrangle - 2-dimensional shape with 4 straight edges.
 * It's an element of mesh,
 * therefore it inherits the most part of
 * functionality from MeshElement class.
 */
class Quadrangle : public MeshElement
{
public:
                /**
                 * The number of vertices of quadrangle
                 */
  static const int n_vertices = 4;

                /**
                 * The number of edges of quadrangle
                 */
  static const int n_edges = 4;

                /**
                 * In Gmsh quadrangle is defined by number 3
                 */
  static const int gmsh_el_type = 3;

                /**
                 * Default constructor.
                 */
  Quadrangle();

                /**
                 * Constructor with parameters
                 * @param ver - quadrangle vertices
                 * @param mat_id - material ID
                 */
  Quadrangle(const std::vector<int> &ver,
             int mat_id = 0);

                /**
                 * Constructor with parameters
                 * @param v1 - first vertex
                 * @param v2 - second vertex
                 * @param v3 - third vertex
                 * @param v4 - fourth vertex
                 * @param mat_id - material ID
                 */
  Quadrangle(int v1,
             int v2,
             int v3,
             int v4,
             int mat_id = 0);
};



//-------------------------------------------------------
//
// IncidenceMatrix
//
//-------------------------------------------------------

/**
 * Incidence matrix describes the relations
 * (connections) between mesh nodes.
 * It's used for edge numeration and for fast
 * finding of edge number knowing 2 vertices,
 * that define the edge.
 * Because this matrix is symmetric
 * we consider its lower triangle only.
 */
class IncidenceMatrix
{
public:
                /**
                 * Constructor
                 * @param n_vertices - the number of all mesh vertices
                 * @param cells - the list of all mesh cells
                 */
  IncidenceMatrix(int n_vertices,
                  const std::vector<MeshElement*> &cells);

                /**
                 * Destructor
                 */
  ~IncidenceMatrix();

                /**
                 * Find a serial number of the non zero element in the matrix.
                 * This number usually coincides with the number of edge.
                 * Since we keep only lower triangle of incidence matrix,
                 * row_number has to be bigger than col_number.
                 * @param row_number - the number of row where we seek (or one edge vertex)
                 * @param col_number - the number of column where we seek (or another edge vertex)
                 * @return Serial number of the non zero element in the matrix.
                 */
  int find(int row_number, int col_number) const;

                /**
                 * Get the number of non zero elements in the matrix.
                 * Can be used to know the number of mesh edges.
                 */
  int get_n_nonzero() const;

private:
                /**
                 * The dimension of the matrix
                 */
  int dim;

                /**
                 * The number of nonzero elements of lower matrix triangle
                 */
  int n_non_zero;

                /**
                 * The number of nonzero elements in each row of lower matrix triangle
                 */
  int *row;

                /**
                 * The numbers of nonzero elements of lower matrix triangle
                 */
  int *col;

                /**
                 * No copies
                 */
  IncidenceMatrix(const IncidenceMatrix&);
  IncidenceMatrix& operator =(const IncidenceMatrix&);
};




//-------------------------------------------------------
//
// Mesh
//
//-------------------------------------------------------
/**
 * Main class that stores all data during program execution.
 */
class Mesh
{
public:
  /**
     * Constructor - nothing special
     */
  Mesh();
  Mesh(Mesh & msh) = default;
  Mesh(Mesh && msh) = default;

  /**
     * Constructor - nothing special
     */
  Mesh(
    std::vector<Point> &verticesVal, 
    std::vector<MeshElement *> &pointsVal, 
    std::vector<MeshElement *> &linesVal,
    std::vector<MeshElement *> &trianglesVal,
    std::vector<MeshElement *> &quaddranglesVal
  );

  Mesh(
    std::vector<Point> &verticesVal, 
    std::vector<MeshElement *> &linesVal,
    std::vector<MeshElement *> &trianglesVal
  );


  /**
     * Destructor - to clean the memory
     */
  ~Mesh();

  /**
     * Read the mesh from file
     * @param file - the name of the mesh file
     */
  void read(const std::string &file);

  /**
     * Conversion from simplices to bricks.
     * Specifically, in 2D - conversion from triangles to quadrangles,
     * in 3D - conversion from tetrahedra to hexahedra.
     */
  void convert();

  /**
     * Write the resulting brick mesh into the file
     * @param file - the name of mesh file where we write the results of conversion
     */
  void write(const std::string &file);

                /**
                 * Get the number of converted quadrangles
                 */
  int get_n_converted_quadrangles() const;

                /**
                 * Get the number of vertices
                 */
  int get_n_vertices() const;

                /**
                 * Get the number of physical points
                 */
  int get_n_points() const;

                /**
                 * Get the number of lines (physical lines)
                 */
  int get_n_lines() const;

                /**
                 * Get the number of triangles
                 */
  int get_n_triangles() const;

                /**
                 * Get the number of tetrahedra
                 */
  int get_n_quadrangles() const;

                /**
                 * Print short information
                 * about mesh into choosing stream.
                 */
  void info(std::ostream &out = std::cout) const;

                /**
                 * Print some statistics (detailed information)
                 * about mesh into choosing stream.
                 */
  void statistics(std::ostream &out = std::cout) const;

                /**
                 * Get the copy of vertex
                 * @param number - the number of vertex
                 */
  Point get_vertex(int number) const;

                /**
                 * Get the physical point
                 * @param number - the number of point
                 */
  MeshElement& get_point(int number) const;

                /**
                 * Get the mesh edge
                 * @param number - the number of edge
                 */
  MeshElement& get_edge(int number) const;
  
                /**
                 * Get the physical line
                 * @param number - the number of line
                 */
  MeshElement& get_line(int number) const;

                /**
                 * Get the mesh triangle
                 * @param number - the number of triangle
                 */
  MeshElement& get_triangle(int number) const;

                /**
                 * Get the mesh quadrangle
                 * @param number - the number of quadrangle
                 */
  MeshElement& get_quadrangle(int number) const;

  /**
                 * Get the copy of vertex
                 * @param number - the number of vertex
                 */
  std::vector<Point>& get_vertices();

                /**
                 * Get the physical point
                 * @param number - the number of point
                 */
  std::vector<MeshElement*>& get_points();

                /**
                 * Get the physical line
                 * @param number - the number of line
                 */
  std::vector<MeshElement*>& get_lines();

                /**
                 * Get the mesh triangle
                 * @param number - the number of triangle
                 */
  std::vector<MeshElement*>& get_triangles();

                /**
                 * Get the mesh quadrangle
                 * @param number - the number of quadrangle
                 */
  std::vector<MeshElement*>& get_quadrangles();


                /**
                 * Set the copy of vertex
                 * @param vertexes - the vector of vertexes
                 */
  void set_vertexes(std::vector<Point> &vertexes);
  /**
                 * Set the copy of vertex
                 * @param vertexes - the vector of vertexes
                 */
  void append_vertexes(std::vector<Point> &vertexes_val);

                /**
                 * Set the physical points
                 * @param points - the vector of points
                 */
  void set_points(std::vector<MeshElement *> &points);

                /**
                 * Set the physical lines
                 * @param lines - the vector of lines
                 */
  void set_lines(std::vector<MeshElement *> &lines);
  /**
                 * Append the physical lines
                 * @param lines - the vector of lines
                 */
  void append_lines(std::vector<MeshElement *> &lines_val);

                /**
                 * Set the mesh triangles
                 * @param triangles - the vector of triangles
                 */
  void set_triangles(std::vector<MeshElement *> &triangles);

                /**
                 * Set the mesh quadrangles
                 * @param quadrangles - the vector of quadrangles
                 */
  void set_quadrangles(std::vector<MeshElement *> &quadrangles);

                /**
                 * Free the memory to read again, for example
                 */
  void clean();
  
//protected: FIXME!
                /**
                 * Mesh vertices (nodes in terms of Gmsh)
                 */
  std::vector<Point> vertices;

                /**
                 * Physical points.
                 *They are not treated - just copied into new mesh file.
                 */
  std::vector<MeshElement*> points;

                /**
                 * Mesh lines - mean physical lines
                 */
  std::vector<MeshElement*> lines;

                /**
                 * Mesh edges (oriented lines)
                 */
  std::vector<MeshElement*> edges;

                /**
                 * Mesh triangles
                 */
  std::vector<MeshElement*> triangles;

                /**
                 * Mesh quadrangles
                 */
  std::vector<MeshElement*> quadrangles;

  typedef std::vector<std::map<int, int> > VectorMap;

                /**
                 * The number of quadrangles that existed in the input mesh
                 * and then were converted to have the same order of vertices for all elements
                 */
  int n_converted_quadrangles;

                /**
                 * The vector of strings representing names of physical entities
                 * of the mesh. This section may or may not be presented in the
                 * mesh file.
                 */
  std::vector<std::string> physical_names;


                /**
                 * Numerate the edges of simplices
                 * @param cells - edges of what elements should me numerated - triangles or tetrahedra
                 * @param incidence_matrix - matrix of vertices incidence
                 * @param initialize_edges - wether we need to initialize the vector of all edges of the mesh
                 *                           Sometimes we need to do it, sometimes we don't need (or even shouldn't).
                 */
  void edge_numeration(std::vector<MeshElement*> &cells,
                       const IncidenceMatrix &incidence_matrix,
                       bool initialize_edges);

                /**
                 * Conversion of 2D meshes (triangles -> quadrangles)
                 */
  void convert_2D();

                /**
                 * Find the global number of face basing on numbers of 2 edges defining that face
                 * @param edge1 - one edge of the face
                 * @param edge2 - another edge of the face
                 * @param vertices_incidence - incidence matrix between mesh vertices
                 * @param edge_vertex_incidence - incidence structure for vertices and opposite edges
                 */
  int find_face_from_two_edges(int edge1, int edge2,
                               const IncidenceMatrix &vertices_incidence,
                               const VectorMap &edge_vertex_incidence) const;

                /**
                 * During conversion we add new vertices -
                 * at the centers of edges, triangles, tetrahedra.
                 * This procedure unifies the approach to adding new vertices
                 * @param elements - elements to which new vertices belong.
                 *                   They may be edges, triangles or tetrahedra.
                 * @param n_old_vertices - the number of original mesh vertices
                 * @param shift - to make dense sequence of vertices we need to
                 *                point out from what number new type of vertices starts
                 */
  void set_new_vertices(const std::vector<MeshElement*> &elements,
                        int n_old_vertices,
                        int shift);

                /**
                 * Conversion from triangles to quadrangles.
                 * It's called from 2 different places.
                 * One call is held during 2D mesh conversion.
                 * Another call - during 3D mesh conversion,
                 * when we need to convert boundary triangles.
                 * @param incidence_matrix - the matrix of incidence between mesh nodes
                 * @param n_old_vertices - the number of original mesh vertices
                 * @param numerate_edges - do we need to numerate edges of triangles, or not.
                 *                         In 2D case we do it before this procedure,
                 *                         so we don't need to do it again.
                 *                         In 3D case edges of boundary triangles
                 *                         are not numbered yet, therefore we must do it.
                 * @param edge_vertex_incidence - the structure of incidence between
                 *                                mesh edges and vertices opposite to them.
                 *                                It used in 3D during boundary triangles conversion.
                 */
  void convert_triangles(const IncidenceMatrix &incidence_matrix,
                         int n_old_vertices,
                         bool numerate_edges,
                         const VectorMap &edge_vertex_incidence = VectorMap());

                /**
                 * During mesh conversion we add new vertices.
                 * They lie at the middle of all lines.
                 * So original physical lines are not valid anymore,
                 * and therefore we need to redefine them.
                 * @param incidence_matrix - the matrix of incidence between mesh nodes
                 * @param n_old_vertices - the number of original mesh vertices
                 */
  void redefine_lines(const IncidenceMatrix &incidence_matrix,
                      int n_old_vertices);

                /**
                 * Convert quadrangles into quadrangles.
                 * It sounds odd, but there is a sense.
                 * Gmsh's numeration of quadrangle vertices differs from deal.II's one.
                 * We make quadrangle vertices numeration in such an order,
                 * that it will be understandable by deal.II
                 */
  void convert_quadrangles();

};




//-------------------------------------------------------
//
// Auxiliary functions
//
//-------------------------------------------------------
                /**
                 * Since this project was originally designed to connect Gmsh's meshes
                 * with deal.II solvers, the main requirement for new meshes
                 * was to be properly read by deal.II.
                 * Therefore before creating quadrangle we check,
                 * that it's correctly numerated. And we check it
                 * like deal.II authors do - using cell_measure.
                 * This procedure is taken from deal.II sources.
                 */
double cell_measure_2D(const std::vector<Point> &vertices,
                       const std::vector<int> &indices);

                /**
                 * Since all mesh elements are derived from one base MeshElement class,
                 * we can use one procedure to treat the writing of all element into mesh file.
                 * @param out - output stream
                 * @param elems - vector of mesh elements
                 * @param serial_number - serial number of mesh element in mesh file
                 */
void write_elements(std::ostream &out,
                    const std::vector<MeshElement*> &elems,
                    int &serial_number);

                /**
                 * Change the order of vertices in such a way,
                 * that it will be good for deal.II
                 * @param dimension - dimension of the element (2D - quadrangle, 3D - hexahedron)
                 * @param all_mesh_vertices - vertices of the mesh
                 * @param vertices - vertices indices of the element, that we need to order
                 */
  void change_vertices_order(int dimension,
                             const std::vector<Point> &all_mesh_vertices,
                             std::vector<int> &vertices);


} // namespace tethex