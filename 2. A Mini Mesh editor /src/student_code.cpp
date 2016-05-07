/*
* Student solution for CMU 15-462 Project 2 (MeshEdit)
*
* Implemented by Ningna Wang.
*
*/

#include "student_code.h"
#include "mutablePriorityQueue.h"

namespace CMU462
{

 VertexIter HalfedgeMesh::splitEdge( EdgeIter e0 )
 {
    // TODO This method should split the given edge and return an iterator to the newly inserted vertex.
    // TODO The halfedge of this vertex should point along the edge that was split, rather than the new edges.

    /**
              v3                v3
               .                 .
              . .               .|.
             .   .             . | .
          v1.-----.v2  ->   v1.--v5--.v2
             .   .             . | .
              . .               .|. 
               .                 .
               v4                v4
    **/

     // get all info for each face
     // face 1: f1
     HalfedgeIter h1 = e0->halfedge();
     HalfedgeIter h11 = h1->next();
     HalfedgeIter h12 = h11->next();
     FaceIter f1 = h1->face();
     VertexIter v1 = h1->vertex();
     VertexIter v2 = h11->vertex();
     VertexIter v3 = h12->vertex();
     // face 2: f2
     HalfedgeIter h2 = h1->twin();
     HalfedgeIter h21 = h2->next();
     HalfedgeIter h22 = h21->next();
     FaceIter f2 = h2->face();
     VertexIter v4 = h22->vertex();

     // handle non-manifold case
     if (e0->isBoundary() ||
         v1->degree() < 3 || v2->degree() < 3) {
       return this->verticesEnd();
     }

     // allocate new point, edges, halfedges & faces
     VertexIter v5 = this->newVertex(); 
     v5->position = (v1->position + v2->position) / 2.;
     EdgeIter e45 = this->newEdge(); 
     EdgeIter e35 = this->newEdge(); 
     EdgeIter e25 = this->newEdge(); 
     HalfedgeIter h3 = this->newHalfedge();
     HalfedgeIter h31 = this->newHalfedge();
     HalfedgeIter h32 = this->newHalfedge();
     HalfedgeIter h4 = this->newHalfedge();
     HalfedgeIter h41 = this->newHalfedge();
     HalfedgeIter h42 = this->newHalfedge();
     FaceIter f3 = this->newFace();
     FaceIter f4 = this->newFace();

     // do split
     // consider halfedge changes
     // face f1
     h1->setNeighbors(h32, h2, v1, h1->edge(), f1); //next, twin, vertex, edge, face
     h32->setNeighbors(h12, h31, v5, e35, f1);
     h12->setNeighbors(h1, h12->twin(), v3, h12->edge(), f1);
     // face f2
     h2->setNeighbors(h21, h1, v5, h2->edge(), f2);
     h21->setNeighbors(h42, h21->twin(), v1, h21->edge(), f2);
     h42->setNeighbors(h2, h41, v4, e45, f2);
     // face f3
     h3->setNeighbors(h11, h4, v5, e25, f3);
     h11->setNeighbors(h31, h11->twin(), v2, h11->edge(), f3);
     h31->setNeighbors(h3, h32, v3, e35, f3);
     // face f4
     h4->setNeighbors(h41, h3, v2, e25, f4);
     h41->setNeighbors(h22, h42, v5, e45, f4);
     h22->setNeighbors(h4, h22->twin(), v4, h22->edge(), f4);

     //consider edge changes
     e35->halfedge() = h32;
     e45->halfedge() = h41;
     e25->halfedge() = h3;

     // consider vertex
     v5->halfedge() = h3;
     if (v2->halfedge() == h2) {
       v2->halfedge() = h4;
     }

     //consider face
     if (f1->halfedge() == h11) {
       f1->halfedge() = h32;
     }
     if (f2->halfedge() == h22) {
       f2->halfedge() = h42;
     }
     f3->halfedge() = h3;
     f4->halfedge() = h4;

     return v5; 
 }

