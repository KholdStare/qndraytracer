#include "kd_tree.h"

namespace {
    const int K = 3; // max number of dimensions. 
    // hopefully later will be templated
}

// =========================================

/**
 * Given faces, find the bounding box
 */
BoundingBox getBoundingBox( std::vector< FaceIter > const& faces ) {

    const double inf = std::numeric_limits<double>::infinity();
    Point3D minPoint(inf, inf, inf);
    Point3D maxPoint(-inf, -inf, -inf);

    // loop through faces and find bounds
    for (auto const& faceIter : faces) {
        Face const& face = *faceIter;

        for (auto const& vertex : face.vertices) {
            Point3D const& v = vertex.point;
            // update min and max points for box bound
            for (int dim = 0; dim < K; ++dim) {
                // check min
                if (v[dim] < minPoint[dim]) {
                    minPoint[dim] = v[dim];
                }

                // check max
                if (v[dim] > maxPoint[dim]) {
                    maxPoint[dim] = v[dim];
                }
            }
        }
    }

    return BoundingBox(minPoint, maxPoint);
}

// =========================================

KDNode::KDNode() : faceBound(Point3D(), Point3D()),
                   boundaryValue(0),
                   lessNode(nullptr),
                   moreNode(nullptr) { }
KDNode::~KDNode() { }

bool KDNode::intersectFaces(Point3D const& origin,
        Vector3D const& dir,
        FaceIntersection& intersection) const {

    bool success = false;
    for (auto const& faceIter : faces) {
        if (intersectFace(*faceIter, origin, dir, intersection) ) {
            success = true;
        }
    }

    return success;
}

// =======================
KDTree::KDTree() : root_(nullptr),
                   box_(Point3D(), Point3D()),
                   fomThreshold_(6),
                   differenceThreshold_(3) { }
KDTree::~KDTree() { clear(); }


void KDTree::build(FaceStorage const& faces, BoundingBox const& box) {
    // clear the tree first
    clear();
    box_ = box;

    // set thresholds

    // find smallest dimension of bounding box
    double minDimension = std::numeric_limits<double>::infinity();
    for (int dim = 0; dim < K; ++dim) {
        double diff = box.maxPoint()[dim] - box.minPoint()[dim];
        if (diff < minDimension) {
            minDimension = diff;
        }
    }

    // to determine smallest bisecting distance
    resolution_ = minDimension / 1000;


    // initialize vector of iterators with all faces
    std::vector< FaceIter > faceIterators;
    for (FaceIter it = begin(faces); it != end(faces); ++it) {
        faceIterators.push_back(it);
    }

//     std::cout << "starting kd build" << std::endl;

    // call recursive function
    root_.reset(buildHelper(faceIterators, box));
}


// recursive function for building the kd tree
KDNode* KDTree::buildHelper(std::vector< FaceIter > const& faces, BoundingBox const& box) {
    
//     std::cout << "calling buildHelper with numfaces: "
//               << faces.size() << std::endl;

    KDNode* node = new KDNode();
    // evaluate all 3 dimensions
    int bestDim = 3;
    PlaneEvaluation bestEval;
    unsigned int bestFom = std::numeric_limits<unsigned int>::max();
    for (int dim = 0; dim < K; ++dim) {
        // skip dimension if it is too thin already
        if (fabs(box.maxPoint()[dim] - box.minPoint()[dim]) < resolution_) {
            continue;
        }

        // create a divison plane
        PlaneEvaluation eval = chooseDimPlane( dim, faces, 0, 0,
                box.minPoint()[dim],
                box.maxPoint()[dim]);
        unsigned int fom = computeFOM(eval);
//         std::cout << "dim fom: " << fom << std::endl;
        if (fom < bestFom) {
            bestEval = std::move(eval);
            bestDim = dim;
            bestFom = fom;
        }
    }

    node->dim = bestDim;

    // if we reach termination conditions, stop recursing
    if ( (bestFom >= faces.size())
            || (bestFom < fomThreshold_) ) {
        node->faces = faces;
        node->faceBound = getBoundingBox(node->faces);
        return node;
    }

    // update the boundary value
    node->boundaryValue = bestEval.boundary;
    // any faces that sit on the boundary will be allocated to this node
    node->faces = std::move(bestEval.sharedFaces);
    node->faceBound = getBoundingBox(node->faces);

//     std::cout << "sharedFaces: " << bestEval.sharedFaces.size()
//               << " leftFaces: " <<  bestEval.leftFaces.size()
//               << " rightFaces: " << bestEval.rightFaces.size()
//               << " fom: " << bestFom 
//               << " dim: " << node->dim
//               << " boundary: " << node->boundaryValue
//               << std::endl;

    // recursively create nodes for left and right sides of the boundary
    if (bestEval.leftFaces.size() > 0) {
        Point3D maxPoint = box.maxPoint();
        maxPoint[bestDim] = bestEval.boundary;
        BoundingBox innerBox(box.minPoint(), maxPoint);
        node->lessNode.reset(buildHelper(bestEval.leftFaces, 
                                    innerBox));
    }

    if (bestEval.rightFaces.size() > 0) {
        Point3D minPoint = box.minPoint();
        minPoint[bestDim] = bestEval.boundary;
        BoundingBox innerBox(minPoint, box.maxPoint());
        node->moreNode.reset(buildHelper(bestEval.rightFaces, 
                                    innerBox));
    }

    return node;
}

