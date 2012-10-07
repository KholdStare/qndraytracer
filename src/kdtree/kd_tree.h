#ifndef _KD_TREE_H_
#define _KD_TREE_H_

#include "../mesh/obj_store.h"
#include "../bounding_volume.h"
#include <memory>

typedef std::vector< Face > FaceStorage;
typedef FaceStorage::const_iterator FaceIter;

// TODO: Binary tree, so could be stored in a vector for cache locality
// TODO: move semantics

struct KDNode {
    KDNode();
    ~KDNode();

    bool isLeaf() { return !lessNode && !moreNode; }
    bool intersectFaces(Point3D const& origin,
                        Vector3D const& dir,
                        FaceIntersection& intersection) const;
    
    std::vector< FaceIter > faces; ///< faces residing at this node
    BoundingBox faceBound; ///< a bounding box around the faces at this node
    
    /**
     * Value of separating boundary of this node in 
     * the separating dimension dim */
    double boundaryValue;
    int dim; ///< dimension of separating boundary

    // pointers to child nodes of the tree
    /**
     * subtree holding all faces whose vertices' coordinates
     * are smaller than the boundary value in dimension dim */
    std::unique_ptr<KDNode> lessNode;
    std::unique_ptr<KDNode> moreNode; ///< other subtree

};

/**
 * Represents a KD tree holding triangular mesh faces,
 * and allows efficient instersection with rays.
 *
 * TODO: make generic for points and dimensions
 */
class KDTree {

public:
    KDTree();
    ~KDTree();

    /**
     * Given a container with Face objects, build a KD tree
     * that holds vectors of iterators into that container
     */
    void build(FaceStorage const& faces, BoundingBox const& box);

    /**
     * Find closest intersection with a face in the KD tree.
     */
    void traverse(Point3D const& origin,
                  Vector3D const& dir,
                  FaceIntersection& intersection) const;

    /**
     * @Return the total number of faces stored in the kd tree
     */
    unsigned int countTotalFaces();

    /**
     * @Return the depth of the tree
     */
    unsigned int depth() { return depth(root_.get()); }

    /**
     * @Return the maximum number of objects at the leaves
     */
    unsigned int maxLeafObjects() { return maxLeafObjects(root_.get()); }

    /**
     * clear the contents of the tree
     */
    void clear();

private:

    enum PlaneSide {
        PlaneSide_Left,
        PlaneSide_Right,
        PlaneSide_Neither,
    };

    struct PlaneEvaluation {
        PlaneEvaluation() : boundary(0),
                            extraLeftFaces(0),
                            extraRightFaces(0) { }

        // implement move semantics
        PlaneEvaluation(PlaneEvaluation && r)
            : boundary(r.boundary),
            leftFaces(std::move(r.leftFaces)),
            sharedFaces(std::move(r.sharedFaces)),
            rightFaces(std::move(r.rightFaces)),
            extraLeftFaces(r.extraLeftFaces),
            extraRightFaces(r.extraRightFaces) {

        }

        PlaneEvaluation& operator= (PlaneEvaluation && r) {
            boundary = r.boundary;
            leftFaces = std::move(r.leftFaces);
            sharedFaces = std::move(r.sharedFaces);
            rightFaces = std::move(r.rightFaces);
            extraLeftFaces = r.extraLeftFaces;
            extraRightFaces = r.extraRightFaces;
            return *this;
        }

        unsigned int leftCount() const {
            return leftFaces.size() + extraLeftFaces;
        }
        unsigned int rightCount() const {
            return leftFaces.size() + extraRightFaces;
        }

        double boundary;
        std::vector< FaceIter > leftFaces;
        std::vector< FaceIter > sharedFaces;
        std::vector< FaceIter > rightFaces;

        // counts of faces not within this segment (outside of segment)
        // that should be taken into account for the figure of merit
        unsigned int extraLeftFaces;
        unsigned int extraRightFaces;
    };

    /**
     * Choose the best axis-aligned separating plane for the faces
     * returns true if separability is satisfied
     */
    bool choosePlane( std::vector< FaceIter > const& faces,
                      KDNode& node,
                      BoundingBox const& vol);

    /**
     * Choose the best axis-aligned separating plane for the faces
     * for a single dimension and @return the plane evaluation
     */
    PlaneEvaluation chooseDimPlane( int dim,
                      std::vector< FaceIter > const& faces,
                      unsigned int extraLeftFaces,
                      unsigned int extraRightFaces,
                      double min, double max );

    /** Traverse a node with a ray */
    bool traverse(KDNode* node, Point3D const& origin, Vector3D const& dir,
                  double tNear, double tFar,
                  FaceIntersection& intersection) const;

    /**
     * Given separating plane, compute figure of merit
     * return which side of plane has more objects 
     */
    void evaluatePlane(int dim,
                       std::vector< FaceIter > const& faces,
                       PlaneEvaluation& eval );

    /** Recursive function for building the kd tree */
    KDNode* buildHelper(std::vector< FaceIter > const& faces,
                        BoundingBox const& box);

    /** Helper method for compute figure of merit on a plane evaluation */
    unsigned int computeFOM(PlaneEvaluation const& eval);

    /** Helper method for combining evaluations */
    void transferEvaluations(PlaneEvaluation& accumulator, PlaneEvaluation& other);

    /** Recursive face counting method */
    unsigned int count(KDNode* node);

    /** Return the depth of the tree rooted at @a node */
    unsigned int depth(KDNode* node);

    /** Return the maximum number of objects at the leaves */
    unsigned int maxLeafObjects(KDNode* node);

private:
    std::unique_ptr<KDNode> root_; ///< root of kd tree
    BoundingBox box_; ///< overall bounding box of all faces in the kd tree

    // various constants for plane classification
    unsigned int fomThreshold_; ///< figure of merit threshold
    int differenceThreshold_; ///< threshold for left/right object count difference
    double resolution_; ///< how thin must a volume be before we stop bisecting

};

#endif // _KD_TREE_H_
