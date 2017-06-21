#ifndef _FRAME_GUI_H_
#define _FRAME_GUI_H_

#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkCellArray.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkProperty.h>
#include <vtkLookupTable.h>

#include "lidarFrame.h"
#include "lidarPoint.h"

// For compatibility with new VTK generic data arrays
#ifdef vtkGenericDataArray_h
#define InsertNextTupleValue InsertNextTypedTuple
#endif

// Creating a lookup table to map 0-255 intensity value to a color
// www.vtk.org/Wiki/VTK/Examples/Cxx/Utilities/ColorLookupTable

//Custom class to display lidar frames
class frameGUI{
private:
	//Maps number in range 0 to 1 to 0 to 255
	u_char mapTo255(float in){
		u_char ret = in * 255.0;
		return ret;
	}
public:
  vtkSmartPointer<vtkPoints> points;
  vtkSmartPointer<vtkPolyData> pointsPolydata;
  vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter;
  vtkSmartPointer<vtkPolyData> polydata;
  vtkSmartPointer<vtkUnsignedCharArray> colors;
  vtkSmartPointer<vtkPolyDataMapper> mapper;
  vtkSmartPointer<vtkActor> actor;
  vtkSmartPointer<vtkRenderer> renderer;
  vtkSmartPointer<vtkRenderWindow> renderWindow;
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;

  //Color lookup table
  vtkSmartPointer<vtkLookupTable> lookupTable;

	frameGUI(){}
	~frameGUI(){
		//Causes segfault in vtkSmartPointer
		/*
		  points->Delete();
		  pointsPolydata->Delete();
		  vertexFilter->Delete();
		  polydata->Delete();
		  //colors->Delete();
		  mapper->Delete();
		  actor->Delete();
		  renderWindow->Delete();
		  renderer->Delete();
		  renderWindowInteractor->Delete();
	  */
	}


	void renderFrame(const lidarFrame& frame){
		const vector<lidarPoint>* lpoints = frame.getPoints();

		points = vtkSmartPointer<vtkPoints>::New();

		int count = lpoints->size();
		int rval = points->Allocate(count);
		if (rval == 1){
			printf("Allocation Succeeded!\n");
		}
		if (rval == 0){
			printf("Allocation Failed!\n");
		}
		//system("pause");

		//Adding all the points to the gui
		for (u_int i = 0; i < count; ++i){
			const float* xyz = lpoints->at(i).xyz;

			try{
				points->InsertNextPoint(xyz);
			}
			catch (const exception& e){
				void;
				system("pause");
				printf("hi");
			}
		}

		pointsPolydata = vtkSmartPointer<vtkPolyData>::New();

		pointsPolydata->SetPoints(points);

		vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
#if VTK_MAJOR_VERSION <= 5
		vertexFilter->SetInputConnection(pointsPolydata->GetProducerPort());
#else
		vertexFilter->SetInputData(pointsPolydata);
#endif
		vertexFilter->Update();

		polydata = vtkSmartPointer<vtkPolyData>::New();
		polydata->ShallowCopy(vertexFilter->GetOutput());

		//---------------------------------------------------------
		//Setup colors:
		lookupTable = vtkSmartPointer<vtkLookupTable>::New();
		lookupTable->SetTableRange(0.0, 255.0);
		// If you don't want to use the whole color range, you can use
		// SetValueRange, SetHueRange, and SetSaturationRange
		lookupTable->Build();

		colors =
			vtkSmartPointer<vtkUnsignedCharArray>::New();
		//Sets char array to have rgb values
		colors->SetNumberOfComponents(3);
		colors->SetName("Colors");

		//Add a color for each point based on its intensity. TODO: Put in previous loop
		//to optimize on branch instructions

		for (int i = 0; i < lpoints->size(); ++i){
			u_char curr_intensity = lpoints->at(i).intensity;
			double colord[3];
			//Get the rgb values
			lookupTable->GetColor(curr_intensity,colord);
			//Halve the amount of memory used for color
			float color[3];
			color[0] = mapTo255(colord[0]);
			color[1] = mapTo255(colord[1]);
			color[2] = mapTo255(colord[2]);

			colors->InsertNextTuple(color);
		}

		//---------------------------------------------------------

		//Set up the colors
		polydata->GetPointData()->SetScalars(colors);
		

		// Visualization
		mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
#if VTK_MAJOR_VERSION <= 5
		mapper->SetInputConnection(polydata->GetProducerPort());
#else
		mapper->SetInputData(polydata);
#endif

		actor = vtkSmartPointer<vtkActor>::New();
		actor->SetMapper(mapper);
		actor->GetProperty()->SetPointSize(3);

		renderer = vtkSmartPointer<vtkRenderer>::New();
		renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
		
		//Width,height
		renderWindow->SetSize(1200,600);
		renderWindow->AddRenderer(renderer);
		renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
		renderWindowInteractor->SetRenderWindow(renderWindow);

		renderer->AddActor(actor);

		printf("About to render the frame\n");
		try{
			renderWindow->Render();
		}
		catch (const exception& e){
			printf("problems");
		}
		printf("Rendered the frame\n");
		//renderWindowInteractor->Start();
	}


};

#endif