 VertexIter HalfedgeMesh::collapseEdge( EdgeIter e )
 {
    // TODO This method should collapse the given edge and return an iterator to the new vertex created by the collapse.
 
    /**
              v3                v3
               .                 .
              . .                |
             .   .               | 
          v1.-----.v2  ->        v1
             .   .               |
              . .                |
               .                 .
               v4                v4
    **/

    // cout << "--------collapseEdge Start--------" << endl;
    /** get all info for each face **/
    // face 1: f1
    HalfedgeIter h1 = e->halfedge();
    HalfedgeIter h11 = h1->next();
    HalfedgeIter h12 = h11->next();
    FaceIter f1 = h1->face();
    VertexIter v1 = h1->vertex();
    VertexIter v2 = h11->vertex();
    VertexIter v3 = h12->vertex();
    // face 2: f2
    HalfedgeIter h2 = h1->twin();
    HalfedgeIter h21 = h2->next();
    HalfedgeIter h22 = h21->next();
    FaceIter f2 = h2->face();
    VertexIter v4 = h22->vertex();

    HalfedgeIter h11_twin = h11->twin();
    HalfedgeIter h12_twin = h12->twin();
    HalfedgeIter h21_twin = h21->twin();
    HalfedgeIter h22_twin = h22->twin();

    EdgeIter e13 = h12->edge();
    EdgeIter e14 = h21->edge();

    // handle non-manifold case
    if (v1->isBoundary() || v2->isBoundary() ||
        v1->degree() < 3 || v2->degree() < 3 || 
        v1->degree() > 8 || v2->degree() > 8 ||
        e->length() == 0.0) {
      return this->verticesEnd();
    }
    // // handle non-manifold case
    set<VertexIter> checkSet;
    HalfedgeIter h = v1->halfedge();
    do {
      HalfedgeIter h_twin = h->twin();
      checkSet.insert(h_twin->vertex());
      h = h_twin->next();
    } while (h != v1->halfedge());
    
    h = v2->halfedge();
    int count = 0;
    do {
      HalfedgeIter h_twin = h->twin();
      if (checkSet.find(h_twin->vertex()) != checkSet.end() && count == 2) {
        return this->verticesEnd();
      }
      if (checkSet.find(h_twin->vertex()) != checkSet.end() && count < 2) {
        count++;
      }
      h = h_twin->next();
    } while (h != v2->halfedge());


    /** do collapse **/
    // maintain all edges which connect with v1
    v1->position = (v1->position + v2->position) / 2;

    // consider halfedge changes
    // modify all edges which connect with v2 
    h = v2->halfedge();
    do {
      EdgeIter e = h->edge();
      HalfedgeIter h_twin = h->twin();
      if (e != h11->edge() && e != h1->edge() && e != h22->edge()) {
        if (h->vertex() == v2) h->vertex() = v1;
        if (h_twin->vertex() == v2) h_twin->vertex() = v1;
      }
      h = h_twin->next();
    } while (h != v2->halfedge());


    // modify halfedges along e13, e23, e14, e24  
    h11_twin->setNeighbors(h11_twin->next(), h12_twin, v3, e13, h11_twin->face()); //next, twin, vertex, edge, face
    h12_twin->setNeighbors(h12_twin->next(), h11_twin, v1, e13, h12_twin->face());
    h21_twin->setNeighbors(h21_twin->next(), h22_twin, v4, e14, h21_twin->face());
    h22_twin->setNeighbors(h22_twin->next(), h21_twin, v1, e14, h22_twin->face());


    // consider vertex change
    if (v1->halfedge() == h21 || v1->halfedge() == h1) {
      v1->halfedge() = h12_twin;
    }
    if(v3->halfedge() == h12) {
      v3->halfedge() = h12_twin->next();
    }
    if (v4->halfedge() == h22) {
      v4->halfedge() = h21_twin;
    }

    // consider edge change
    if (e13->halfedge() == h12) {
      e13->halfedge() = h11_twin;
    }
    if (e14->halfedge() == h21) {
      e14->halfedge() = h22_twin;
    }


    /** delete **/
    this->deleteVertex(v2);

    this->deleteEdge(h1->edge());
    this->deleteEdge(h11->edge());
    this->deleteEdge(h22->edge());

    this->deleteHalfedge(h1);
    this->deleteHalfedge(h11);
    this->deleteHalfedge(h12);
    this->deleteHalfedge(h2); 
    this->deleteHalfedge(h21);
    this->deleteHalfedge(h22);

    this->deleteFace(f1);
    this->deleteFace(f2);


  	return v1;


 }




