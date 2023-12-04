#include "cnpy.h"
#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleMultiTouchCamera.h>
#include <vtkRenderer.h>
#include "vtkAutoInit.h"
#include "vtkPoints.h"
#include "vtkVertexGlyphFilter.h"
#include "vtkPointData.h"
#include "vtkFloatArray.h"
#include "vtkLineSource.h"
#include "vtkTransform.h"
#include "vtkInteractorStyleMultiTouchCamera.h"
#include "vtkAxesActor.h"
#include "vtkAutoInit.h"
#include <iostream>


VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType)
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2)//体素


using namespace std;

vector<vector<int32_t>> readnpy(const string& npy_name) {
	cnpy::NpyArray arr = cnpy::npy_load(npy_name);
	vector<vector<int32_t>> read_vec(arr.shape[0], vector<int32_t>(arr.shape[1], 0.0));
	int32_t* loaded_data = arr.data<int32_t>();
	for (int i = 0; i < arr.shape[0]; ++i) {
		for (int j = 0; j < arr.shape[1]; ++j) {
			read_vec[i][j] = loaded_data[i * arr.shape[1] + j];
		}
	}
	return read_vec;
}

int main(int argc, char* argv[]) {
	if (argc == 2) {
		string filename = argv[1];
		if (filename.find(".npy") == string::npos) {
			exit(0);
		}
		vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
		vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();	//_存放细胞顶点，用于渲染（显示点云所必须的）
		vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
		vtkSmartPointer<vtkFloatArray> myscalar = vtkSmartPointer<vtkFloatArray>::New();
		vector<vector<int32_t>> data = readnpy(filename);
		int j = 0;
		vector<float> x, y, z;
		for (int i = 0; i < data.size(); ++i) {
			x.push_back((double)data[i][0]/1000);
			y.push_back((double)data[i][1]/1000);
			z.push_back((double)data[i][2]/1000);
			points->InsertPoint(j, (double)data[i][0]/1000, (double)data[i][1]/1000, (double)data[i][2]/1000);
			myscalar->InsertNextTuple1((double)1 - (double)data[i][3] / 50);
			vertices->InsertNextCell(1);
			vertices->InsertCellPoint(j);
			j += 1;
		}
		polyData->SetPoints(points);		//_设置点集
		polyData->SetVerts(vertices);		//_设置渲染顶
		polyData->GetPointData()->SetScalars(myscalar);
		vtkNew<vtkPolyDataMapper> mapper;
		mapper->SetInputData(polyData);

		vtkNew<vtkActor> actor;
		actor->SetMapper(mapper);

		vtkNew<vtkTransform> transform;
		transform->Translate(1, 1, 1);

		vtkNew<vtkAxesActor> axes;
		axes->SetUserTransform(transform);
		float max_x = *max_element(x.begin(), x.end());
		axes->SetTotalLength(5, 5, 5);

		vtkNew<vtkRenderer> renderer;
		renderer->SetBackground(0.15, 0.15, 0.15);
		renderer->AddActor(axes);
		renderer->AddActor(actor);
		int nums = max_x / 10;
		for (int i = 1; i < nums + 1; ++i) {
			vtkNew<vtkLineSource> line;
			line->SetPoint1((double)i * 10, -20, 0);
			line->SetPoint2((double)i * 10, 20, 0);
			vtkNew<vtkPolyDataMapper> lineMapper;
			lineMapper->SetInputConnection(line->GetOutputPort());
			vtkNew<vtkActor> lineActor;
			lineActor->SetMapper(lineMapper);
			lineActor->GetProperty()->SetLineWidth(4);
			vtkNew<vtkNamedColors> cols;
			lineActor->GetProperty()->SetColor(cols->GetColor3d("White").GetData());
			renderer->AddActor(lineActor);
		}

		vtkNew<vtkRenderWindow> renderWindow;
		renderWindow->AddRenderer(renderer);
		renderWindow->SetWindowName("Line");
		renderWindow->SetSize(800, 600);
		renderWindow->SetPosition(100, 100);//设置渲染窗口再屏幕坐标中的位置
		renderWindow->Render();

		vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
		renderWindowInteractor->SetRenderWindow(renderWindow);
		vtkInteractorStyleMultiTouchCamera* style = vtkInteractorStyleMultiTouchCamera::New();
		renderWindowInteractor->SetInteractorStyle(style);

		renderWindowInteractor->Initialize();
		renderWindowInteractor->Start();
	}
	else {
		exit(0);
	}
    return 0;
}