void KDTree::clear() {
    root_.reset();
}

void KDTree::traverse(Point3D const& origin,
        Vector3D const& dir,
        FaceIntersection& intersection) const {

    // if tree is empty, no intersection
    if (!root_) {
        return;
    }

    // first intersect the overall bounding box
    double tNear, tFar;
    if ( !box_.fastIntersect(origin, dir, tNear, tFar) ) {
        // exit if no intersection
        return;
    }

    // now that we have tNear and tFar to restrict the ray segment
    // to the bounding volume, we can start the recursion
    traverse(root_.get(), origin, dir, tNear, tFar, intersection);
}

bool KDTree::traverse(KDNode* node, Point3D const& origin, Vector3D const& dir,
                  double tNear, double tFar,
                  FaceIntersection& intersection) const {

    // if node is null, no intersections
    if ( !node ) {
        return false;
    }

    // intersect all faces directly on the boundary
    bool intersectedHere = false;
    if (node->faces.size() > 0) {
        if ( node->faceBound.fastIntersect(origin, dir) ) {
            intersectedHere = node->intersectFaces(origin, dir, intersection);
        }
    }

    // if this is a leaf, we are done
    if ( node->isLeaf() ) {
        return intersectedHere;
    }

    // now we have to determine where the ray segment is situated,
    // relative to the division boundary.

    // set up a few useful variables
    Point3D startPoint = getInterPoint(tNear, origin, dir);
    int const& dim = node->dim; // dimension of the boundary plane
    double tBoundary = 0;   // t_value at boundary
    bool rayCrosses;        // boolean whether ray crosses boundary plane

    // intersect the ray with the boundary plane
    if (dir[dim] == 0) {
        // if the ray is parallel to the boundary
        // and is on the boundary then we are done, since
        // all boundary faces have been intersected
        if ( origin[dim] == node->boundaryValue ) {
            return intersectedHere;
        }
        rayCrosses = false;
    }
    else {
        tBoundary = (node->boundaryValue - origin[dim]) / dir[dim];
        // determine whether the plane intersection occurs within or outside
        // the ray segment
        rayCrosses = ( tNear < tBoundary && tBoundary < tFar);
    }

    // see if the ray starts on the left or right side of the boundary
    bool startsOnLeft = (startPoint[dim] < node->boundaryValue);

    // with the path of the ray determined, determine how to
    // proceed with the recursion

    // if the ray doesn't cross the boundary, we only
    // have to look at one half of volume
    if ( !rayCrosses ) {
        if ( startsOnLeft && node->lessNode ) {
            if (traverse(node->lessNode.get(), origin, dir, tNear, tFar, intersection) ) {
                return true;
            }
        }
        else if ( !startsOnLeft && node->moreNode ) {
            if (traverse(node->moreNode.get(), origin, dir, tNear, tFar, intersection) ) {
                return true;
            }
        }
    }
    else {
        // at this point we know the ray has crossed the boundary
        if ( startsOnLeft && node->lessNode ) {
            if (traverse(node->lessNode.get(), origin, dir, tNear, tBoundary, intersection) ) {
                return true;
            }
            return traverse(node->moreNode.get(), origin, dir, tBoundary, tFar, intersection)
                || intersectedHere;
        }
        else if ( !startsOnLeft && node->moreNode ) {
            if (traverse(node->moreNode.get(), origin, dir, tNear, tBoundary, intersection) ) {
                return true;
            }
            return traverse(node->lessNode.get(), origin, dir, tBoundary, tFar, intersection)
                || intersectedHere;
        }
    }

    return intersectedHere;
}

// Move elements from the second vector into the first.
// Second vector is empty after the tranfser
template <typename T>
void transferElements(std::vector<T>& accumulator, std::vector<T>& other) {
    typedef typename std::vector<T>::const_iterator iter;
    for (auto const& elem : other) {
        accumulator.push_back(elem);
    }
    other.clear();
}

// computes figure of merit
inline unsigned int computeFOM(unsigned int leftCount,
                        unsigned int rightCount,
                        unsigned int sharedCount) {
    return abs(leftCount - rightCount) + sharedCount;
}

inline unsigned int KDTree::computeFOM(KDTree::PlaneEvaluation const& eval) {
    return ::computeFOM(eval.leftFaces.size() + eval.extraLeftFaces,
                        eval.rightFaces.size() + eval.extraRightFaces,
                        eval.sharedFaces.size());
}