 EdgeIter HalfedgeMesh::flipEdge( EdgeIter e0 )
 {
    // TODO This method should flip the given edge and return an iterator to the flipped edge.


    /**
              v3                v3
               .                 .
              . .               .|.
          v1.-----.v2  ->    v1. | .v2
              . .               .|. 
               .                 .
               v4                v4
    **/


    // get all info for each face
    // face 1: f1
    HalfedgeIter h1 = e0->halfedge();
    HalfedgeIter h11 = h1->next();
    HalfedgeIter h12 = h11->next();
    FaceIter f1 = h1->face();
    VertexIter v1 = h1->vertex();
    VertexIter v2 = h11->vertex();
    VertexIter v3 = h12->vertex();
    // face 2: f2
    HalfedgeIter h2 = h1->twin();
    HalfedgeIter h21 = h2->next();
    HalfedgeIter h22 = h21->next();
    FaceIter f2 = h2->face();
    VertexIter v4 = h22->vertex();


    // handle non-manifold case
    if (e0->isBoundary() ||
        v1->degree() < 3 || v2->degree() < 3) {
      return e0;
    }

    // handle non-manifold case
    set<VertexIter> checkSet;
    HalfedgeIter h = v1->halfedge();
    do {
      HalfedgeIter h_twin = h->twin();
      checkSet.insert(h_twin->vertex());
      h = h_twin->next();
    } while (h != v1->halfedge());
    
    h = v2->halfedge();
    int count = 0;
    do {
      HalfedgeIter h_twin = h->twin();
      if (checkSet.find(h_twin->vertex()) != checkSet.end() && count == 2) {
        return this->edgesEnd();
      }
      if (checkSet.find(h_twin->vertex()) != checkSet.end() && count < 2) {
        count++;
      }
      h = h_twin->next();
    } while (h != v2->halfedge());


    /** do flip **/
    // consider halfedge change
    h1->setNeighbors(h22, h2, v3, h1->edge(), f1); //next, twin, vertex, edge, face
    h22->setNeighbors(h11, h22->twin(), v4, h22->edge(), f1);
    h11->setNeighbors(h1, h11->twin(), v2, h11->edge(), f1);
    h2->setNeighbors(h12, h1, v4, h2->edge(), f2);
    h12->setNeighbors(h21, h12->twin(), v3, h12->edge(), f2);
    h21->setNeighbors(h2, h21->twin(), v1, h21->edge(), f2);
    e0->halfedge() = h1;

    // consider vertex change
    if (v1->halfedge() == h1) {
     v1->halfedge() = h21;
    }
    if (v2->halfedge() == h2) {
     v2->halfedge() = h11;
    }

    // consider face change
    if (f1->halfedge() == h12) {
     f1->halfedge() = h22;
    }
    if (f2->halfedge() == h22) {
     f2->halfedge() = h21;
    }

    return e0;
 }

