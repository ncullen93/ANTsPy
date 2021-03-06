

#ifndef __ANTSPYTRANSFORM_H
#define __ANTSPYTRANSFORM_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include <algorithm>
#include <vector>
#include <string>
#include <stdexcept>

#include "itkMacro.h"
#include "itkImage.h"
#include "itkVectorImage.h"
#include "itkVector.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "vnl/vnl_vector_ref.h"
#include "itkTransform.h"
#include "itkTransformBase.h"
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

#include "antsUtilities.h"
#include "itkAffineTransform.h"
#include "antsImage.h"

namespace py = pybind11;

template <typename TransformType>
class ANTsTransform {
public:
    typedef TransformType itkTransformType;
    std::string precision;
    unsigned int dimension;
    std::string type;
    py::capsule pointer;

    py::list getParameters();
    void setParameters( std::vector< double > );

    py::list getFixedParameters();
    void setFixedParameters( std::vector< double > );

    // transformations
    std::vector< double > transformPoint( std::vector< double > );
    std::vector< double > transformVector( std::vector< double > );
    
    template <typename ImageType>
    ANTsImage<ImageType> transformImage( ANTsImage<ImageType>&, ANTsImage<ImageType>&, std::string);

    template <typename ReturnTransformType>
    ANTsTransform<ReturnTransformType> inverse();
};


// gets associated ITK pixel type as string
std::string getScalarTypeStringFromDummy( unsigned char dummyval ) { return "unsigned char"; }
std::string getScalarTypeStringFromDummy( char dummyval ) { return "char"; }
std::string getScalarTypeStringFromDummy( unsigned short dummyval ) { return "unsigned short"; }
std::string getScalarTypeStringFromDummy( short dummyval ) { return "short"; }
std::string getScalarTypeStringFromDummy( unsigned int dummyval ) { return "unsigned int"; }
std::string getScalarTypeStringFromDummy( int dummyval ) { return "int"; }
std::string getScalarTypeStringFromDummy( float dummyval ) { return "float"; }
std::string getScalarTypeStringFromDummy( double dummyval ) { return "double"; }

// --------------------------------------------------------------

template <typename TransformType>
ANTsTransform<TransformType> wrap_transform( const typename TransformType::Pointer & itkTransform )
{
    typedef typename TransformType::Pointer TransformPointerType;
    TransformPointerType * ptr = new TransformPointerType( itkTransform );

    //typename TransformType::TransformBase::ScalarType dummyval = 1;

    ANTsTransform<TransformType> antsTransform;
    antsTransform.precision         = "float";//getScalarTypeStringFromDummy(dummyval);
    antsTransform.dimension         = TransformType::InputSpaceDimension;
    antsTransform.type              = itkTransform->GetNameOfClass();
    antsTransform.pointer           = py::capsule(ptr, "itk::Transform::Pointer");

    return antsTransform;
}

template <typename TransformType>
typename TransformType::Pointer as_transform( ANTsTransform<TransformType> & transform )
{
    void *ptr = transform.pointer;
    typename TransformType::Pointer * real  = reinterpret_cast<typename TransformType::Pointer *>(ptr); // static_cast or reinterpret_cast ??

    return *real;
}

// --------------------------------------------------------------

template <typename TransformType>
py::list getParametersHelper( typename TransformType::Pointer itkTransform )
{
    py::list parameterslist;
    for (unsigned int i = 0; i < itkTransform->GetNumberOfParameters(); i++ )
    {
        parameterslist.append( itkTransform->GetParameters()[i] );
    }
    return parameterslist;
}

template <typename TransformType>
py::list ANTsTransform<TransformType>::getParameters()
{
    typename TransformType::Pointer itkTransform = as_transform<TransformType>( *this );
    return getParametersHelper<TransformType>( itkTransform );
}


template <typename TransformType>
void setParametersHelper( typename TransformType::Pointer &itkTransform, std::vector<double> new_parameters)
{
    typename TransformType::ParametersType itkParameters;
    itkParameters.SetSize( itkTransform->GetNumberOfParameters() );

    for (unsigned int i = 0; i < itkTransform->GetNumberOfParameters(); i++)
    {
        itkParameters[i] = new_parameters[i];
    }

    itkTransform->SetParameters( itkParameters );
}

