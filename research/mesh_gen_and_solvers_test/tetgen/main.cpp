#include "tetgen.h"
#include <iostream>

void print_tetgenio(tetgenio &io)
{

}


int main(void)
{
		tetgenio in, out;
		tetgenio::facet *f;
		tetgenio::polygon *p;
		int i;

		// All indices start from 1.
		in.firstnumber = 1;

		in.numberofpoints = 6;
		in.pointlist = new REAL[in.numberofpoints * 3];
		in.pointlist[0] = 0;  // node 1.
		in.pointlist[1] = 0;
		in.pointlist[2] = 0;

		in.pointlist[3] = 1;  // node 2.
		in.pointlist[4] = 0;
		in.pointlist[5] = 0;

		in.pointlist[6] = 1;  // node 3.
		in.pointlist[7] = 1;
		in.pointlist[8] = 0;

		in.pointlist[9] = 0;  // node 4.
		in.pointlist[10] = 1;
		in.pointlist[11] = 0;

		in.pointlist[12] = 0.1;  // node 5.
		in.pointlist[13] = 0.1;
		in.pointlist[14] = 0;

		in.pointlist[15] = 0.9;  // node 6.
		in.pointlist[16] = 0.8;
		in.pointlist[17] = 0;
		
		in.numberofedges = 1;
		in.edgelist = new int[2 * in.numberofedges];
		in.edgelist[0] = 5;
		in.edgelist[0] = 6;


		in.numberoffacets = 1;
		in.facetlist = new tetgenio::facet[in.numberoffacets];
		in.facetmarkerlist = new int[in.numberoffacets];
		in.facetmarkerlist[0] = -1;

		// Facet 1. The leftmost facet.
		f = &in.facetlist[0];
		f->numberofpolygons = 1;
		f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
		f->numberofholes = 0;
		f->holelist = NULL;
		p = &f->polygonlist[0];
		p->numberofvertices = 4;
		p->vertexlist = new int[p->numberofvertices];
		p->vertexlist[0] = 1;
		p->vertexlist[1] = 2;
		p->vertexlist[2] = 3;
		p->vertexlist[3] = 4;
		

		// Output the PLC to files 'barin.node' and 'barin.poly'.
		//in.save_nodes("barin");
		//in.save_poly("barin");

		// Tetrahedralize the PLC. Switches are chosen to read a PLC (p),
		//   do quality mesh generation (q) with a specified quality bound
		//   (1.414), and apply a maximum volume constraint (a0.1).

		tetgenbehavior b;
		tetrahedralize(&b, &in, &out);

		// Output mesh to files 'barout.node', 'barout.ele' and 'barout.face'.
	

		return 0;
}