 void MeshResampler::upsample( HalfedgeMesh& mesh )
 // This routine should increase the number of triangles in the mesh using Loop subdivision.
 {
    // Each vertex and edge of the original surface can be associated with a vertex in the new (subdivided) surface.
    // Therefore, our strategy for computing the subdivided vertex locations is to *first* compute the new positions
    // using the connectity of the original (coarse) mesh; navigating this mesh will be much easier than navigating
    // the new subdivided (fine) mesh, which has more elements to traverse.  We will then assign vertex positions in
    // the new mesh based on the values we computed for the original mesh.


    // TODO Compute new positions for all the vertices in the input mesh, using the Loop subdivision rule,
    // TODO and store them in Vertex::newPosition. At this point, we also want to mark each vertex as being
    // TODO a vertex of the original mesh.
    VertexIter v = mesh.verticesBegin();
    while (v != mesh.verticesEnd()) {
      v->isNew = false;
      VertexIter nextVertex = v;
      nextVertex++;

      Size n = v->degree();
      // sum(neighbors)
      Vector3D sum = Vector3D(0, 0, 0);
      HalfedgeIter h = v->halfedge();
      do {
        HalfedgeIter h_twin = h->twin();
        // VertexIter neighbor = h_twin->vertex();
        // sum += neighbor->position;
        sum += h->next()->vertex()->position;
        h = h_twin->next();
      } while (h != v->halfedge());
      // compute newPosition 
      double s;
      if (n == 3) s = (double) 3.0 / 16.0;
      else s = (double) 3.0 / (8.0 * n);
      v->newPosition = (1.0 - n * s) * v->position + s * sum;

      v = nextVertex;
    }



    // TODO Next, compute the updated vertex positions associated with edges, and store it in Edge::newPosition.
    // iterate over all edges in the mesh
    EdgeIter e = mesh.edgesBegin();
    while(e != mesh.edgesEnd())
    {
       // get the next edge NOW!
       EdgeIter nextEdge = e;
       nextEdge++;
       HalfedgeIter h1 = e->halfedge();
       HalfedgeIter h2 = h1->twin();
       Vector3D sum1 = h1->vertex()->position + 
                        h2->vertex()->position;
       Vector3D sum2 = h1->next()->next()->vertex()->position +
                        h2->next()->next()->vertex()->position;
       e->newPosition = 3.0 / 8.0 * sum1 + 1.0 / 8.0 * sum2;
       e = nextEdge;
    }


    // TODO Next, we're going to split every edge in the mesh, in any order.  For future
    // TODO reference, we're also going to store some information about which subdivided
    // TODO edges come from splitting an edge in the original mesh, and which edges are new,
    // TODO by setting the flat Edge::isNew.  Note that in this loop, we only want to iterate
    // TODO over edges of the original mesh---otherwise, we'll end up splitting edges that we
    // TODO just split (and the loop will never end!)
    Size nOriginalEdges = mesh.nEdges();
    e = mesh.edgesBegin();
    for (Index i = 0; i < nOriginalEdges; i++) {
       EdgeIter nextEdge = e; 
       nextEdge++;

       VertexIter edgePoint = mesh.splitEdge(e);
       if (edgePoint == mesh.verticesEnd()) continue;
       // flag and update position of new vertex 
       edgePoint->isNew = true;
       edgePoint->newPosition = e->newPosition;

       // flag whether an edge is a piece of an edge in the original mesh, 
       // or is an entirely new edge created during the subdivision step.
       // HalfedgeIter oldHalfEdge = edgePoint->halfedge();
       // oldHalfEdge->next()->next()->edge()->isNew = true;
       // oldHalfEdge->twin()->next()->edge()->isNew = true;
       bool isNew = false;
       HalfedgeIter h = edgePoint->halfedge();
       do
       {
          h->edge()->isNew = isNew;
          h = h->twin()->next();
          isNew = !isNew;
       }
       while( h != edgePoint->halfedge() );

       e = nextEdge;
    }


    // TODO Now flip any new edge that connects an old and new vertex.
    e = mesh.edgesBegin();
    while (e != mesh.edgesEnd())
    { 
      // get the next edge NOW!
      EdgeIter nextEdge = e;
      nextEdge++;
      if (e->isNew) {
        HalfedgeIter h1 = e->halfedge();
        HalfedgeIter h2 = h1->twin();
        if ((h1->vertex()->isNew == true && h2->vertex()->isNew == false) ||
            (h2->vertex()->isNew == true && h1->vertex()->isNew == false)) {
          EdgeIter e_flip = mesh.flipEdge(e);
          if (e_flip == mesh.edgesEnd()) continue;
        }
      }
       e = nextEdge;
    }


    // TODO Finally, copy the new vertex positions into final Vertex::position.
    v = mesh.verticesBegin();
    while (v != mesh.verticesEnd()) {
      VertexIter nextVertex = v;
      nextVertex++;
      v->position = v->newPosition;
      v = nextVertex;
    }

 }