template <typename TransformType>
void ANTsTransform<TransformType>::setParameters( std::vector<double> new_parameters )
{
    typename TransformType::Pointer itkTransform = as_transform<TransformType>( *this );
    setParametersHelper<TransformType>( itkTransform, new_parameters );
}

// --------------------------------------------------------------

template <typename TransformType>
py::list getFixedParametersHelper( typename TransformType::Pointer itkTransform )
{
    py::list parameterslist;
    for (unsigned int i = 0; i < itkTransform->GetNumberOfFixedParameters(); i++ )
    {
        parameterslist.append( itkTransform->GetFixedParameters()[i] );
    }
    return parameterslist;
}

template <typename TransformType>
py::list ANTsTransform<TransformType>::getFixedParameters()
{
    typename TransformType::Pointer itkTransform = as_transform<TransformType>( *this );
    return getFixedParametersHelper<TransformType>( itkTransform );
}


template <typename TransformType>
void setFixedParametersHelper( typename TransformType::Pointer &itkTransform, std::vector<double> new_parameters)
{
    typename TransformType::FixedParametersType itkParameters;
    itkParameters.SetSize( itkTransform->GetNumberOfFixedParameters() );

    for (unsigned int i = 0; i < itkTransform->GetNumberOfFixedParameters(); i++)
    {
        itkParameters[i] = new_parameters[i];
    }

    itkTransform->SetFixedParameters( itkParameters );
}

template <typename TransformType>
void ANTsTransform<TransformType>::setFixedParameters( std::vector<double> new_parameters )
{
    typename TransformType::Pointer itkTransform = as_transform<TransformType>( *this );
    setParametersHelper<TransformType>( itkTransform, new_parameters );
}

// --------------------------------------------------------------

template <typename TransformType>
std::vector< double > ANTsTransform<TransformType>::transformPoint( std::vector< double > inPoint )
{
    typedef typename TransformType::Pointer          TransformPointerType;
    typedef typename TransformType::InputPointType   InputPointType;
    typedef typename TransformType::OutputPointType  OutputPointType;

    TransformPointerType itkTransform = as_transform<TransformType>( *this );
    
    InputPointType inItkPoint;
    for (unsigned int i = 0; i < InputPointType::PointDimension; i++)
    {
        inItkPoint[i] = inPoint[i];
    }
    
    OutputPointType outItkPoint = itkTransform->TransformPoint( inItkPoint );

    std::vector< double > outPoint( OutputPointType::PointDimension );
    for (unsigned int i = 0; i < OutputPointType::PointDimension; i++)
    {
        outPoint[i] = outItkPoint[i];
    }

    return outPoint;
}

template <typename TransformType>
std::vector< double > ANTsTransform<TransformType>::transformVector( std::vector< double > inVector )
{
    typedef typename TransformType::Pointer          TransformPointerType;
    typedef typename TransformType::InputVectorType   InputVectorType;
    typedef typename TransformType::OutputVectorType  OutputVectorType;

    TransformPointerType itkTransform = as_transform<TransformType>( *this );

    InputVectorType inItkVector;
    for (unsigned int i = 0; i < InputVectorType::Dimension; i++)
    {
        inItkVector[i] = inVector[i];
    }

    OutputVectorType outItkVector = itkTransform->TransformVector( inItkVector );

    std::vector< double > outVector( OutputVectorType::Dimension );
    for (unsigned int i = 0; i < OutputVectorType::Dimension; i++)
    {
        outVector[i] = outItkVector[i];
    }

    return outVector;
}

template <typename TransformType>
template <typename ReturnTransformType>
ANTsTransform<ReturnTransformType> ANTsTransform<TransformType>::inverse()
{
    std::string type = this->type;
    typedef typename TransformType::Pointer  TransformPointerType;
    TransformPointerType itkTransform = as_transform< TransformType >( *this );

    if ( !itkTransform->IsLinear() )
    {
        throw std::invalid_argument("Only linear transforms may be inverted with this method");
    }

    TransformPointerType inverse = itkTransform->GetInverseTransform();
    return wrap_transform< ReturnTransformType >( inverse );
}

template <typename ImageType>
ANTsImage<ImageType> wrapHelper( typename ImageType::Pointer & image )
{
    return wrap<ImageType>( image );
}


