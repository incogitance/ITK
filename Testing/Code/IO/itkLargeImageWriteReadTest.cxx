/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkLargeImageWriteReadTest.cxx
  Language:  C++
  Date:      $Date$xgoto-l

  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkImageFileReader.h"
#include "itkImageRegionIterator.h"

int itkLargeImageWriteReadTest(int ac, char* av[])
{

  if (ac < 3)
    {
    std::cout << "usage: itkIOTests itkLargeImageWriteReadTest outputFileName numberOfPixelsInOneDimension" << std::endl;
    return EXIT_FAILURE;
    }

  typedef unsigned short  PixelType;
  typedef itk::Image<PixelType,2> ImageType;

  typedef itk::ImageFileWriter< ImageType >   WriterType;
  typedef itk::ImageFileReader< ImageType >   ReaderType;

  ImageType::Pointer image = ImageType::New();
  ImageType::RegionType region;
  ImageType::IndexType index;
  ImageType::SizeType size;

  const unsigned long numberOfPixelsInOneDimension = atol( av[2] );
 
  size.Fill( numberOfPixelsInOneDimension );
  index.Fill(0);
  region.SetSize(size);
  region.SetIndex(index);
  
  image->SetRegions(region);

  const unsigned long sizeInMegaBytes =
    ( sizeof(PixelType) * numberOfPixelsInOneDimension * numberOfPixelsInOneDimension ) /
    ( 1024.0 * 1024.0 );

  std::cout << "Trying to allocate an image of size " << sizeInMegaBytes << " Mb " << std::endl; 

  image->Allocate();

  std::cout << "Initializing pixel values " << std::endl;
  typedef itk::ImageRegionIterator< ImageType >  IteratorType;
  typedef itk::ImageRegionConstIterator< ImageType >  ConstIteratorType;

  IteratorType itr( image, region );
  itr.GoToBegin();

  PixelType pixelValue = itk::NumericTraits< PixelType >::Zero;

  while( !itr.IsAtEnd() )
    {
    itr.Set( pixelValue );
    ++pixelValue;
    ++itr;
    }

  std::cout << "Trying to write the image to disk" << std::endl;
  try
    {
    WriterType::Pointer writer = WriterType::New();
    writer->SetInput(image);
    writer->SetFileName(av[1]);
    writer->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Trying to read the image back from disk" << std::endl;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(av[1]);

  try
    {
    reader->Update();
    }
  catch (itk::ExceptionObject &ex)
    {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
    }

  ImageType::ConstPointer readImage = reader->GetOutput();

  ConstIteratorType ritr( readImage, region );
  IteratorType oitr( image, region );

  ritr.GoToBegin();
  oitr.GoToBegin();

  while( !ritr.IsAtEnd() )
    {
    if( oitr.Get() != ritr.Get() )
      {
      std::cerr << "Pixel comparison failed at index = " << oitr.GetIndex() << std::endl;
      return EXIT_FAILURE;
      }
    ++oitr;
    ++ritr;
    }


  std::cout << "Comparing the pixel values.. :" << std::endl;

  return EXIT_SUCCESS;

}
