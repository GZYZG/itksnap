#ifndef NIFTIIMAGEREADER_H
#define NIFTIIMAGEREADER_H
#include <itkImage.h>
#include <itkImageBase.h>
#include <itkImageFileReader.h>
#include <itkSmartPointer.h>
#include <itkExtractImageFilter.h>
#include <itkNiftiImageIO.h>

#include <itkVTKImageToImageFilter.h>
#include <itkImageToVTKImageFilter.h>

// vtk
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkImageFlip.h>
#include <vtkImageViewer2.h>
#include <vtkInteractorStyleMultiTouchCamera.h>
#include <vtkAxesActor.h>
#include <vtkProperty2D.h>
#include <vtkTextProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

#include "utils.h"

template <typename _PixelType,
          unsigned int _Dimension>
class NiftiImageReader{
public:
    using Self = NiftiImageReader;
    //using Pointer = itk::SmartPointer<Self>;

    using PixelType = _PixelType;
    using ImageType = itk::Image<_PixelType, _Dimension>;
    using ReaderType = itk::ImageFileReader<ImageType>;
    using ImageIOType = itk::NiftiImageIO;

    using ImageType2D = itk::Image<_PixelType, 2>;

    typename ImageIOType::Pointer imageIO;
    typename ReaderType::Pointer reader;
    typename ImageType::Pointer image;



    NiftiImageReader(std::string filename) : NiftiImageReader(){
        this->setFileName(filename);
        this->update();
        this->image = reader->GetOutput();
        std::cout << this->image << std::endl;
    }

    ~NiftiImageReader(){

    }

    typename ImageType::Pointer extractSlice(unsigned int dim, unsigned int startSliceIndex, unsigned int sliceNum){
        typename ImageType::Pointer input_data = image; //reader->GetOutput();
        typename ImageType::RegionType inputRegion = input_data->GetBufferedRegion(); // GetLargestPossibleRegion();

        typename ImageType::SizeType size = inputRegion.GetSize();
        size[dim] = sliceNum;// 提取垂直于Z轴的切片（XYZ轴随意）

        // 设置切片在Z轴的位置及切片大小与起始索引
        typename ImageType::IndexType start = inputRegion.GetIndex();
        start[dim] = startSliceIndex;

        typename ImageType::RegionType desiredRegion;
        //无聊的话可以试试自己改变size和start，温馨提示：中间有坑，新手司机请注意路况
        desiredRegion.SetSize(  size  );
        desiredRegion.SetIndex( start );

        // 创建提取图像滤波器
        typedef itk::ExtractImageFilter< ImageType, ImageType > FilterType;
        typename FilterType::Pointer filter = FilterType::New();
        filter->SetInput( input_data);

        // 设置提取切片区域
        filter->SetExtractionRegion( desiredRegion );
        filter->Update();
        typename ImageType::Pointer outputData = filter->GetOutput();

        return outputData;
    }

    typename ImageType::Pointer extractOneSlice(unsigned int dim, unsigned  int startSliceIndex){
        using OutputImageType = ImageType;

        typename ImageType::Pointer input_data = image; //reader->GetOutput();
        typename ImageType::RegionType inputRegion = input_data->GetBufferedRegion();

        typename ImageType::SizeType size = inputRegion.GetSize();
        // std::cout << size << std::endl;
        size[dim] = 0;// 提取垂直于Z轴的切片（XYZ轴随意）

        typename ImageType2D::SizeType outSize;
        unsigned int j =0;
        for(unsigned int i=0; i < 3; i++){
            if( i != dim){
                outSize[j++] = size[i];
            }
        }

        // 设置切片在Z轴的起始索引
        typename ImageType::IndexType start = inputRegion.GetIndex();
        start[dim] = startSliceIndex;

        typename ImageType2D::IndexType outIndex;
        j = 0;
        for(unsigned int i=0; i < 3; i++){
            if( i != dim){
                outIndex[j++] = start[i];
            }
        }

        typename OutputImageType::RegionType desiredRegion;
        //无聊的话可以试试自己改变size和start，温馨提示：中间有坑，新手司机请注意路况
        desiredRegion.SetSize(  size  );
        desiredRegion.SetIndex( start);

        // 创建提取图像滤波器
        typedef itk::ExtractImageFilter< ImageType, OutputImageType > FilterType;
        typename FilterType::Pointer filter = FilterType::New();
        filter->SetInput( input_data);

        // 设置提取切片区域
        filter->SetExtractionRegion( desiredRegion );
        filter->Update();
        typename OutputImageType::Pointer outputData = filter->GetOutput();

        return outputData;
    }