template <typename TransformType>
template <typename ImageType>
ANTsImage<ImageType> ANTsTransform<TransformType>::transformImage( ANTsImage<ImageType> & image, ANTsImage<ImageType> & ref, std::string interp)
{
    typedef typename TransformType::Pointer          TransformPointerType;

    const unsigned int Dimension = TransformType::InputSpaceDimension;

    std::string type = this->type;

    TransformPointerType transform = as_transform<TransformType>( *this );

    typedef typename TransformType::ParametersValueType              PrecisionType;

    //typedef itk::Image<PixelType,TransformType::InputSpaceDimension> ImageType;
    typedef typename ImageType::Pointer                              ImagePointerType;

    // Use base for reference image so we can ignore it's pixeltype
    typedef itk::ImageBase<TransformType::InputSpaceDimension> ImageBaseType;
    typedef typename ImageBaseType::Pointer                    ImageBasePointerType;

    ImagePointerType inputImage = as<ImageType>( image );
    //ImageBasePointerType refImage = as<ImageBaseType>( ref );
    ImagePointerType refImage = as<ImageType>( ref );

    typedef itk::ResampleImageFilter<ImageType,ImageType,PrecisionType,PrecisionType> FilterType;
    typename FilterType::Pointer filter = FilterType::New();

    typedef itk::InterpolateImageFunction<ImageType, PrecisionType> InterpolatorType;
    typename InterpolatorType::Pointer interpolator = ITK_NULLPTR;

  if( interp == "linear" )
    {
    typedef itk::LinearInterpolateImageFunction<ImageType, PrecisionType> LinearInterpolatorType;
    typename LinearInterpolatorType::Pointer linearInterpolator = LinearInterpolatorType::New();
    interpolator = linearInterpolator;
    }
  else if( interp == "nearestneighbor" )
    {
    typedef itk::NearestNeighborInterpolateImageFunction<ImageType, PrecisionType> NearestNeighborInterpolatorType;
    typename NearestNeighborInterpolatorType::Pointer nearestNeighborInterpolator = NearestNeighborInterpolatorType::New();
    interpolator = nearestNeighborInterpolator;
    }
  else if( interp == "bspline" )
    {
    typedef itk::BSplineInterpolateImageFunction<ImageType, PrecisionType> BSplineInterpolatorType;
    typename BSplineInterpolatorType::Pointer bSplineInterpolator = BSplineInterpolatorType::New();
    interpolator = bSplineInterpolator;
    }
  else if( interp == "gaussian" )
    {
    typedef itk::GaussianInterpolateImageFunction<ImageType, PrecisionType> GaussianInterpolatorType;
    typename GaussianInterpolatorType::Pointer gaussianInterpolator = GaussianInterpolatorType::New();
    double sigma[Dimension];
    for( unsigned int d = 0; d < Dimension; d++ )
      {
      sigma[d] = inputImage->GetSpacing()[d];
      }
    double alpha = 1.0;
    gaussianInterpolator->SetParameters( sigma, alpha );
    interpolator = gaussianInterpolator;
    }
  else if( interp ==  "cosinewindowedsinc" )
    {
    typedef itk::WindowedSincInterpolateImageFunction
                 <ImageType, 3, itk::Function::CosineWindowFunction<3, PrecisionType, PrecisionType>, itk::ConstantBoundaryCondition< ImageType >, PrecisionType> CosineInterpolatorType;
    typename CosineInterpolatorType::Pointer cosineInterpolator = CosineInterpolatorType::New();
    interpolator = cosineInterpolator;
    }
  else if( interp == "hammingwindowedsinc" )
    {
    typedef itk::WindowedSincInterpolateImageFunction
                 <ImageType, 3, itk::Function::HammingWindowFunction<3, PrecisionType, PrecisionType >, itk::ConstantBoundaryCondition< ImageType >, PrecisionType> HammingInterpolatorType;
    typename HammingInterpolatorType::Pointer hammingInterpolator = HammingInterpolatorType::New();
    interpolator = hammingInterpolator;
    }
  else if( interp == "lanczoswindowedsinc" )
    {
    typedef itk::WindowedSincInterpolateImageFunction
                 <ImageType, 3, itk::Function::LanczosWindowFunction<3, PrecisionType, PrecisionType>, itk::ConstantBoundaryCondition< ImageType >, PrecisionType > LanczosInterpolatorType;
    typename LanczosInterpolatorType::Pointer lanczosInterpolator = LanczosInterpolatorType::New();
    interpolator = lanczosInterpolator;
    }
  else if( interp == "blackmanwindowedsinc" )
    {
    typedef itk::WindowedSincInterpolateImageFunction
                 <ImageType, 3, itk::Function::BlackmanWindowFunction<3, PrecisionType, PrecisionType>, itk::ConstantBoundaryCondition< ImageType >, PrecisionType > BlackmanInterpolatorType;
    typename BlackmanInterpolatorType::Pointer blackmanInterpolator = BlackmanInterpolatorType::New();
    interpolator = blackmanInterpolator;
    }
  else if( interp == "welchwindowedsinc" )
    {
    typedef itk::WindowedSincInterpolateImageFunction
                 <ImageType, 3, itk::Function::WelchWindowFunction<3, PrecisionType, PrecisionType>, itk::ConstantBoundaryCondition< ImageType >, PrecisionType > WelchInterpolatorType;
    typename WelchInterpolatorType::Pointer welchInterpolator = WelchInterpolatorType::New();
    interpolator = welchInterpolator;
    }
  else if( interp == "multilabel" )
    {
    const unsigned int NVectorComponents = 1;
    typedef ants::VectorPixelCompare<PrecisionType, NVectorComponents> CompareType;
    typedef typename itk::LabelImageGaussianInterpolateImageFunction<ImageType, PrecisionType,
        CompareType> MultiLabelInterpolatorType;
    typename MultiLabelInterpolatorType::Pointer multiLabelInterpolator = MultiLabelInterpolatorType::New();
    double sigma[Dimension];
    for( unsigned int d = 0; d < Dimension; d++ )
      {
      sigma[d] = inputImage->GetSpacing()[d];
      }
    double alpha = 4.0;
    multiLabelInterpolator->SetParameters( sigma, alpha );
    interpolator = multiLabelInterpolator;
    }
    
    filter->SetInput( inputImage );
    filter->SetSize( refImage->GetLargestPossibleRegion().GetSize() );
    filter->SetOutputSpacing( refImage->GetSpacing() );
    filter->SetOutputOrigin( refImage->GetOrigin() );
    filter->SetOutputDirection( refImage->GetDirection() );
    filter->SetInterpolator( interpolator );

    filter->SetTransform( transform );
    filter->Update();

    ImagePointerType filterOutput = filter->GetOutput();

    return wrapHelper<ImageType>( filterOutput );
}


