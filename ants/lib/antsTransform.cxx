
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include <algorithm>
#include <vector>
#include <string>

#include "itkMacro.h"
#include "itkImage.h"
#include "itkVectorImage.h"
#include "itkVector.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "vnl/vnl_vector_ref.h"
#include "itkTransform.h"
#include "itkAffineTransform.h"

#include "antsUtilities.h"
#include "itkAffineTransform.h"
#include "itkAffineTransform.h"
#include "itkCenteredAffineTransform.h"
#include "itkEuler2DTransform.h"
#include "itkEuler3DTransform.h"
#include "itkRigid2DTransform.h"
#include "itkRigid3DTransform.h"
#include "itkCenteredRigid2DTransform.h"
#include "itkCenteredEuler3DTransform.h"
#include "itkSimilarity2DTransform.h"
#include "itkCenteredSimilarity2DTransform.h"
#include "itkSimilarity3DTransform.h"
#include "itkQuaternionRigidTransform.h"
#include "itkTranslationTransform.h"
#include "itkResampleImageFilter.h"
#include "itkTransformFileReader.h"
#include "itkCompositeTransform.h"
#include "itkMatrixOffsetTransformBase.h"
#include "itkDisplacementFieldTransform.h"
#include "itkConstantBoundaryCondition.h"

#include "itkBSplineInterpolateImageFunction.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkGaussianInterpolateImageFunction.h"
#include "itkInterpolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkWindowedSincInterpolateImageFunction.h"
#include "itkLabelImageGaussianInterpolateImageFunction.h"
#include "itkTransformFileWriter.h"

#include "itkMacro.h"
#include "itkImage.h"
#include "itkVectorImage.h"
#include "itkVector.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "vnl/vnl_vector_ref.h"
#include "itkTransform.h"
#include "itkAffineTransform.h"

#include "antsTransform.h"
#include "antsImage.h"

namespace py = pybind11;


template <typename TransformType, unsigned int Dim>
void wrapANTsTransform(py::module & m, std::string const & suffix) {
    py::class_<ANTsTransform<TransformType>>(m, ("ANTsTransform" + suffix).c_str())
        //.def(py::init<>())
        // read only properties
        .def_readonly("precision", &ANTsTransform<TransformType>::precision)
        .def_readonly("dimension", &ANTsTransform<TransformType>::dimension)
        .def_readonly("type", &ANTsTransform<TransformType>::type)
        .def_readonly("pointer", &ANTsTransform<TransformType>::pointer)

        // read-write properties (parameters, fixed parameters, etc)
        .def("get_parameters", &ANTsTransform<TransformType>::getParameters)
        .def("set_parameters", &ANTsTransform<TransformType>::setParameters)
        .def("get_fixed_parameters", &ANTsTransform<TransformType>::getFixedParameters)
        .def("set_fixed_parameters", &ANTsTransform<TransformType>::setFixedParameters)

        // functions
        .def("transform_point", &ANTsTransform<TransformType>::transformPoint)
        .def("transform_vector", &ANTsTransform<TransformType>::transformVector)

        .def("transform_imageUC", &ANTsTransform<TransformType>::template transformImage<itk::Image<unsigned char, Dim>>)
        .def("transform_imageUI", &ANTsTransform<TransformType>::template transformImage<itk::Image<unsigned int, Dim>>)
        .def("transform_imageF", &ANTsTransform<TransformType>::template transformImage<itk::Image<float, Dim>>)
        .def("transform_imageD", &ANTsTransform<TransformType>::template transformImage<itk::Image<double, Dim>>)

        .def("inverse", &ANTsTransform<TransformType>::template inverse<TransformType>);
}

PYBIND11_MODULE(antsTransform, m) {
    wrapANTsTransform<itk::Transform<float, 2, 2>, 2>(m, "F2");
    wrapANTsTransform<itk::Transform<float, 3, 3>, 3>(m, "F3");
    wrapANTsTransform<itk::Transform<float, 4, 4>, 4>(m, "F4");
    wrapANTsTransform<itk::Transform<double, 2, 2>, 2>(m, "D2");
    wrapANTsTransform<itk::Transform<double, 3, 3>, 3>(m, "D3");
    wrapANTsTransform<itk::Transform<double, 4, 4>, 4>(m, "D4");

    m.def("composeTransformsF2", &composeTransforms<itk::Transform<float, 2, 2>, float, 2>);
    m.def("composeTransformsF3", &composeTransforms<itk::Transform<float, 3, 3>, float, 3>);
    m.def("composeTransformsF4", &composeTransforms<itk::Transform<float, 4, 4>, float, 4>);
    m.def("composeTransformsD2", &composeTransforms<itk::Transform<double, 2, 2>, double, 2>);
    m.def("composeTransformsD3", &composeTransforms<itk::Transform<double, 3, 3>, double, 3>);
    m.def("composeTransformsD4", &composeTransforms<itk::Transform<double, 4, 4>, double, 4>);

    m.def("readTransformF2", &readTransform<itk::Transform<float, 2, 2>, float, 2>);
    m.def("readTransformF3", &readTransform<itk::Transform<float, 3, 3>, float, 3>);
    m.def("readTransformF4", &readTransform<itk::Transform<float, 4, 4>, float, 4>);
    m.def("readTransformD2", &readTransform<itk::Transform<double, 2, 2>, double, 2>);
    m.def("readTransformD3", &readTransform<itk::Transform<double, 3, 3>, double, 3>);
    m.def("readTransformD4", &readTransform<itk::Transform<double, 4, 4>, double, 4>);

    m.def("writeTransformF2", &writeTransform<itk::Transform<float, 2, 2>>);
    m.def("writeTransformF3", &writeTransform<itk::Transform<float, 3, 3>>);
    m.def("writeTransformF4", &writeTransform<itk::Transform<float, 4, 4>>);
    m.def("writeTransformD2", &writeTransform<itk::Transform<double, 2, 2>>);
    m.def("writeTransformD3", &writeTransform<itk::Transform<double, 3, 3>>);
    m.def("writeTransformD4", &writeTransform<itk::Transform<double, 4, 4>>);

    m.def("matrixOffsetF2", &matrixOffset<itk::Transform<float, 2, 2>, float, 2>);
    m.def("matrixOffsetF3", &matrixOffset<itk::Transform<float, 3, 3>, float, 3>);
    m.def("matrixOffsetF4", &matrixOffset<itk::Transform<float, 4, 4>, float, 4>);
    m.def("matrixOffsetD2", &matrixOffset<itk::Transform<double, 2, 2>, double, 2>);
    m.def("matrixOffsetD3", &matrixOffset<itk::Transform<double, 3, 3>, double, 3>);
    m.def("matrixOffsetD4", &matrixOffset<itk::Transform<double, 4, 4>, double, 4>);


}