// return the depth of the tree
unsigned int KDTree::depth(KDNode* node) {

    if (!node) {
        return 0;
    }

    return 1 + std::max(depth(node->lessNode.get()), depth(node->moreNode.get()));
}

// return the maximum number of objects at the leaves
unsigned int KDTree::maxLeafObjects(KDNode* node) {
    if (!node) {
        return 0;
    } 
    
//     std::cout << "faces at node: " << node->faces.size() << std::endl;

    // if this is a leaf return number of faces
    if (node->isLeaf()) {
        return node->faces.size();
    }

    // otherwise recursively query children for maximum leaf objects
    return std::max(maxLeafObjects(node->lessNode.get()), maxLeafObjects(node->moreNode.get()));
}

// merge the two PlaneEvaluation structs
void KDTree::transferEvaluations(PlaneEvaluation& accumulator, PlaneEvaluation& other) {
    accumulator.boundary = other.boundary;
    transferElements( accumulator.leftFaces  , other.leftFaces   );
    transferElements( accumulator.rightFaces , other.rightFaces  );
    transferElements( accumulator.sharedFaces, other.sharedFaces );
}

KDTree::PlaneEvaluation KDTree::chooseDimPlane( int dim,
                                     std::vector< FaceIter > const& faces,
                                     unsigned int extraLeftFaces,
                                     unsigned int extraRightFaces,
                                     double min, double max ) {

//     std::cout << "choose dim plane called with min: " << min
//               << " max: " << max 
//               << " numfaces: " << faces.size()
//               << std::endl;

    unsigned int fom = std::numeric_limits<unsigned int>::max();

    PlaneEvaluation eval;
    eval.extraLeftFaces = extraLeftFaces;
    eval.extraRightFaces = extraRightFaces;

    // bisect volume in the current dimension
    eval.boundary = (min + max) / 2.0;

    evaluatePlane(dim, faces, eval);
    fom = computeFOM(eval);

    // Check which side we should bisect further
    bool bisectRight = false;
    if ( eval.rightCount() > eval.leftCount() ) {
        bisectRight = true;
        min = eval.boundary;
    }
    else {
        max = eval.boundary; // left side
    }

    // resolution limit reached, or all faces on boundary,
    // or figure of merit is lower than threshold, or perfect balance
    if ( (fabs(max - min) < resolution_) || fom == faces.size()
            || fom < fomThreshold_
            || eval.rightFaces.size() == eval.leftFaces.size()) { 
//         std::cout << "termination condition reached! "
//                   << (fabs(max - min) < resolution_)
//                   << (fom == faces.size())
//                   << (fom < fomThreshold_)
//                   << (eval.rightCount() == eval.leftCount()) << std::endl;
        // we are done bisecting so
        // return the separation of faces, and boundary value
        return eval;
    }

//     std::cout << "sharedFaces: " << eval.sharedFaces.size()
//               << " leftFaces: " << eval.leftFaces.size()
//               << " rightFaces: " << eval.rightFaces.size()
//               << " fom: " << fom
//               << std::endl;

    // otherwise, bisect the faces

    // populate the faces that need to be further separated
    std::vector< FaceIter > newFaces;
    transferElements(newFaces, eval.sharedFaces);
    if (bisectRight) {
        extraLeftFaces = eval.leftCount();
        transferElements(newFaces, eval.rightFaces);
    }
    else {
        extraRightFaces = eval.rightCount();
        transferElements(newFaces, eval.leftFaces);
    }

    // recursively evaluate the smaller subset of faces
    PlaneEvaluation newEval = chooseDimPlane(dim, newFaces,
                                             extraLeftFaces,
                                             extraRightFaces,
                                             min, max);
    // merge the evaluations
    transferEvaluations(eval, newEval);
    // and pass the combined result back up
    return eval;
}

unsigned int KDTree::countTotalFaces() {
    if (!root_) {
        return 0;
    }

    return count(root_.get());
}

unsigned int KDTree::count(KDNode* node) {

    unsigned int total = node->faces.size();

    if (node->lessNode) {
        total += count(node->lessNode.get());
    }

    if (node->moreNode) {
        total += count(node->moreNode.get());
    }

    return total;
}
    
void KDTree::evaluatePlane(int dim, std::vector< FaceIter > const& faces,
                           PlaneEvaluation& eval ) {

    // for each face, see which side of the boundary it is on
    for (auto const& faceIter : faces) {
        Face const& face = *faceIter;

        // count of vertices on the right side of boundary
        int vertexCount = 0;
        // look at each vertex
        for (int i = 0; i < 3; ++i) {
            if (face.vertices[i].point[dim] < eval.boundary) { vertexCount--; }
            else { vertexCount++; }
        }

        // update lists
        if      (vertexCount == 3)  { eval.rightFaces.push_back(faceIter); }
        else if (vertexCount == -3) { eval.leftFaces.push_back(faceIter); }
        else                        { eval.sharedFaces.push_back(faceIter); }
    }
}