template <typename TransformBaseType, typename PrecisionType, unsigned int Dimension>
ANTsTransform<TransformBaseType> composeTransforms( std::vector<ANTsTransform<TransformBaseType>> tformlist, 
                                                    std::string precision, unsigned int dimension)
{
    //ANTsTransform<TransformBaseType> compositeANTsTransform;
    //compositeANTsTransform.dimension = dimension;
    //compositeANTsTransform.precision = precision;

    typedef typename TransformBaseType::Pointer  TransformBasePointerType;
    typedef typename itk::CompositeTransform<PrecisionType, Dimension> CompositeTransformType;

    typename CompositeTransformType::Pointer comp_transform = CompositeTransformType::New();

    for ( unsigned int i = 0; i < tformlist.size(); i++ )
    {
        //ANTsTransform<TransformBaseType> * tf = tformlist[i].cast<ANTsTransform<TransformBaseType> *>();
        TransformBasePointerType t = as_transform<TransformBaseType>( tformlist[i] );
        comp_transform->AddTransform( t );
    }

    //TransformBasePointerType transform = dynamic_cast<TransformBaseType *>(comp_transform.GetPointer());
    //std::string type = comp_transform->GetNameOfClass();
    //composeANTsTransform.type = type;

    //TransformBasePointerType * rawPointer = new TransformBasePointerType( transform );
    //py::capsule xptr = wrap_transform< TransformBaseType >( rawPointer );
    //composeANTsTransform.pointer = xptr;
    
    //return composeANTsTransform;
    return wrap_transform< TransformBaseType >( comp_transform.GetPointer() );
}

