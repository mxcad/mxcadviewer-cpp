#pragma once

#include <memory>

class MxCADObjectWrapper
{
public:
	MxCADObjectWrapper();
    ~MxCADObjectWrapper();


	bool Create();

	int ReadFile(const char* fileName);

	int WriteFile(const char* fileName);

	int DrawLine(double x1, double y1, double x2, double y2);
    int DrawCircle(double x, double y, double r);

private:
	struct Private;
	std::unique_ptr<Private> m_p;
};

