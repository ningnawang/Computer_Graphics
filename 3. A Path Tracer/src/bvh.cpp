#include "bvh.h"

#include "CMU462/CMU462.h"
#include "static_scene/triangle.h"

#include <iostream>
#include <stack>

using namespace std;

namespace CMU462 { namespace StaticScene {

BVHAccel::BVHAccel(const std::vector<Primitive *> &_primitives,
                   size_t max_leaf_size) {

  this->primitives = _primitives;

  // TODO:
  // Construct a BVH from the given vector of primitives and maximum leaf
  // size configuration. The starter code build a BVH aggregate with a
  // single leaf node (which is also the root) that encloses all the
  // primitives.

  BBox bb;
  for (size_t i = 0; i < primitives.size(); ++i) {
    bb.expand(primitives[i]->get_bbox());
  }

  root = new BVHNode(bb, 0, primitives.size());

  BVHBuild(root, primitives, max_leaf_size);

}


void BVHAccel::BVHBuild(BVHNode * node, std::vector<Primitive *> &_primitives, size_t max_leaf_size) {

  // return leaf node 
  if (_primitives.size() <= max_leaf_size) {
    return;
  } 

  // define partition num
  double plan_num = _primitives.size() > 12 ? 12 : _primitives.size();


  // choose partition pivot
  BBox best_leftbb;
  BBox best_rightbb;
  std::vector<Primitive *> best_left_primitives;
  std::vector<Primitive *> best_right_primitives;
  std::vector<Primitive *> best_primitives = primitives;


  double min_cost = INF_D;

  min_cost = bestCost("x", &best_primitives, &best_left_primitives, &best_right_primitives, 
                      node, node->bb, min_cost, plan_num);
  min_cost = bestCost("y", &best_primitives, &best_left_primitives, &best_right_primitives, 
                      node, node->bb, min_cost, plan_num);
  min_cost = bestCost("z", &best_primitives, &best_left_primitives, &best_right_primitives, 
                      node, node->bb, min_cost, plan_num);

  primitives = best_primitives;

  BVHBuild(node->l, best_left_primitives, max_leaf_size);
  BVHBuild(node->r, best_right_primitives, max_leaf_size);
}



double BVHAccel::bestCost(std::string flag, 
                          std::vector<Primitive *> *best_primitives, 
                          std::vector<Primitive *> *best_left_primitives, 
                          std::vector<Primitive *> *best_right_primitives,
                          BVHNode * node,
                          BBox currbb, 
                          double best_cost, 
                          double plan_num) {


  std::vector<Primitive *> left_primitives;
  std::vector<Primitive *> right_primitives;


  for (size_t split = 1; split < plan_num; split++) {

    int leftCount = 0;
    int rightCount = 0;
    BBox leftbb;
    BBox rightbb;

    // partition on x-axis
    if (flag == "x") {
      double offset = (currbb.max.x - currbb.min.x) / plan_num;
      double pivot = currbb.min.x + split * offset;

      left_primitives.clear();
      right_primitives.clear();

      for (int i = node->start; i < node->start + node->range; i++) {

        if (primitives[i]->get_bbox().centroid().x <= pivot) {
          left_primitives.push_back(primitives[i]);
          leftbb.expand(primitives[i]->get_bbox());
          leftCount++;
        }
        else {
          right_primitives.push_back(primitives[i]);
          rightbb.expand(primitives[i]->get_bbox());
          rightCount++;
        }
      }

    } // if (flag == "x")


    // partition on y-axis
    if (flag == "y") {
      double offset = (currbb.max.y - currbb.min.y) / plan_num;
      double pivot = currbb.min.y + split * offset;

      left_primitives.clear();
      right_primitives.clear();

      for (int i = node->start; i < node->start + node->range; i++) {

        if (primitives[i]->get_bbox().centroid().y <= pivot) {
          left_primitives.push_back(primitives[i]);
          leftbb.expand(primitives[i]->get_bbox());
          leftCount++;
        }
        else {
          right_primitives.push_back(primitives[i]);
          rightbb.expand(primitives[i]->get_bbox());
          rightCount++;
        }
      }

    } // if (flag == "y")


    // partition on z-axis
    if (flag == "z") {
      double offset = (currbb.max.z - currbb.min.z) / plan_num;
      double pivot = currbb.min.z + split * offset;

      left_primitives.clear();
      right_primitives.clear();

      for (int i = node->start; i < node->start + node->range; i++) {

        if (primitives[i]->get_bbox().centroid().z <= pivot) {
          left_primitives.push_back(primitives[i]);
          leftbb.expand(primitives[i]->get_bbox());
          leftCount++;
        }
        else {
          right_primitives.push_back(primitives[i]);
          rightbb.expand(primitives[i]->get_bbox());
          rightCount++;

        }
      }

    } // if (flag == "z")
 

    // update primitives vector
    for (int i = node->start, j = 0; i < node->start + leftCount && j < leftCount; i++, j++) {
      primitives[i] = left_primitives[j];
    }
    for (int i = node->start + leftCount, j = 0; i < node->start + node->range && j < rightCount; i++, j++) {
      primitives[i] = right_primitives[j];
    }

    // compute current cost
    double currCost = computeCost(currbb, leftbb, rightbb, leftCount, rightCount);

    // update cost
    if (currCost < best_cost) {

      best_cost = currCost;
      best_left_primitives->clear();
      best_right_primitives->clear();

      // update best_left_primitives and best_right_primitives
      for (std::vector<Primitive *>::iterator it = left_primitives.begin(); it != left_primitives.end(); it++) {
        Primitive * temp = *it;
        best_left_primitives->push_back(temp);
      }
      for (std::vector<Primitive *>::iterator it = right_primitives.begin(); it != right_primitives.end(); it++) {
        Primitive * temp = *it;
        best_right_primitives->push_back(temp);
      }

      best_primitives->clear();
      for (std::vector<Primitive *>::iterator it = primitives.begin(); it != primitives.end(); it++) {
        Primitive * temp = *it;
        best_primitives->push_back(temp);
      }

      node->l = new BVHNode(leftbb, node->start, leftCount);
      node->r = new BVHNode(rightbb, node->start + leftCount, rightCount);
    } 

  }

  return best_cost;
}



double BVHAccel::computeCost(BBox currbb, BBox leftbb, BBox rightbb, int leftCount, int rightCount) {

  double sa_curr = (currbb.extent.x * currbb.extent.y + currbb.extent.x * currbb.extent.z +
                currbb.extent.y * currbb.extent.z) * 2;

  double sa_left = (leftbb.extent.x * leftbb.extent.y + leftbb.extent.x * leftbb.extent.z +
                leftbb.extent.y * leftbb.extent.z) * 2;

  double sa_right = (rightbb.extent.x * rightbb.extent.y + rightbb.extent.x * rightbb.extent.z +
                rightbb.extent.y * rightbb.extent.z) * 2;

  double cost = sa_left / sa_curr * leftCount + sa_right / sa_curr * rightCount;

  return cost;

}


BVHAccel::~BVHAccel() {

  // TODO:
  // Implement a proper destructor for your BVH accelerator aggregate

  delete root;

}

BBox BVHAccel::get_bbox() const {
  return root->bb;
}




bool BVHAccel::intersect(const Ray &ray) const {

  // TODO:
  // Implement ray - bvh aggregate intersection test. A ray intersects
  // with a BVH aggregate if and only if it intersects a primitive in
  // the BVH that is not an aggregate.

  // return find_any_hit(ray, root);

  // bool hit = false;
  // for (size_t p = 0; p < primitives.size(); ++p) {
  //   if(primitives[p]->intersect(ray)) hit = true;
  // }

  // return hit;

  Intersection * i;
  find_closest_hit(ray, root, i);
  if (i->t == INF_D) {
    return false;
  }
  return true;

  // return find_closest_hit(ray, root, i);


}


bool BVHAccel::intersect(const Ray &ray, Intersection *i) const {

  // TODO:
  // Implement ray - bvh aggregate intersection test. A ray intersects
  // with a BVH aggregate if and only if it intersects a primitive in
  // the BVH that is not an aggregate. When an intersection does happen.
  // You should store the non-aggregate primitive in the intersection data
  // and not the BVH aggregate itself.

  // if (!find_any_hit(ray, root)) 
  //   return false;

  // find_closest_hit(ray, root, i);
  // return true;

  find_closest_hit(ray, root, i);
  if (i->t == INF_D) {
    return false;
  }
  return true;

  // return find_closest_hit(ray, root, i);


  // bool hit = false;
  // for (size_t p = 0; p < primitives.size(); ++p) {
  //   if(primitives[p]->intersect(ray, i)) hit = true;
  // }

  // return hit;

}


/**
  * helper function, called only ray intersects bbox of node
**/
void BVHAccel::find_closest_hit(const Ray &ray, BVHNode * node, Intersection * closet_isect) const {

  // if leaf node
  if (node->isLeaf()) {
    double t0_temp = ray.min_t;
    double t1_temp = ray.max_t;
    // no intersetion with node's bbox
    if (!node->bb.intersect(ray, t0_temp, t1_temp)) {
      return; // false
    }
    // check all primitives
    for (int i = node->start; i < node->start + node->range; i++) {
      // update closet_isect when intersect exists
      primitives[i]->intersect(ray, closet_isect);
    }
  }
  else { // not leaf node
    double tmin1 = ray.min_t;
    double tmax1 = ray.max_t;
    double tmin2 = ray.min_t;
    double tmax2 = ray.max_t;
    bool hit1 = node->l->bb.intersect(ray, tmin1, tmax1);
    bool hit2 = node->r->bb.intersect(ray, tmin2, tmax2);

    if (hit1 && hit2) {
      BVHNode * first = tmin1 < tmin2 ? node->l : node->r;
      BVHNode * second = tmin1 < tmin2 ? node->r : node->l;
      find_closest_hit(ray, first, closet_isect);
      if (tmin2 < closet_isect->t) {
        find_closest_hit(ray, second, closet_isect);
      }
    }
    else if (hit1) {
      find_closest_hit(ray, node->l, closet_isect);
    }
    else if (hit2) {
      find_closest_hit(ray, node->r, closet_isect);
    }
  }

}


}  // namespace StaticScene
}  // namespace CMU462