template <typename TransformBaseType, class PrecisionType, unsigned int Dimension>
ANTsTransform< TransformBaseType > readTransform( std::string filename, unsigned int dimension, std::string precision )
{
    typedef typename TransformBaseType::Pointer               TransformBasePointerType;
    typedef typename itk::CompositeTransform<PrecisionType, Dimension> CompositeTransformType;

    typedef itk::TransformFileReaderTemplate<PrecisionType> TransformReaderType;
    typedef typename TransformReaderType::TransformListType TransformListType;

    typename TransformReaderType::Pointer reader = TransformReaderType::New();
    reader->SetFileName( filename );
    reader->Update();

    const typename TransformReaderType::TransformListType * transformList = reader->GetTransformList();

    TransformBasePointerType transform;

    if ( transformList->size() > 1 )
    {
        typename CompositeTransformType::Pointer comp_transform = CompositeTransformType::New();
        typedef typename TransformListType::const_iterator TransformIteratorType;
        for (TransformIteratorType i = transformList->begin(); i != transformList->end(); ++i)
        {
            comp_transform->AddTransform( dynamic_cast<TransformBaseType *>( i->GetPointer()) );
        }
        transform = dynamic_cast<TransformBaseType *>(comp_transform.GetPointer());
        return wrap_transform< TransformBaseType >( transform );
    }
    else
    {
        transform = dynamic_cast<TransformBaseType *>( transformList->front().GetPointer() );
        return wrap_transform< TransformBaseType >( transform );
    }
    
    return wrap_transform< TransformBaseType >( transform );
}


template <typename TransformType>
void writeTransform( ANTsTransform< TransformType > transform, std::string filename )
{
    typedef typename TransformType::Pointer          TransformPointerType;
    TransformPointerType itkTransform = as_transform<TransformType>( transform );
    typedef itk::TransformFileWriter TransformWriterType;
    typename TransformWriterType::Pointer transformWriter = TransformWriterType::New();
    transformWriter->SetInput( itkTransform );
    transformWriter->SetFileName( filename.c_str() );
    transformWriter->Update();
}



// ------------------------------------------------------------------

