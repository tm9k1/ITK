/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkPatternIntensityPointSetToImageMetric.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) 2000 National Library of Medicine
  All rights reserved.

  See COPYRIGHT.txt for copyright details.

=========================================================================*/
#ifndef _itkPatternIntensityPointSetToImageMetric_txx
#define _itkPatternIntensityPointSetToImageMetric_txx

#include "itkPatternIntensityPointSetToImageMetric.h"

namespace itk
{

/**
 * Constructor
 */
template < class TTarget, class TMapper > 
PatternIntensityPointSetToImageMetric<TTarget,TMapper>
::PatternIntensityPointSetToImageMetric()
{
}




/**
 * Get the match Measure
 */
template < class TTarget, class TMapper > 
PatternIntensityPointSetToImageMetric<TTarget,TMapper>::MeasureType
PatternIntensityPointSetToImageMetric<TTarget,TMapper>
::GetValue( const ParametersType & parameters )
{

  std::cout << "GetValue( " << parameters << " ) = ";

  typename TargetType::PointType point;  

  double ReferenceValue;
  double TargetValue;

  typedef  typename  TargetType::PointsContainerPointer     
                                              PointsContainerPointerType;

  typedef  typename  TargetType::PointDataContainerPointer 
                                              PointsDataContainerPointerType;

  typedef  typename  TargetType::PointsContainer     
                                              PointsContainerType;

  typedef  typename  TargetType::PointDataContainer 
                                              PointsDataContainerType;


  typename  PointsContainerType::Iterator       pt;
  typename  PointsDataContainerType::Iterator   vl;

  TargetPointer target = Superclass::GetTarget();

  PointsContainerPointerType       points = target->GetPoints();
  PointsDataContainerPointerType   data   = target->GetPointData();

  pt = points->Begin();
  vl = data->Begin();

  m_MatchMeasure = 0;
  
  bool insidePoint; 

  unsigned int  count = 0;

  MapperPointer mapper = Superclass::GetMapper();
  mapper->GetTransformation()->SetParameters( parameters );

  double sab = 0.0;
  double saa = 0.0;
  double sbb = 0.0;

  while( pt != points->End()  || vl != data->End() )
  {
    point       = pt.Value();
    TargetValue = vl.Value();

    insidePoint = true;

    try {
     ReferenceValue = mapper->Evaluate( point );
    }

    //If the Mapped Voxel is outside the image
    catch (MapperException) 
    {  
      insidePoint = false;
    }

    if(insidePoint) 
    {
      count++;
      const double diff = ReferenceValue - TargetValue; 
      m_MatchMeasure += 1.0 / ( 1.0 + diff * diff ); 
    }  
  
   ++pt;
   ++vl;
  }

  if(count == 0) 
  {
    std::cout << "All the mapped image is outside !" << std::endl;
    return 100000;
  } 

  // The sign is changed because the optimization method looks for minima
  m_MatchMeasure = -m_MatchMeasure;
  std::cout<<"m_MatchMeasure= "<<m_MatchMeasure<<std::endl; 
  return m_MatchMeasure;

}





/**
 * Get the Derivative Measure
 */
template < class TTarget, class TMapper> 
const PatternIntensityPointSetToImageMetric<TTarget,TMapper>::DerivativeType &
PatternIntensityPointSetToImageMetric<TTarget,TMapper>
::GetDerivative( const ParametersType & parameters )
{

  const double delta = 0.001;
  ParametersType testPoint;
  testPoint = parameters;

  for( unsigned int i=0; i<SpaceDimension; i++) 
  {
    testPoint[i] -= delta;
    const MeasureType valuep0 = GetValue( testPoint );
    testPoint[i] += 2*delta;
    const MeasureType valuep1 = GetValue( testPoint );
    m_MatchMeasureDerivatives[i] = (valuep1 - valuep0 ) / ( 2.0 * delta );
    testPoint[i] = parameters[i];
  }
  std::cout << "PatternIntensity Derivative = ";
  std::cout << m_MatchMeasureDerivatives << std::endl;
  return m_MatchMeasureDerivatives;

}




/**
 * Get both the match Measure and theDerivative Measure 
 */
template < class TTarget, class TMapper > 
void
PatternIntensityPointSetToImageMetric<TTarget,TMapper>
::GetValueAndDerivative(const ParametersType & parameters, 
                        MeasureType & Value, DerivativeType  & Derivative)
{
  Value      = GetValue( parameters );
  Derivative = GetDerivative( parameters );
}



} // end namespace itk


#endif
