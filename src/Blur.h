// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// Blur.h - Blur widget
//
// Written by: Khadeeja Din, 2016
// ======================================================================

#ifndef BLUR_H
#define BLUR_H

#include "ImageFilter.h"

class Blur : public ImageFilter {
	Q_OBJECT

public:
	Blur	(QWidget *parent = 0);	               // constructor
	QGroupBox*	controlPanel	();		             // create control panel
	bool		applyFilter(ImagePtr, ImagePtr);     // apply filter to input to init output
	void		reset		();		                       // reset parameters

protected:
	void blur(ImagePtr I1, int xsz, int ysz, ImagePtr I2);
	template <class T>
	void IP_blur1D (ChannelPtr<T> src, int len, int stride, double ww, ChannelPtr<T> dst);

protected slots:
	void    changeWidth (int);
	void    changeHeight (int);
	void    changeBoth (int);

private:
	// blur controls
	QSlider		*m_sliderW ;	// filter width slider
	QSlider		*m_sliderH ;	// filter height slider
	QSpinBox	*m_spinBoxW;	// filter width spinbox
	QSpinBox	*m_spinBoxH;	// filter height spinbox
	QCheckBox *m_checkBox;  // checkbox for sz * sz filter

	// widgets and groupbox
	QGroupBox	*m_ctrlGrp;	// groupbox for panel
};

#endif	// BLUR_H