template< typename TransformBaseType, class PrecisionType, unsigned int Dimension >
ANTsTransform<TransformBaseType> matrixOffset( std::string type, std::string precision, unsigned int dimension, 
                                               std::vector<std::vector<double> > matrix,
                                               std::vector<double> offset,
                                               std::vector<double> center,
                                               std::vector<double> translation,
                                               std::vector<double> parameters,
                                               std::vector<double> fixedparameters)
{

    typedef itk::MatrixOffsetTransformBase< PrecisionType, Dimension, Dimension> MatrixOffsetBaseType;
    typedef typename MatrixOffsetBaseType::Pointer                               MatrixOffsetBasePointerType;
    typedef typename TransformBaseType::Pointer                                  TransformBasePointerType;

    MatrixOffsetBasePointerType matrixOffset = NULL;

    // Initialize transform by type
    if ( type == "AffineTransform" )
    {
        typedef itk::AffineTransform<PrecisionType,Dimension> TransformType;
        typename TransformType::Pointer transformPointer = TransformType::New();
        matrixOffset = dynamic_cast<MatrixOffsetBaseType*>( transformPointer.GetPointer() );
    }
    else if ( type == "CenteredAffineTransform" )
    {
        typedef itk::CenteredAffineTransform<PrecisionType,Dimension> TransformType;
        typename TransformType::Pointer transformPointer = TransformType::New();
        matrixOffset = dynamic_cast<MatrixOffsetBaseType*>( transformPointer.GetPointer() );
    }
    else if ( type == "Euler3DTransform" )
    {
        typedef itk::Euler3DTransform<PrecisionType> TransformType;
        typename TransformType::Pointer transformPointer = TransformType::New();
        matrixOffset = dynamic_cast<MatrixOffsetBaseType*>( transformPointer.GetPointer() );
    }
    else if ( type == "Euler2DTransform" )
    {
        typedef itk::Euler2DTransform<PrecisionType> TransformType;
        typename TransformType::Pointer transformPointer = TransformType::New();
        matrixOffset = dynamic_cast<MatrixOffsetBaseType*>( transformPointer.GetPointer() );
    }
    else if ( type == "QuaternionRigidTransform" )
    {
        typedef itk::QuaternionRigidTransform<PrecisionType> TransformType;
        typename TransformType::Pointer transformPointer = TransformType::New();
        matrixOffset = dynamic_cast<MatrixOffsetBaseType*>( transformPointer.GetPointer() );
    }
    else if ( type == "Rigid2DTransform" )
    {
        typedef itk::Rigid2DTransform<PrecisionType> TransformType;
        typename TransformType::Pointer transformPointer = TransformType::New();
        matrixOffset = dynamic_cast<MatrixOffsetBaseType*>( transformPointer.GetPointer() );
    }
    else if ( type == "CenteredEuler3DTransform" )
    {
        typedef itk::CenteredEuler3DTransform<PrecisionType> TransformType;
        typename TransformType::Pointer transformPointer = TransformType::New();
        matrixOffset = dynamic_cast<MatrixOffsetBaseType*>( transformPointer.GetPointer() );
    }
    else if ( type == "CenteredRigid2DTransform" )
    {
        typedef itk::CenteredRigid2DTransform<PrecisionType> TransformType;
        typename TransformType::Pointer transformPointer = TransformType::New();
        matrixOffset = dynamic_cast<MatrixOffsetBaseType*>( transformPointer.GetPointer() );
    }
    else if ( type == "Similarity3DTransform" )
    {
        typedef itk::Similarity3DTransform<PrecisionType> TransformType;
        typename TransformType::Pointer transformPointer = TransformType::New();
        matrixOffset = dynamic_cast<MatrixOffsetBaseType*>( transformPointer.GetPointer() );
    }
    else if ( type == "Similarity2DTransform" )
    {
        typedef itk::Similarity2DTransform<PrecisionType> TransformType;
        typename TransformType::Pointer transformPointer = TransformType::New();
        matrixOffset = dynamic_cast<MatrixOffsetBaseType*>( transformPointer.GetPointer() );
    }
    else if ( type == "CenteredSimilarity2DTransform" )
    {
        typedef itk::CenteredSimilarity2DTransform<PrecisionType> TransformType;
        typename TransformType::Pointer transformPointer = TransformType::New();
        matrixOffset = dynamic_cast<MatrixOffsetBaseType*>( transformPointer.GetPointer() );
    }
    else
    {
        typedef itk::AffineTransform<PrecisionType,Dimension> TransformType;
        typename TransformType::Pointer transformPointer = TransformType::New();
        matrixOffset = dynamic_cast<MatrixOffsetBaseType*>( transformPointer.GetPointer() );
    }

    matrixOffset->SetIdentity();

    if (matrix.size() > 1)
    {
        typename MatrixOffsetBaseType::MatrixType itkMatrix;
        for ( unsigned int i=0; i<dimension; i++)
            for ( unsigned int j=0; j<dimension; j++)
            {
                itkMatrix(i,j) = matrix[i][j];
            }
        matrixOffset->SetMatrix( itkMatrix );    
    }
    
    if (translation.size() > 0)
    {
        typename MatrixOffsetBaseType::OutputVectorType itkTranslation;
        for ( unsigned int i=0; i<dimension; i++)
        {
            itkTranslation[i] = translation[i];
        }
        matrixOffset->SetTranslation( itkTranslation );
    }

    if (offset.size() > 0)
    {
        typename MatrixOffsetBaseType::OutputVectorType itkOffset;
        for ( unsigned int i=0; i<dimension; i++)
        {
            itkOffset[i] = offset[i];
        }
        matrixOffset->SetOffset( itkOffset );
    }

    if (center.size() > 0)
    {
        typename MatrixOffsetBaseType::InputPointType itkCenter;
        for ( unsigned int i=0; i<dimension; i++)
        {
            itkCenter[i] = center[i];
        }
        matrixOffset->SetCenter( itkCenter );
    }

    if (parameters.size() > 0)
    {
        typename MatrixOffsetBaseType::ParametersType itkParameters;
        itkParameters.SetSize( matrixOffset->GetNumberOfParameters() );
        for ( unsigned int i=0; i<matrixOffset->GetNumberOfParameters(); i++)
        {
            itkParameters[i] = parameters[i];
        }
        matrixOffset->SetParameters( itkParameters );
    }

    if (fixedparameters.size() > 0)
    {
        typename MatrixOffsetBaseType::FixedParametersType itkFixedParameters;
        itkFixedParameters.SetSize( matrixOffset->GetNumberOfFixedParameters() );
        for ( unsigned int i=0; i<matrixOffset->GetNumberOfFixedParameters(); i++)
        {
            itkFixedParameters[i] = fixedparameters[i];
        }
        matrixOffset->SetFixedParameters( itkFixedParameters );
    }

    TransformBasePointerType itkTransform = dynamic_cast<TransformBaseType*>( matrixOffset.GetPointer() );
    
    return wrap_transform< TransformBaseType >( itkTransform );
}


#endif