 // Given an edge, the constructor for EdgeRecord finds the
 // optimal point associated with the edge's current quadric,
 // and assigns this edge a cost based on how much quadric
 // error is observed at this optimal point.
 EdgeRecord::EdgeRecord( EdgeIter& _edge )
 : edge( _edge )
 {
    // TODO Compute the combined quadric from the edge endpoints.
    HalfedgeIter h1 = _edge->halfedge();
    HalfedgeIter h2 = h1->twin();
    Matrix4x4 K = h1->vertex()->quadric + h2->vertex()->quadric;

    // TODO Build the 3x3 linear system whose solution minimizes
    // the quadric error associated with these two endpoints.
    // compute Ax = b from Matrix4x4 K:
    Matrix3x3 A = Matrix3x3();
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        A(i, j) = K(i,j);
      }
    }
    Vector3D b = -1.0 * K.column(3).to3D();

    // TODO Use this system to solve for the optimal position, and
    // TODO store it in EdgeRecord::optimalPoint.
    Vector3D x = A.inv() * b;
    this->optimalPoint = x;

    // TODO Also store the cost associated with collapsing this edge
    // TODO in EdgeRecord::Cost.
    Vector4D x_4D = Vector4D(x[0], x[1], x[2], 1.0);
    this->score = dot(x_4D, K * x_4D);

    this->edge = _edge;

 }

 void MeshResampler::downsample( HalfedgeMesh& mesh )
 {
    // TODO Compute initial quadrics for each face by simply writing the plane
    // equation for the face in homogeneous coordinates.  These quadrics should
    // be stored in Face::quadric
    FaceIter f = mesh.facesBegin();
    while (f != mesh.facesEnd()) {
      Vector3D n = f->normal();
      Vector3D p = f->halfedge()->vertex()->position;
      double d = -1.0 * dot(n, p);
      Vector4D v = Vector4D(n[0], n[1], n[2], d);
      f->quadric = outer(v, v);
      f++;
    }

    // TODO Compute an initial quadric for each vertex as the sum of the quadrics
    // associated with the incident faces, storing it in Vertex::quadric
    VertexIter v = mesh.verticesBegin();
    while (v != mesh.verticesEnd()) {
      HalfedgeIter h = v->halfedge();
      v->quadric = Matrix4x4();
      do {
        HalfedgeIter h_twin = h->twin();
        v->quadric += (h->face()->quadric + h_twin->face()->quadric);
        h = h_twin->next()->twin()->next();
      } while (h != v->halfedge());
      v++;
    }

    // TODO Build a priority queue of edges according to their quadric error cost,
    // TODO i.e., by building an EdgeRecord for each edge and sticking it in the queue.
    MutablePriorityQueue<EdgeRecord> queue;
    EdgeIter e = mesh.edgesBegin();
    while (e != mesh.edgesEnd()) {
      EdgeRecord myRecord(e);
      e->record = myRecord;
      queue.insert(e->record);
      e++;
    }

    // TODO Until we reach the target edge budget, collapse the best edge.  Remember
    // TODO to remove from the queue any edge that touches the collapsing edge BEFORE
    // TODO it gets collapsed, and add back into the queue any edge touching the collapsed
    // TODO vertex AFTER it's been collapsed.  Also remember to assign a quadric to the
    // TODO collapsed vertex, and to pop the collapsed edge off the top of the queue.

    // set this number to 1/4th the number of triangles in the input
    double budget = (double) 1.0/4.0 * mesh.nFaces();

    while (mesh.nFaces() > ceil(budget) && !queue.empty()) {
      // 1. Get the cheapest edge from the queue
      // 2. Remove the cheapest edge from the queue by calling pop()
      EdgeRecord eRecord_best = queue.top();
      queue.pop();
      EdgeIter e_best = eRecord_best.edge;

      // 3. Compute the new quadric by summing the quadrics at its two endpoints.
      VertexIter v1 = e_best->halfedge()->vertex();
      VertexIter v2 = e_best->halfedge()->twin()->vertex();
      Matrix4x4 quadric_new = v1->quadric + v2->quadric;
      
      // 4. Remove any edge touching either of its endpoints from the queue.
      HalfedgeIter h1 = v1->halfedge();
      do {
        EdgeIter e_neighbor = h1->edge();
        queue.remove(e_neighbor->record);
        h1 = h1->twin()->next();
      } while (h1 != v1->halfedge());

      HalfedgeIter h2 = v2->halfedge();
      do {
        EdgeIter e_neighbor = h2->edge();
        queue.remove(e_neighbor->record);
        h2 = h2->twin()->next();
      } while (h2 != v2->halfedge());

      // 5. Collapse the edge
      Vector3D newPosition = e_best->record.optimalPoint;
      VertexIter v_new = mesh.collapseEdge(e_best);
      if (v_new == mesh.verticesEnd()) continue;
      v_new->position = newPosition; // set position to optimalPoint
      
      // 6. Set the quadric of the new vertex to the quadric computed in Step 3
      v_new->quadric = quadric_new;
      // 7. Insert any edge touching the new vertex into the queue, creating new edge records for each of them
      HalfedgeIter h = v_new->halfedge();
      do {
        EdgeIter e_neighbor = h->edge();
        EdgeRecord myRecord(e_neighbor);
        e_neighbor->record = myRecord;
        queue.insert(e_neighbor->record);
        h = h->twin()->next();
      } while (h != v_new->halfedge());
    }

 }

 // return deviation 
 int deviation(int v1, int v2, int v3, int v4) {
  return abs(v1 - 6) + abs(v2 - 6) + abs(v3 - 6) + abs(v4 - 6);
 }

 void Vertex::computeCentroid( void )
 {
    // TODO Compute the average position of all neighbors of this vertex, and
    // TODO store it in Vertex::centroid.  This value will be used for resampling.

    Vector3D avg_position = Vector3D();
    double count = 0.0;
    HalfedgeIter h = this->halfedge();
    do {
      HalfedgeIter h_twin = h->twin();
      // VertexIter v_neighbor = h_twin->vertex();
      // avg_position += v_neighbor->position;
      avg_position += h->next()->vertex()->position;
      h = h_twin->next();
      count++;
    } while (h != this->halfedge());

    avg_position /= count;
    this->centroid = avg_position;
 }

 Vector3D Vertex::normal( void ) const
 // TODO Returns an approximate unit normal at this vertex, computed by
 // TODO taking the area-weighted average of the normals of neighboring
 // TODO triangles, then normalizing.
 {
    // TODO Compute and return the area-weighted unit normal.
    Vector3D v = this->position;
    Vector3D avg_weighted = Vector3D();
    HalfedgeCIter h = this->halfedge();
    do {
      // compute the triangle's unit normal
      FaceCIter f = h->face();
      Vector3D v1 = h->next()->vertex()->position;
      Vector3D v2 = h->next()->next()->vertex()->position;

      Vector3D e1 = v1 - v;
      Vector3D e2 = v2 - v;

      Vector3D normal = cross(e1, e2);
      // Aij * Nij 
      // = 1/2 * (e1 x e2) * (e1 x e2) / |(e1 x e2)| 
      // = 1/2 * (e1 x e2) 
      double area = normal.norm() / 2.f;
      normal = normal / 2.0;
      avg_weighted += normal;
      h = h->twin()->next();
    } while (h != this->halfedge());

		return avg_weighted;
 }

 void MeshResampler::resample( HalfedgeMesh& mesh )
 {
    // TODO Compute the mean edge length.
    EdgeIter e = mesh.edgesBegin();
    double avg = 0.0;
    while (e != mesh.edgesEnd()) {
      avg += e->length();
      e++;
    }
    // avg *= (double) 1.0 / mesh.nEdges(); 
    avg /= (double) mesh.nEdges();
    avg *= .9;

    // TODO Repeat the four main steps for 5 or 6 iterations
    for (int i = 0; i < 5; i++) {

      // TODO Split edges much longer than the target length (being careful about how the loop is written!)
      // if an edge is longer than 4L/3, split it
      e = mesh.edgesBegin();
      while (e != mesh.edgesEnd()) {
        EdgeIter next = e;
        next++;
        if (e->length() > 4.0 * avg / 3.0) {
          // VertexIter v_split = mesh.splitEdge(e);
          // if (v_split == mesh.verticesEnd()) {
          //   e = next;
          //   continue;
          // }
          // e = v_split->halfedge()->edge();
          // continue;
          mesh.splitEdge(e);
        }
        e = next;
      }

      // TODO Collapse edges much shorter than the target length.  Here we need to be EXTRA careful about
      // TODO advancing the loop, because many edges may have been destroyed by a collapse (which ones?)
      // cout << "----- collapse --------" << endl;
      // if an edge is shorter than 4L/5, collapse it. 
      e = mesh.edgesBegin();
      while (e != mesh.edgesEnd()) {
        EdgeIter next = e;
        next++;
        if (e->length() < 4.0 * avg / 5.0) {
        //   VertexIter v_collapse = mesh.collapseEdge(e);
        //   if (v_collapse == mesh.verticesEnd()) {
        //     e = next;
        //     continue;
        //   }
        //   e = v_collapse->halfedge()->edge();
        //   continue;
          EdgeIter e1 = e->halfedge()->next()->edge();
          EdgeIter e2 = e->halfedge()->next()->next()->edge();
          EdgeIter e3 = e->halfedge()->twin()->next()->edge();
          EdgeIter e4 = e->halfedge()->twin()->next()->next()->edge();
          while( next == e  || next == e1 ||
                 next == e2 || next == e3 || next == e4 ) {
             next++;
          }

          mesh.collapseEdge(e);
        }

        e = next;
      }

      
      // TODO Now flip each edge if it improves vertex degree
      // Flip all edges that decrease the total deviation from degree 6.
      e = mesh.edgesBegin();
      while (e != mesh.edgesEnd()) {
        EdgeIter next = e;
        next++;

        HalfedgeIter h1 = e->halfedge();
        HalfedgeIter h2 = h1->twin();
        VertexIter v1 = h1->vertex();
        VertexIter v2 = h2->vertex();
        VertexIter v3 = h1->next()->next()->vertex();
        VertexIter v4 = h2->next()->next()->vertex();

        if (deviation(v1->degree(), v2->degree(), v3->degree(), v4->degree()) > 
            deviation(v1->degree() - 1, v2->degree() - 1, v3->degree() + 1, v4->degree() + 1)) {
          EdgeIter e_flip = mesh.flipEdge(e);
          if (e_flip == mesh.edgesEnd()) {
            e = next;
            continue;
          }
          e = e_flip++;
          continue;
        }
        e = next;
      }

      // TODO Finally, apply some tangential smoothing to the vertex positions
      VertexIter v = mesh.verticesBegin();
      while (v != mesh.verticesEnd()) {
        VertexIter next = v;
        next++;
        v->computeCentroid();
        Vector3D c = v->centroid;
        Vector3D d = c - v->position;
        Vector3D N = v->normal();
        d -= dot(dot(N, d), N);
        v->position += 1.0 / 5.0 * d;
        v = next;
      }
    }

 }
}
