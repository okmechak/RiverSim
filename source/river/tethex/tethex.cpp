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

#include "tethex.hpp"
#include <algorithm>
#include <fstream>
#include <cmath>

namespace tethex {

//-------------------------------------------------------
//
// expect and require
//
//-------------------------------------------------------
void requirement_fails(const char *file,
                       int line,
                       std::string message)
{
  std::string exc = "Exception:\nfile = " + std::string(file) +
                    "\nline = " + d2s(line) +
                    "\nmessage = " + message + "\n";
  throw std::runtime_error(exc);
}




//-------------------------------------------------------
//
// Point
//
//-------------------------------------------------------
Point::Point()
{
  for (int i = 0; i < n_coord; ++i)
    coord[i] = 0.;
}

Point::Point(const double coordinates[])
{
  for (int i = 0; i < n_coord; ++i)
    coord[i] = coordinates[i];
}


Point::Point(const double x_coord,
             const double y_coord,
             const double z_coord,
             const int regionTagVal,
             const double mesh_size)
{
  regionTag = regionTagVal;
  coord[0] = x_coord;
  if (n_coord > 1) coord[1] = y_coord;
  if (n_coord > 2) coord[2] = z_coord;
  meshSize = mesh_size;
}

Point::Point(const Point &p)
{
  for (int i = 0; i < n_coord; ++i)
    coord[i] = p.coord[i];
  regionTag = p.regionTag;
  meshSize = p.meshSize;
}

Point& Point::operator =(const Point &p)
{
  for (int i = 0; i < n_coord; ++i)
    coord[i] = p.coord[i];
  regionTag = p.regionTag;
  meshSize = p.meshSize;
  return *this;
}

double Point::get_coord(int number) const
{
  expect(number >= 0 && number < n_coord,
         "The number of coordinate is incorrect: " +
         d2s(number) + ". It should be in the range: [0, " +
         d2s(n_coord) + ")");

  return coord[number];
}

void Point::set_coord(int number, double value)
{
  expect(number >= 0 && number < n_coord,
         "The number of coordinate is incorrect: " +
         d2s(number) + ". It should be in the range: [0, " +
         d2s(n_coord) + ")");

  coord[number] = value;
}


std::ostream & operator<< (std::ostream &out, const Point &p)
{
  std::cout << "Point{";
  for(int i = 0; i < p.n_coord - 1; ++i)
    std::cout << p.coord[i] << ", ";

  std::cout << p.coord[p.n_coord - 1] << "}  ";
  std::cout << "region tag: " << p.regionTag;
  std::cout << ", mesh size: " << p.meshSize;
  
  return out;
}




//-------------------------------------------------------
//
// MeshElement
//
//-------------------------------------------------------
MeshElement::MeshElement(int n_ver,
                         int n_edg,
                         int el_type)
  : n_vertices(n_ver)
  , vertices()
  , n_edges(n_edg)
  , edges()
  , material_id(0)
  , gmsh_el_type(el_type)
{
  vertices.resize(n_vertices, 0);
  edges.resize(n_edges, 0);
}

MeshElement::~MeshElement()
{ }

inline int MeshElement::get_n_vertices() const
{
  expect(n_vertices == vertices.size(),
         "Memory for vertices is not allocated properly (size is " + d2s(vertices.size()) +
         "), or n_vertices (" + d2s(n_vertices) + ") is set to wrong number");
  return n_vertices;
}

inline int MeshElement::get_n_edges() const
{
  expect(n_edges == edges.size(),
         "Memory for edges is not allocated properly (size is " + d2s(edges.size()) +
         "), or n_edges (" + d2s(n_edges) + ") is set to wrong number");
  return n_edges;
}

inline int MeshElement::get_gmsh_el_type() const
{
  return gmsh_el_type;
}

int MeshElement::get_material_id() const
{
  return material_id;
}

MeshElement::MeshElement(const MeshElement &elem)
  : n_vertices(elem.n_vertices)
  , vertices(elem.vertices)
  , n_edges(elem.n_edges)
  , edges(elem.edges)
  , material_id(elem.material_id)
  , gmsh_el_type(elem.gmsh_el_type)
{ }

MeshElement& MeshElement::operator =(const MeshElement &elem)
{
  n_vertices = elem.n_vertices;
  n_edges = elem.n_edges;
  gmsh_el_type = elem.gmsh_el_type;
  material_id = elem.material_id;
  vertices = elem.vertices;
  edges = elem.edges;
  return *this;
}

int MeshElement::get_vertex(int number) const
{
  expect(number >= 0 && number < n_vertices,
         "The local number of vertex is incorrect: " + d2s(number) +
         ". It has to be in range [0, " + d2s(n_vertices) + ").");
  return vertices[number];
}

int MeshElement::get_edge(int number) const
{
  expect(number >= 0 && number < n_edges,
         "The local number of edge is incorrect: " + d2s(number) +
         ". It has to be in range [0, " + d2s(n_edges) + ").");
  return edges[number];
}


void MeshElement::set_vertex(int local_number, int global_number)
{
  expect(local_number >= 0 && local_number < get_n_vertices(),
         "Local number (" + d2s(local_number) +
         ") is incorrect. It must be in the range [0, " + d2s(n_edges) + ")");
  vertices[local_number] = global_number;
}

void MeshElement::set_edge(int local_number, int global_number)
{
  expect(local_number >= 0 && local_number < get_n_edges(),
         "Local number (" + d2s(local_number) +
         ") is incorrect. It must be in the range [0, " + d2s(n_edges) + ")");
  edges[local_number] = global_number;
}


bool MeshElement::contains(int vertex) const
{
  for (int i = 0; i < n_vertices; ++i)
    if (vertex == vertices[i])
      return true;
  return false;
}

std::ostream & operator<< (std::ostream &out, const MeshElement &el)
{
  std::cout << "gmsh type: " << el.gmsh_el_type;
  std::cout << "  vert {";
  for(int i = 0; i < el.n_vertices - 1; ++i)
    std::cout << el.vertices[i] << ", ";
  
  if(el.n_vertices != 0)
    std::cout << el.vertices[el.n_vertices - 1];


  std::cout << "}  edges {";
  for(int i = 0; i < el.n_edges - 1; ++i)
    std::cout << el.edges[i] << ", ";
  if(el.n_edges != 0)
    std::cout << el.edges[el.n_edges - 1];


  std::cout << "}  mat id " << el.material_id;

  return out;
}




//-------------------------------------------------------
//
// PhysPoint
//
//-------------------------------------------------------
PhysPoint::PhysPoint()
  : MeshElement(n_vertices, n_edges, gmsh_el_type)
{ }

PhysPoint::PhysPoint(const std::vector<int> &ver,
                     int mat_id)
  : MeshElement(n_vertices, n_edges, gmsh_el_type)
{
  expect(vertices.size() == ver.size(),
         "The size of list of vertices (" + d2s(ver.size()) +
         ") is not equal to really needed number of vertices (" + d2s(n_vertices) + ")");
  vertices = ver;
  material_id = mat_id;
}

PhysPoint::PhysPoint(int ver, int mat_id)
  : MeshElement(n_vertices, n_edges, gmsh_el_type)
{
  vertices[0] = ver;
  material_id = mat_id;
}




//-------------------------------------------------------
//
// Line
//
//-------------------------------------------------------
Line::Line()
  : MeshElement(n_vertices, n_edges, gmsh_el_type)
{ }

Line::Line(const std::vector<int> &ver,
           int mat_id)
  : MeshElement(n_vertices, n_edges, gmsh_el_type)
{
  expect(vertices.size() == ver.size(),
         "The size of list of vertices (" + d2s(ver.size()) +
         ") is not equal to really needed number of vertices (" + d2s(n_vertices) + ")");
  vertices = ver;
  material_id = mat_id;
}

Line::Line(int v1,
           int v2,
           int mat_id)
  : MeshElement(n_vertices, n_edges, gmsh_el_type)
{
  vertices[0] = v1;
  vertices[1] = v2;
  material_id = mat_id;
}

int Line::common_vertex(const Line& line) const
{
  for (int i = 0; i < n_vertices; ++i)
    if (line.contains(vertices[i]))
      return vertices[i];
  require(false, "There is no common vertex between these two lines!");
  return 0; // to calm compiler down
}

int Line::another_vertex(int vertex) const
{
  if (vertex == vertices[0])
    return vertices[1];
  else if (vertex == vertices[1])
    return vertices[0];
  else
    require(false, "This line doesn't contain the vertex. So we can't find another one.");
  return 0; // to calm compiler down
}





//-------------------------------------------------------
//
// Triangle
//
//-------------------------------------------------------
Triangle::Triangle()
  : MeshElement(n_vertices, n_edges, gmsh_el_type)
{ }


Triangle::Triangle(const std::vector<int> &ver,
                   int mat_id)
  : MeshElement(n_vertices, n_edges, gmsh_el_type)
{
  expect(vertices.size() == ver.size(),
         "The size of list of vertices (" + d2s(ver.size()) +
         ") is not equal to really needed number of vertices (" + d2s(n_vertices) + ")");
  vertices = ver;
  material_id = mat_id;
}

Triangle::Triangle(int v1,
                   int v2,
                   int v3,
                   int mat_id)
  : MeshElement(n_vertices, n_edges, gmsh_el_type)
{
  vertices[0] = v1;
  vertices[1] = v2;
  vertices[2] = v3;
  material_id = mat_id;
}


//-------------------------------------------------------
//
// Quadrangle
//
//-------------------------------------------------------
Quadrangle::Quadrangle()
  : MeshElement(n_vertices, n_edges, gmsh_el_type)
{ }


Quadrangle::Quadrangle(const std::vector<int> &ver,
                       int mat_id)
  : MeshElement(n_vertices, n_edges, gmsh_el_type)
{
  expect(vertices.size() == ver.size(),
         "The size of list of vertices (" + d2s(ver.size()) +
         ") is not equal to really needed number of vertices (" + d2s(n_vertices) + ")");
  vertices = ver;
  material_id = mat_id;
}

Quadrangle::Quadrangle(int v1,
                       int v2,
                       int v3,
                       int v4,
                       int mat_id)
  : MeshElement(n_vertices, n_edges, gmsh_el_type)
{
  vertices[0] = v1;
  vertices[1] = v2;
  vertices[2] = v3;
  vertices[3] = v4;
  material_id = mat_id;
}



//-------------------------------------------------------
//
// IncidenceMatrix
//
//-------------------------------------------------------
IncidenceMatrix::IncidenceMatrix(int n_vertices,
                                 const std::vector<MeshElement*> &cells)
  : dim(n_vertices)
  , n_non_zero(0)
  , row(nullptr)
  , col(nullptr)
{
  std::vector<int> *vec = new std::vector<int>[dim]; // for lower triangle
  // look through all mesh cells
  for (size_t cell = 0; cell < cells.size(); ++cell)
  {
    // look at all pairs of cell vertices
    for (int i = 0; i < cells[cell]->get_n_vertices(); ++i)
    {
      const int ii = cells[cell]->get_vertex(i);
      for (int j = 0; j < cells[cell]->get_n_vertices(); ++j)
      {
        const int jj = cells[cell]->get_vertex(j);
        if (ii > jj) // we consider only lower triangle of matrix
        {
          // add to vector, if the vector doesn't contain this number
          if (std::find(vec[ii].begin(), vec[ii].end(), jj) == vec[ii].end())
            vec[ii].push_back(jj);
        }
      }
    }
  }

  // sorting the vectors
  for (int i = 0; i < dim; ++i)
    std::sort(vec[i].begin(), vec[i].end());

  // the number of non zero elements in each row of lower triangle
  row = new int[dim + 1];
  row[0] = 0;
  for (int i = 0; i < dim; ++i)
    row[i + 1] = row[i] + vec[i].size();

  n_non_zero = row[dim]; // the number of all non zero elements in lower triangle

  // numbers of non zero elements in lower triangle
  col = new int[n_non_zero];
  int k = 0;
  for (int i = 0; i < dim; ++i)
  {
    for (size_t j = 0; j < vec[i].size(); ++j)
    {
      col[k] = vec[i][j];
      k++;
    }
  }

  // free the memory
  for (int i = 0; i < dim; ++i)
    vec[i].clear();
  delete[] vec;
}



IncidenceMatrix::~IncidenceMatrix()
{
  delete[] row;
  delete[] col;
}



int IncidenceMatrix::find(int row_number,
                          int col_number) const
{
  // because we seek in lower triangle, row must be bigger than col
  expect(row_number > col_number,
         "We seek values in lower triangle, so row should be bigger than column. But in this case row_number = " +
         d2s(row_number) + ", col_number = " + d2s(col_number) + "!");

  for (int i = row[row_number]; i < row[row_number + 1]; ++i)
  {
    if (col[i] == col_number)
      return i;
  }

  for (int i = row[col_number]; i < row[col_number + 1]; ++i)
  {
    if (col[i] == row_number)
      return i;
  }

  // if the number cannot be found in this row, we throw exception
  //require(false,
  //        "The right value wasn't found for such row and column numbers: row_number = " +
  //        d2s(row_number) + ", col_number = " + d2s(col_number) + "!");
  return -1; // to calm compiler down about returned value
}


inline int IncidenceMatrix::get_n_nonzero() const
{
  return n_non_zero;
}




//-------------------------------------------------------
//
// Mesh
//
//-------------------------------------------------------
Mesh::Mesh()
  : vertices()
  , points()
  , lines()
  , triangles()
  , quadrangles()
  , n_converted_quadrangles(0)
  , physical_names()
{ }


Mesh::Mesh(
    std::vector<Point> &verticesVal, 
    std::vector<MeshElement *> &linesVal,
    std::vector<MeshElement *> &trianglesVal
  ): vertices(verticesVal)
  , points()
  , lines(linesVal)
  , triangles(trianglesVal)
  , quadrangles()
  , n_converted_quadrangles(0)
  , physical_names()
{}

Mesh::Mesh(
  std::vector<Point> &verticesVal, 
  std::vector<MeshElement *> &pointsVal, 
  std::vector<MeshElement *> &linesVal,
  std::vector<MeshElement *> &trianglesVal,
  std::vector<MeshElement *> &quaddranglesVal
): vertices(verticesVal)
  , points(pointsVal)
  , lines(linesVal)
  , triangles(trianglesVal)
  , quadrangles(quaddranglesVal)
  , n_converted_quadrangles(0)
  , physical_names()//TODO implement this if necessery
{}


Mesh::~Mesh()
{
  clean();
}




void Mesh::set_vertexes(std::vector<Point> &vertexesVal)
{
  //TODO: free up from memory previous vector!
  vertices = vertexesVal;
}
                /**
                 * Set the physical points
                 * @param points - the vector of points
                 */
  void Mesh::set_points(std::vector<MeshElement *> &pointsVal)
  {
    //TODO: free up from memory previous vector!
    points = pointsVal;
  }

