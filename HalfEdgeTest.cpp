#include "stdio.h"
#include <vector>
using namespace std;
int main() {
	return 0;
}
/*HalfEdge data structure definition*/

struct HalfEdge;
struct vec3 {
	float x,y,z;
};
struct VertexBase {
	vec3 p;
	vec3 n;
};
struct Face {
	vec3 n;
};
struct Vertex {
	HalfEdge *halfedge;
	VertexBase base;


};
struct HalfEdge {
	Vertex* vertex; /**/
	Face* face;
	HalfEdge * next;
	HalfEdge * oposite;
};
/*control class */


class HalfEdgeStorage {
public:
	/*Init from base*/
	HalfEdgeStorage(const std::vector<VetexBase> &vertexSoup); 
	~HaldEdgeStorage();
	/*Iterators*/
	int nextEdge();
	int prevEdge();

private:
	HalfEdge *c; /*current*/
};