    /**/
    vtkSmartPointer<vtkImageData> itkToVtk(typename ImageType::Pointer img)
    {
        typedef itk::ImageToVTKImageFilter< ImageType> itkTovtkFilterType;
        typename itkTovtkFilterType::Pointer itkTovtkImageFilter = itkTovtkFilterType::New();
        itkTovtkImageFilter->SetInput(img);  // 设置图像数据从ITK转向VTK
        itkTovtkImageFilter->Update();

        vtkSmartPointer< vtkImageFlip > flip1 = vtkSmartPointer< vtkImageFlip >::New();
        flip1->SetInputData(itkTovtkImageFilter->GetOutput());

        flip1->SetFilteredAxes(1);
        flip1->Update();
        return flip1->GetOutput();
    }

    void show3dImage(typename ImageType::Pointer img, vtkRenderWindow* window)
    {
        vtkSmartPointer<vtkImageViewer2> viewer =
            vtkSmartPointer<vtkImageViewer2>::New();
        viewer->SetRenderWindow(window);
        viewer->SetInputData(itkToVtk(img));
        //设置基本属性
        viewer->SetSize(640, 480);
        viewer->SetColorLevel(500);
        viewer->SetColorWindow(2000);
        viewer->SetSlice(40);
        viewer->SetSliceOrientationToXY();
        viewer->Render();
        viewer->GetRenderer()->SetBackground(0, 0, 0);
        viewer->GetRenderWindow()->SetWindowName("ImageViewer2D");

        vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
            vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
        vtkSmartPointer<vtkRenderWindowInteractor> rwi =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
        //设置交互属性
        viewer->SetupInteractor(rwi);
        //viewer->GetRenderer()->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);

        vtkSmartPointer<vtkSliderRepresentation2D> sliderRep =
            vtkSmartPointer<vtkSliderRepresentation2D>::New();
        sliderRep->SetMinimumValue(viewer->GetSliceMin());
        sliderRep->SetMaximumValue(viewer->GetSliceMax());
        sliderRep->SetValue(5.0);
        sliderRep->GetSliderProperty()->SetColor(1, 0, 0);//red
        sliderRep->GetTitleProperty()->SetColor(1, 0, 0);//red
        sliderRep->GetLabelProperty()->SetColor(1, 0, 0);//red
        sliderRep->GetSelectedProperty()->SetColor(0, 1, 0);//green
        sliderRep->GetTubeProperty()->SetColor(1, 1, 0);//yellow
        sliderRep->GetCapProperty()->SetColor(1, 1, 0);//yellow
        sliderRep->GetPoint1Coordinate()->SetCoordinateSystemToDisplay();
        sliderRep->GetPoint1Coordinate()->SetValue(40, 40);
        sliderRep->GetPoint2Coordinate()->SetCoordinateSystemToDisplay();
        sliderRep->GetPoint2Coordinate()->SetValue(500, 40);
        vtkSmartPointer<vtkSliderWidget> sliderWidget =
            vtkSmartPointer<vtkSliderWidget>::New();
        sliderWidget->SetInteractor(rwi);
        sliderWidget->SetRepresentation(sliderRep);
        sliderWidget->SetAnimationModeToAnimate();
        sliderWidget->EnabledOn();

        vtkSmartPointer<vtkSliderCallback> callback =
            vtkSmartPointer<vtkSliderCallback>::New();
        callback->viewer = viewer;

        sliderWidget->AddObserver(vtkCommand::InteractionEvent, callback);
        vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
        double axesSize[3] = { 100,100,100 };
        axes->SetTotalLength(axesSize);
        axes->SetConeRadius(0.1);
        axes->SetShaftTypeToLine();
        axes->SetAxisLabels(false);
        viewer->GetRenderer()->AddActor(axes);
        rwi->Start();
        viewer->Render();
    }



    typename ImageType::SizeType getImageSize(){
        typename ImageType::RegionType inputRegion = image->GetBufferedRegion();

        return inputRegion.GetSize();
    }

    static typename ImageType::SizeType getImageSize(typename ImageType::Pointer img){
        typename ImageType::RegionType inputRegion = img->GetBufferedRegion();
        return inputRegion.GetSize();
    }

    static typename ImageType2D::SizeType getImageSize(typename ImageType2D::Pointer img){
        typename ImageType2D::RegionType inputRegion = img->GetBufferedRegion();
        return inputRegion.GetSize();
    }

    void setFileName(std::string filename){
        reader->SetFileName(filename);
        reader->SetImageIO(imageIO);
    }

    void update(){
        try {
            reader->Update();
        } catch (itk::ExceptionObject &e) {
            std::cerr << "exception in file reader " << std::endl;
            std::cerr << e.GetDescription() << std::endl;
            std::cerr << e.GetLocation() << std::endl;
        }
    }



private:
    NiftiImageReader(){
        this->reader = ReaderType::New();
        this->imageIO = ImageIOType::New();
    }
};


#endif // NIFTIIMAGEREADER_H