                /**
                 * Set the physical lines
                 * @param lines - the vector of lines
                 */
  void Mesh::set_lines(std::vector<MeshElement *> &linesVal)
  { 
    //TODO: free up from memory previous vector!
    lines = linesVal;
  }

                /**
                 * Set the mesh triangles
                 * @param triangles - the vector of triangles
                 */
  void Mesh::set_triangles(std::vector<MeshElement *> &trianglesVal)
  {
    for (size_t i = 0; i < triangles.size(); ++i)
      delete triangles[i];
    triangles.clear();

    triangles = trianglesVal;
  }
   
                /**
                 * Set the mesh quadrangles
                 * @param quadrangles - the vector of quadrangles
                 */
  void Mesh::set_quadrangles(std::vector<MeshElement *> &quadranglesVal)
  {
    for (size_t i = 0; i < quadrangles.size(); ++i)
      delete quadrangles[i];
    quadrangles.clear();
    
    quadrangles = quadranglesVal;
  }



void Mesh::clean()
{
  vertices.clear();
  for (size_t i = 0; i < points.size(); ++i)
    delete points[i];
  points.clear();
  for (size_t i = 0; i < lines.size(); ++i)
    delete lines[i];
  lines.clear();
  for (size_t i = 0; i < edges.size(); ++i)
    delete edges[i];
  edges.clear();
  for (size_t i = 0; i < triangles.size(); ++i)
    delete triangles[i];
  triangles.clear();
  for (size_t i = 0; i < quadrangles.size(); ++i)
    delete quadrangles[i];
  quadrangles.clear();

  physical_names.clear();

  n_converted_quadrangles = 0;
}




void Mesh::read(const std::string &file)
{
  std::ifstream in(file.c_str());
  require(in, "File " + file + " cannot be opened!");

  clean(); // free the memory for mesh elements

  std::string str;
  in >> str; // the first string of Gmsh file is "$MeshFormat"
  expect(str == "$MeshFormat", "The first string of the Gmsh file " + file +
         " doesn't equal to \"$MeshFormat\". The actual string is \"" + str + "\"");

  // read the information about the mesh
  double version;
  int binary, dsize;
  in >> version >> binary >> dsize;
  // The function has been testing for meshes corresponding
  // to Gmsh versions since 2.6.0, therefore
  // in debug mode you'll have an exception if the mesh version is less than 2.2.
  // There is no exception in release mode though.
  // So to read the mesh with version 2.1 and less, check that DEBUG variable is set to 0.
  // But NOTE that msh files of 1.0 format have absolutely different structure!
  expect(version >= 2.2, "The version of Gmsh's mesh is too old (" + d2s(version) +
         "). The library was tested for versions 2.2+.");
  expect(dsize == sizeof(double), "The size of Gmsh's double (" + d2s(dsize) +
         ") doesn't equal to size of double type (" + d2s(sizeof(double)) + ")");

  getline(in, str); // read some empty string

// there is additional 1 (the number - one) in binary format
  if (binary)
  {
    int one;
    in.read(reinterpret_cast<char*>(&one), sizeof(int));
    require(one == 1, "The binary one (" + d2s(one) + ") doesn't equal to 1!");
  }

  // we make a map between serial number of the vertex and its number in the file.
  // it will help us when we create mesh elements
  std::map<int, int> vertices_map;

  // read lines of mesh file.
  // if we face specific keyword, we'll treat the section.
  while (in >> str)
  {
    if (str == "$PhysicalNames") // read the section of names of physical entities
    {
      int n_names;
      in >> n_names;
      getline(in, str);
      physical_names.resize(n_names);
      for (int i = 0; i < n_names; ++i)
        getline(in, physical_names[i]);
    }

    else if (str == "$Nodes") // read the mesh vertices
    {
      int n_vertices; // the number of all mesh vertices (that are saved in the file)
      in >> n_vertices; // read that number
      vertices.resize(n_vertices); // allocate the memory for mesh vertices
      getline(in, str); // read some empty string

      int number; // the number of the vertex
      // Cartesian coordinates of the vertex (Gmsh produces 3D mesh regardless
      // its real dimension)
      double coord[Point::n_coord];

      // read vertices
      for (int ver = 0; ver < n_vertices; ++ver)
      {
        if (binary) // binary format
        {
          in.read(reinterpret_cast<char*>(&number), sizeof(int));
          in.read(reinterpret_cast<char*>(coord), Point::n_coord * sizeof(double));
        }
        else // ASCII format
        {
          in >> number;
          for (int i = 0; i < Point::n_coord; ++i)
            in >> coord[i];
        }
        vertices[ver] = Point(coord); // save the vertex
        vertices_map[number] = ver; // add the number of vertex to the map
      }

      expect(n_vertices == vertices_map.size(),
             "Vertices numbers are not unique: n_vertices = " + d2s(n_vertices) +
             " vertices_map.size() = " + d2s(vertices_map.size()));

    } // read the vertices

    else if (str == "$Elements") // read the mesh elements
    {
      int n_elements; // the number of mesh elements
      in >> n_elements; // read that number
      getline(in, str); // empty string

      int number; // the number of the element [1, nElements]
      int el_type; // the type of the element (1 - line, 2 - triangle, etc)
      int n_tags; // the number of tags describing the element
      int phys_domain; // the physical domain where the element takes place
//      int elem_domain; // the elementary domain where the element takes place
//      int partition; // the partition in which the element takes place

      // the map between the type of the element,
      // and the number of nodes that describe it
      std::map<int, int> type_nodes;
      type_nodes[1] = 2; // 2-nodes line
      type_nodes[2] = 3; // 3-nodes triangle
      type_nodes[3] = 4; // 4-nodes quadrangle
      type_nodes[4] = 4; // 4-nodes tetrahedron
      type_nodes[5] = 8; // 8-nodes hexahedron
      type_nodes[15]= 1; // 1-node point

      {      
        for (int el = 0; el < n_elements; ++el)
        {
          in >> number >> el_type >> n_tags;
          std::vector<int> data(n_tags); // allocate the memory for some data
          for (int i = 0; i < n_tags; ++i) // read this information
            in >> data[i];
          phys_domain = (n_tags > 0) ? data[0] : 0; // physical domain - the most important value
//          elem_domain = (n_tags > 1) ? data[1] : 0; // elementary domain
//          partition   = (n_tags > 2) ? data[2] : 0; // partition (Metis, Chaco, etc)
          data.clear(); // other data isn't interesting for us

          // how many vertices (nodes) describe the element
          std::map<int, int>::const_iterator el_type_iter =
              type_nodes.find(el_type);

         require(el_type_iter != type_nodes.end(),
                  "This type of the Gmsh's element (" + d2s(el_type) +
                  ") in the mesh file \"" + file + "\" is unknown!");

          const int n_elem_nodes = el_type_iter->second; // the number of nodes
          std::vector<int> nodes(n_elem_nodes); // allocate memory for nodes
          for (int i = 0; i < n_elem_nodes; ++i)
          {
            in >> nodes[i]; // read the numbers of nodes
            // vertices can be numerated not sequentially (or not from 0)
            nodes[i] = vertices_map.find(nodes[i])->second;
          }

          switch (el_type)
          {
          case 15: // 1-node point
            points.push_back(new PhysPoint(nodes, phys_domain));
            break;
          case 1: // 2-nodes line
            lines.push_back(new Line(nodes, phys_domain));
            break;
          case 2: // 3-nodes triangle
            triangles.push_back(new Triangle(nodes, phys_domain));
            break;
          case 3: // 4-nodes quadrangle
            quadrangles.push_back(new Quadrangle(nodes, phys_domain));
            break;
          default:
            require(false, "Unknown type of the Gmsh's element (" +
                    d2s(el_type) + ") in the file " + file + "!");
          }

          nodes.clear();
        }

        // check some expectations
        expect(number == n_elements, "The number of the last read Gmsh's "
               "element (" + d2s(number) + ") is not equal to the amount of "
               "all elements in the mesh (" + d2s(n_elements) + ")!");

      } // ASCII format

      // requirements after reading elements
      require(!triangles.empty() || !quadrangles.empty(),
             "There are no any 2D or 3D elements in the mesh!");

      // we prevent mixing of simplices and bricks in one mesh.
      // at least for the moment
      if (!triangles.empty())
        require(quadrangles.empty(), "There are simplices "
                "and bricks in the same mesh. It's prohibited. Mesh file " + file);

    } // read the elements
  }

  in.close(); // close the file
}




void Mesh::convert()
{
  if (!triangles.empty())
    convert_2D();

  if (!quadrangles.empty())
    convert_quadrangles();
}



void Mesh::set_new_vertices(const std::vector<MeshElement*> &elements,
                            int n_old_vertices,
                            int shift)
{
  for (size_t elem = 0; elem < elements.size(); ++elem)
  {
    for (int coord = 0; coord < Point::n_coord; ++coord)
    {
      double coordinate = 0.;
      for (int ver = 0; ver < elements[elem]->get_n_vertices(); ++ver)
      {
        const int cur_vertex = elements[elem]->get_vertex(ver);
        expect(cur_vertex < n_old_vertices,
               "The element has a vertex (" + d2s(cur_vertex) +
               ") that is more than the number of old vertices (" + d2s(n_old_vertices) + ")");
        coordinate += vertices[cur_vertex].get_coord(coord);
      }
      vertices[n_old_vertices + shift + elem].set_coord(coord,
                                                        coordinate / elements[elem]->get_n_vertices());
    }
  }
}




void Mesh::convert_2D()
{
  // firstly we need to numerate all edges
  const IncidenceMatrix incidence_matrix(vertices.size(), triangles);

  // third parameter - whether we need to initialize the vector of all edges of the mesh.
  // yes - we need it
  edge_numeration(triangles, incidence_matrix, true);

  // after edge numbering
  // we should add new nodes -
  // one node at the middle of every edge and
  // one node at the center of every triangle
  const int n_old_vertices = vertices.size();
  vertices.resize(n_old_vertices + edges.size() + triangles.size());

  // add 'edge'-nodes - at the middle of edge
  set_new_vertices(edges, n_old_vertices, 0);
  //add_edge_nodes(n_old_vertices);

  // add 'triangle'-nodes - at the center of triangle
  set_new_vertices(triangles, n_old_vertices, edges.size());
  //add_triangle_nodes(triangles, n_old_vertices);

  // convert triangles into quadrangles.
  // third parameter - whether we need to numerate edges of triangles,
  // no - we've already done it
  convert_triangles(incidence_matrix, n_old_vertices, false);

  // now we don't need triangles anymore
  //for (size_t i = 0; i < triangles.size(); ++i)
  //  delete triangles[i];
  //triangles.clear();

  // after that we check boundary elements (lines),
  // because after adding new vertices they need to be redefined
  redefine_lines(incidence_matrix, n_old_vertices);

  for (size_t i = 0; i < edges.size(); ++i)
    delete edges[i];
  edges.clear();
}




void Mesh::convert_triangles(const IncidenceMatrix &incidence_matrix,
                             int n_old_vertices,
                             bool numerate_edges,
                             const VectorMap &edge_vertex_incidence)
{
  // quadrangles generation
  std::vector<int> quadrangle_vertices(Quadrangle::n_vertices);

  // we need to numerate edges of boundary triangles
  // numerate_edges = true - the case of 3D mesh, when we numerate edges of boundary triangles,
  //                         but in this case we mustn't numerate edges themselves,
  //                         because they were already numerated during process of numeration edges of tetrahedra,
  // numerate_edges = false - the case of 2D mesh, when we have already numerated edges of triangles,
  //                          so we don't need to do that again
  if (numerate_edges)
    // third parameter - whether we need to initialize the vector of all edges of the mesh,
    // no we shouldn't
    edge_numeration(triangles, incidence_matrix, false);

  for (size_t tri = 0; tri < triangles.size(); ++tri)
  {
    for (int ver = 0; ver < Triangle::n_vertices; ++ver)
    {
      // current vertex
      const int cur_vertex = triangles[tri]->get_vertex(ver);

      // we are looking for 2 edges that contain current vertex
      std::vector<int> seek_edges;
      for (int edge = 0; edge < Triangle::n_edges; ++edge)
      {
        const int cur_edge = triangles[tri]->get_edge(edge);
        if (edges[cur_edge]->contains(cur_vertex))
          seek_edges.push_back(cur_edge);

      }
      expect(seek_edges.size() == 2,
             "The number of edges to which every vertex belongs must be equal to 2");

      // numeration of quadrangle vertices
      quadrangle_vertices[0] = cur_vertex;
      quadrangle_vertices[1] = n_old_vertices + seek_edges[0];
      // !!! need to repair !!!
      if (numerate_edges) // distinguish 2D and 3D cases
      {
        // 3D case - boundary triangles
        quadrangle_vertices[2] = n_old_vertices + edges.size() +
                                 find_face_from_two_edges(seek_edges[0],
                                                          seek_edges[1],
                                                          incidence_matrix,
                                                          edge_vertex_incidence);
      }
      else
      {
        // 2D case - triangles themselves
        quadrangle_vertices[2] = n_old_vertices + edges.size() + tri;
      }
      quadrangle_vertices[3] = n_old_vertices + seek_edges[1];

      seek_edges.clear();

      // though the order of vertices is right, it may be clockwise or counterclockwise,
      // and it's important not to mix these 2 directions.
      // so, we need additional check as deal.II authors do.
      change_vertices_order(2, vertices, quadrangle_vertices);

//      // taken from deal.II
//      if (cell_measure_2D(vertices, quadrangle_vertices) < 0)
//        // change 2 vertices to reverse the order
//        std::swap(quadrangle_vertices[1], quadrangle_vertices[3]);

      // now we are ready to generate quadrangle
      quadrangles.push_back(new Quadrangle(quadrangle_vertices,
                                           triangles[tri]->get_material_id()));

    } // for every vertex we have one quadrangle
  } // triangles

  require(triangles.size() * 3 == quadrangles.size(),
          "The number of quadrangles (" + d2s(quadrangles.size()) +
          ") is not equal to number of triangles (" + d2s(triangles.size()) +
          ") multiplying by 3 (" + d2s(3 * triangles.size()) + ")");
}




void Mesh::convert_quadrangles()
{
  for (size_t elem = 0; elem < quadrangles.size(); ++elem)
  {
    std::vector<int> quad_vertices(Quadrangle::n_vertices);
    for (int i = 0; i < Quadrangle::n_vertices; ++i)
      quad_vertices[i] = quadrangles[elem]->get_vertex(i);

    const int ver = quad_vertices[1];

    change_vertices_order(2, vertices, quad_vertices);

    // since only first and third vertices are swapped (if any)
    // we compare only one vertex number
    if (quad_vertices[1] != ver)
      ++n_converted_quadrangles;

    for (int i = 0; i < Quadrangle::n_vertices; ++i)
      quadrangles[elem]->set_vertex(i, quad_vertices[i]);
  }
}



void Mesh::redefine_lines(const IncidenceMatrix &incidence_matrix,
                          int n_old_vertices)
{
  const int n_old_lines = lines.size();
  for (int line = 0; line < n_old_lines; ++line)
  {
    // we need to find an edge that coincides with this line
    const int ver1 = lines[line]->get_vertex(0);
    const int ver2 = lines[line]->get_vertex(1);
    const int edge = incidence_matrix.find(std::max(ver1, ver2),
                                                    std::min(ver1, ver2));

    // we change existing line and add new line at the end of list
    lines[line]->set_vertex(1, n_old_vertices + edge); // changing existing line
    lines.push_back(new Line(n_old_vertices + edge, ver2,
                             lines[line]->get_material_id())); // add new line
  }

  require(n_old_lines * 2 == (int)lines.size(),
          "The number of physical lines (" + d2s(lines.size()) +
          ") is not equal to number of original physical lines (" + d2s(n_old_lines) +
          ") multiplying by 2 (" + d2s(2 * n_old_lines) + ")");
}





void Mesh::edge_numeration(std::vector<MeshElement*> &cells,
                           const IncidenceMatrix &incidence_matrix,
                           bool initialize_edges)
{
  //// matrix of incidence between vertices of the mesh
  //const IncidenceMatrix incidence_matrix(vertices.size(), cells);

  // the number of edges in such mesh - the number of non zero elements in incidence matrix
  const int n_edges = incidence_matrix.get_n_nonzero();

  // allocate memory for edges
  if (initialize_edges)
    edges.resize(n_edges);

  // look through all cells of the mesh
  for (size_t cell = 0; cell < cells.size(); ++cell)
  {
    int lne = 0; // local number of the edge (0 <= lne < cell::n_edges)
    for (int i = 0; i < cells[cell]->get_n_vertices(); ++i)
    {
      const int ii = cells[cell]->get_vertex(i);
      for (int j = 0; j < cells[cell]->get_n_vertices(); ++j)
      {
        const int jj = cells[cell]->get_vertex(j);
        if (ii > jj) // ii must be bigger than jj
        {
          const int gne = incidence_matrix.find(ii, jj); // global number of edge
          // set the global number of edge to cell
          cells[cell]->set_edge(lne, gne);
          // initialize edge
          if (initialize_edges)
          {
            delete edges[gne];
            edges[gne] = new Line(std::min(ii, jj),
                                  std::max(ii, jj),
                                  cells[cell]->get_material_id());
          }
          // increase local number of edge
          ++lne;
        }
      }
    }
    expect(lne == cells[cell]->get_n_edges(),
           "lne must be equal to " + d2s(cells[cell]->get_n_edges()) +
           ", but it is " + d2s(lne));
  }

} // edge numeration


void Mesh::write(const std::string &file)
{
  std::ofstream out(file.c_str());
  require(out, "File " + file + " cannot be opened for writing!");

  out.setf(std::ios::scientific);
  out.precision(16);

  out << "$MeshFormat\n2.2 0 8\n$EndMeshFormat\n";
  if (!physical_names.empty())
  {
    out << "$PhysicalNames\n";
    out << physical_names.size() << "\n";
    for (size_t i = 0; i < physical_names.size(); ++i)
      out << physical_names[i] << "\n";
    out << "$EndPhysicalNames\n";
  }

  out << "$Nodes\n" << vertices.size() << "\n";
  for (size_t ver = 0; ver < vertices.size(); ++ver)
  {
    out << ver + 1 << " ";
    for (int coord = 0; coord < Point::n_coord; ++coord)
      out << vertices[ver].get_coord(coord) << " ";
    out << "\n";
  }

  int n_all_elements = points.size() +
                    + lines.size()
                    + triangles.size()
                    + quadrangles.size();
  if(!quadrangles.empty())
    n_all_elements -= triangles.size();

  out << "$EndNodes\n$Elements\n" << n_all_elements << "\n";

  int serial_number = 0;

  write_elements(out, points, serial_number);
  write_elements(out, lines, serial_number);
  if(quadrangles.empty())
    write_elements(out, triangles, serial_number);
  write_elements(out, quadrangles, serial_number);

  out << "$EndElements\n";

  out.close();
}



int Mesh::get_n_vertices() const
{
  return vertices.size();
}

int Mesh::get_n_points() const
{
  return points.size();
}

int Mesh::get_n_lines() const
{
  return lines.size();
}

int Mesh::get_n_triangles() const
{
  return triangles.size();
}

int Mesh::get_n_quadrangles() const
{
  return quadrangles.size();
}


int Mesh::get_n_converted_quadrangles() const
{
  return n_converted_quadrangles;
}



Point Mesh::get_vertex(int number) const
{
  expect(number >= 0 && number < vertices.size(),
         "The required number (" + d2s(number) +
         " is bigger than the number of vertices (" + d2s(vertices.size()) + "))");
  return vertices[number];
}

MeshElement& Mesh::get_point(int number) const
{
  expect(number >= 0 && number < points.size(),
         "The required number (" + d2s(number) +
         " is bigger than the number of physical points (" + d2s(points.size()) + "))");
  return *(points[number]);
}

MeshElement& Mesh::get_edge(int number) const
{
  expect(number >= 0 && number < edges.size(),
         "The required number (" + d2s(number) +
         " is bigger than the number of edges (" + d2s(edges.size()) + "))");
  return *(edges[number]);
}

MeshElement& Mesh::get_line(int number) const
{
  expect(number >= 0 && number < lines.size(),
         "The required number (" + d2s(number) +
         " is bigger than the number of lines (" + d2s(lines.size()) + "))");
  return *(lines[number]);
}

MeshElement& Mesh::get_triangle(int number) const
{
  expect(number >= 0 && number < triangles.size(),
         "The required number (" + d2s(number) +
         " is bigger than the number of triangles (" + d2s(triangles.size()) + "))");
  return *(triangles[number]);
}

MeshElement& Mesh::get_quadrangle(int number) const
{
  expect(number >= 0 && number < quadrangles.size(),
         "The required number (" + d2s(number) +
         " is bigger than the number of quadrangles (" + d2s(quadrangles.size()) + "))");
  return *(quadrangles[number]);
}



std::vector<Point>& Mesh::get_vertices()
{
  return vertices;
}

std::vector<MeshElement*>& Mesh::get_points()
{
  return points;
}

std::vector<MeshElement*>& Mesh::get_lines()
{
  return lines;
}

std::vector<MeshElement*>& Mesh::get_triangles()
{
  return triangles;
}

std::vector<MeshElement*>& Mesh::get_quadrangles()
{
  return quadrangles;
}




void Mesh::info(std::ostream &out) const
{
  out << "\nvertices       : " << vertices.size()
      << "\npoints (phys)  : " << points.size()
      << "\nlines          : " << lines.size()
      << "\ntriangles      : " << triangles.size()
      << "\nquadrangles    : " << quadrangles.size()
      << "\nconverted quads: " << n_converted_quadrangles
      << "\n\n";
  
  int i = 0;
  //Points
  std::cout << std::endl;
  std::cout << "----------" << std::endl;
  std::cout << "Points" << std::endl;
  std::cout << "----------" << std::endl;
  for(auto & p: vertices)
  {
    std::cout << i << ") "<< p << std::endl;
    i++;
  }


  //Edges
  i = 0;
  std::cout << std::endl;
  std::cout << "----------" << std::endl;
  std::cout << "Edges" << std::endl;
  std::cout << "----------" << std::endl;
  for(auto e: edges)
  {
    std::cout << i << ") " << *e << std::endl;
    i++;
  }


  //Lines
  i = 0;
  std::cout << std::endl;
  std::cout << "----------" << std::endl;
  std::cout << "Lines" << std::endl;
  std::cout << "----------" << std::endl;
  for(auto l: lines)
  {
    std::cout << i << ") " << *l << std::endl;
    i++;
  }

  //Triangles
  i = 0;
  std::cout << std::endl;
  std::cout << "----------" << std::endl;
  std::cout << "Triangles" << std::endl;
  std::cout << "----------" << std::endl;
  for(auto t: triangles)
  {
    std::cout << i << ") "<< *t << std::endl;
    i++;
  }


  //Quadrangles
  i = 0;
  std::cout << std::endl;
  std::cout << "----------" << std::endl;
  std::cout << "Quadrangles" << std::endl;
  std::cout << "----------" << std::endl;
  for(auto q: quadrangles)
  {
    std::cout << i << ") "<< *q << std::endl;
    i++;
  }
   
}



void Mesh::statistics(std::ostream &out) const
{
  out << "\nvertices:\n";
  for (size_t i = 0; i < vertices.size(); ++i)
  {
    out << i << " ";
    for (int j = 0; j < Point::n_coord; ++j)
      out << vertices[i].get_coord(j) << " ";
    out << "\n";
  }

  out << "\nedges:\n";
  for (size_t i = 0; i < edges.size(); ++i)
  {
    out << i << " ";
    for (int j = 0; j < Line::n_vertices; ++j)
      out << edges[i]->get_vertex(j) << " ";
    out << "\n";
  }
}



int Mesh::find_face_from_two_edges(int edge1, int edge2,
                                   const IncidenceMatrix &vertices_incidence,
                                   const VectorMap &edge_vertex_incidence) const
{
  // initialize auxiliary lines
  Line line1(edges[edge1]->get_vertex(0), edges[edge1]->get_vertex(1), edges[edge1]->get_material_id());
  Line line2(edges[edge2]->get_vertex(0), edges[edge2]->get_vertex(1), edges[edge2]->get_material_id());

  // find common vertex
  const int common_vertex = line1.common_vertex(line2);

  // find other 2 vertices
  const int ver1 = line1.another_vertex(common_vertex);
  const int ver2 = line2.another_vertex(common_vertex);

  // find opposite edge
  const int opposite_edge = vertices_incidence.find(std::max(ver1, ver2), std::min(ver1, ver2));

  // find the number of face
  return edge_vertex_incidence[opposite_edge].find(common_vertex)->second;
}





//-------------------------------------------------------
//
// Auxiliary functions
//
//-------------------------------------------------------
void write_elements(std::ostream &out,
                    const std::vector<MeshElement*> &elems,
                    int &serial_number)
{
  for (size_t el = 0; el < elems.size(); ++el, ++serial_number)
  {
    out << serial_number + 1 << " "             /* serial number of element */
        << elems[el]->get_gmsh_el_type()        /* type of element suitable for Gmsh */
        << " 2 "                                /* the number of tags */
        << elems[el]->get_material_id() << " "  /* physical domain */
        << elems[el]->get_material_id() << " "; /* elemetary domain - let it be the same */
    for (int ver = 0; ver < elems[el]->get_n_vertices(); ++ver)
      out << elems[el]->get_vertex(ver) + 1 << " ";
    out << "\n";
  }
}




void change_vertices_order(int dimension,
                           const std::vector<Point> &all_mesh_vertices,
                           std::vector<int> &vertices)
{
  if (dimension == 2)
  {
    // convert the order of vertices to suitable for deal.II to check the cell measure
    std::vector<int> vertices_dealII_order(Quadrangle::n_vertices);
    int order_to_deal[] = { 0, 1, 3, 2 };

    for (int i = 0; i < Quadrangle::n_vertices; ++i)
      vertices_dealII_order[order_to_deal[i]] = vertices[i];

    if (cell_measure_2D(all_mesh_vertices, vertices_dealII_order) < 0)
      // reorder vertices - swap first and third vertices
      std::swap(vertices[1], vertices[3]);
  }else
    require(false, "This feature is not implemented!");
}




double cell_measure_2D(const std::vector<Point> &vertices,
                       const std::vector<int> &indices)
{
  const double x[] = { vertices[indices[0]].get_coord(0),
                       vertices[indices[1]].get_coord(0),
                       vertices[indices[2]].get_coord(0),
                       vertices[indices[3]].get_coord(0)
                     };
  const double y[] = { vertices[indices[0]].get_coord(1),
                       vertices[indices[1]].get_coord(1),
                       vertices[indices[2]].get_coord(1),
                       vertices[indices[3]].get_coord(1)
                     };
  return (-x[1]*y[0]+x[1]*y[3]+
          y[0]*x[2]+x[0]*y[1]-
          x[0]*y[2]-y[1]*x[3]-
          x[2]*y[3]+x[3]*y[2]) / 2.;
}
} // namespace tethex

