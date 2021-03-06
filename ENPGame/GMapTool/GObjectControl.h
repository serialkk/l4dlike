#pragma once

#include "GObjectForm.h"

// GObjectControl

class GObjectControl : public CDockablePane
{
	DECLARE_DYNAMIC(GObjectControl)

public:
	GObjectControl();
	virtual ~GObjectControl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
};


