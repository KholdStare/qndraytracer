#ifndef _QND_AFFINE_TRANSFORMATION_H
#define _QND_AFFINE_TRANSFORMATION_H

#include <Eigen/Dense>

namespace qnd {

    /**
     * An affine transformation in Dim dimensions,
     * using Scalar data type
     */
    template <typename Scalar, int Dim>
    class AffineTransformation {

    public:
        typedef Eigen::Matrix<Scalar, Dim, Dim> EigenMatrixType;
        typedef Eigen::Matrix<Scalar, Dim, 1> EigenVectorType;
        typedef AffineTransformation<Scalar, Dim> SelfType;

        // Affine transformation parameters
        EigenMatrixType A; ///< An arbitrary NxN matrix
        EigenVectorType t; ///< A translation vector

        /**
         * Default construct an identity transformation
         */
        AffineTransformation() : A(EigenMatrixType::Identity()),
                                 t(EigenVectorType::Zero()) { }

        /**
         * Construct an AffineTransformation from Eigen types
         */
        AffineTransformation(EigenMatrixType const& otherA,
                             EigenVectorType const& otherT)
                                : A(otherA),
                                  t(otherT) { }

        /**
         * Compse two affine transformations
         */
        SelfType operator *(SelfType const& other) {
            return SelfType(A * other.A,
                            t + A*other.t);
        }
        // TODO: move rotate/translate/scale stuff into this class ?

        auto transformVector(EigenVectorType const& v) const -> decltype(A * v) {
            return A * v;
        }

        auto transformPoint(EigenVectorType const& p) const -> decltype(A * p + t) {
            return A * p + t;
        }
        
        auto invTransNorm(EigenVectorType const& n) const -> decltype(A.transpose() * n) {
            return A.transpose() * n;
        }

    };

}


#endif // _QND_AFFINE_TRANSFORMATION_H
