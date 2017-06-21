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

#include "lidarFrame.h"
#include "lidarPoint.h"

// For compatibility with new VTK generic data arrays
#ifdef vtkGenericDataArray_h
#define InsertNextTupleValue InsertNextTypedTuple
#endif

//Custom class to display lidar frames
class frameGUI{
public:
  vtkSmartPointer<vtkPoints> points;
  vtkSmartPointer<vtkPolyData> pointsPolydata;
  vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter;
  vtkSmartPointer<vtkPolyData> polydata;
  //vtkSmartPointer<vtkUnsignedCharArray> colors;
  vtkSmartPointer<vtkPolyDataMapper> mapper;
  vtkSmartPointer<vtkActor> actor;
  vtkSmartPointer<vtkRenderer> renderer;
  vtkSmartPointer<vtkRenderWindow> renderWindow;
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;

	frameGUI(){}
	~frameGUI(){
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

		points =
			vtkSmartPointer<vtkPoints>::New();

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

pointsPolydata =
			vtkSmartPointer<vtkPolyData>::New();

		pointsPolydata->SetPoints(points);

vertexFilter =
			vtkSmartPointer<vtkVertexGlyphFilter>::New();
#if VTK_MAJOR_VERSION <= 5
		vertexFilter->SetInputConnection(pointsPolydata->GetProducerPort());
#else
		vertexFilter->SetInputData(pointsPolydata);
#endif
		vertexFilter->Update();

polydata =
			vtkSmartPointer<vtkPolyData>::New();
		polydata->ShallowCopy(vertexFilter->GetOutput());

		/*
		// Setup colors
		unsigned char red[3] = { 255, 0, 0 };
		unsigned char green[3] = { 0, 255, 0 };
		unsigned char blue[3] = { 0, 0, 255 };

colors =
			vtkSmartPointer<vtkUnsignedCharArray>::New();

		colors->SetNumberOfComponents(3);
		colors->SetName("Colors");
		colors->InsertNextTupleValue(red);
		colors->InsertNextTupleValue(green);
		colors->InsertNextTupleValue(blue);
		

		polydata->GetPointData()->SetScalars(colors);
		*/

		// Visualization
mapper =
			vtkSmartPointer<vtkPolyDataMapper>::New();
#if VTK_MAJOR_VERSION <= 5
		mapper->SetInputConnection(polydata->GetProducerPort());
#else
		mapper->SetInputData(polydata);
#endif

actor =
			vtkSmartPointer<vtkActor>::New();
		actor->SetMapper(mapper);
		actor->GetProperty()->SetPointSize(5);

renderer =
			vtkSmartPointer<vtkRenderer>::New();
renderWindow =
			vtkSmartPointer<vtkRenderWindow>::New();
		renderWindow->AddRenderer(renderer);
renderWindowInteractor =
			vtkSmartPointer<vtkRenderWindowInteractor>::New();
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
